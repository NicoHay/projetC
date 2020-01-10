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

//variables

int compteurInterne = 0;
int monIndex        = 1;
int continuer       = 1;
int zmClientBrain[50];
int zmServerBrain[50];
int listePort[2];

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

    printf ("SERVEUR === Socket cree\n");

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

    while(monIndex < NBRE_TOURS )
    {
        int                descripteurDeSocketClient;
        struct sockaddr_in adresseDuClient;
        unsigned int       longueurDeAdresseDuClient;

        printf ("SERVEUR ++ Socket en attente\n");

        descripteurDeSocketClient = accept (descripteurDeSocketServeur, (struct sockaddr *)&adresseDuClient, &longueurDeAdresseDuClient);

        memset(&messagefromclient, 0, sizeof messagefromclient);

        printf("\n SERVEUR ++ Lecture de la requete : \n");
            
        recv (descripteurDeSocketClient, &messagefromclient, sizeof(messagefromclient), 0);

        printf("index               ---> %d\n", messagefromclient.indexinterne );
        printf("message             ---> %s\n", messagefromclient.text );
        printf("zone critique       ---> %d\n", messagefromclient.zonecritique );
        printf("compteur interne    ---> %d\n", messagefromclient.estampille );
        printf("pid                 ---> %d\n", messagefromclient.monpid );

        fflush(stdout);
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
    
    struct Messageinfos messageclient;
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

    strcpy(messageclient.text   , "coucou test avec struct");
    messageclient.indexinterne  = monIndex;
    messageclient.zonecritique  = 1;
    messageclient.estampille    = compteurInterne;
    messageclient.monpid        = getpid();

    send(descripteurDeSocket, &messageclient, sizeof(messageclient), 0);

    printf ("\nCLIENT ++ FIN\n");
    fflush(stdin);
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
 
    printf("\n============================== *** on envoi un message bidon\n");
    sleep(1);
    zmClientBrain[monIndex] = 10; // on ajoute un element au tab  zmclient -> brain
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

    printf("\n============================== ***  on demande a entrer en zone critique\n");
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
    printf("\n ============================== *** on monte le compteur interne \n");
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

    if (myaction == 0 )
    {
        pthread_create (&threadclient, NULL, client, &args->port);
        messageBidon();
        pthread_join (threadclient, &retourthreadclient);
    }
    if (myaction == 1)
    {
        pthread_create (&threadclient, NULL, client, &args->port);
        zoneCritique();
        pthread_join (threadclient, &retourthreadclient);
    }
    if (myaction == 2)
    {
        actionInterne();
    }
    if (myaction > 2)
    {
        printf("\n ================ pas compris qoi que tu veut =============");   
    }
    pthread_exit( (void*) 0);
}