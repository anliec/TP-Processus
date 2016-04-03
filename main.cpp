/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Réalisation du module <Main> (fichier main.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "main.h"
#include "Simulation.h"
#include "config.h"
#include "Sortie.h"
#include "Entree.h"
#include "Heure.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main()
{
	InitialiserApplication( XTERM );
		
	// segment de memoire partagee contant l'etat de chaque place de 
	// parking
	int memParking = shmget(ftok(PATH_TO_MP_PARKING, PROJECT_ID),
		sizeof(Voiture) * NB_PLACES, IPC_CREAT | DROITS_ACCES); 
	
	// le semaphore general contenant tous les semaphores
	int sem = semget(ftok(PATH_TO_SEM, PROJECT_ID), NUMBER_OF_SEM, 
		IPC_CREAT | DROITS_ACCES);
	// initalisation des semaphores elementaires
	// nombre de places dispos 
	semctl(sem, SEMELM_PLACEDISPO, SETVAL, 8);
	// exlusion mutuelle d'acces a la mp
	semctl(sem, SEMELM_MP_PARKING, SETVAL, 1);
	// semaphores de priorite d'entree lorsque le parking est plein
	semctl(sem, SEMELM_SINC_ENTREE_A, SETVAL, 0);
	semctl(sem, SEMELM_SINC_ENTREE_GB, SETVAL, 0);
	semctl(sem, SEMELM_SINC_ENTREE_P, SETVAL, 0);
	
	// boites aux lettres generale
	int msggen = msgget(ftok(PATH_TO_MSGBUF, PROJECT_ID), IPC_CREAT 
		| DROITS_ACCES);
	
	// pid des processus fils
	int pidSimul, pidEntreeP, pidEntreeA, pidEntreeGB, pidSortie, pidHeure;
	if((pidSimul = fork()) == 0)
	{
		Simulation();
	} 
	else if((pidSortie = fork()) == 0)
	{
		Sortie();
	}
	else if((pidEntreeP = fork()) == 0)	
	{
		Entree(PROF_BLAISE_PASCAL);
	}
	else if((pidEntreeA = fork()) == 0)	
	{
		Entree(AUTRE_BLAISE_PASCAL);
	}
	else if((pidEntreeGB = fork()) == 0)
	{
		Entree(ENTREE_GASTON_BERGER);
	}
	else 
	{	
		bool heureActive = ((pidHeure = ActiverHeure()) != -1);
		
		// attend que l'arret de Simulation indique la fin du programme
		waitpid(pidSimul, NULL, 0);
		// arret de l'heure, si activee
		if(heureActive)
		{
			kill(pidHeure, SIGUSR2);
			waitpid(pidHeure, NULL, 0);
		}
		// arret de Sortie
		kill(pidSortie, SIGUSR2);
		waitpid(pidSortie, NULL, 0);
		// arret des Entree
		kill(pidEntreeP, SIGUSR2);
		waitpid(pidEntreeP, NULL, 0);
		kill(pidEntreeA, SIGUSR2);
		waitpid(pidEntreeA, NULL, 0);
		kill(pidEntreeGB, SIGUSR2);
		waitpid(pidEntreeGB, NULL, 0);
		// liberation des ressources partagees
		msgctl(msggen, IPC_RMID, 0);
		semctl(sem, IPC_RMID, 0);
		shmctl(memParking, IPC_RMID, 0);
		TerminerApplication( true );
		exit(0);
	}
	
	return 0;
} //----- fin de main

