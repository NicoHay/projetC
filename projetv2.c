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


#define requete     "bonjour"
#define reponse     "reponse du serveur"
#define NBRE_TOURS  30


int    compteurInterne;
int    zmClientBrain[50] ;
int    zmServerBrain[50];
int    monIndex = 0;
int    listePort[2];

// #define nomMachine "localhost"
// #define ipMachine  "127.0.0.1"

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
    sleep(3);
    printf("MAIN *** on sort de la zone critique\n");
}

void ActionInterne(){
    printf("on monte le compteur interne \n");
    compteurInterne ++;
    zmClientBrain[monIndex] = 30;
    monIndex++;
    printf("MAIN ***  le compteur est a : %d\n", compteurInterne);
}


void*  serveur(void* arg){

    /* Etape 1 */
    /***********/
    struct sockaddr_in adresseDuServeur;

    adresseDuServeur.sin_family      = AF_INET;
    adresseDuServeur.sin_port        = htons(listePort[0]);
    adresseDuServeur.sin_addr.s_addr = INADDR_ANY;

    /* Etape 2 */
    /***********/

    int descripteurDeSocketServeur = socket (PF_INET, SOCK_STREAM, 0);

    if (descripteurDeSocketServeur < 0)
    {
        printf ("SERVEUR === Problemes pour creer la socket\n");
        pthread_exit( (void*) -1);
    }

    printf ("SERVEUR === Socket cree\n");

    /* Etape 3 */
    /***********/

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



    while(monIndex <= NBRE_TOURS)
    {
        int                descripteurDeSocketClient;
        struct sockaddr_in adresseDuClient;
        unsigned int       longueurDeAdresseDuClient;

        printf ("Socket en attente\n");

        descripteurDeSocketClient = accept (descripteurDeSocketServeur,
                                        (struct sockaddr *)&adresseDuClient,
                                        &longueurDeAdresseDuClient);


        unsigned int       nbCaracteres;
        unsigned int       i;
        char               buffer[1024];
        memset (buffer, 0, 1024);

        printf("\nLecture de la requete : \n");

        do
        {
        nbCaracteres = recv (descripteurDeSocketClient, buffer, 1024, 0);

        for (i=0; i<nbCaracteres; i++)
            printf ("%c", buffer [i]);
        }
        while (nbCaracteres == 1024);


        printf("\nEcriture de la reponse : %s\n", reponse);

        send(descripteurDeSocketClient, reponse, strlen(reponse), 0);  

        close (descripteurDeSocketClient);
    }
    fflush(stdin);
    printf ("SERVEUR === FIN\n");

        pthread_exit( (void*) 0);

}
int client(){
   	/* Déclaration de l'adresse IP, du numéro de port et du protocole */
  /******************************************************************/

  struct sockaddr_in adr;

  adr.sin_family      = AF_INET;
  adr.sin_port        = htons(listePort[1]);
  adr.sin_addr.s_addr = INADDR_ANY ;

  /* Creation de la socket */
  /*************************/

  int descripteurDeSocket = socket (AF_INET, SOCK_STREAM, 0);

  if (descripteurDeSocket < 0)
   {
    printf ("CLIENT ++ Problemes pour creer la socket\n");
    return -1;
   }

  printf ("CLIENT ++ socket cree\n");


  if (connect ( descripteurDeSocket,
                (struct sockaddr *) &adr,
                sizeof(adr)                ) < 0)
   {
    printf ("CLIENT ++ Problemes pour se connecter au serveur\n");
    return -1;
   }

  printf ("CLIENT ++ socket connectee\n");

  printf ("CLIENT ++ Envoi de la requete : %s \n", requete);

  send(descripteurDeSocket, requete, 8, 0);


  char          buffer[1024];
  unsigned int  nbCaracteres;
  unsigned int  i;

  printf ("CLIENT ++ Reception de la reponse : \n");

  do{
        nbCaracteres = recv (descripteurDeSocket, buffer, 1024, 0);

        for (i = 0; i < nbCaracteres; i++)
        printf ("%c", buffer [i]);

   }while (nbCaracteres == 1024);

  printf ("CLIENT ++ FIN\n");

  close(descripteurDeSocket);

  return 0;
}

int main (int argc, char *argv[]){

    // declaration des variables 
    pthread_t threadserveur;
    pthread_t threadclient;
    time_t  t;
    int     i, action;
    void* retourserveur ;

    
    
    listePort[0] = atoi( argv[1]);
    listePort[1] = atoi( argv[2]);
    
    sleep(5);
 
    srand((unsigned) time(&t));
    pthread_create (&threadserveur, NULL, serveur, NULL);

    for (int i = 0; i < NBRE_TOURS; i++)
    {
   

        printf("\n====================\ntaille du tableau %ld  l'index est a : %d\n====================\n le port est le %d\n====================\n le compteur interne est a %d\n====================\n", sizeof(zmClientBrain)/sizeof(int), monIndex, listePort[0],compteurInterne);
        
        action =  randomAction();

        if (action == 0 )
        {
            messageBidon();
            
            client();
            
        }
        else if (action == 1)
        {
            zoneCritique();

        }
        else
        {
            ActionInterne();
        }

    }

    pthread_join (threadserveur, &retourserveur);


    //AFFICHAGE DU TABLEAU 
	// for (int i = 0; i < sizeof(zmClientBrain)/sizeof(zmClientBrain[0]); i++)
	// {
	// 	printf("[%d] ",zmClientBrain[i]);
	// }
	

    return 0;
}