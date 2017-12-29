#include "biblio_srv.h"

/*Traitement du thread de service */
void * traitementThread (void* sd)
{
	//Cast l'argument en int*
	int* mySocket = (int*) sd;
	
	dialogueClient(mySocket); //dialogue
	close(*mySocket); //fermer socket dialogue
	
	// On verrouille et dévérouille le mutex //
	pthread_mutex_lock(&mutex_ensembleClient); 
		nbClient--;
	pthread_mutex_unlock(&mutex_ensembleClient);

	pthread_exit(0);
}

/*Fonction pour le dialogue avec un client*/
void dialogueClient(int* sd)
{

	char req[MAX_BUFF]; //stocke la requete
	char rep[MAX_BUFF]; //stocke la reponse
	char req_decoupe[5][MAX_BUFF];

	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de reponse

	int idCommande=0; //stocke l'id de la commande initialisation à 0pour passer au - moins une fois dans le while
	char *result;

		//while(idCommande != 160){ //CMD=160 -> Quitter communication

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
				
				//Première connexion
				case 110 : {

						int i=0;
						while (result != NULL){
							strcpy(req_decoupe[i],result);							
							result = strtok( NULL, "\\");
							i++;
						}						
						
						//Pour recup la socket client 
						struct sockaddr_in clientRecup; 
						socklen_t lenClientRecup;
						lenClientRecup = sizeof(clientRecup);

						getpeername(*sd,(struct sockaddr*)&clientRecup,&lenClientRecup);


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
								 ensClient[j].mode = atoi(req_decoupe[2]);
								 ensClient[j].socketClient = clientRecup;
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
				case 120:
						// On verrouille et dévérouille le mutex //
						pthread_mutex_lock(&mutex_ensembleClient); 
							
							sprintf(rep,"%i\\%d",220,nbClient); 

							for(int i=0; i<MAX_CLIENT; i++){
								
								//Verfie que ce n'est pas une place vide
								if(ensClient[i].mode != -1){
									strcat(rep,"\\");
									strcat(rep,ensClient[i].pseudo);
								}
							}		

						pthread_mutex_unlock(&mutex_ensembleClient);

						CHECK(write(*sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
				break;
			
			}
		//} 

}


