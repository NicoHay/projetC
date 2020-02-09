Pour tester le projet 


--- Dans un premier terminal  tapez :

    make
    make run PORT1=<port1> PORT2=<port2>


--- Dans un second terminal tapez :

    make
    make run PORT1=<port2> PORT2=<port1>


PORT1 sera le port du process local
PORT2 sera le port du second process

/!\ PENSEZ A INVERSER LA VALEUR DES PORTS D'UN TERMINAL A L'AUTRE 


    Pour changer le nombre d'itérations du programme (par default 100): 
    Modifiez la variable NBRE_TOURS  dans le fichier <projet.h> 

