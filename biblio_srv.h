#include "stream.h"


typedef struct{
	char pseudo[MAX_CLIENT]; //le pseudo
	int mode;	//le mode 1:privé 2:public 3:spectateur 4:archive
	int etat;  //etat 0: libre  etat 1: occupé
	struct sockaddr_in socketClient; //contient la socket du client
} client;

typedef struct {
	char nomDebat[MAX_BUFF];
	struct sockaddr_in socketCltServeur;	
} debat;

//VARIABLES 
extern int nbClient;
extern client ensClient[MAX_CLIENT];

extern int nbDebatEnCours;
extern debat ensDebat[MAX_DEBAT];

//MUTEX
extern pthread_mutex_t  mutex_ensembleClient;

//PROTOTYPES
void * traitementThread (void*);
void dialogueClient(int*, char*);


