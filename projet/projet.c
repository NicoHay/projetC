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

// entete 
#include "projet.h" 


/*
***************************************************************
* thread serveur 
*
* void*         arg  numero du port
*
*return         void* 
***************************************************************
*/
void*  serveur(void* arg){

    
    struct sockaddr_in adresseDuServeur;

    adresseDuServeur.sin_family      = AF_INET;
    adresseDuServeur.sin_port        = htons( *(int*)arg);

    adresseDuServeur.sin_addr.s_addr = INADDR_ANY;



    int descripteurDeSocketServeur = socket (PF_INET, SOCK_STREAM, 0);

    if (descripteurDeSocketServeur < 0)
    {
        printf ("SERVEUR === Problemes pour creer la socket\n");
        pthread_exit( (void*) -1);
    }

    // printf ("SERVEUR === Socket cree\n");

    if ( bind ( descripteurDeSocketServeur, 
                (struct sockaddr *)&adresseDuServeur, 
                sizeof(struct sockaddr_in) ) < 0)
    {
        printf ("SERVEUR === Problemes pour faire le bind\n");
        pthread_exit((void*) -1) ;
    }

    // printf ("SERVEUR === Socket liee\n");



    if ( listen ( descripteurDeSocketServeur, 1) < 0)
    {
        printf ("SERVEUR === Problemes pour faire le listen\n");
        pthread_exit((void*) -1); 
    }



    while(monIndex < NBRE_TOURS -1)
    {
        int                descripteurDeSocketClient;
        struct sockaddr_in adresseDuClient;
        unsigned int       longueurDeAdresseDuClient;

        // printf ("SERVEUR ++ Socket en attente\n");

        descripteurDeSocketClient = accept (descripteurDeSocketServeur,
                                        (struct sockaddr *)&adresseDuClient,
                                        &longueurDeAdresseDuClient);


        unsigned int       nbCaracteres;
        unsigned int       i;
        char               buffer[1024];
        memset (buffer, 0, 1024);

        printf("\n SERVEUR ++ Lecture de la requete : \n");

        do
        {
        nbCaracteres = recv (descripteurDeSocketClient, buffer, 1024, 0);

        for (i=0; i<nbCaracteres; i++)
            printf ("%c", buffer [i]);
        }
        while (nbCaracteres == 1024);


        printf("\n SERVEUR ++ Ecriture de la reponse : %s\n", reponse);

        send(descripteurDeSocketClient, reponse, strlen(reponse), 0);  

        close (descripteurDeSocketClient);
    }



        pthread_exit( (void*) 0);

}

/*
***************************************************************
* thread client 
*
* void*         arg  numero du port a envoyer
*
*return         void* 
***************************************************************
*/
void* client(void* arg){

    struct Messageinfos messageclient;
    struct sockaddr_in adr;

  adr.sin_family      = AF_INET;
  adr.sin_port        = htons( *(int *)arg);
  adr.sin_addr.s_addr = INADDR_ANY ;
  int descripteurDeSocket = socket (AF_INET, SOCK_STREAM, 0);


  if (descripteurDeSocket < 0 )
   {
    printf ("CLIENT ++ Problemes pour creer la socket\n");
  
    pthread_exit( (void*) -1);
   }

//   printf ("CLIENT ++ socket cree\n");


  if (connect ( descripteurDeSocket,
                (struct sockaddr *) &adr,
                sizeof(adr)                ) < 0)
   {
    printf ("CLIENT ++ Problemes pour se connecter au serveur\n");
    pthread_exit( (void*) -1);

   }

       
        printf ("CLIENT ++ socket connectee\n");
        // strcpy(messageclient.text, "coucou test avec struct");
        // messageclient.indexinterne  = monIndex;
        // messageclient.zonecritique  = 1;
        // messageclient.estampille    = 10;
        // pintf ("CLIENT ++ Envoi de la requete : %s \n", requete);

        // send(descripteurDeSocket, &messageclient, sizeof(messageclient), 0);
        send(descripteurDeSocket, &requete, strlen(requete), 0);


        char          buffer[1024];
        unsigned int  nbCaracteres;
        unsigned int  i;

        printf ("CLIENT ++ Reception de la reponse : \n");

        do{
                nbCaracteres = recv (descripteurDeSocket, buffer, 1024, 0);

                for (i = 0; i < nbCaracteres; i++)
                printf ("%c", buffer [i]);

        }while (nbCaracteres == 1024);

            // printf ("\nCLIENT ++ FIN\n");

        close(descripteurDeSocket);
    
    
   
    pthread_exit( (void*) 0);
  
}
/*
***************************************************************
* randomAction
*
*return         int 
***************************************************************
*/
int randomAction(){
    return rand() % 3;
}

/*
***************************************************************
* message bidon
*
*return         null 
***************************************************************
*/
void messageBidon(){
    printf("MAIN *** on envoi un message bidon\n");
    zmClientBrain[monIndex] = 10;
    monIndex++;
}

/*
***************************************************************
* SIMULATION CRITIQUE
*
*return         null 
***************************************************************
*/
void simcritique(){

    for (int i = 0; i < 5; i++)
    {
    printf(".......\n");
    sleep(1);
    }

}


/*
***************************************************************
* zoneCritique
*
*return         null 
***************************************************************
*/
void zoneCritique(){
    printf("MAIN ***  on demande a entrer en zone critique\n");

    simcritique();
    zmClientBrain[monIndex] = 20;
    monIndex++;
   
    printf("MAIN *** on sort de la zone critique\n");
}

/*
***************************************************************
* actionInterne
*
*return         null 
***************************************************************
*/
void actionInterne(){
    printf("MAIN *** on monte le compteur interne \n");
    compteurInterne ++;
    zmClientBrain[monIndex] = 30;
    monIndex++;
    printf("MAIN ***  le compteur est a : %d\n", compteurInterne);
}


/*
***************************************************************
* thread brain 
*
* void*         action  action
* void*         arg  action
*
*
*return         void*     else
***************************************************************
*/
void* theBrain(void* arguments)
{
 
    struct arg_struct *args = (struct arg_struct *)arguments;
    pthread_t threadclient;  
    void * retourthreadclient;
    int myaction =  args->action;

    printf("\n========================================\n INDEX ACTION: %d\n MON PORT :  %d\n COMPTEUR INTERNE : %d\n MON PID : %d\n========================================\n",  monIndex,args->port, compteurInterne, getpid());   

    if (myaction == 0 )
        {
            pthread_create (&threadclient, NULL, client, &args->port);
            messageBidon();
            pthread_join (threadclient, &retourthreadclient);
            printf("========  le retour du thread client est %ld\n", (long) retourthreadclient);
        }
        else if (myaction == 1)
        {
            zoneCritique();
        }
        else if (myaction == 2)
        {
            actionInterne();
        }
        else
        {
            printf("Je ne connais pas cette action ! \n");
        }
}
