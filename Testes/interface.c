/*** Projeto de conlusão de curso de Engenharia de Computação - PUC Rio ***/
/*** Autor: Miguel Baldez Boing ***/

#ifndef EVENT_INTERFACE_H
#define EVENT_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "nvs_flash.h"

#include "master.h"
#include "interface.h"

//Definicoes de constantes para configurar o wifi
#define TCPServerIP "192.168.1.1"
#define AP_SSID_HIDDEN 0
#define MAX_CONNECTIONS 4
#define AP_AUTHMODE WIFI_AUTH_WPA2_PSK
#define AP_BEACON_INTERVAL 100
#define LISTENQ 2

//Define o máximo de estacoes que um AP pode ter
//#define MAX_STATIONS 10

//Constante para o envio de mensagem para o AP
//#define AP_NODE (MAX_STATIONS+1)

//Estrutura de lista de alarmes
typedef struct alarm_node {
    unsigned int id;
	TimerHandle_t handle;
	int repeat;
    struct alarm_node * next;
} Alarm_node;

//Estrutura de id de uma estação
typedef struct sta_id {
    uint8_t mac[6];
	int connected;
	int socket;
}Sta_id;

//Lista de estacoes
Sta_id stations_list[MAX_STATIONS];

//Variável de controle da posicao da lista de estacoes
unsigned int next_id = 0;

//Lista de alarmes
struct alarm_node * alarm_list = NULL;

//Variavel de controle das ids dos alarmes
unsigned int alarm_ids = 1;

//Sockets para a comunicacao TCP
int client_socket;
int server_socket;

//Funcao para debug
void print_lista(){
	Alarm_node * current = alarm_list;
	printf("Inicio\n");
    while (current != NULL) {
		printf("%d\n",current->id);		
        current = current->next;
	}
	printf("Fim\n");
}


//Função de retorno definida para o timer
void tm_clbk(TimerHandle_t tm_hndl){

	Alarm_node * current = alarm_list;
	Alarm_node * last = NULL;
	unsigned int id = 0;

        //Para debug
	//print_lista();

	//Itera na lista de alarmes
    while (current != NULL) {

		//Quando encontrar o alarme desejado
		if(current->handle == tm_hndl){

			//Pega o id
			id = current->id;

			//Se o alarme nao for para repetir
			if(current->repeat == 0){

				//Remove o alarme da lista
				if(last == NULL){
					alarm_list = current->next;
				}
				else{
					last->next = current->next;
				}
				free(current);
			}

			break;
		}

		last = current;
        current = current->next;
    }

	//Se o alarme foi encontrado, chama a callback com o id
	if(id)
		when_alarm_fired(id);

}

//Funcao que cria o alarme
unsigned int create_alarm(unsigned int time_in_ms, int repeat){

    char tm_name[11];
    int ret;
	Alarm_node * new_alarm = NULL;

	sprintf(tm_name, "%d", alarm_ids);

	//Cria um novo alarme
	new_alarm = malloc(sizeof(Alarm_node));

	if(new_alarm == NULL)
		return 0;
	new_alarm->id = alarm_ids;
	new_alarm->repeat = repeat;
	if(repeat)
    	new_alarm->handle =  xTimerCreate(tm_name, time_in_ms/portTICK_PERIOD_MS, pdTRUE, (void *) 0, tm_clbk);
	else
    	new_alarm->handle =  xTimerCreate(tm_name, time_in_ms/portTICK_PERIOD_MS, pdFALSE, (void *) 0, tm_clbk);

	//Testa se foi possivel criar
    if(new_alarm->handle == NULL)
        return 0;
	else {
        ret = xTimerStart(new_alarm->handle, 0);
        if(ret != pdPASS)
            return 0;
    }
	
	//Adiciona o alarme na lista de alarmes
	new_alarm->next = alarm_list;
	alarm_list = new_alarm;

	//Atualiza a variavel global que controla os ids
	alarm_ids++;


	//print_lista();

	return new_alarm->id;

	return 0;

}

//Funcao que remove um alarme
void remove_alarm(unsigned int alarm_id){

	Alarm_node * current = alarm_list;
	Alarm_node * last = NULL;
	TimerHandle_t tm_hndl;

	//Itera na lista de alarmes
    while (current != NULL) {

		//Quando encontrar o alarme desejado
		if(current->id == alarm_id){

			//Pega o handle
			tm_hndl = current->handle;

			//Remove o alarme da lista
			if(last == NULL)
				alarm_list = current->next;
			else
				last->next = current->next;
			free(current);

			//Desliga o timer
			xTimerDelete(tm_hndl,0);

			break;
		}
		last = current;
        current = current->next;
    }

}

