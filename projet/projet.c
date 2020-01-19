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
int continuer       = 1;
int zmClientBrain[50];
int zmServerBrain[50];
int listePort[2];
sem_t semaphore;

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

    struct Messageinfos messagefromclient;
    struct Messageinfos messagetoclient;
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
    // printf ("SERVEUR === Socket cree\n");
    if ( bind ( descripteurDeSocketServeur, (struct sockaddr *)&adresseDuServeur, sizeof(struct sockaddr_in) ) < 0)
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

        printf ("SERVEUR ++ Socket en attente\n");

        descripteurDeSocketClient = accept (descripteurDeSocketServeur, (struct sockaddr *)&adresseDuClient, &longueurDeAdresseDuClient);

        memset(&messagefromclient, 0, sizeof messagefromclient);

        printf("\n SERVEUR ++ Lecture de la requete : \n");
            
        recv (descripteurDeSocketClient, &messagefromclient, sizeof(messagefromclient), 0);

        //debug 
        printf("mon index       ---> %d\n", messagefromclient.indexinterne );
        printf("mon message     ---> %s\n", messagefromclient.text );
        printf("mon estampile   ---> %d\n", messagefromclient.estampille );
        printf("mon pid         ---> %d\n", messagefromclient.monpid );
        printf("\n SERVEUR ++ Ecriture de la reponse \n");


        messagetoclient.indexinterne  = monIndex;
        messagetoclient.estampille    = 10;
        messagetoclient.monpid        = getpid();


        // send(descripteurDeSocketClient, reponse, strlen(reponse), 0);  
        send(descripteurDeSocketClient, &messagetoclient, sizeof(messagetoclient), 0);
        close (descripteurDeSocketClient);
        fflush(stdin);
    }
        close (descripteurDeSocketServeur);
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
    
    struct Messageinfos messagetoserveur;
    struct sockaddr_in adr;

    adr.sin_family          = AF_INET;
    adr.sin_port            = htons( *(int *)arg);
    adr.sin_addr.s_addr     = inet_addr("127.0.0.1");
    int descripteurDeSocket = socket (AF_INET, SOCK_STREAM, 0);


    if (descripteurDeSocket < 0 )
    {
        printf ("CLIENT ++ Problemes pour creer la socket\n");
        pthread_exit( (void*) -1);
    }
      printf ("CLIENT ++ socket cree\n");

    if (connect ( descripteurDeSocket,(struct sockaddr *) &adr, sizeof(adr)) < 0)
    {
        printf ("CLIENT ++ Problemes pour se connecter au serveur\n");
        pthread_exit( (void*) -1);
    }

    printf ("CLIENT ++ socket connectee\n");

    strcpy(messagetoserveur.text   , "coucou test avec struct");
    messagetoserveur.indexinterne  = monIndex;
    messagetoserveur.estampille    = 10;
    messagetoserveur.monpid        = getpid();

    send(descripteurDeSocket, &messagetoserveur, sizeof(messagetoserveur), 0);

    fflush(stdin);
    close(descripteurDeSocket);
    printf ("\nCLIENT ++ FIN\n");
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
    return rand() % 2;
}

/*
***************************************************************
* message bidon
*
*return         null 
***************************************************************
*/
void messageBidon(){
    printf("\n==============================\n");   
    printf("MAIN *** on envoi un message bidon\n");
    printf("\n==============================\n");   
    sleep(1);

    // zmClientBrain[monIndex] = 10; // on ajoute un element au tab  zmclient -> brain
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
    printf("\n ==============================\n");   
    printf("MAIN *** on monte le compteur interne \n");
    printf("==============================\n");   
    sleep(1);
    compteurInterne ++;
    zmClientBrain[monIndex] = 30;
    monIndex++;
    printf("MAIN ***  le compteur est a : %d\n", compteurInterne);
}

/*
***************************************************************
* thread brain 
*
* VOID* action  action
* VOID* arg  action
*
*return VOID* 
***************************************************************
*/
void* theBrain(void* arguments)
{
    struct arg_struct *args = (struct arg_struct *)arguments;
    pthread_t threadclient;  
    void * retourthreadclient;
    int myaction =  args->action;
    printf("\n ==============================\n MON PORT :  %d\n ==============================\n", args->port );   

    if (myaction == 0 )
    {
        pthread_create (&threadclient, NULL, client, &args->port);
        messageBidon();
        pthread_join (threadclient, &retourthreadclient);
    }
    if (myaction == 1)
    {
        zoneCritique();
    }
    if (myaction == 2)
    {
        actionInterne();
    }
    pthread_exit( (void*) 0);
}

void    semaphoreZoneMemoire()
{

  
    sem_wait(&semaphore);  // On attend la disponibilité du sémaphore

    // section critique *
    // action a effectuer
   
    sem_post(&semaphore);  // On relache le sémaphore

}
