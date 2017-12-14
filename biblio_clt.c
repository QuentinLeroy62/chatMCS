#include "biblio_clt.h"

/* Affiche le menu de l'app*/
void afficherMenu(int* choix){
	printf("Veuillez-choisir le mode d'utilisation :\n\n");
	printf("1: Tchat privé\n");
	printf("2: Tchat public\n");
	printf("3: Mode spectateur\n");
	printf("4: Consultation archive\n\n"); 
	printf("Votre choix : ");
	scanf("%d",choix);
}


/*Etablit une connexion avec le serveur central*/
void connexionServeurCentral(int* sDialogue){
	
	//Sa prend la valeur du canal (Mode TCP)
	CHECK(*sDialogue=socket(AF_INET,SOCK_STREAM,0),"probleme creation socket");  
//coucou
	
	//Preparation de l'adressage serveur
	struct sockaddr_in svc;
	svc.sin_family=AF_INET;
	svc.sin_port=htons(PORT_SRV);  
	svc.sin_addr.s_addr=inet_addr(IP_SRV);
	memset(&svc.sin_zero,0,8);

	//Etablissement connexion serveur
	CHECK(connect(*sDialogue,(struct sockaddr *)&svc,sizeof(svc)),"Erreur connect");	
}



/*Dialogue avec le serveurCentral*/
void dialogueServeurCentral(int* sDialogue, int* mode, char* pseudo)
{
	char req[MAX_BUFF]; //requete
	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	
	//Dmd connexion 
	sprintf(req,"%i\\%s\\%i",110,pseudo,*mode); 
/*	
	traiterReq(sa,req,&tailleN);
	
	//Attente choix du

	//Fin communication
	sprintf(req,"%i",0); 
	traiterReq(sa,req,&tailleN);
*/

}

/*
//Traite les requêtes
void traiterReq(int* sa,char* req, int* taille)
{
	char rep[MAX_BUFF]; //stockage réponse
	char element[MAX_BUFF]; //stockage element i
	int repId; //stockage Id reponse

	//Envoi requete
	CHECK(write(sa,req,strlen(req)+1),"Erreur Envoi Requete");
	
	//Reception reponse
	CHECK(read(sa,rep,MAX_BUFF),"Erreur Reception Reponse");

	//Reccupération id reponse
	sscanf(rep,"%i",&repId);
	
	switch(repId) {

		case 0 : //Fin comm
			printf("Communication terminée \n");
		break; 

		case 100 : //Affiche taille
			sscanf(rep,"%i:%i",&repId,taille);  //séparateur de champ :
			printf("Taille = %i \n", *taille);
		break;	

		case 200 : //Affiche elements
			sscanf(rep,"%i:%s",&repId,element);
			printf("%s \n",element);	 
		break;
		
		case 250 : //Affiche out of range
			printf("Depassement tableau \n");	
		break;
	
	}
}  
*/
