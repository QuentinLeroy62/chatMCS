#include "stream.h"



//PROTOTYPES
void afficherMenu(int*);
void connexionServeurCentral(int *);
void identification(int*,int*,char*);
void demandePseudo(char*);
void modeTchat(int*);
void creationSocketEcoute(int*,struct sockaddr_in*);
void * EcouteClient(void*);
void * traitementThreadClient(void*);