//Funcoes de callback de acordo com o pino
void int_clbk_0(){
		when_interrupt_emitted(0, gpio_get_level(GPIO_NUM_0));
}

void int_clbk_2(){
		when_interrupt_emitted(2, gpio_get_level(GPIO_NUM_2));
}

void int_clbk_4(){
		when_interrupt_emitted(4, gpio_get_level(GPIO_NUM_4));
}

void int_clbk_5(){
		when_interrupt_emitted(5, gpio_get_level(GPIO_NUM_5));
}

void int_clbk_12(){
		when_interrupt_emitted(12, gpio_get_level(GPIO_NUM_12));
}

void int_clbk_13(){
		when_interrupt_emitted(13, gpio_get_level(GPIO_NUM_13));
}

void int_clbk_14(){
		when_interrupt_emitted(14, gpio_get_level(GPIO_NUM_14));
}

void int_clbk_15(){
		when_interrupt_emitted(15, gpio_get_level(GPIO_NUM_15));
}

void int_clbk_16(){
		when_interrupt_emitted(16, gpio_get_level(GPIO_NUM_16));
}

void int_clbk_17(){
		when_interrupt_emitted(17, gpio_get_level(GPIO_NUM_17));
}

void int_clbk_18(){
		when_interrupt_emitted(18, gpio_get_level(GPIO_NUM_18));
}

void int_clbk_19(){
		when_interrupt_emitted(19, gpio_get_level(GPIO_NUM_19));
}

void int_clbk_21(){
		when_interrupt_emitted(21, gpio_get_level(GPIO_NUM_21));	
}

void int_clbk_22(){
		when_interrupt_emitted(22, gpio_get_level(GPIO_NUM_22));
}

void int_clbk_23(){
		when_interrupt_emitted(23, gpio_get_level(GPIO_NUM_23));
}

void int_clbk_25(){
		when_interrupt_emitted(25, gpio_get_level(GPIO_NUM_25));
}

void int_clbk_26(){
		when_interrupt_emitted(26, gpio_get_level(GPIO_NUM_26));
}

void int_clbk_27(){
		when_interrupt_emitted(27, gpio_get_level(GPIO_NUM_27));
}

void int_clbk_32(){
		when_interrupt_emitted(32, gpio_get_level(GPIO_NUM_32));
}

void int_clbk_33(){
		when_interrupt_emitted(33, gpio_get_level(GPIO_NUM_33));
}

void int_clbk_34(){
		when_interrupt_emitted(34, gpio_get_level(GPIO_NUM_34));
}

void int_clbk_35(){
		when_interrupt_emitted(35, gpio_get_level(GPIO_NUM_35));
}

//Funcao que tranforma um inteiro na constante do pino correspondente
uint64_t get_gpio_bit_mask(unsigned int pin){

	uint64_t gpios[36] = {GPIO_SEL_0,
						0,
						GPIO_SEL_2,
						0,
						GPIO_SEL_4,
						GPIO_SEL_5,
						0,
						0,
						0,
						0,
						0,
						0,
						GPIO_SEL_12,
						GPIO_SEL_13,
						GPIO_SEL_14,
						GPIO_SEL_15,
						GPIO_SEL_16,
						GPIO_SEL_17,
						GPIO_SEL_18,
						GPIO_SEL_19,
						0,
						GPIO_SEL_21,
						GPIO_SEL_22,
						GPIO_SEL_23,
						0,
						GPIO_SEL_25,
						GPIO_SEL_26,
						GPIO_SEL_27,
						0,
						0,
						0,
						0,
						GPIO_SEL_32,
						GPIO_SEL_33,
						GPIO_SEL_34,
						GPIO_SEL_35};
	if (pin > 35)
		return 0;
	else
		return gpios[pin];
	
}

