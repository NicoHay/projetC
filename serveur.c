/****************************/
/* Socket serveur itérative */
/****************************/

#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#define reponse "Serveur maison !!"

int main (int argc, char** argv)
 {
  /* Etape 1 */
  /***********/

  struct sockaddr_in adresseDuServeur;

  adresseDuServeur.sin_family      = PF_INET;
  adresseDuServeur.sin_port        = htons(2000);
  adresseDuServeur.sin_addr.s_addr = INADDR_ANY;

  /* Etape 2 */
  /***********/

  int descripteurDeSocketServeur = socket (PF_INET, SOCK_STREAM, 0);

  if (descripteurDeSocketServeur < 0)
   {
    printf ("Problemes pour creer la socket");
    return -1;
   }

  printf ("Socket cree\n");

  /* Etape 3 */
  /***********/

  if ( bind ( descripteurDeSocketServeur, 
              (struct sockaddr *)&adresseDuServeur, 
              sizeof(struct sockaddr_in) ) < 0)
   {
    printf ("Problemes pour faire le bind");
    return -1;
   }

  printf ("Socket liee\n");

  /* Etape 4 */
  /***********/

  if ( listen ( descripteurDeSocketServeur, 1) < 0)
   {
    printf ("Problemes pour faire le listen");
    return -1;
   }

  /* Etape 5 */
  /***********/

  while(1)
   {
    int                descripteurDeSocketClient;
    struct sockaddr_in adresseDuClient;
    unsigned int       longueurDeAdresseDuClient;

    printf ("Socket en attente\n");

    descripteurDeSocketClient = accept (descripteurDeSocketServeur,
                                     (struct sockaddr *)&adresseDuClient,
                                     &longueurDeAdresseDuClient);

    /* Etape 6 */
    /***********/

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

    /* Etape 7 */
    /***********/

    printf("\nEcriture de la reponse : %s\n", reponse);

    send(descripteurDeSocketClient, reponse, strlen(reponse), 0);  

    /* Etape 8 */
    /***********/

    close (descripteurDeSocketClient);
   }

  close (descripteurDeSocketServeur);

  return 0;
 }
