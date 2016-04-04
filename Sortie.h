/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Interface de la tache <Sortie> (fichier sortie.h) ---------
#if ! defined ( SORTIE_H )
#define SORTIE_H

//------------------------------------------------------------------------
// Rôle du module <Sortie>
// Sortie prend en charge la gestion des demandes de sortie générer pas
// le module Simulation, ce module ce charge aussi d'ordonencer les
// entrées affin de choisir les quels sont prioritaire
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Sortie(int idMsgBuff, int iDMpParking, int idSem);
// Mode d'emploi :
//	Execute toute les actions du module Sortie ainsi que sont
//  inisialisation
// Contrat :
//	affin de garantir le fonctionnement de cette methode il faut que les
//  différents objets partagé dont les id sont passé en paramètre soit
//  corectement initialisé.

void Sortie();
// Mode d'emploi :
//	Execute toute les actions du module Sortie ainsi que sont
//  inisialisation
// Contrat :
//	affin de garantir le fonctionnement de cette methode il faut que les
//  différents objets partagé soit corectement initialiser (cf config.h
//  pour avoir les paramètre utilisé).

#endif // SORTIE_H

