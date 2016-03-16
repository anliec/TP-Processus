/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Réalisation du module <Simulation> (fichier simulation.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Menu.h"
#include "Simulation.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Fonctions privee
void envoyerVoiture(unsigned barriere, unsigned typeUtilisateur)
{

}

void sortieVoiture(unsigned numPlace)
{

}

//---------------------------------------------------- Fonctions publiques
void Simulation()
{
	//TODO boucle infini ???
	for(;;)
	{
		Menu();
	}
}

void Commande(char code, unsigned valeur)
{
	switch(code)
	{
		case 'E':
			exit(0);
			break;
		case 'P':
			break;
		case 'A':
			break;
		case 'S':
			break;
	}
}

