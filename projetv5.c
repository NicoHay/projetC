    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////   
    ///////////////////////           TODO  LIST          ///////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    // -- CHECK SUR LE CONTINUER COMMENT LE GERER                              //
    // -- ENVOIE DES STRUCT DEPUIS LE CLIENT                                   //
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    



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

// GLOBALES
#define requete     "CLIENT  +++ message du client => bonjour ici le client"
#define reponse     "SERVEUR +++ message du serveur => bonsoir ici le serveur"
#define NBRE_TOURS  10

//VARIABLES
int     compteurInterne = 0, monIndex = 0, continuer = 1;
int     zmClientBrain[50] ;
int     zmServerBrain[50];
int     listePort[2];


// STRUCTS
struct Messageinfos {
    int      indexinterne;
    int      estampille;
    char     text[50];
    int      zonecritique;
};  


struct arg_struct {
    int         action;
    int         port;
    int         nbre_tour;
    pthread_t  thread;
}args;
// FONCTIONS HEADER
void*   theBrain(void* arguments);
void*   serveur(void* arg);
void*   client(void* arg);
int     randomAction();
void    messageBidon();
void    simcritique();
void    zoneCritique();
void    actionInterne();


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
        strcpy(messageclient.text, "coucou test avec struct");
        messageclient.indexinterne  = monIndex;
        messageclient.zonecritique  = 1;
        messageclient.estampille    = 10;
        // pintf ("CLIENT ++ Envoi de la requete : %s \n", requete);

        send(descripteurDeSocket, &messageclient, sizeof(messageclient), 0);


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
