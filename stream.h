#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

//regarder si la socket est créée et si non on renvoie le message d'erreur fournie
#define CHECK(sts,msg) if((sts)==-1){perror(msg);exit(-1);}

#define PORT_SRV 5000
#define IP_SRV "127.0.0.1" //boucle locale
#define MAX_BUFF 1024
#define MAX_CLIENT 50


