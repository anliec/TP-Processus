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
#include <unistd.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "main.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main()
{
	InitialiserApplication ( XTERM );
	sleep(10);
	TerminerApplication ( false );
} //----- fin de main

