/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Interface  <Config> (fichier config.h) ----------------------
#if ! defined ( CONFIG_H )
#define CONFIG_H

//------------------------------------------------------------------------
// Rôle de <Config>
// Contient toutes les definitions de types necessaire à toute
// l'application (l'ensemble des taches de l'application). En particulier
// la definiton d'une voiture et d'une requete
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "Outils.h"
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
typedef struct
{
	TypeUsager type;
	int immatriculation;
	int heureArrivee;
	int heureDepart;
} Voiture;

typedef struct
{
	TypeUsager type;
	int heureArrivee;
} Requete;


#endif // CONFIG_H