int define_interrupt(int pin){

	gpio_config_t gpioConfig;

	//Retorna 0 se o pino for inválido
	if(get_gpio_bit_mask(pin) == 0) return 0;

	//Configura o pino escolhido
	gpioConfig.pin_bit_mask = get_gpio_bit_mask(pin);
	gpioConfig.mode         = GPIO_MODE_INPUT;
	gpioConfig.pull_up_en   = GPIO_PULLUP_ENABLE;
	gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
	gpioConfig.intr_type    = GPIO_INTR_POSEDGE;
	gpio_config(&gpioConfig);

	//Define a função de callback da interrupção de acordo com o pino
	if(pin == 0)  gpio_isr_handler_add(pin, int_clbk_0, NULL);
	else if(pin == 2)  gpio_isr_handler_add(pin, int_clbk_2, NULL);
	else if(pin == 4)  gpio_isr_handler_add(pin, int_clbk_4, NULL);
	else if(pin == 5)  gpio_isr_handler_add(pin, int_clbk_5, NULL);
	else if(pin == 12)  gpio_isr_handler_add(pin, int_clbk_12, NULL);
	else if(pin == 13)  gpio_isr_handler_add(pin, int_clbk_13, NULL);
	else if(pin == 14)  gpio_isr_handler_add(pin, int_clbk_14, NULL);
	else if(pin == 15)  gpio_isr_handler_add(pin, int_clbk_15, NULL);
	else if(pin == 16)  gpio_isr_handler_add(pin, int_clbk_16, NULL);
	else if(pin == 17)  gpio_isr_handler_add(pin, int_clbk_17, NULL);
	else if(pin == 18)  gpio_isr_handler_add(pin, int_clbk_18, NULL);
	else if(pin == 19)  gpio_isr_handler_add(pin, int_clbk_19, NULL);
	else if(pin == 21)  gpio_isr_handler_add(pin, int_clbk_21, NULL);
	else if(pin == 22)  gpio_isr_handler_add(pin, int_clbk_22, NULL);
	else if(pin == 23)  gpio_isr_handler_add(pin, int_clbk_23, NULL);
	else if(pin == 25)  gpio_isr_handler_add(pin, int_clbk_25, NULL);
	else if(pin == 26)  gpio_isr_handler_add(pin, int_clbk_26, NULL);
	else if(pin == 27)  gpio_isr_handler_add(pin, int_clbk_27, NULL);
	else if(pin == 32)  gpio_isr_handler_add(pin, int_clbk_32, NULL);
	else if(pin == 33)  gpio_isr_handler_add(pin, int_clbk_33, NULL);
	else if(pin == 34)  gpio_isr_handler_add(pin, int_clbk_34, NULL);
	else if(pin == 35)  gpio_isr_handler_add(pin, int_clbk_35, NULL);
	
	
	//Retorna 1 indicando sucesso
	return 1;

}


void add_station(int new_socket){
	int i, j;
	int mac_found = 0;
	wifi_sta_list_t	stations;

	if(esp_wifi_ap_get_sta_list(&stations) == ESP_OK){

        //Para debug
        //printf("stations num:%d\n",stations.num);

		//Para cada estação conectada
		for(i=0; i<stations.num; i++){
	
			mac_found = 0;

			//Verifica se o mac já está na lista global
			for(j=0; j<next_id; j++){
				if(strcmp((char*) stations_list[j].mac, (char*) stations.sta[i].mac) == 0){
					mac_found = 1;

					//Se estiver desconectado, reconecta
					if(stations_list[j].connected == 0){
						stations_list[j].connected = 1;
						stations_list[j].socket = new_socket;
						when_a_station_connects(j);
						return;
					}
					break;
				}
			}

			//Se não tiver, o adiciona
			if(!mac_found){
				//bcopy((char *)&afds, (char*)&rfds, sizeof(afds));
                                //printf("%s\n",(char*) stations.sta[i].mac);


				strcpy((char*) stations_list[next_id].mac, (char*) stations.sta[i].mac);
				stations_list[next_id].connected = 1;
				next_id++;
				when_a_station_connects(next_id - 1);
				return;
			}
		}
	}

}

void remove_station(){
	int i, j;
	wifi_sta_list_t	stations;

        //Para debug
        //printf("Removing station.. \n");

	if(esp_wifi_ap_get_sta_list(&stations) == ESP_OK){

		//Busca o mac que está na lista global, mas não está nas conexoes
		for(i=0; i<next_id; i++){
			//Somente se a estacao estiver com o flag de conectada
			if(stations_list[i].connected){
				int mac_found = 0;
				for(j=0; j<stations.num; j++){
					if(strcmp((char*) stations_list[i].mac, (char*) stations.sta[j].mac) == 0){
						mac_found = 1;
					}
				}
				if(!mac_found){

					stations_list[i].connected = 0;
					close(stations_list[i].socket);
					when_a_station_disconnects(i);
					break;
				}
			}
		}
	}
}

//Funcao de conectar o wifi na rede especificada
void wifi_connect(char * ssid, char * password){
	wifi_mode_t modo;

	//Testa se o modo é station
	if(esp_wifi_get_mode(&modo) == ESP_OK){
		if(modo == WIFI_MODE_STA || modo == WIFI_MODE_APSTA){

			wifi_config_t cfg = {
				.sta = {
					.scan_method = WIFI_ALL_CHANNEL_SCAN,
					.bssid_set = 0,
					.channel = 0,		
				},
			};

			strcpy((char*) cfg.sta.ssid, ssid);
			strcpy((char*) cfg.sta.password, password);

                        //Para debug
			//printf("Connecting to %s %s\n", (char*) cfg.sta.ssid, (char*) cfg.sta.password);

			esp_wifi_disconnect();
			esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg);
			esp_wifi_connect();
		}
	}
}

