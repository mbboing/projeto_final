/*** Projeto de conlusão de curso de Engenharia de Computação - PUC Rio ***/
/*** Autor: Miguel Baldez Boing ***/
/*** Atualizações mantidas em: https://github.com/mbboing/projeto_final ***/

/************************************ Codigo da arvore geradora****************************************/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "interface.h"

//Identificador do no
#define ID 1

//Tempo de timeout caso nao se encontre filhos
#define TIMEOUT 15000

//Valor para simular o valor de um sensor
#define VIRTUALSENSOR 10

//Timers
unsigned int timeout_alarm, request_alarm;

//Ids das estaçoes, nós filhos
int stations_ids[10];

//Contador da quantidade de filhos e de ramos concluidos
int total_de_estacoes = 0;
int ramos_concluidos = 0;

//Nome da própria rede WiFi e a rede do nó pai
char ssid_proprio[6];
char ssid_pai[30];

//String com a próxima mensagem a ser enviada
char mensagem[64];

//Indica se a árvore já está construída a partir deste nó e se já avisou isso ao nó pai
int ramo_concluido = 0;
int aviso_enviado = 0;

//Estado de requisição do valor do sensor
int requesting = 0;

//Debouncing para a interrupção de entrada
int debouncing = 0;


void when_system_starts(void){

	//Se for base station liga o wifi como AP, senao liga como station
	if(ID == 1){

		define_interrupt(12);

		//O nome da rede WiFi será sempre ESP seguido do ID do nó
		initialise_wifi_ap("ESP1", "eventinterface");
	}
	else
		initialise_wifi_sta();

}

void when_alarm_fired(unsigned int alarm_id){

	//Verfifica se o alarme que disparou é o do timeout de busca de filhos
	if(alarm_id == timeout_alarm){

		//Se não for base station
		if(ID != 1){

			printf("Timeout! Não foram encontrados filhos!\n");

			//A sub-arvore que se ramifica desse nó já está completa

			ramo_concluido = 1;

			//Desiste de buscar filhos, se reconectando ao nó pai

			initialise_wifi_sta();
		}
		else{

			//A arvore já está completa com um nó

			ramo_concluido = 1;

			printf("Arvore concluída sem filhos.\n");
		}
	}

	else if(alarm_id == request_alarm){

		//Desativa o debouncing
		debouncing = 0;

		//No caso da base station
		if(ID == 1){

			//Envia mensagem pedindo o valor dos sensores para todos os filhos
			if(total_de_estacoes > 0){
				ramos_concluidos = 0;
				//printf("total_de_estacoes: %d\n", total_de_estacoes);
				for(int i=0; i<total_de_estacoes; i++){
					printf("%d: Request to %d\n", i, stations_ids[i]);
					send_message("Sensor request", stations_ids[i]);
				}
			}

			//Se não tiver filhos, mostra somente o próprio valor
			else{
				printf("ID:SENSOR:\n%d:%d\n", ID, VIRTUALSENSOR);
			}
		}
	}
}

void when_interrupt_emitted(unsigned int pin, int signal_edge){

	//ets_printf("\nInterrupt of pin %d emitted. Signal_edge: %d\n", pin, signal_edge);

	if(debouncing == 0){
		if(signal_edge == 1){
			//if(ramo_concluido){

				request_alarm = create_alarm(500, 0);

				//Ativa o debouncing
				debouncing = 1;
		
			//}
		}
	}
}

void when_wifi_starts(int ap){

	//Se for AP é o pai, senão é um filho que deve buscar um pai
	if(ap){

        //Para debug
        printf("Wifi started - AP mode\n");

		//Estado de montagem da arvore
		if(!ramo_concluido){
			//Cria um timer de timeout caso não encontre filhos
			timeout_alarm = create_alarm(TIMEOUT, 0);
		}
	}
	else{

		//Para debug
		printf("Wifi started - STA mode\n");

		
		//Se ainda estiver na etapa de construção da árvore
		if(!ramo_concluido){

			
			char ssids[30][30];
			char first_letters[4];
			unsigned int ssids_totals, i;
			unsigned int ssid_found = 0;

			//Busca um ssid com o nome começado com 'ESP' e salva como ssid do pai
			do{
				ssids_totals = scan_wifi(ssids, 30);
				for(i=0; i<ssids_totals; i++){
					strcpy(first_letters, ssids[i]);
					first_letters[3] = '\0';

					if(strcmp(first_letters,"ESP") == 0){
						strcpy(ssid_pai, ssids[i]);
						ssid_found = 1;
					}
				}
			}while(!ssid_found);

			//Para debug
			printf("wifi found: %s\n", ssid_pai);
			strcpy(ssid_pai, "ESP1");

			//wifi_connect(ssid_pai, "eventinterface");
			
		}
		
		//Senão conecta ao WiFi do nó pai
		else{
			wifi_connect(ssid_pai, "eventinterface");
		}
		

		//wifi_connect("ESP1", "eventinterface");
	}

}

