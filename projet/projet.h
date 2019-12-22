#ifndef PROJET_H
#define PROJET_H

//==============================
// VARIABLES GLOBALES 
//==============================

#define requete     "CLIENT  +++ message du client => bonjour ici le client"
#define reponse     "SERVEUR +++ message du serveur => bonsoir ici le serveur"
#define NBRE_TOURS  10

//VARIABLES
// extern int    compteurInterne ;
// extern int    monIndex ;
// extern int    continuer;
extern int    zmClientBrain[50] ;
extern int    zmServerBrain[50];
extern int    listePort[2];


// STRUCTS


extern struct Messageinfos {
    int      indexinterne;
    int      estampille;
    char     text[50];
    int      zonecritique;
}Messageinfos;  


extern struct arg_struct {
    int         action;
    int         port;
    int         nbre_tour;
    pthread_t  thread;
}args;

//==============================
// PROTOTYPE DE FONCTIONS 
//==============================

void*   theBrain(void* arguments);
void*   serveur(void* arg);
void*   client(void* arg);
int     randomAction();
void    messageBidon();
void    simcritique();
void    zoneCritique();
void    actionInterne();

#endif