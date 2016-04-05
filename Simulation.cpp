/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Realisation de la tache <Simulation> (fichier simulation.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <time.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Menu.h"
#include "config.h"
#include "Simulation.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int msgbuffId;
static int plaqueCourante;

//------------------------------------------------------- Fonctions privee
static void init();
static void moteur();
static void envoyerVoiture(unsigned barriere, TypeUsager typeUtilisateur);
static void sortieVoiture(unsigned numPlace);

//////////////////////////////////////////////////////////////////  PUBLIC

//---------------------------------------------------- Fonctions publiques
void Simulation(int idMsgBuff)
{
	msgbuffId = idMsgBuff;
	moteur();
}

void Simulation()
{
	init();
	moteur();
}

static void moteur()
{
	for (;;)
	{
		Menu();
	}
}

void Commande(char code, unsigned int valeur)
{
	switch(code)
	{
		case 'E':
			exit(0);
			break;
		case 'P':
			envoyerVoiture(2*valeur-1,PROF);
			break;
		case 'A':
			envoyerVoiture(valeur+1,AUTRE);
			break;
		case 'S':
			sortieVoiture(valeur);
			break;
	}
}

static void init()
{
	plaqueCourante = 1;
	//récupération de la boite au lettre:
	key_t keyMsgBuf = ftok(PATH_TO_MSGBUF,PROJECT_ID);
	if(keyMsgBuf<0)
	{
		std::cerr << "unable to get key for msgbuf on " << PATH_TO_MSGBUF << std::endl;
		exit(2);
	}
	else if((msgbuffId = msgget(keyMsgBuf,0660)) <0)
	{
		std::cerr << "unable to open msgbuff on Sortie" << std::endl;
	}
}

static void envoyerVoiture(unsigned barriere, TypeUsager typeUtilisateur)
{
	Voiture voiture;
	voiture.typeUsager = typeUtilisateur;
	voiture.heureArrivee = time(NULL);
	voiture.immatriculation = plaqueCourante++;
	switch (barriere)
	{
		case PROF_BLAISE_PASCAL:
			voiture.type = MSGBUF_ID_ENTREE_P;
			break;
		case AUTRE_BLAISE_PASCAL:
			voiture.type = MSGBUF_ID_ENTREE_A;
			break;
		case ENTREE_GASTON_BERGER:
			voiture.type = MSGBUF_ID_ENTREE_GB;
			break;
		default:
			return;
	}
	msgsnd(msgbuffId,&voiture,sizeof(Voiture)-sizeof(long),0);
}

static void sortieVoiture(unsigned numPlace)
{
	CommandeStruct cmd;
	cmd.valeur = numPlace;
	cmd.type = MSGBUF_ID_SORTIE;
	msgsnd(msgbuffId,&cmd,sizeof(CommandeStruct)-sizeof(long),0);
}




