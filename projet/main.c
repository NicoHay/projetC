#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> 
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <time.h> 

#include "projet.h"

/*
***************************************************************
*
* main
*
***************************************************************
*/
int main (int argc, char *argv[]){

    // declaration des variables 
    pthread_t threadserveur;
    pthread_t threadbrain;
    time_t  t;
    int     i, action;
    void* retourserveur ;
    void* retourbrain ;
 
    struct arg_struct args;

    
   listePort[0] = atoi( argv[1]);
   listePort[1] = atoi( argv[2]);
    
 
    srand((unsigned) time(&t));
 
    pthread_create (&threadserveur, NULL, serveur, (void *) &listePort[0]);
   
    sleep(3);

    for (int i = 0; i < NBRE_TOURS; i++)
    {
        action =  randomAction();

        args.action    = action;
        args.port      = listePort[0];
        args.nbre_tour = i;

        pthread_create (&threadbrain, NULL, theBrain,(void* ) &args);
        pthread_join (threadbrain, &retourbrain);
        
    }

    pthread_join (threadserveur, &retourserveur);
    printf("======= le retour du thread SERVEUR est :  %ld\n", (long) retourserveur);


	for (int i = 0; i < sizeof(zmClientBrain)/sizeof(zmClientBrain[0]); i++)
	{
		printf("[%d] ",zmClientBrain[i]);
	}
	

    return 0;
}