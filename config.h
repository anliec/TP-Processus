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
#include <string>
#include "Outils.h"
//------------------------------------------------------------- Constantes
const int PROJECT_ID = 0;
//boite aux lettres
const std::string PATH_TO_MSGBUF = "msgbuf";
const long MSGBUF_ID_ENTREE_P = 0;
const long MSGBUF_ID_ENTREE_A = 1;
const long MSGBUF_ID_ENTREE_GB = 2;
const long MSGBUF_ID_SORTIE = 3;
//sémaphore
const int NUMBER_OF_SEM = 5;
const std::string PATH_TO_SEM = "semaphore";
const int SEMELM_MP_PLACEDISPO = 0;
const int SEMELM_MP_PARKING = 1;
const int SEMELM_SINC_ENTREE_P = 2;
const int SEMELM_SINC_ENTREE_A = 3;
const int SEMELM_SINC_ENTREE_GB = 4;
//mémoires partagées
const std::string PATH_TO_MP_PLACEDISPO = "mpPlaceDispo";
const std::string PATH_TO_MP_PARKING = "mpParking";

//------------------------------------------------------------------ Types
typedef struct
{
	TypeUsager type;
	unsigned immatriculation;
	time_t heureArrivee;
	time_t heureDepart;
} Voiture;

typedef struct
{
	TypeUsager type;
	time_t heureArrivee;
} Requete;

typedef  struct
{
	unsigned valeur;
} Commande;


#endif // CONFIG_H

