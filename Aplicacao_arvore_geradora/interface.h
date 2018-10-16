/*** Projeto de conlusão de curso de Engenharia de Computação - PUC Rio ***/
/*** Autor: Miguel Baldez Boing ***/
/*** Atualizações mantidas em: https://github.com/mbboing/projeto_final ***/

//Define o máximo de estacoes que um AP pode ter
#define MAX_STATIONS 10

//Constante para o envio de mensagem para o AP
#define AP_NODE (MAX_STATIONS+1)

unsigned int create_alarm(unsigned int time_in_ms, int repeat);

void remove_alarm(unsigned int alarm_id);

int define_interrupt(int pin);

void initialise_wifi_ap(char * ssid, char * password);

void initialise_wifi_sta();

void wifi_connect(char * ssid, char * password);

unsigned int scan_wifi(char (*ssids)[30], unsigned int ssids_qnt);

void send_message(char message[64], unsigned int to_node);
