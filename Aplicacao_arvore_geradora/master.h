/*** Projeto de conlusão de curso de Engenharia de Computação - PUC Rio ***/
/*** Autor: Miguel Baldez Boing ***/

#include "interface.h"

void when_system_starts(void);

void when_alarm_fired(unsigned int alarm_id);

void when_interrupt_emitted(unsigned int pin, int signal_edge);

void when_wifi_starts(int ap);

void when_wifi_connects();

void when_a_station_connects(unsigned int station_connected);

void when_a_station_disconnects(unsigned int station_disconnected);

void when_receive_message(unsigned int node, char* message);
