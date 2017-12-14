#include "stream.h"

#define MAX_CLIENT 50

struct client{
	char pseudo[MAX_PSEUDO]; //le pseudo
	int mode;	//le mode
	int etat;  //etat 0: libre  etat 1: occupé
	struct sockaddr_in; //contient la socket du client
}


//PROTOTYPES
void dialogueClient(int);
void derouter(int);