void tcp_client(void *pvParam){
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = inet_addr(TCPServerIP);
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons( 3000 );
    char recv_buf[64];	//Buffer de leitura
    int r;				//Tamanho do buffer
    while(1){

		//Cria o socket
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(client_socket < 0) {
            //Para debug
            //printf("... Failed to allocate socket.\n");
            continue;
        }
        printf("... allocated socket\n");

		//Conecta com o servidor
        if(connect(client_socket, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
            //Para debug
            //printf("... socket connect failed errno=%d \n", errno);
            close(client_socket);
            continue;
        }
        printf("... connected \n");

		//Pequeno delay para garantir que a estação seja adicionada a lista do AP
		vTaskDelay(500 / portTICK_PERIOD_MS);

        when_wifi_connects();

		while(1){
			//Se ler alguma mensagem, chama a callback when_receive_message
		    bzero(recv_buf, sizeof(recv_buf));
		    r = read(client_socket, recv_buf, sizeof(recv_buf)-1);
			if(r > 0)
				when_receive_message(AP_NODE, recv_buf);
		
		
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
	}
}

void tcp_server(void *pvParam){
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons( 3000 );
    static struct sockaddr_in remote_addr;
    static unsigned int socklen;
    socklen = sizeof(remote_addr);
    char recv_buf[64];	//Buffer de leitura
	fd_set rfds; //read file descriptor set
	fd_set afds; //active file descriptor set
	int fd;
	int select_ret;
	int maxfd;
    while(1){

		//Cria o socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socket < 0) {
            //Para debug
            //printf("... Failed to allocate server socket.\n");
            continue;
        }
        printf("... allocated server socket\n");

		//Conecta o socket ao cliente
         if(bind(server_socket, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
            //Para debug
            //printf("... server socket bind failed errno=%d \n", errno);
            close(server_socket);
            continue;
        }
        printf("... server socket bind done \n");
        if(listen (server_socket, LISTENQ) != 0) {
            //Para debug
            //printf("... server socket listen failed errno=%d \n", errno);
            close(server_socket);
            continue;
        }
	
		FD_ZERO(&afds);
		FD_SET(server_socket, &afds);
		maxfd = server_socket;
	
        while(1){

			bcopy((char *)&afds, (char*)&rfds, sizeof(afds));
			//select(FD_SETSIZE, &rfds, (fd_set*) 0, (fd_set*) 0, (struct timeval *) 0);
			select_ret = select(maxfd + 1, &rfds, NULL, NULL, NULL);
			for (fd = 0; fd <= maxfd; fd++){
		        if (FD_ISSET(fd,&rfds)){ 

                        //Para debug
                        //printf("Select: %d\n",fd);

		            if (fd == server_socket){
		                //new connection
		                int newfd = accept(server_socket,(struct sockaddr *)&remote_addr, &socklen);
		                FD_SET(newfd,&afds);
						if(newfd > maxfd)
							maxfd = newfd;

						add_station(newfd);
		            }
					else{
						fcntl(fd,F_SETFL,O_NONBLOCK);
					    bzero(recv_buf, sizeof(recv_buf));
					    recv(fd, recv_buf, sizeof(recv_buf)-1,0);
						for(int i=0; i<next_id; i++)
							if(stations_list[i].socket == fd)
								when_receive_message(i, recv_buf);
		            }
		        }
			}
        }
	
    }
}

//Funcao de tratamento do Event Loop
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    static struct sockaddr_in remote_addr;
    static unsigned int socklen;
    socklen = sizeof(remote_addr);

    switch(event->event_id) {
    	
    //Quando o modo sation for iniciado
    case SYSTEM_EVENT_STA_START:
        when_wifi_starts(0);
        break;
    
    //Quando tiver obtido o IP em modo station
    case SYSTEM_EVENT_STA_GOT_IP:
		xTaskCreate(&tcp_client,"tcp_client",4048,NULL,5,NULL);
        break;
        
	//Quando o wifi desconectar
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        break;
        
    //Quando o modo AP for iniciado
	case SYSTEM_EVENT_AP_START:
		xTaskCreate(&tcp_server,"tcp_server",4096,NULL,5,NULL);
        when_wifi_starts(1);
        break;
        
    //Quando uma estacao se conectar
	case SYSTEM_EVENT_AP_STACONNECTED:

                //Para debug
		//printf("STACONN\n");
		
		break;
		
	//Quando uma estacao se desconectar
	case SYSTEM_EVENT_AP_STADISCONNECTED:

                //Para debug
		//printf("STADISCONN\n");
		
		remove_station();

		break;
    default:
        break;
    }
    return ESP_OK;
}

//Inicia o DHCP para acionar o modo AP
static void start_dhcp_server(){
    
    	// Inicializa o TCPIP
	    tcpip_adapter_init();
        // Desliga o DHCP
        tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
        // Define o IP da rede
        tcpip_adapter_ip_info_t info;
        memset(&info, 0, sizeof(info));
        IP4_ADDR(&info.ip, 192, 168, 1, 1);
        IP4_ADDR(&info.gw, 192, 168, 1, 1);
        IP4_ADDR(&info.netmask, 255, 255, 255, 0);
        tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info);
        // Inicia o DHCP  
        tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);
        printf("DHCP server started \n");
}

