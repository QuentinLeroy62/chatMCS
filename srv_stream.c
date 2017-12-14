#include "biblio_srv.h"

int main(){
	
client ensClient[MAX_CLIENT];
int nbClient=0;
int pid;


/////Mise en place du déroutement///
	struct sigaction newAct;
	
	newAct.sa_handler=derouter;
	newAct.sa_flags=SA_RESTART;
	
	CHECK(sigemptyset(&newAct.sa_mask),"Erreur sigemptyset");
	CHECK(sigaction(SIGCHLD,&newAct,NULL),"Erreur sigaction");
/////////////////////////////////////////////////////////
		
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


	while(1){			
		
		//Accepte la connexion et création socket clienet
		lenSd = sizeof(client);

		CHECK(sd=accept(se,(struct sockaddr*)&client,&lenSd),"Erreur Accept");
		nbClient++;
		
		//printf("Accepted connection from %s:%d\n", inet_ntoa(dialogue.sin_addr), ntohs(dialogue.sin_port));
		
		//Creation d'un processus de service pour le client
		CHECK(pid = fork(), "Erreur creation processus");
		
		//Fils dialogue avec le client
		if(pid == 0){
		   		close(se); //fermer socket ecoute
		  		dialogueClient(sd,&nbClient,ensClient); //dialogue
		  		close(sd); //fermer socket dialogue
		  		exit(0); //tue le fils
		}
	
	}

close(se);//fermer la socket d'écoute
return 0;

}


