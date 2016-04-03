/*************************************************************************
                           Parking  -  description
                             -------------------
    debut                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Interface du module <config> (fichier config.h) -------------
#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------------------
// Role de <config>
// Contient toutes les definitions de types et constantes et fonctions de 
// gestion de semaphores necessaires a toute l'application (l'ensemble des 
// taches de l'application). En particulier la definiton d'une voiture et 
// d'une requete
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisees
#include "Outils.h"
//------------------------------------------------------------- Constantes
const int PROJECT_ID = 0;
const int DROITS_ACCES = 0660;
// boite aux lettres
const char* const PATH_TO_MSGBUF = "msgbuf";
const long MSGBUF_ID_ENTREE_P = 1;
const long MSGBUF_ID_ENTREE_A = 2;
const long MSGBUF_ID_ENTREE_GB = 3;
const long MSGBUF_ID_SORTIE = 4;
const long MSGBUF_ID_REQUETE_P = 5;
const long MSGBUF_ID_REQUETE_A = 6;
const long MSGBUF_ID_REQUETE_GB = 7;
// sémaphore
const int NUMBER_OF_SEM = 5;
const char* const PATH_TO_SEM = "semaphore";
const int SEMELM_PLACEDISPO = 0;
const int SEMELM_MP_PARKING = 1;
const int SEMELM_SINC_ENTREE_P = 2;
const int SEMELM_SINC_ENTREE_A = 3;
const int SEMELM_SINC_ENTREE_GB = 4;
// mémoires partagées
const char* const PATH_TO_MP_PARKING = "mpParking";

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
	long type;
	unsigned valeur;
} CommandeStruct;


#endif // CONFIG_H

