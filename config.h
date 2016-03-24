/*************************************************************************
                           Parking  -  description
                             -------------------
    debut                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Interface  <Config> (fichier config.h) ----------------------
#if ! defined ( CONFIG_H )
#define CONFIG_H

//------------------------------------------------------------------------
// Role de <Config>
// Contient toutes les definitions de types et constantes necessaire a
// toute l'application (l'ensemble des taches de l'application). En
// particulier la definiton d'une voiture et d'une requete
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisees
#include "Outils.h"
//------------------------------------------------------------- Constantes
const int PROJECT_ID = 0;
const int DROITS_ACCES = 0660;
// boite aux lettres
const char *PATH_TO_MSGBUF = "msgbuf";
const long MSGBUF_ID_ENTREE_P = 0;
const long MSGBUF_ID_ENTREE_A = 1;
const long MSGBUF_ID_ENTREE_GB = 2;
const long MSGBUF_ID_SORTIE = 3;
const long MSGBUF_ID_REQUETE_P = 4;
const long MSGBUF_ID_REQUETE_A = 5;
const long MSGBUF_ID_REQUETE_GB = 6;

// sémaphore
const int NUMBER_OF_SEM = 5;
const char *PATH_TO_SEM = "semaphore";
const int SEMELM_MP_PLACEDISPO = 0;
const int SEMELM_MP_PARKING = 1;
const int SEMELM_SINC_ENTREE_P = 2;
const int SEMELM_SINC_ENTREE_A = 3;
const int SEMELM_SINC_ENTREE_GB = 4;
// mémoires partagées
const char* PATH_TO_MP_PLACEDISPO = "mpPlaceDispo";
const char* PATH_TO_MP_PARKING = "mpParking";
const char* PATH_TO_MP_ENTREE_P = "mpDemandeEntreeP";
const char* PATH_TO_MP_ENTREE_A = "mpDemandeEntreeA";
const char* PATH_TO_MP_ENTREE_GB = "mpDemandeEntreeGB";

//------------------------------------------------------------------ Types
// Structure du message : voiture en entree ou en sortie
typedef struct
{
	long type;
	TypeUsager typeUsager;
	unsigned immatriculation;
	time_t heureArrivee;
	time_t heureDepart;
} Voiture;

// Structure du message : requete de voiture en entree alors que le parking
// est plein
typedef struct
{
	long type;
	TypeUsager typeUsager;
	time_t heureArrivee;
} Requete;

typedef  struct
{
	unsigned valeur;
} CommandeStruct;


#endif // CONFIG_H