//Funcao que inicia o wifi
void initialise_wifi_ap(char * ssid, char * password)
{
	int i,j;

	esp_wifi_stop();

	//Desliga o log do wifi
    esp_log_level_set("wifi", ESP_LOG_NONE);
	
	start_dhcp_server();

	//Configuracao padrao
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);
	
	//Modo AP
	esp_wifi_set_mode(WIFI_MODE_AP);

	// Configura de acordo com as definicoes
	wifi_config_t ap_config = {
	    .ap = {
			.ssid_len = 0,
			.channel = 0,
			.authmode = AP_AUTHMODE,
			.ssid_hidden = AP_SSID_HIDDEN,
			.max_connection = MAX_CONNECTIONS,
			.beacon_interval = AP_BEACON_INTERVAL,			
	    },
	};
	strcpy((char*) ap_config.ap.ssid, ssid);
	strcpy((char*) ap_config.ap.password, password);
	esp_wifi_set_config(WIFI_IF_AP, &ap_config);

        //Para debug
	//printf("WiFi iniciado em modo AP \n");

	//Zera as estações conectadas
	for(i=0; i<MAX_STATIONS; i++){
		for(j=0; j<6; j++)	{
			stations_list[i].mac[j] = 0x00;
			stations_list[i].connected = 0;
		}
	}
	
	//Inicia wifi
	esp_wifi_start();
}

void initialise_wifi_sta()
{
	esp_wifi_stop();

	//Desliga o log do wifi
    esp_log_level_set("wifi", ESP_LOG_NONE);
    
	tcpip_adapter_init();
	
	//Configuracao padrao
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	
	//Modo STA
	esp_wifi_set_mode(WIFI_MODE_STA);
	//printf("WiFi iniciado em modo STA \n");
	
	//Inicia wifi
	esp_wifi_start();
}

unsigned int scan_wifi(char (*ssids)[30], unsigned int ssids_qnt){

	wifi_scan_config_t config;
	uint16_t ap_qnt = ssids_qnt;
	int i;

	wifi_ap_record_t* aps = malloc(ssids_qnt * sizeof(wifi_ap_record_t));
	if(aps == NULL)
		return 0;

	config.ssid = NULL;
	config.bssid = NULL;
	config.channel = 0;
	config.show_hidden = 0;
	config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
	config.scan_time.active.min = 0;
	config.scan_time.active.max = 0;

	if(esp_wifi_scan_start(&config, 1) == ESP_OK){

		if(esp_wifi_scan_get_ap_records(&ap_qnt, aps) == ESP_OK){

			for(i = 0; i < ap_qnt; i++){
				strcpy(ssids[i],(char*)aps[i].ssid);
			}

			free(aps);
			return ap_qnt;

		}

	}

	free(aps);
	return 0;
}

void send_message(char message[64], unsigned int to_node){

	if(to_node == AP_NODE){
		write(client_socket , message , strlen(message));

                //Para debug
		//if( write(client_socket , message , strlen(message)) < 0)
		//	printf("... Send failed \n");
		//else
		//	printf("... socket sent \n");
	}
	else{
		write(stations_list[to_node].socket , message , strlen(message));
	}

}

//Função main
void app_main()
{

	//Instala o serviço de interrupção dos pinos
	gpio_install_isr_service(0);	

	//Inicia o event loop para obter os eventos do modulo de wifi
    esp_event_loop_init(event_handler, NULL);
    
    //Inicia a memoria nao volatil, necessaria para usar o wifi
    nvs_flash_init();

	//Chama a funcao inicial do usuario
	when_system_starts();

}

#endif /* EVENT_INTERFACE_H */
