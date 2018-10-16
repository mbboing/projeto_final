/*** Projeto de conlusão de curso de Engenharia de Computação - PUC Rio ***/
/*** Autor: Miguel Baldez Boing ***/

/************************************ Codigo para testes da interface**********************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "interface.h"

int alarm_test, alarm_message;

void when_system_starts(void){

	printf("System starts.\n");

	define_interrupt(12);

	//create_alarm(5000,1);
	
	//create_alarm(4000,0);

	//initialise_wifi_ap("ESP32", "eventinterface");

	//initialise_wifi_sta();

	//alarm_test = create_alarm(10000,1);

}

void when_alarm_fired(unsigned int alarm_id){

	if(alarm_id == alarm_test)
		printf("Main task is running.\n");
	//else if(alarm_id == alarm_message)
		//send_message("Ola ESP, hoje é 25/07");

}

void when_interrupt_emitted(unsigned int pin, int signal_edge){

	ets_printf("\nInterrupt of pin %d emitted.\n", pin);

}

void when_wifi_starts(int ap){

	
	char ssids[30][30];
	unsigned int ssids_totals, i;

	printf("Wifi started\n");

	if(ap){
		
	}
	else{
		ssids_totals = scan_wifi(ssids, 30);

		printf("Wifi encontradas:\n");
		for(i=0; i<ssids_totals; i++)
			printf("%s\n", ssids[i]);

		wifi_connect("ESP32", "eventinterface");
		alarm_message = create_alarm(5000,1);
	}

}

void when_a_station_connects(unsigned int station_connected){

}

void when_a_station_disconnects(unsigned int station_disconnected){

}

void when_wifi_connects(){

	printf("wifi connected!\n");

}

void when_receive_message(char* message, unsigned int size){

	printf("%s\n",message);

}

/*******************************************************************************************************/
