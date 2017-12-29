#include "biblio_clt.h"


int main(){

char pseudo[MAX_CLIENT];

int mode, sDialogueServeur;
	
	afficherMenu(&mode);
	demandePseudo(pseudo);
	
	connexionServeurCentral(&sDialogueServeur);

	identification(&sDialogueServeur,&mode,pseudo);	
	
	switch(mode)
	{
		case (1) : 
				modeTchat(&sDialogueServeur);		
		break;
	
	
	
	}
	
	close(sDialogueServeur); //Fermeture socket dialogue

	return 0;
}


