#include "biblio_clt.h"

/////MODE CLIENT-SERVEUR///////

/*Creation d'une socket Ecoute pour le client*/
void creationSocketEcoute(int* se,struct sockaddr_in* svc){

	pthread_t thService; 
	void ** resultat; 

	//////////Adressage du Client - serveur////////////
	CHECK(*se=socket(AF_INET,SOCK_STREAM,0),"probleme creation socket Ecoute");
	
	int portAlea = (rand()%(65535-6501)+6501);	

	//preparation de l'adressage
	svc->sin_family=AF_INET;
	svc->sin_port=0; //prend un port libre aléatoire  
	svc->sin_addr.s_addr=inet_addr(IP_SRV);
	memset(svc->sin_zero,0,8);

	CHECK(bind(*se,(struct sockaddr*)svc,sizeof(*svc)),"test associer adr a la socket Ecoute");

	//Recupération du port et de l'adresse de la socke écoute
	struct sockaddr_in socketRecup;
	int tailleStruct = sizeof(socketRecup);
	getsockname(*se,(struct sockaddr*)&socketRecup,&tailleStruct);
	
	svc->sin_addr.s_addr=socketRecup.sin_addr.s_addr;
	svc->sin_port=socketRecup.sin_port;

 
	//CONFIGURATION DU CLIENT SEVEUR EN ECOUTE
	listen(*se,20); //20 en backlog (20 connection gardée en mémoire max si occupé) 
	
	//Creation du thread de service
	CHECK(pthread_create(&thService,NULL,EcouteClient,(void*)se),"Pb creation thread");

	//Detachement du thread de service
	CHECK(pthread_detach(thService),"Pb detachement thread");

	return;
}


/*Thread qui écoute en continu la socket du Client serveur*/
void * EcouteClient(void* argSe){

	//Cast l'argument en int*
	int* se = (int*) argSe;

	//Socket Dialogue
	int sd;

	//Socket Client 
	struct sockaddr_in client; 
	socklen_t lenSd;

	pthread_t thService; 
	void ** resultat; 

	while(1){			
		
		//Accepte la connexion et création socket clienet
		lenSd = sizeof(client);

		//CHECK(sd=accept(*se,(struct sockaddr*)&client,&lenSd),"Erreur Accept Ecoute");

		//printf("Accepted connection from %s:%d\n", inet_ntoa(dialogue.sin_addr), ntohs(dialogue.sin_port));

		//Creation du thread de service
	  	CHECK(pthread_create(&thService,NULL,traitementThreadClient,(void*)&sd),"Pb creation thread");
		
		//Detachement du thread de service
		CHECK(pthread_detach(thService),"Pb detachement thread");
	}
	
	//fermer la socket d'écoute et le thread
	close(*se);
	pthread_exit(0);
}

/*Thread qui traite la conenxion du client*/
void * traitementThreadClient(void* argSd){

	//printf("Traitement du thread Client\n");
	pthread_exit(0);
}


//////MODE CLIENT////////

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

/*Demande le pseudo au moment de la connexion*/
void demandePseudo(char* pseudo) {
	printf("Veuillez-saisir votre pseudo \n");
	scanf("%s",pseudo);
}

/*Etablit une connexion avec le serveur central*/
void connexionServeurCentral(int* sDialogue){
	
	//Creation de la socket de dialogue
	CHECK(*sDialogue=socket(AF_INET,SOCK_STREAM,0),"probleme creation socket Dialogue Serveur Central");
	
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
void identification(int* sa,int* mode, char* pseudo, struct sockaddr_in* svcClt)
{

	char req[MAX_BUFF]; //requete
	char rep[MAX_BUFF]; //stockage réponse
	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete
	
	char element[MAX_BUFF]; //stockage element i
	int repId = 0; //stockage Id reponse
	
	//Dmd connexion 
	sprintf(req,"%i\\%s\\%i\\%i\\%i",110,pseudo,svcClt->sin_addr.s_addr,svcClt->sin_port,*mode); 
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
				sprintf(req,"%i\\%s\\%i\\%i\\%i",110,pseudo,svcClt->sin_addr.s_addr,svcClt->sin_port,*mode); 
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


