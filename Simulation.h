/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Interface de la tache <Simulation> (fichier simulation.h) ---------
#if ! defined ( SIMULATION_H )
#define SIMULATION_H

//------------------------------------------------------------------------
// Rôle du module <Simulation>
// [...] TODO
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Simulation(int balGenerale);
// Mode d'emploi :
//	TODO
// Contrat :
//	Aucun

void Commande(char code, unsigned valeur);

#endif // SIMULATION_H

