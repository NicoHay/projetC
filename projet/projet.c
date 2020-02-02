/*
****************************************************************************************************

            ______          _      _     _                                      _   
            | ___ \        (_)    | |   | |                                    | |  
            | |_/ / __ ___  _  ___| |_  | |     __ _ _ __ ___  _ __   ___  _ __| |_ 
            |  __/ '__/ _ \| |/ _ \ __| | |    / _` | '_ ` _ \| '_ \ / _ \| '__| __|
            | |  | | | (_) | |  __/ |_  | |___| (_| | | | | | | |_) | (_) | |  | |_ 
            \_|  |_|  \___/| |\___|\__| \_____/\__,_|_| |_| |_| .__/ \___/|_|   \__|
                          _/ |                                | |                   
                          |__/                                |_|                   
                                             
****************************************************************************************************
**                                       TODO LIST                                                **
****************************************************************************************************
*
*   -  REQUEST PLUS IMPLICITE 
* 
**************************************************************************************************** 
*/
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
#include <semaphore.h>

// entete 
#include "projet.h" 

//variables
int compteurInterne = 0;
int monIndex        = 0;
struct Messageinfos messagefromclient;
struct Messageinfos  zmClientBrain[NBRE_TOURS];
struct Messageinfos  zmServerBrain[NBRE_TOURS];
sem_t semaphore1;
sem_t semaphore2;

/**
***************************************************************
* thread serveur 
*
* @param void*    arg  numero de port

* @return         void* 
***************************************************************
*/
void*  serveur(void* arg )
{
    struct sockaddr_in adresseDuServeur;

    adresseDuServeur.sin_family         = AF_INET;
    adresseDuServeur.sin_port           = htons( *(int*)arg);
    adresseDuServeur.sin_addr.s_addr    = inet_addr("127.0.0.1");
    int descripteurDeSocketServeur      = socket (PF_INET, SOCK_STREAM, 0);



    if (descripteurDeSocketServeur < 0)
    {
        printf ("SERVEUR === Problemes pour creer la socket\n");
        pthread_exit( (void*) -1);
    }

    if ( bind ( descripteurDeSocketServeur, (struct sockaddr *)&adresseDuServeur, sizeof(struct sockaddr_in) ) < 0)
    {
        printf ("SERVEUR === Problemes pour faire le bind\n");
        pthread_exit((void*) -1) ;
    }

    if ( listen ( descripteurDeSocketServeur, 1) < 0)
    {
        printf ("SERVEUR === Problemes pour faire le listen\n");
        pthread_exit((void*) -1); 
    }

    while( NBRE_TOURS > monIndex  )
    {
         
        int                descripteurDeSocketClient;
        struct sockaddr_in adresseDuClient;
        unsigned int       longueurDeAdresseDuClient;

        descripteurDeSocketClient = accept (descripteurDeSocketServeur, (struct sockaddr *)&adresseDuClient, &longueurDeAdresseDuClient);
        recv (descripteurDeSocketClient, &messagefromclient, sizeof(messagefromclient), 0);

        // incrementation du compteur interne en fonction du message recu par les autres process 
        compteurInterne  = MAX(messagefromclient.estampille, compteurInterne) + 1 ;

        if ( (messagefromclient.action == 1 ) ) // si cest une demande zone critique
        {
            printf("\nle processus  %d veut rentrer en section "
                   "\n================================ "
                   "\navec l'estampille : %d "
                   "\nl'action interne est a : %d" 
                   "\n================================",messagefromclient.monpid ,messagefromclient.estampille,compteurInterne);

            if ((  messagefromclient.estampille < compteurInterne ))
            {
                zoneCritique(); // on simule la section critique

            }else {
                printf("\nDemande refusée");
            }
        } 

        if ( (messagefromclient.action == 0 )  )  // ceci est un message bidon 
        {
            messageBidon();
            printf( "\n\nle serveur recois un message bidon suivant : "
                    "\n================================"
                    "\nestampile du client  ---> %d" 
                    "\npid  du client       ---> %d"
                    "\ncompteur interne     ---> %d"
                    "\n================================", messagefromclient.estampille, messagefromclient.monpid, compteurInterne);
        }      

        fflush(stdout);
        close (descripteurDeSocketClient);
    }

    close (descripteurDeSocketServeur);
    fflush(stdout);
    pthread_exit( (void*) 0);
}

