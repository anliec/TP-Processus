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
// Le module Simulation a pour but de recevoir et tranmettre toute les
// infos recus de la part de l'interface graphique vers le reste du
// programme.
//
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Simulation(int idMsgBuff);
// Mode d'emploi :
//	Execute toute les actions du module Simulation ainsi que sont
//  inisialisation.
// Contrat :
//	La boite au lettre dont l'id est fournie en paramettre est corectement
//  initialisé.

void Simulation();
// Mode d'emploi :
//	Execute toute les actions du module Simulation ainsi que sont
//  inisialisation.
// Contrat :
//	La boite au lettre défini dans config.h est corectement initialisé.

void Commande(char code, unsigned int valeur);

#endif // SIMULATION_H

