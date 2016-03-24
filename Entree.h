/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Interface de la tache <Entree> (fichier entree.h) ---------
#if ! defined ( ENTREE_H )
#define ENTREE_H

//------------------------------------------------------------------------
// Rôle du module <Entree>
// [...] TODO
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Entree(TypeBarriere typeBarriere);
// Mode d'emploi :
// Fonction principale du processus de gestion des entrees d'une barriere
// typeBarriere est le type d'entree correspondant au processus
// Contrat :
// msgbufEntreeId = PROF_BLAISE_PASCAL ou AUTRE_BLAISE_PASCAL ou
// ENTREE_GASTON_BERGER

#endif // ENTREE_H

