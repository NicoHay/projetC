/*****************/
/* Socket client */
/*****************/

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define nomMachine "localhost"
#define ipMachine  "127.0.0.1"
#define numPort    55555
#define requete    "bonjour"

int main (int argc, char** argv)
 {
  /* Déclaration de l'adresse IP, du numéro de port et du protocole */
  /******************************************************************/

  struct sockaddr_in adr;

  adr.sin_family      = PF_INET;
  adr.sin_port        = htons(numPort);
  adr.sin_addr.s_addr = inet_addr (ipMachine);

  /* Recherche de l'adresse IP avec la fonction gethostbyname */
  /************************************************************/

  struct hostent     *host;

  if ((host = gethostbyname(nomMachine)) == NULL)
   {
    perror("gethostbyname");
    return -1;
   }

  memcpy (&(adr.sin_addr.s_addr), host->h_addr_list[0], host->h_length);


  /* Affichage des informations de connexion */
  /*******************************************/

  printf ("Connexion vers la machine ");

  unsigned char* adrIP = (unsigned char*) &(adr.sin_addr.s_addr);

  printf ( "%d.",  *(adrIP    ) );
  printf ( "%d.",  *(adrIP + 1) );
  printf ( "%d.",  *(adrIP + 2) );
  printf ( "%d" ,  *(adrIP + 3) );
  printf (" sur le port %u \n", numPort);

  /* Creation de la socket */
  /*************************/

  int descripteurDeSocket = socket (AF_INET, SOCK_STREAM, 0);

  if (descripteurDeSocket < 0)
   {
    printf ("Problemes pour creer la socket");
    return -1;
   }

  printf ("socket cree\n");

  /* Connexion de la socket au serveur */
  /*************************************/

  if (connect ( descripteurDeSocket,
                (struct sockaddr *) &adr,
                sizeof(adr)                ) < 0)
   {
    printf ("Problemes pour se connecter au serveur");
    return -1;
   }

  printf ("socket connectee\n");

  /* Envoi la requête grâce à send */
  /*********************************/

  printf ("\nEnvoi de la requete : %s \n", requete);

  send(descripteurDeSocket, requete, strlen(requete), 0);

  /* Lecture la réponse grâce à recv */
  /***********************************/

  char          buffer[1024];
  unsigned int  nbCaracteres;
  unsigned int  i;

  printf ("\nReception de la reponse : \n");

  do
   {
    nbCaracteres = recv (descripteurDeSocket, buffer, 1024, 0);

    for (i=0; i<nbCaracteres; i++)
     printf ("%c", buffer [i]);
   }
   while (nbCaracteres == 1024);

  printf ("\n\nFIN\n\n");

  /* Fermeture de la connexion */
  /*****************************/

  close(descripteurDeSocket);

  return 0;
 }