void when_wifi_connects(){

    //Para debug
	printf("wifi connected!\n");
		
	//Se ainda estiver na etapa de construção da árvore, muda para o modo AP para buscar filhos
	if(!ramo_concluido){
		char id[3];

		//O nome da rede WiFi será sempre ESP seguido do ID do nó
		sprintf(id, "%d", ID);
		strcpy(ssid_proprio, "ESP");
		strcat(ssid_proprio, id);

		initialise_wifi_ap(ssid_proprio, "eventinterface");
	}

	//Senão, avisa que o ramo está concluído na primeira reconexão
	else if(!aviso_enviado){
		send_message("Ramo concluido", AP_NODE);
		aviso_enviado = 1;
	}

	//Se estiver no estado de requisição dos valores
	if(requesting){

		//Sai do estado de requisição
		requesting = 0;

		//Repassa a mensagem para o pai
		send_message(mensagem, AP_NODE);
	}

}

void when_a_station_connects(unsigned int station_connected){

	int id_found = 0;

    //Para debug
	printf("Estacao conectada: %d\n", station_connected);

	//Remove o alarme de timeout
	remove_alarm(timeout_alarm);

	//Verifica se a estação conectada é nova, isto é, se não está no vetor de estações	
	for(int i=0; i<total_de_estacoes; i++){
		if(stations_ids[i] == station_connected){

			id_found = 1;
			break;
		}
	}

	//Caso seja nova, adiciona ao vetor de estações
	if(!id_found){

		stations_ids[total_de_estacoes] = station_connected;
		total_de_estacoes++;
	}

	//Estado de requisicao dos valores
	if(requesting){

		//Repassa o pedido
		send_message("Sensor request", station_connected);
	}

}

void when_a_station_disconnects(unsigned int station_disconnected){

        //Para debug
        printf("Estacao desconectada: %d\n", station_disconnected);

}

void when_receive_message(unsigned int node, char* message){
        //Para debug
        printf("Nova mensagem do no %d: %s\n", node, message);

	//Mensagem que indica a conclusão do nó filho
	if(strcmp(message, "Ramo concluido") == 0){
		ramos_concluidos++;

		//Se todos os filho já estiverem com os ramos concluídos
		if(ramos_concluidos == total_de_estacoes){
					
			ramo_concluido = 1;

			//Para os outros nós
			if(ID != 1){

				//Se reconecta ao nó pai

				initialise_wifi_sta();
			}
		}
	}

	//Mensagem pedindo o valor do sensor
	else if(strcmp(message, "Sensor request") == 0){

		//Se for um nó folha responde a mensagem somente com o próprio valor
		if(total_de_estacoes == 0){
			sprintf(mensagem, "%d:%d\n", ID, VIRTUALSENSOR);
			send_message(mensagem, AP_NODE);
		}

		//Senão reinicia com AP para reenviar as mensagens
		else{
			//Entra no estado de requisição
			requesting = 1;
			//Reinicia a variavel de ramos concluidos para fazer a contagem das respostas
			ramos_concluidos = 0;
			//Inicia a mensagem com o próprio valor
			sprintf(mensagem, "%d:%d\n", ID, VIRTUALSENSOR);
			initialise_wifi_ap(ssid_proprio, "eventinterface");
		}
	}

	//Outra mensagem, no caso do programa, com o valor do sensor
	else{
		ramos_concluidos++;
		
		//Para debug
		//printf("ramos concluidos: %d\n",ramos_concluidos);

		//Armazena o valor recebido na mensagem de resposta a ser enviada
		strcat(mensagem,message);

		//Se todos os filhos já tiverem respondido
		if(ramos_concluidos == total_de_estacoes){
			
			//Se for base station, mostra os valores
			if(ID == 1)
				printf("ID:SENSOR:\n%d:%d\n%s\n", ID, VIRTUALSENSOR, mensagem);

			//Se não for, se reconecta ao pai para repassar os valores
			else
				initialise_wifi_sta();

			//Limpa a string de mensagem
			strcpy(mensagem,"");
		}
	}
}

/*******************************************************************************************************/
