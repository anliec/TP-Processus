/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Realisation de la tache <Entree> (fichier Entree.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <map>
#include <iostream>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Entree.h"
#include "config.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int msgbuffId;
static int mpParkingId;
static int semId;

static Voiture *mpParking;
// structure combinant les voiturier et les voitures qu'il garent
// jamais deux processus de voiturier n'auront le meme pid et on doit
// pouvoir les retrouver dans n'importe quel ordre, donc utilisation d'une
// map
static std::map<pid_t, Voiture> mapVoiturier;
//---------------------------------------------------- Fonctions internes
static void init();
static void initId();
static void attachSharedMemory();
static void sigChldHandler(int noSig, siginfo_t *sigInfo, void *context);
static void sigUsr2Handler(int noSig);
static void semP(unsigned short sem_num, bool saRestart=true);
static void semV(unsigned short sem_num);
static int semVal(int semNum);


static void init()
{
    struct sigaction sigactionUSR, sigactionCHLD;
    //associer le signal SIGCHLD a son handler
    sigactionCHLD.sa_sigaction = &sigChldHandler;
    sigemptyset(&(sigactionCHLD.sa_mask));
    sigactionCHLD.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &sigactionCHLD, NULL);
    //associer le signal SIGUSR2 a son handler
    sigactionUSR.sa_handler = &sigUsr2Handler;
    sigemptyset(&(sigactionUSR.sa_mask));
    sigaddset(&(sigactionUSR.sa_mask), SIGCHLD);
    // on recoit SIGUSR2 une seule fois avant destruction de ce processus
    sigactionUSR.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR2, &sigactionUSR, NULL);
    //initialistion des ID des resources partagees
    initId();
    //attachement des memoires partagees
    attachSharedMemory();
}

static void initId()
{
    //récupération de la boite au lettre:
    key_t keyMsgBuf = ftok(PATH_TO_MSGBUF,PROJECT_ID);
    if(keyMsgBuf<0)
    {
        std::cerr << "unable to get key for msgbuf on " << PATH_TO_MSGBUF << std::endl;
    }
    else if((msgbuffId = msgget(keyMsgBuf,DROITS_ACCES)) < 0)
    {
        std::cerr << "unable to open msgbuf on Entree" << std::endl;
    }
    //récupération de la mémoire partager représentant le parking
    key_t keyMpP = ftok(PATH_TO_MP_PARKING,PROJECT_ID);
    if(keyMpP<0)
    {
        std::cerr << "unable to get key for MP on " << PATH_TO_MP_PARKING << std::endl;
    }
    else if((mpParkingId=shmget(keyMpP,0,DROITS_ACCES)) < 0)
    {
        std::cerr << "unable to open MP Parking on Entree" << std::endl;
    }
    //récupération des sémaphores
    key_t keySem = ftok(PATH_TO_SEM,PROJECT_ID);
    if(keySem<0)
        std::cerr << "unable to get key for semaphores on Entree" << std::endl;
    else if((semId=semget(keySem,NUMBER_OF_SEM,DROITS_ACCES)) <0)
        std::cerr << "unable to open semaphores on Entree" << std::endl;
}

static void attachSharedMemory()
{
    if((mpParking = (Voiture *)shmat(mpParkingId,NULL,0)) == NULL)
    {
        std::cerr << "unable to attach shared memory Parking." << std::endl;
    }
}

static void sigChldHandler(int noSig, siginfo_t *siginfo, void *context)
{
	std::map<pid_t, Voiture>::iterator voiturierIt = mapVoiturier.find(siginfo->si_pid);
	if(voiturierIt == mapVoiturier.end())
    {
    	return; //si le signal n'a pas ete envoye par un voiturier
    }
    else
    {
        //affiche sur l'IHM
        AfficherPlace(siginfo->si_status,voiturierIt->second.typeUsager,voiturierIt->second.immatriculation,
                      voiturierIt->second.heureArrivee);
        //ajoute la voiture au parking
    	semP(SEMELM_MP_PARKING);
    	mpParking[siginfo->si_status] = voiturierIt->second;
    	semV(SEMELM_MP_PARKING);
    	mapVoiturier.erase(voiturierIt);
    }
}

