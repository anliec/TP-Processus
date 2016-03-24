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
#include "config.h"
#include "Simulation.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
const int DROITS_ACCES = 0660;
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main()
{
	InitialiserApplication( XTERM );
	
	// segment de mem partagee indiquant le nombre places dispo
	int memPlacesDispo = shmget(ftok(PATH_TO_MP_PLACEDISPO, PROJECT_ID), 
		sizeof(int), IPC_CREAT | DROITS_ACCES);
	
	// segment de memoire partagee contant l'etat de chaque place de 
	// parking
	int memParking = shmget(ftok(PATH_TO_MP_PARKING, PROJECT_ID),
		sizeof(Voiture) * 8, IPC_CREAT | DROITS_ACCES); 
	
	// le semaphore general contenant tous les semaphores
	int sem = semget(ftok(PATH_TO_SEM, PROJECT_ID), NUMBER_OF_SEM, 
		IPC_CREAT | DROITS_ACCES);
	
	// boites aux lettres generale
	int msgbuf = msgget(ftok(PATH_TO_MSGBUF, PROJECT_ID), IPC_CREAT 
		| DROITS_ACCES);
	
	// pid de Simulation
	int pidSimul;
	if((pidSimul = fork()) == 0)
	{
		Simulation(0); 	
	} 
	else
	{
		waitpid(pidSimul, NULL, 0);
		msgctl(msgbuf, IPC_RMID, 0);
		semctl(sem, IPC_RMID, 0);
		shmctl(memParking, IPC_RMID, 0);
		shmctl(memPlacesDispo, IPC_RMID, 0); 
		TerminerApplication( true );
		exit(0);
	}
	
	return 0;
} //----- fin de main

