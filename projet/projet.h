#ifndef PROJET_H
#define PROJET_H

//==============================
// VARIABLES GLOBALES 
//==============================

#define NBRE_TOURS  100     //nombre de tours à effectuer
#define MAX(x, y) ( ( (x) > (y) ) ? (x) : (y) )   //recupère la plus grande valeur de deux int


// STRUCTS
extern struct Messageinfos {
    int      estampille;
    int      monpid;
    int      action;
}Messageinfos;  

extern struct arg_struct {
    int         action;
    int         port;
    pthread_t  thread;
}args;

//VARIABLES
extern struct Messageinfos  zmClientBrain[NBRE_TOURS];
extern struct Messageinfos  zmServerBrain[NBRE_TOURS];

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
void    semaphoreZoneMemoireClient(struct Messageinfos mess);
void    semaphoreZoneCritique();
void    printlogo();

#endif