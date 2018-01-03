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
	socklen_t tailleStruct = sizeof(socketRecup);
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

		CHECK(sd=accept(*se,(struct sockaddr*)&client,&lenSd),"Erreur Accept Ecoute");

		printf("Accepted connection from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

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
	
	int* socketTchat = (int*) argSd; 

	char req[MAX_BUFF]; //stocke la requete
	char rep[MAX_BUFF]; //stocke la reponse
	char req_decoupe[5][MAX_BUFF];
	char pseudoDestinataire[MAX_TAILLE_PSEUDO];
	char nomDebat[MAX_BUFF];
	char nameFile[MAX_BUFF];
	char message[MAX_BUFF];

	FILE* archive = NULL;

	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de reponse

	int idCommande=0; //stocke l'id de la commande initialisation à 0pour passer au - moins une fois dans le while
	char *result;

	while(1){

		memset(req,MAX_BUFF,0); //nettoyer la chaine de requete

		//Reception requete
		CHECK(read(*socketTchat,req,MAX_BUFF),"Erreur Reception Requete");

		//Recuppère l'id de la commande
		printf("La requete est: %s \n",req);
		result=strtok(req,"\\");
		printf("La commande est: %s \n",result);

		//Récuperer la commande en int
		idCommande = atoi(result);
	
		switch (idCommande){
		
			case 130 : {
				
				int i=0;
				int choix=0;
				while (result != NULL){
					strcpy(req_decoupe[i],result);							
					result = strtok( NULL, "\\");
					i++;
				}	

				printf("L'utilisateur %s propose le débat %s\n",req_decoupe[1],req_decoupe[2]);
				printf("0 : Accepter   1 : Refuser \n");

				strcpy(pseudoDestinataire,req_decoupe[1]);
				strcpy(nomDebat,req_decoupe[2]);

				void* resultat;

				//Si saisie en cours
				if(thSaisie != 0) CHECK(pthread_cancel(thSaisie),"Pb cancel le thread de saisie");

				CHECK(pthread_create(&thSaisie,NULL,SaisieChoixUtilisateur,NULL),"Pb creation thread"); 

				pthread_join(thSaisie,&resultat);

				thSaisie = 0;

				choix = atoi(resultat);
				printf("Choix %d\n",choix);
				
				memset(rep,MAX_BUFF,0);

				if(choix==0){
					sprintf(rep,"%i",230);

					//Fichier archive
					time_t now = time(NULL);
					struct tm * p_dateheure = localtime(&now);
					char buffName[8];
					
					strcpy(nameFile,nomDebat);
					strcat(nameFile,"-");

					sprintf(buffName,"%02i",p_dateheure->tm_mday);
					strcat(nameFile,buffName);
					strcat(nameFile,"-");

					sprintf(buffName,"%02i",(p_dateheure->tm_mon)+1);
					strcat(nameFile,buffName);
					strcat(nameFile,"-");

					sprintf(buffName,"%i",(p_dateheure->tm_year)+1900);
					strcat(nameFile,buffName);
					strcat(nameFile,".txt");

					archive = fopen(nameFile, "w");

					if (archive != NULL)
					{
					fputs(nomDebat,archive);
					fputs(" - ",archive);
					fputs(pseudo,archive);
					fputs(" - ",archive);
					fputs(pseudoDestinataire,archive);
					fputs(" - Le : ",archive);

					sprintf(buffName,"%02i",p_dateheure->tm_mday);
					fputs(buffName,archive);

					fputs("-",archive);
					sprintf(buffName,"%02i",(p_dateheure->tm_mon)+1);
					fputs(buffName,archive);

					fputs("-",archive);
					sprintf(buffName,"%i",(p_dateheure->tm_year)+1900);
					fputs(buffName,archive);

					fputs("\n",archive);
					fputs("\n",archive);

					fclose(archive);
					}
				}	
				else sprintf(rep,"%i",330);
				CHECK(write(*socketTchat,rep,strlen(req)+1),"Erreur Envoi Requete");

			/*
				//Prevenir le serveur central
				memset(req,MAX_BUFF,0);
				memset(rep,MAX_BUFF,0);

				struct sockaddr_in socketTchatRecup;
				socklen_t tailleStruct = sizeof(socketTchatRecup);
				getsockname(*socketTchat,(struct sockaddr*)&socketTchatRecup,&tailleStruct);

				//sprintf(req,"%i\\%i\\%i\\%s\\%s",131,socketTchatRecup.sin_addr.s_addr,socketTchatRecup.sin_port,nomDebat,pseudoDestinataire);

				CHECK(write(sDialogueServeur,req,strlen(req)),"Erreur Envoi Requete");

				//Reception reponse
				CHECK(read(sDialogueServeur,rep,MAX_BUFF),"Erreur Reception Requete");

				//Recuppère l'id de la commande
				printf("La réponse est: %s \n",rep);
				result=strtok(rep,"\\");
				printf("La commande est: %s \n",result);

				//Récuperer la commande en int
				idCommande = atoi(result);
			*/			

			}
			break;
			
			
			case 132 :{

				int i=0;
				while (result != NULL){
					strcpy(req_decoupe[i],result);							
					result = strtok( NULL, "\\");
					i++;
				}	

				//Sauvergarde message dans le fichier d'archive
				archive = fopen(nameFile, "a");
				if (archive != NULL)
				{
				fputs(req_decoupe[1],archive);
				fputs("\n",archive);
				fclose(archive);
				}

				//Affichage du message 
				printf("%s\n",req_decoupe[1]);

				memset(message,MAX_BUFF,0);

				char buff[MAX_BUFF];

				strcpy(message,pseudo);
				strcat(message," : ");

				gets(buff);
				strcat(message,buff);

				//Sauvergarde message dans le fichier d'archive
				archive = fopen(nameFile, "a");
				if (archive != NULL)
				{
				fputs(message,archive);
				fputs("\n",archive);
				fclose(archive);
				}

				sprintf(req,"%i\\%s",232,message); 
				CHECK(write(*socketTchat,req,strlen(req)+1),"Erreur Envoi Requete");
			}	
			break;
		}
	} 

	printf("Traitement du thread Client\n");
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
	
	void* resultat;

	if(thSaisie != 0) CHECK(pthread_cancel(thSaisie),"Pb cancel le thread de saisie");
	CHECK(pthread_create(&thSaisie,NULL,SaisieChoixUtilisateur,NULL),"Pb creation thread"); 
	pthread_join(thSaisie,&resultat);

	thSaisie=0;

	*choix = atoi(resultat);
}

