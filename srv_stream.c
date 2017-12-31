#include "biblio_srv.h"

//Variables globales
int nbClient = 0;
client ensClient[MAX_CLIENT];


//Mutex
pthread_mutex_t  mutex_ensembleClient  = PTHREAD_MUTEX_INITIALIZER;


int main(){

pthread_t thService; 
void ** resultat; 

		
//////////Défintion des sockets//////////////
	//Socket écoute	
	int se;
	struct sockaddr_in svc;

	//Socket Dialogue
	int sd;
	
	//Socket Client 
	struct sockaddr_in client; 
	socklen_t lenSd;
///////////////////////////////////////

//////////Adressage du serveur///////////////////
	
	CHECK(se=socket(AF_INET,SOCK_STREAM,0),"probleme creation socket");  
	
	//preparation de l'adressage
	svc.sin_family=AF_INET;
	svc.sin_port=htons(PORT_SRV);  
	svc.sin_addr.s_addr=inet_addr(IP_SRV);
	memset(&svc.sin_zero,0,8);


	CHECK(bind(se,(struct sockaddr*)&svc,sizeof(svc)),"test associer adr a la socket");
	
	//CONFIGURATION DU SEVEUR EN ECOUTE
	listen(se,20); //20 en backlog (20 connection gardée en mémoire max si occupé) 
	
//////////////////////////////

/////////Initialisation ensClient////////

	pthread_mutex_lock(&mutex_ensembleClient); 
		
		//Mise en place du tableau creux
		for(int i=0; i<MAX_CLIENT;i++){
			ensClient[i].mode = -1;
		}	

	pthread_mutex_unlock(&mutex_ensembleClient);

/////////////////////////////////////////

	while(1){			
		
		//Accepte la connexion et création socket clienet
		lenSd = sizeof(client);

		CHECK(sd=accept(se,(struct sockaddr*)&client,&lenSd),"Erreur Accept");

		// On verrouille et dévérouille le mutex //
		pthread_mutex_lock(&mutex_ensembleClient); 
			nbClient++;
		pthread_mutex_unlock(&mutex_ensembleClient);


		//printf("Accepted connection from %s:%d\n", inet_ntoa(dialogue.sin_addr), ntohs(dialogue.sin_port));

		//Creation du thread de service
	  	CHECK(pthread_create(&thService,NULL,traitementThread,(void*)&sd),"Pb creation thread");
		
		//Detachement du thread de service
		CHECK(pthread_detach(thService),"Pb detachement thread");
		
	
	}

close(se);//fermer la socket d'écoute
return 0;

}


