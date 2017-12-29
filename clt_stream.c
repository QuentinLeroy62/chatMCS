#include "biblio_clt.h"


int main(){


char pseudo[MAX_CLIENT];
int mode, sDialogueServeur;

//Pour le mode serveur
int se;
struct sockaddr_in svc;

	
	afficherMenu(&mode);
	demandePseudo(pseudo);

	creationSocketEcoute(&se,&svc);
	
	//connexionServeurCentral(&sDialogueServeur);

	//identification(&sDialogueServeur,&mode,pseudo);	
	
	switch(mode)
	{
		case (1) : 
				modeTchat(&sDialogueServeur);		
		break;
	
	
	
	}
	
	close(sDialogueServeur); //Fermeture socket dialogue

	return 0;
}


