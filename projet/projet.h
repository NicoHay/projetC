#ifndef PROJET_H
#define PROJET_H

//==============================
// VARIABLES GLOBALES 
//==============================


#define NBRE_TOURS  15 

// STRUCTS
extern struct Messageinfos {
    int      compteurinterne;
    int      estampille;
    int      monpid;
}Messageinfos;  

//VARIABLES
extern struct Messageinfos  zmClientBrain[50];
extern struct Messageinfos  zmServerBrain[50];


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
int     randomNum();
void    messageBidon();
void    simcritique();
void    zoneCritique();
void    actionInterne();
void    semaphoreZoneMemoireClient(struct Messageinfos mess1 );
void    semaphoreZoneMemoireServeur(struct Messageinfos mess2);

#endif