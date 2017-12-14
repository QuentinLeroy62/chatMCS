#include "biblio_srv.h"


/*Fonction pour le déroutement*/
void derouter(int sg)
{
  int status;
  int pid = wait(&status);
  printf("Le processus de service [PID=%d], vient de se terminer avec le status %d\n",pid,status);
}

/*Fonction pour le dialogue avec un client*/
void dialogueClient(int sd, int* nbClient, client* client)
{

	char req[MAX_BUFF]; //stocke la requete
	char rep[MAX_BUFF]; //stocke la reponse

	memset(req,MAX_BUFF,0); //nettoyer la chaine de requete
	memset(rep,MAX_BUFF,0); //nettoyer la chaine de reponse

	int idCommande=0; //stocke l'id de la commande initialisation à 0pour passer au - moins une fois dans le while

		//while(idCommande != 160){ //CMD=160 -> Quitter communication

			//Reception requete
			CHECK(read(sd,req,MAX_BUFF),"Erreur Reception Requete");

			//Recuppère l'id de la commande
			//sscanf(req,"%i",&idCommande);
			printf("La requete est: %s",req);
		/*	
			switch (idCommande){

				case 0 : //Fin communication
						sprintf(rep,"%i",0);
						CHECK(write(sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
				break; 
			 
				case 100 : //DMD Taille N  
						tailleN = (sizeof(lstr)/1024);
						sprintf(rep,"100:%i",tailleN);
						CHECK(write(sd,rep,strlen(rep)+1),"Erreur Envoi Requete");
				break; 
				   	
				case 200 : //DMD Indice 
				   	
					//Recuppère l'id de la commande
					sscanf(req,"%i:%i",&idCommande,&indice);

					//Depassement indice
				   	if(indice<tailleN){
				   		sprintf(rep,"200:%s",lstr[indice]);
				   	} 
					else{
						sprintf(rep,"250");
					} 

					CHECK(write(sd,rep,strlen(rep)+1),"Erreur Envoi Requete");

				break;
			
			}*/
		//} 

}