/**
***************************************************************
* thread client 
*
* @param void*    void*   argument 
*
* @return         void* 
***************************************************************
*/
void* client(void* argument)
{
    struct Messageinfos messagetoserveur;
    struct sockaddr_in adr;
     
    struct arg_struct *args = (struct arg_struct *)argument;


    adr.sin_family          = AF_INET;
    adr.sin_port            = htons( args->port);
    adr.sin_addr.s_addr     = inet_addr("127.0.0.1");
    int descripteurDeSocket = socket (AF_INET, SOCK_STREAM, 0);

    if (descripteurDeSocket < 0 )
    {
        printf ("CLIENT ++ Problemes pour creer la socket\n");
        pthread_exit( (void*) -1);
    }
      
    if (connect ( descripteurDeSocket,(struct sockaddr *) &adr, sizeof(adr)) < 0)
    {
        printf ("CLIENT ++ Problemes pour se connecter au serveur\n");
        exit( 0); // on quitte le programme

    }else{
           
        // Affectation des valeurs a envoyer
        messagetoserveur.estampille         = compteurInterne;
        messagetoserveur.monpid             = getpid();
        messagetoserveur.action             = args->action;

        semaphoreZoneMemoireClient(messagetoserveur);
        send(descripteurDeSocket, &messagetoserveur, sizeof(messagetoserveur), 0);

        close(descripteurDeSocket);
        fflush(stdout);

    }
    pthread_exit( (void*) 0);
}


/**
***************************************************************
* randomNum
*
* @return         int 
***************************************************************
*/
int randomNum()
{
    return rand() % 3;
}

/**
***************************************************************
* message bidon
*
* @return         null 
***************************************************************
*/
void messageBidon()
{
    compteurInterne++;
    printf("\n*** on envoi un message bidon\n");
 
}

/**
***************************************************************
* ZONE CRITIQUE
*
* @return         null 
***************************************************************
*/
void   zoneCritique()
{
    compteurInterne++;
    sem_init(&semaphore2, 0, 1);
    sem_wait(&semaphore2);  // On attend la disponibilité du sémaphore
    printf("\n.... start critique ...\n");
    printf("\n.... end critique ...\n");
    sem_post(&semaphore2);  // On relache le sémaphore

}


/**
***************************************************************
* actionInterne
*
* @return         null 
***************************************************************
*/
void actionInterne()
{
    compteurInterne++;
    printf(" \n***  le compteur interne augmente est passe  a : %d\n", compteurInterne);
    fflush(stdout);
}

/**
***************************************************************
* thread brain 
*
* @param void* ARGUMENTS
*
* @return VOID* 
***************************************************************
*/
void* theBrain(void* arguments)
{
    struct arg_struct *args = (struct arg_struct *)arguments;
    pthread_t threadclient;  
    void * retourthreadclient;
    int myaction =  args->action;

    switch (myaction)
    {
        case 0: //message bidon
            monIndex++;
            sleep(1);
            pthread_create (&threadclient, NULL, client, args );
            pthread_join (threadclient, &retourthreadclient);
            break;
        case 1: //zone critique
            monIndex++;
            sleep(1);
            pthread_create (&threadclient, NULL, client, args);
            pthread_join (threadclient, &retourthreadclient);
            break;
        case 2:  //action interne
            monIndex++;
            sleep(1);
            actionInterne();
            break;
    }

    pthread_exit( (void*) 0);

}

/**
****************************************************************
* Semaphore  ZM client
* 
* @param STRUCT MessageInfos mess
* 
* @return VOID
***************************************************************
*/
void   semaphoreZoneMemoireClient(struct Messageinfos mess )
{
    sem_init(&semaphore1, 0, 1);
    sem_wait(&semaphore1);  // On attend la disponibilité du sémaphore
    zmClientBrain[monIndex] = mess;
    sem_post(&semaphore1);  // On relache le sémaphore

}
/**
 ***************************************************************
 *  JUSTE UN PEU DE DECO
 *  
 * @return VOID
 ***************************************************************
 */
void printlogo(){

    printf("\n______          _      _     _                                      _                   ");
    printf("\n| ___ \\        (_)    | |   | |                                    | |                 ");
    printf("\n| |_/ / __ ___  _  ___| |_  | |     __ _ _ __ ___  _ __   ___  _ __| |_                 ");
    printf("\n|  __/ '__/ _ \\| |/ _ \\ __| | |    / _` | '_ ` _ \\| '_ \\ / _ \\| '__| __|           ");
    printf("\n| |  | | | (_) | |  __/ |_  | |___| (_| | | | | | | |_) | (_) | |  | |_                 ");
    printf("\n\\_|  |_|  \\___/| |\\___|\\__| \\_____/\\__,_|_| |_| |_| .__/ \\___/|_|   \\__|        ");
    printf("\n              _/ |                                | |                                   ");
    printf("\n              |__/                                |_|                                   ");
    printf("\n                                                                                        ");
}

