#ifndef PROJET_H
#define PROJET_H

//==============================
// VARIABLES GLOBALES 
//==============================


#define NBRE_TOURS  30 

//VARIABLES
extern int    zmClientBrain[50] ;
extern int    zmServerBrain[50];



// STRUCTS
extern struct Messageinfos {
    int      compteurinterne;
    int      estampille;
    int      monpid;
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
int     randomNum();
void    messageBidon();
void    simcritique();
void    zoneCritique();
void    actionInterne();
void    semaphoreZoneMemoireServeur(int num);
void    semaphoreZoneMemoireClient(int num1 );

#endif