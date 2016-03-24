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
#include <unistd.h>
#include <signal.h>
#include <vector>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Entree.h"
#include "config.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int msgbuffId;
static int mpPlaceDispoId;
static int mpParkingId;
static int semId;

static int *mpPlaceDispo;
static Voiture *mpParking;
//---------------------------------------------------- Fonctions internes
static void init();
static void initId();

static void init()
{
    //catch signals
    struct sigaction sigactionUSR, sigactionCHLD;
    sigactionCHLD.sa_handler = &sigChldHandler;
    sigactionUSR.sa_handler = &sigUsr2Handler;
    sigaction(SIGCHLD, &sigactionCHLD, NULL);
    sigaction(SIGUSR2, &sigactionUSR, NULL);
    //initialistion des ID des resources partagées
    initId();
    //attachement des mémoires partagé
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
    else if(msgbuffId = msgget(keyMsgBuf,DROITS_ACCES) <0)
    {
        std::cerr << "unable to open msgbuf on Entree" << std::endl;
    }
    //récupération de la mémoire partager pour le nombre de place dispo
    key_t keyMpPD = ftok(PATH_TO_MP_PLACEDISPO,PROJECT_ID);
    if(keyMpPD<0)
    {
        std::cerr << "unable to get key for MP on " << PATH_TO_MP_PLACEDISPO << std::endl;
    }
    else if(mpPlaceDispoId=shmget(keyMpPD,0,DROITS_ACCES) <0)
    {
        std::cerr << "unable to open MP PD on Sortie" << std::endl;
    }
    //récupération de la mémoire partager représentant le parking
    key_t keyMpP = ftok(PATH_TO_MP_PARKING,PROJECT_ID);
    if(keyMpP<0)
    {
        std::cerr << "unable to get key for MP on " << PATH_TO_MP_PARKING << std::endl;
    }
    else if(mpParkingId=shmget(keyMpP,0,DROITS_ACCES) <0)
    {
        std::cerr << "unable to open MP Parking on Sortie" << std::endl;
    }
    //récupération des sémaphores
    key_t keySem = ftok(PATH_TO_SEM,PROJECT_ID);
    if(keySem<0)
        std::cerr << "unable to get key for semaphores on Sortie" << std::endl;
    else if(semId=semget(keySem,NUMBER_OF_SEM,DROITS_ACCES) <0)
        std::cerr << "unable to open semaphores on Sortie" << std::endl;
}

static void attachSharedMemory()
{
    if(mpPlaceDispo = shmat(mpParkingId,NULL,0) == NULL)
    {
        std::cerr << "unable to attach shared memory Parking." << std::endl;
    }
    if(mpParking = shmat(mpParkingId,NULL,0) == NULL)
    {
        std::cerr << "unable to attach shared memory PlaceDispo." << std::endl;
    }
}

static void sigChldHandler(int noSig)
{

}

static void sigUsr2Handler(int noSig)
{

}

void semP(unsigned short int sem_num)
{
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_flg = 0;
    op.sem_op = -1;
    semop(semId,&op,1);
}

void semV(unsigned short int sem_num)
{
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_flg = 0;
    op.sem_op = 1;
    semop(semId,&op,1);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


void Entree(TypeBarriere typeBarriere)
{
	//definition des id d'acces au boites aux lettres et semaphores
	int sgBufEntreeId = MSGBUF_ID_ENTREE_P;
	int msgbufRequeteId = MSGBUF_ID_REQUETE_P;
	int msgbufRequeteId = MSGBUF_ID_REQUETE_P;
	switch (typeBarriere)
	{
		case PROF_BLAISE_PASCAL :
			//deja initialises aux bonnes valeurs;
			break;
			
		case AUTRE_BLAISE_PASCAL :
		
			msgBufEntreeId = MSGBUF_ID_ENTREE_A;
			msgbufRequeteId = MSGBUF_ID_REQUETE_A;
			semElementSyncEntree = SEMELM_SINC_ENTREE_A;
			break;

		case ENTREE_GASTON_BERGER :
		
			msgBufEntreeId = MSGBUF_ID_ENTREE_GB;
			msgbufRequeteId = MSGBUF_ID_REQUETE_GB;
			semElementSyncEntree = SEMELM_SINC_ENTREE_GB;
			break;

		default :

			std::cerr << "not an entry process error" << std::endl;
			break;
	}
    //initialisation
    init();

    //phase moteur
    std::vector<pid_t> listeVoiturier;
    while(1)
    {
        Voiture voiture;
        //appel bloquant, attente d'une demande d'entree
        msgrcv(msgbuffId,&voiture, sizeof(Voiture),msgBufEntreeId,0);
		if(mpPlaceDispo[0] > 0)
		{
		
		}
		else
		{
		
		}
        //lance voiturier
        pid_t voiturier = SortirVoiture(message.valeur);
        if(voiturier == -1) //s'il n'y a pas de voiture à cette place on attend une nouvelle demande
            continue;
        listeVoiturier.push_back(voiturier);

        //affiche message de sortie
        semP(SEMELM_MP_PARKING);
        Voiture voiture = mpParkingId[message.valeur];
        semV(SEMELM_MP_PARKING);
        AfficherSortie(voiture.type,voiture.immatriculation,voiture.heureArrivee, voiture.heureDepart);
    }
}
