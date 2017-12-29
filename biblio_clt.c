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


void demandePseudo(char* pseudo) {
	printf("Veuillez-saisir votre pseudo \n");
	scanf("%s",pseudo);
}

/*Etablit une connexion avec le serveur central*/
void connexionServeurCentral(int* sDialogue){
	
	//Sa prend la valeur du canal (Mode TCP)
	CHECK(*sDialogue=socket(AF_INET,SOCK_STREAM,0),"probleme creation socket");  
	
	//Preparation de l'adressage serveur
	struct sockaddr_in svc;
	svc.sin_family=AF_INET;
	svc.sin_port=htons(PORT_SRV);  
	svc.sin_addr.s_addr=inet_addr(IP_SRV);
	memset(&svc.sin_zero,0,8);

	//Etablissement connexion serveur
	CHECK(connect(*sDialogue,(struct sockaddr *)&svc,sizeof(svc)),"Erreur connect");	
}





//Traite les requêtes d'identification
void identification(int* sa,int* mode, char* pseudo)
{

	char req[MAX_BUFF]; //requete
	char rep[MAX_BUFF]; //stockage réponse
	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete
	
	char element[MAX_BUFF]; //stockage element i
	int repId = 0; //stockage Id reponse
	
	//Dmd connexion 
	sprintf(req,"%i\\%s\\%i",110,pseudo,*mode); 
	CHECK(write(*sa,req,strlen(req)+1),"Erreur Envoi Requete");

	//Reception reponse
	CHECK(read(*sa,rep,MAX_BUFF),"Erreur Reception Reponse");

	//Reccupération id reponse
	sscanf(rep,"%i",&repId);
	
	
	while(repId != 0){
	
		memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete
		printf("Repid : %d \n", repId);
	
		switch(repId) {

			case 0 : //Fin comm
				printf("Communication terminée \n");
			break; 

			case 210 :
					printf("Pseudo OK\n");
					repId=0;
			break;

			case 310 : 
			{
				afficherMenu(mode);
				demandePseudo(pseudo);
				//Renvoyer les données
				memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
				sprintf(req,"%i\\%s\\%i",110,pseudo,*mode); 
				CHECK(write(*sa,req,strlen(req)+1),"Erreur Envoi Requete");
				
			}
			break;	
		}

	}
	
	return;
} 


//Traite les reqûetes quand mode Tchat (mode 1 ou 2)
void modeTchat(int* sa){
			
	char req[MAX_BUFF]; //requete
	char rep[MAX_BUFF]; //stockage réponse
	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete
	
	char element[MAX_BUFF]; //stockage element i
	int repId = 0; //stockage Id reponse
	char rep_decoupe[MAX_CLIENT+2][MAX_BUFF]; //contient la rep decoupe.
	char *result;
	
	//Dmd connexion 
	sprintf(req,"%i\\",120); 
	CHECK(write(*sa,req,strlen(req)+1),"Erreur Envoi Requete");

	
	//Reception reponse
	CHECK(read(*sa,rep,MAX_BUFF),"Erreur Reception Reponse");
	
	result=strtok(req,"\\");

	//Récuperer la commande en int
	repId = atoi(result);
	
	while(repId != 0){

		switch(repId) {

			case 0 : //Fin comm
				printf("Communication terminée \n");
			break; 

			case 220 :{
					
				int i=0,nbClient;
				
				printf("result 0 est %s",result);
				result = strtok( NULL, "\\");
				printf("result 1 est %s",result);
				result = strtok( NULL, "\\");
				printf("result 2 est %s",result);
/*
				while (result != NULL){
					
					if(i == 1)
					{
						nbClient=atoi(result);	
						char clientConnect[nbClient][MAX_BUFF];		
						printf("Il y'a %d client(s) connecté(s)\n",nbClient);  
					}
					
					if(i>1)
					{
					
						strcpy(clientConnect[i-2],result);
						printf("* %d : %s \n",clientConnect[i-2]); 
					
					}
												
					result = strtok( NULL, "\\");
					i++;
				}	
		*/
					
			break;
			}

			case 320 : 
			{
				printf("Echec lors de la réception de la liste des clients\n");
			}
			break;	
		}

	}

}


