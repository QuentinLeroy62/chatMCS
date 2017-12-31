#include "biblio_srv.h"

/*Traitement du thread de service */
void * traitementThread (void* sd)
{
	//Cast l'argument en int*
	int* mySocket = (int*) sd;

	//Pour stocker le pseudo du Client de ce thread
	char pseudo[MAX_TAILLE_PSEUDO];	
	
	dialogueClient(mySocket,pseudo); //dialogue
	close(*mySocket); //fermer socket dialogue
	
	// On verrouille et dévérouille le mutex //
	pthread_mutex_lock(&mutex_ensembleClient);
		nbClient--;

		//On retire le client de l'ensemble des clients connectés
		for(int i=0; i<MAX_CLIENT; i++){
			
			//Recherche du client et suppression du tableau 
			if(strcmp(pseudo,ensClient[i].pseudo)==0){
				
				strcpy(ensClient[i].pseudo,"");
				ensClient[i].mode = -1;
				ensClient[i].etat = 0;
				memset(&ensClient[i].socketClient,0,sizeof(struct sockaddr_in));
				
				break;
			}  

		}	
	pthread_mutex_unlock(&mutex_ensembleClient);

	pthread_exit(0);
}

/*Fonction pour le dialogue avec un client*/
void dialogueClient(int* sd, char* pseudo)
{

	char req[MAX_BUFF]; //stocke la requete
	char rep[MAX_BUFF]; //stocke la reponse
	char req_decoupe[5][MAX_BUFF];

	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de reponse

	int idCommande=0; //stocke l'id de la commande initialisation à 0pour passer au - moins une fois dans le while
	char *result;

		while(1){

			//Reception requete
			CHECK(read(*sd,req,MAX_BUFF),"Erreur Reception Requete");

			//Recuppère l'id de la commande
			//sscanf(req,"%i",&idCommande);
			printf("La requete est: %s \n",req);
			result=strtok(req,"\\");
			printf("La commande est: %s \n",result);

			//Récuperer la commande en int
			idCommande = atoi(result);
		
			printf("Il y a %d client \n",nbClient);
			
			switch (idCommande){
				
				//Arreter communication avec le serveur central	
				case 0 :
					printf("Communication avec le serveur terminée\n"); 
					return;
				break;

				//Première connexion
				case 110 : {

						int i=0;
						while (result != NULL){
							strcpy(req_decoupe[i],result);							
							result = strtok( NULL, "\\");
							i++;
						}						
						
						//Reconstitution de la socket Ecoute client 
						struct sockaddr_in clientRecup; 
						clientRecup.sin_family=AF_INET;
						clientRecup.sin_addr.s_addr=atoi(req_decoupe[2]);
						clientRecup.sin_port=atoi(req_decoupe[3]);
						memset(&clientRecup.sin_zero,0,8);	
					
						// On verrouille et dévérouille le mutex //
						pthread_mutex_lock(&mutex_ensembleClient); 
							int j = 0;
							short testPseudo = 1;

							for(int i=0; i<MAX_CLIENT; i++){
								
								//Recherche d'une place libre dans l'ensClient
								if(ensClient[i].mode == -1 && j==0 ) j=i;
					
								//Verification du pseudo
								if(strcmp(req_decoupe[1],ensClient[i].pseudo)==0){
									 testPseudo = 0; 
									break;
								}  

							}										
											
							if(testPseudo){
								 strcpy(ensClient[j].pseudo,req_decoupe[1]);
								 ensClient[j].mode = atoi(req_decoupe[4]);
								 ensClient[j].socketClient = clientRecup;

								 //Sauvergarde du pseudo pour le client en cours
								 strcpy(pseudo,ensClient[j].pseudo);
							}

						pthread_mutex_unlock(&mutex_ensembleClient);

							if(testPseudo == 1){
								sprintf(rep,"%i",210); 
								CHECK(write(*sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
							}else{
								sprintf(rep,"%i",310); 
								CHECK(write(*sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
							}

				}
				break; 

				//Liste des clients 
				case 120:{

						char infoClient[nbClient*MAX_TAILLE_PSEUDO];
						memset(infoClient,nbClient*MAX_TAILLE_PSEUDO,0); //nettoyer le tableau
						int nbClientTchat=0;

						// On verrouille et dévérouille le mutex //
						pthread_mutex_lock(&mutex_ensembleClient); 
							
							for(int i=0; i<MAX_CLIENT; i++){

								//Verfie qu'on est en mode Tchat et que ce n'est pas nous même 
								if((ensClient[i].mode == 1 || ensClient[i].mode == 2) && (strcmp(ensClient[i].pseudo,pseudo)!=0) && ensClient[i].etat == 0){
									strcat(infoClient,"\\");
									strcat(infoClient,ensClient[i].pseudo);
									nbClientTchat++;
								}
							}		

						pthread_mutex_unlock(&mutex_ensembleClient);

						sprintf(rep,"%i\\%d",220,nbClientTchat);

						strcat(rep,infoClient);
							
						CHECK(write(*sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
				}		
				break;

				//Demande info d'un client
				case 121 :{

						int i=0;
						int flag=0;
						while (result != NULL){
							strcpy(req_decoupe[i],result);							
							result = strtok( NULL, "\\");
							i++;
						}	

						printf("pseudo demandé %s\n",req_decoupe[1]);

						// On verrouille et dévérouille le mutex //
						pthread_mutex_lock(&mutex_ensembleClient); 

							for(int i=0; i<MAX_CLIENT; i++){
								
								//Verfie que ce n'est pas une place vide et que ce n'est pas nous même 
								if(ensClient[i].mode != -1 && strcmp(ensClient[i].pseudo,req_decoupe[1])==0){
									flag=1;
									sprintf(rep,"%i\\%d\\%d",221,ensClient[i].socketClient.sin_addr.s_addr,ensClient[i].socketClient.sin_port); 
									break;
								}
							}		
						pthread_mutex_unlock(&mutex_ensembleClient);

						if(flag == 1){
							CHECK(write(*sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
						} 
						else{
							sprintf(rep,"%i",321);
							CHECK(write(*sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
						}

				}		
				break;
			
			}
		} 

	return;
}