static void sigUsr2Handler(int noSig)
{
	// phase destruction
	// liberation de SIGCHLD
	struct sigaction sigactionCHLD;
	sigactionCHLD.sa_handler = SIG_DFL;
	sigactionCHLD.sa_flags = 0;
	sigaction(SIGCHLD, &sigactionCHLD, NULL);
	for(std::pair<pid_t,Voiture> voiturier : mapVoiturier)
	{
		kill(voiturier.first, SIGUSR2);
		waitpid(voiturier.first, NULL, 0);
	}
	// detachement auto de la memoire partagee a l'arret du porcessus
	exit(0);
}

static int semVal(int semNum)
{
	return semctl(semId, semNum, GETVAL, 0);
}

static void semP(unsigned short semNum, bool saRestart)
{
    struct sembuf op;
    op.sem_num = semNum;
    op.sem_flg = 0;
    op.sem_op = -1;
    int returnValue;
    do{
        returnValue = semop(semId,&op,1);
    }while(returnValue==-1 && saRestart);
}

static void semV(unsigned short semNum)
{
    struct sembuf op;
    op.sem_num = semNum;
    op.sem_flg = 0;
    op.sem_op = 1;
    semop(semId,&op,1);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


void Entree(TypeBarriere typeBarriere)
{
	//definition des id d'acces au boites aux lettres et semaphores
	long msgBufEntreeId;
	long msgbufRequeteId;
	unsigned short semElementSyncEntree;
    TypeZone zoneEcranRequete;
	switch (typeBarriere)
	{
		case PROF_BLAISE_PASCAL :
			//deja initialises aux bonnes valeurs;
			msgBufEntreeId = MSGBUF_ID_ENTREE_P;
			msgbufRequeteId = MSGBUF_ID_REQUETE_P;
	 		semElementSyncEntree = SEMELM_SINC_ENTREE_P;
            zoneEcranRequete = REQUETE_R1;
			break;
			
		case AUTRE_BLAISE_PASCAL :
		
			msgBufEntreeId = MSGBUF_ID_ENTREE_A;
			msgbufRequeteId = MSGBUF_ID_REQUETE_A;
			semElementSyncEntree = SEMELM_SINC_ENTREE_A;
            zoneEcranRequete = REQUETE_R2;
			break;

		case ENTREE_GASTON_BERGER :
		
			msgBufEntreeId = MSGBUF_ID_ENTREE_GB;
			msgbufRequeteId = MSGBUF_ID_REQUETE_GB;
			semElementSyncEntree = SEMELM_SINC_ENTREE_GB;
            zoneEcranRequete = REQUETE_R3;
			break;

		default :

			std::cerr << "not an entry process error" << std::endl;
            return;
			break;
	}
    //initialisation
    init();

    //phase moteur
    pid_t pidCurr;
    Voiture voiture;
    for(;;)
    {
        //appel bloquant, attente d'une demande d'entree
        if(msgrcv(msgbuffId, &voiture, sizeof(Voiture), msgBufEntreeId, 0) == -1)
            continue; // sarestart (sur les erreurs aussi)
        //std::cerr << "voiture get on msgBuf: " << msgBufEntreeId << std::endl;
		DessinerVoitureBarriere(typeBarriere, voiture.typeUsager);
		
		if(semVal(SEMELM_PLACEDISPO) > 0)
		{
			if((pidCurr = GarerVoiture(typeBarriere)) != -1)
			{
				semP(SEMELM_PLACEDISPO);
				mapVoiturier.insert(std::pair<pid_t, Voiture>(pidCurr, voiture));
			}
		}
		else
		{
            //affiche requete
            AfficherRequete(typeBarriere,voiture.typeUsager,voiture.heureArrivee);
            Requete re;
            re.type = msgbufRequeteId;
            re.heureArrivee = voiture.heureArrivee;
            re.typeUsager = voiture.typeUsager;
            if(msgsnd(msgbuffId, &re, sizeof(Requete), 0 )==-1);
                //cerr << "not posted: " << errno << endl;
            //std::cerr << "posted on " << msgbuffId << " | " << re.type << std::endl;
            //std::cerr << "wait a GO  " << semVal(semElementSyncEntree) << std::endl;
			semP(semElementSyncEntree);
            //std::cerr << "get a GO ! " << semVal(semElementSyncEntree) << std::endl;
			if((pidCurr = GarerVoiture(typeBarriere)) != -1)
			{
                //met à jour l'heure d'entree dans le parking
                voiture.heureArrivee = time(NULL);
				mapVoiturier.insert(std::pair<pid_t, Voiture>(pidCurr, voiture));
			}
            Effacer(zoneEcranRequete);
		}
    }
}