/*Demande le pseudo au moment de la connexion*/
void demandePseudo(char* pseudo) {
	printf("Veuillez-saisir votre pseudo \n");

	void* resultat;

	if(thSaisie != 0) CHECK(pthread_cancel(thSaisie),"Pb cancel le thread de saisie");
	CHECK(pthread_create(&thSaisie,NULL,SaisieChoixUtilisateur,NULL),"Pb creation thread"); 
	pthread_join(thSaisie,&resultat);

	thSaisie = 0;

	strcpy(pseudo,resultat);
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
	
	
	while(1){

		memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete

		//Reception reponse
		CHECK(read(*sa,rep,MAX_BUFF),"Erreur Reception Reponse");

		//Reccupération id reponse
		sscanf(rep,"%i",&repId);

		printf("Repid : %d \n", repId);
	
		switch(repId) {

			case 0 : //Fin comm
				printf("Communication terminée \n");
				return;
			break; 

			case 210 :
					printf("Pseudo OK\n");
					return;
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
void modeTchat(int* sa,char* pseudo){
			
	char req[MAX_BUFF]; //requete
	char rep[MAX_BUFF]; //stockage réponse
	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete
	
	char element[MAX_BUFF]; //stockage element i
	int repId = 0; //stockage Id reponse
	char rep_decoupe[MAX_CLIENT][MAX_BUFF]; //contient la rep decoupe.
	char *result;
	
	//Dmd connexion 
	sprintf(req,"%i\\",120); 
	CHECK(write(*sa,req,strlen(req)+1),"Erreur Envoi Requete");
	printf("Je suis dans mode tchat.\nJ'ai envoyé : %s \n", req);

	
	while(1){

		memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete

		//Reception reponse
		CHECK(read(*sa,rep,MAX_BUFF),"Erreur Reception Reponse");
		
		printf("J'ai reçu dans le mode tchat %s\n",rep);

		result=strtok(rep,"\\");

		//Récuperer la commande en int
		repId = atoi(result);

		switch(repId) {

			case 0 : //Fin comm
				printf("Communication terminée \n");
			break; 

			case 220 :{
					
				int i=0,nbClient,indiceClient;
				char (*clientConnect)[MAX_BUFF] = NULL;
				
				while (result != NULL){
					
					if(i == 1)
					{
						nbClient=atoi(result);	
						//Allocation dynamique du tableau des clients connectés
						clientConnect = malloc(nbClient * sizeof(char));	
						printf("Il y'a %d client(s) disponible(s) en mode Tchat\n",nbClient);  
					}
					
					if(i>1)
					{
					
						strcpy(clientConnect[i-1],result);
						printf("* %d : %s \n",i-1,clientConnect[i-1]); 
					
					}
												
					result = strtok( NULL, "\\");
					i++;
				}	

				printf("Veuillez-saisir l'indice du client à contacter\n");

				
				void* resultat;

				if(thSaisie != 0) CHECK(pthread_cancel(thSaisie),"Pb cancel le thread de saisie");

				CHECK(pthread_create(&thSaisie,NULL,SaisieChoixUtilisateur,NULL),"Pb creation thread"); 
	
				pthread_join(thSaisie,&resultat);

				thSaisie = 0;

				indiceClient = atoi(resultat);

				printf("Client choisit %d = %s\n",indiceClient,clientConnect[indiceClient]);

				//Renvoyer les données
				memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
				sprintf(req,"%i\\%s",121,clientConnect[indiceClient]); 

				CHECK(write(*sa,req,strlen(req)+1),"Erreur Envoi Requete");

				free(*clientConnect);
					
			break;
			}

			case 221 :{

				int i=0;
				while (result != NULL){
					strcpy(rep_decoupe[i],result);							
					result = strtok( NULL, "\\");
					i++;
				}						
				
				//Adresse,Port
				dialogueClient(rep_decoupe[1],atoi(rep_decoupe[2]),pseudo);
			}
			break;

			case 320 : 
			{
				printf("Echec lors de la réception de la liste des clients\n");
			}
			break;	

			case 321 : 
			{
				printf("Echec lors de la récupération des infos de ce client\n");

				//Ressaie
				sprintf(req,"%i\\",120); 
				CHECK(write(*sa,req,strlen(req)+1),"Erreur Envoi Requete");
			}
			break;	
		}

	}

}




//Permet de faire le switch dans le main pour réaliser les traitements associés à chaque mode
void switchMode(int* sDialogueServeur, int mode, char* pseudo) {
	switch(mode)
	{
		case 1 : 
			printf("Je suis dans le mode 1: Tchat Privé.\n");	
			modeTchat(sDialogueServeur, pseudo);		
		break;

		case 2 :
			printf("Je suis dans le mode 2: Tchat Public.\n");	
			modeTchat(sDialogueServeur, pseudo);	
		break;
	}
}


////// DIALOGUE ENTRE 2 CLIENTS ////
void dialogueClient(char* adresse, int port, char* pseudo){

	int sDialogueClient,repId;

	char rep_decoupe[2][MAX_BUFF]; //contient la rep decoupe.
	char *result;

	char req[MAX_BUFF]; //requete
	char rep[MAX_BUFF]; //stockage réponse
	char nomDebat[MAX_BUFF]; //nom du débat en cours
	char message[MAX_BUFF];
	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(nomDebat,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(message,MAX_BUFF,0); //nettoyer la chaine de requete

	//Creation de la socket de dialogue
	CHECK(sDialogueClient=socket(AF_INET,SOCK_STREAM,0),"probleme creation socket Dialogue Serveur Central");
	
	//Preparation de l'adressage Client-Serveur
	struct sockaddr_in svc;
	svc.sin_family=AF_INET;
	svc.sin_port=port;  
	svc.sin_addr.s_addr=atoi(adresse);
	memset(&svc.sin_zero,0,8);

	//Etablissement connexion Client-Serveur
	CHECK(connect(sDialogueClient,(struct sockaddr *)&svc,sizeof(svc)),"Erreur connect");	

	printf("Veuillez-saisir le nom du débat\n");


	void* resultat;
	
	if(thSaisie != 0) CHECK(pthread_cancel(thSaisie),"Pb cancel le thread de saisie");

	CHECK(pthread_create(&thSaisie,NULL,SaisieChoixUtilisateur,NULL),"Pb creation thread"); 

	pthread_join(thSaisie,&resultat);
	thSaisie = 0;

	strcpy(nomDebat,resultat);


	sprintf(req,"%i\\%s\\%s",130,pseudo,nomDebat); 
	CHECK(write(sDialogueClient,req,strlen(req)+1),"Erreur Envoi Requete");

	while(1){

		memset(rep,MAX_BUFF,0); //nettoyer la chaine de requete

		//Reception reponse
		CHECK(read(sDialogueClient,rep,MAX_BUFF),"Erreur Reception Reponse");

		printf("J'ai reçu dans le mode débat %s\n",rep);

		result=strtok(rep,"\\");

		//Récuperer la commande en int
		repId = atoi(result);

		switch(repId) {

			case 0 : //Fin comm
				printf("Communication terminée \n");
			break; 

			case 230 :
				printf("Connexion acceptée\n");	

				printf("Saisir un message\n");
				
				memset(message,MAX_BUFF,0);

				char buff[MAX_BUFF];

				strcpy(message,pseudo);
				strcat(message," : ");

				gets(buff);
				strcat(message,buff);

				sprintf(req,"%i\\%s",132,message); 

				CHECK(write(sDialogueClient,req,strlen(req)+1),"Erreur Envoi Requete");

			break;

			case 232 :{
				int i=0;
				while (result != NULL){
					strcpy(rep_decoupe[i],result);							
					result = strtok( NULL, "\\");
					i++;
				}	

				//Affichage du message 
				printf("%s\n",rep_decoupe[1]);

				memset(message,MAX_BUFF,0);

				char buff[MAX_BUFF];

				strcpy(message,pseudo);
				strcat(message," : ");

				gets(buff);
				strcat(message,buff);

				sprintf(req,"%i\\%s",132,message); 
				CHECK(write(sDialogueClient,req,strlen(req)+1),"Erreur Envoi Requete");
			}	
			break;

			case 330 : 
				printf("Connexion refusée\n");
			break; 
		}

	}



}


void* SaisieChoixUtilisateur(void* arg){

	char *resultat;
	resultat = malloc(3*sizeof(char)); 

	scanf("%s",resultat);

	pthread_exit(resultat);
}
