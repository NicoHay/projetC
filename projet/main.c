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
    int    listePort[2];

    int action;
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

        action =  randomNum();

        args.action    = action;
        args.port      = listePort[1];
        args.nbre_tour = i;

        pthread_create (&threadbrain, NULL, theBrain,(void* ) &args);
        pthread_join (threadbrain, &retourbrain);
        
    }

    pthread_join (threadserveur, &retourserveur);

    //debug de la zone memoire
    fflush(stdout);
	for (int i = 0; i < sizeof(zmClientBrain)/sizeof(zmClientBrain[0]); i++)
	{
		printf("[%d] ",zmClientBrain[i]);
	}
	
    return 0;
}