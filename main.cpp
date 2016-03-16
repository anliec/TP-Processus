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
#include <unistd.h>
#include <stlib.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "main.h"
#include "config.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main()
{
	InitialiserApplication( XTERM );
	
	// segment de mem partagee indiquant le nombre places dispo
	int memPlacesDispo = shmget( IPC_PRIVATE, sizeof(int), IPC_CREAT | 600 );
	// segment de memoire partagee contant l'etat de chaque place de parking
	int memParking = shmget( IPC_PRIVATE, sizeof(Voiture) * 8, IPC_CREAT | 600 ); 
	// et le semaphore general d'acces aux deux mem partagees precedentes
	int semParking = semget( IPC_PRIVATE, 2, IPC_CREAT | 600 );
	// boites aux lettres de voitures en entree
	
	// pid de Simulation
	int pidSimul
	if( pidSimul = fork() ) == 0 )
	{
		Simulation(memPlacesDispo, memParking, semParking); 	
	} 
	else
	{
		waitpid(pidSimul, NULL, 0);
		semctl( semParking, IPC_RMID, 0 );
		shmctl( memParking, IPC_RMID, 0 );
		shmctl( memPlacesDispo, IPC_RMID, 0 ); 
		TerminerApplication( true );
		exit(0);
	}
	
	return 0;
} //----- fin de main

