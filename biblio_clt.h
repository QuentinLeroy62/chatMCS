#include "stream.h"



//PROTOTYPES
void afficherMenu(int*);
void connexionServeurCentral(int *);
void identification(int*,int*,char*,struct sockaddr_in*);
void demandePseudo(char*);
void modeTchat(int*,char*);
void creationSocketEcoute(int*,struct sockaddr_in*);
void * EcouteClient(void*);
void * traitementThreadClient(void*);
void switchMode(int*,int,char*);
void dialogueClient(char*,int,char*);


