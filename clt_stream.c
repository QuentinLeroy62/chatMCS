#include "biblio_clt.h"


int main(){

char pseudo[MAX_PSEUDO];
int mode, sDialogueServeur;
	
	afficherMenu(&mode);

	printf("Veuillez-saisir votre pseudo \n");
	scanf("%s",pseudo);
	
	connexionServeurCentral(&sDialogueServeur);

	dialogueServeurCentral(&sDialogueServeur,&mode,pseudo);	
	
	close(sDialogueServeur); //Fermeture socket dialogue

	return 0;
}


