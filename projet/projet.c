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
int zmClientBrain[50];
int zmServerBrain[50];
sem_t semaphore1;
sem_t semaphore2;

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
    printf ("SERVEUR === Socket liee\n");
    if ( listen ( descripteurDeSocketServeur, 1) < 0)
    {
        printf ("SERVEUR === Problemes pour faire le listen\n");
        pthread_exit((void*) -1); 
    }


    while( compteurInterne < NBRE_TOURS )
    {
 
        int                descripteurDeSocketClient;
        struct sockaddr_in adresseDuClient;
        unsigned int       longueurDeAdresseDuClient;

        printf ("SERVEUR ++ Socket en attente\n");
        fflush(stdout);
        descripteurDeSocketClient = accept (descripteurDeSocketServeur, (struct sockaddr *)&adresseDuClient, &longueurDeAdresseDuClient);


        recv (descripteurDeSocketClient, &messagefromclient, sizeof(messagefromclient), 0);

        //debug 
        printf(" index du client      ---> %d\n", messagefromclient.indexinterne );
        printf(" estampile du client  ---> %d\n", messagefromclient.estampille );
        printf(" pid  du client       ---> %d\n", messagefromclient.monpid );
        fflush(stdout);

        close (descripteurDeSocketClient);
    }
        close (descripteurDeSocketServeur);
        printf("***********----------fin du serveur ");
        fflush(stdout);
        pthread_exit( 0);
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
        
        pthread_exit(0);
    }

    printf ("CLIENT ++ socket connectee\n");
    fflush(stdout);

    messagetoserveur.indexinterne  = compteurInterne;
    messagetoserveur.estampille    = 10;
    messagetoserveur.monpid        = getpid();

    send(descripteurDeSocket, &messagetoserveur, sizeof(messagetoserveur), 0);

    close(descripteurDeSocket);
    printf ("\nCLIENT ++ FIN\n");
    fflush(stdout);
    pthread_exit( (void*) 0);
}
/*
***************************************************************
* randomNum
*
*return         int 
***************************************************************
*/
int randomNum(){
 
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
    compteurInterne++;
    printf("\n== *** on envoi un message bidon\n");
    semaphoreZoneMemoireClient(10);
 
}

/*
***************************************************************
* SIMULATION CRITIQUE
*
*return         null 
***************************************************************
*/
void simcritique(){

    int tps;
    tps = randomNum();
    for (int i = 0; i < tps; i++)
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
    compteurInterne++;
    simcritique();
    semaphoreZoneMemoireClient(20);
}
/*
***************************************************************
* actionInterne
*
*return         null 
***************************************************************
*/
void actionInterne(){

    compteurInterne++;
    zmClientBrain[monIndex] = 30;
    printf(" ***  le compteur est a : %d\n", compteurInterne);
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
 

    switch (myaction)
    {
    case 0:
        monIndex++;
        pthread_create (&threadclient, NULL, client, &args->port);
        messageBidon();
        pthread_join (threadclient, &retourthreadclient);
        printf("fin du message bidon");
        break;
    case 1:
        monIndex++;
        zoneCritique();
        break;
    case 2: 
        monIndex++;
        actionInterne();
        break;
    }

    pthread_exit( (void*) 0);

}

void    semaphoreZoneMemoireClient(int num1 )
{
    sem_init(&semaphore1, 0, 1);
    sem_wait(&semaphore1);  // On attend la disponibilité du sémaphore
    zmClientBrain[monIndex] = num1;
    sem_post(&semaphore1);  // On relache le sémaphore

}


void    semaphoreZoneMemoireServeur(int num)
{
    sem_init(&semaphore2, 0, 1);
    sem_wait(&semaphore2);  // On attend la disponibilité du sémaphore
    zmServerBrain[monIndex] = num;
    sem_post(&semaphore2);  // On relache le sémaphore

}



// TODO        
//              POUSSER LA STRUCT DANS LE TABLEAU POUR CHECKER LAMPORT
//              IMPLEMENTER LAMPORT