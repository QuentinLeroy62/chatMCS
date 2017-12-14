#include "stream.h"

#define MAX_CLIENT 50

typedef struct{
	char pseudo[MAX_PSEUDO]; //le pseudo
	int mode;	//le mode
	int etat;  //etat 0: libre  etat 1: occupé
	struct sockaddr_in socketClient; //contient la socket du client
} client;


//PROTOTYPES
void dialogueClient(int,int*,client*);
void derouter(int);
