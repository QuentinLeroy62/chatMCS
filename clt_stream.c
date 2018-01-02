#include "biblio_clt.h"

char pseudo[MAX_TAILLE_PSEUDO];
int sDialogueServeur;

int main(){

int mode; 

//Pour le mode serveur
int se;
struct sockaddr_in svcClt;


	afficherMenu(&mode);
	demandePseudo(pseudo);

	
	creationSocketEcoute(&se,&svcClt);

	connexionServeurCentral(&sDialogueServeur);

	identification(&sDialogueServeur,&mode,pseudo,&svcClt);	
	
	switchMode(&sDialogueServeur, mode, pseudo);
	
	close(sDialogueServeur); //Fermeture socket dialogue

	return 0;
}


