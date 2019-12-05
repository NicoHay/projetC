    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////   
    ///////////////////////           TODO  LIST          ///////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    // -- CHECK SUR LE CONTINUER COMMENT LE GERER                              //
    // --                                                                      //
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


#define requete     "CLIENT  +++ message du client => bonjour ici le client"
#define reponse     "SERVEUR +++ message du serveur => bonsoir ici le serveur"
#define NBRE_TOURS  10


int     compteurInterne = 0, monIndex = 0;
int     zmClientBrain[50] ;
int     zmServerBrain[50];
int     listePort[2];

struct Messageinfos {
   int      indexinterne ;
   int      estampille;
   char     text[50];
   int      zonecritique;
} messageinfos;  






int randomAction(){
    return rand() % 3;
}

void messageBidon(){
    printf("MAIN *** on envoi un message bidon\n");
    zmClientBrain[monIndex] = 10;
    monIndex++;
}

void zoneCritique(){
    printf("MAIN ***  on demande a entrer en zone critique\n");
    zmClientBrain[monIndex] = 20;
    monIndex++;
    sleep(5);
    printf("MAIN *** on sort de la zone critique\n");
}

void actionInterne(){
    printf("MAIN *** on monte le compteur interne \n");
    compteurInterne ++;
    zmClientBrain[monIndex] = 30;
    monIndex++;
    printf("MAIN ***  le compteur est a : %d\n", compteurInterne);
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
    // adresseDuServeur.sin_port        = hton * s(listePort[0]);

      adresseDuServeur.sin_addr.s_addr = INADDR_ANY;



    int descripteurDeSocketServeur = socket (PF_INET, SOCK_STREAM, 0);

    if (descripteurDeSocketServeur < 0)
    {
        printf ("SERVEUR === Problemes pour creer la socket\n");
        pthread_exit( (void*) -1);
    }

    printf ("SERVEUR === Socket cree\n");

    if ( bind ( descripteurDeSocketServeur, 
                (struct sockaddr *)&adresseDuServeur, 
                sizeof(struct sockaddr_in) ) < 0)
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



    while(monIndex < NBRE_TOURS -1)
    {
        int                descripteurDeSocketClient;
        struct sockaddr_in adresseDuClient;
        unsigned int       longueurDeAdresseDuClient;

        printf ("SERVEUR ++ Socket en attente\n");

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


  struct sockaddr_in adr;

  adr.sin_family      = AF_INET;
  adr.sin_port        = htons( *(int*)arg);

  adr.sin_addr.s_addr = INADDR_ANY ;

  int descripteurDeSocket = socket (AF_INET, SOCK_STREAM, 0);

  if (descripteurDeSocket < 0 )
   {
    printf ("CLIENT ++ Problemes pour creer la socket\n");
  
    pthread_exit( (void*) -1);
   }

  printf ("CLIENT ++ socket cree\n");


  if (connect ( descripteurDeSocket,
                (struct sockaddr *) &adr,
                sizeof(adr)                ) < 0)
   {
    printf ("CLIENT ++ Problemes pour se connecter au serveur\n");
    pthread_exit( (void*) -1);

   }

       
        printf ("CLIENT ++ socket connectee\n");

        printf ("CLIENT ++ Envoi de la requete : %s \n", requete);

        send(descripteurDeSocket, requete, strlen(requete), 0);


        char          buffer[1024];
        unsigned int  nbCaracteres;
        unsigned int  i;

        printf ("CLIENT ++ Reception de la reponse : \n");

        do{
                nbCaracteres = recv (descripteurDeSocket, buffer, 1024, 0);

                for (i = 0; i < nbCaracteres; i++)
                printf ("%c", buffer [i]);

        }while (nbCaracteres == 1024);

            printf ("\nCLIENT ++ FIN\n");

        close(descripteurDeSocket);
    
    
   
    pthread_exit( (void*) 0);
  
}

int main (int argc, char *argv[]){

    // declaration des variables 
    pthread_t threadserveur;
    pthread_t threadclient;
    time_t  t;
    int     i, action;
    void* retourserveur ;
    void* retourclient ;
    struct Messageinfos message;
    
   listePort[0] = atoi( argv[1]);
   listePort[1] = atoi( argv[2]);
    
 
    srand((unsigned) time(&t));
 
    pthread_create (&threadserveur, NULL, serveur, (void *) &listePort[0]);
   
    sleep(3);



    for (int i = 0; i < NBRE_TOURS; i++)
    {
        printf("\n========================================\n INDEX ACTION: %d\n MON PORT :  %d\n COMPTEUR INTERNE : %d\n MON PID : %d\n========================================\n",  monIndex, listePort[0],compteurInterne,getpid());   
        action =  randomAction();
        if (action == 0 )
        {
            pthread_create (&threadclient, NULL, client,(void *) &listePort[0]);
            messageBidon();
            pthread_join (threadclient, &retourclient);
            printf("========  le retour du thread client est %ld\n", (long) retourclient);
        }
        else if (action == 1)
        {
            zoneCritique();
        }
        else
        {
            actionInterne();
        }

    }
    pthread_join (threadserveur, &retourserveur);
    printf("======= le retour du thread serveur est :  %ld\n", (long) retourserveur);


	for (int i = 0; i < sizeof(zmClientBrain)/sizeof(zmClientBrain[0]); i++)
	{
		printf("[%d] ",zmClientBrain[i]);
	}
	

    return 0;
}