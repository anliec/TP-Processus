/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 16/03/2016
    copyright            : (C)2016 par B3129
    e-mail               : pierre-louis.lefebvre@insa-lyon.fr
    					   nicolas.six@insa-lyon.fr
*************************************************************************/


//---------- Realisation de la tache <Sortie> (fichier sortie.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <list>
#include <iostream>
#include <algorithm>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Sortie.h"
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

static std::list<pid_t> listeVoiturier;

//------------------------------------------------------- Fonctions privee
static void init();
static void initId();
static void attachSharedMemory();
static void sigChldHandler(int signum,siginfo_t *siginfo,void* ucontext);
static void sigUsr2Handler(int signum);
static void semP(unsigned short int sem_num);
static void semV(unsigned short int sem_num);

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


void Sortie()
{
    //initialisation
    init();

    //phase moteur
    while(1)
    {
        CommandeStruct message;
        //appel bloquant, attente d'une demande de sortie
        msgrcv(msgbuffId,&message, sizeof(CommandeStruct),MSGBUF_ID_SORTIE,0);

        //lance voiturier
        pid_t voiturier = SortirVoiture(message.valeur);
        if(voiturier == -1) //s'il n'y a pas de voiture à cette place on attend une nouvelle demande
            continue;
        listeVoiturier.push_back(voiturier);

        //affiche message de sortie
        semP(SEMELM_MP_PARKING);
        Voiture voiture = mpParking[message.valeur];
        semV(SEMELM_MP_PARKING);
        AfficherSortie(voiture.typeUsager,voiture.immatriculation,voiture.heureArrivee, voiture.heureDepart);
    }
}

static void init()
{
    //catch signals
    struct sigaction sigactionUSR, sigactionCHLD;
    sigactionCHLD.sa_sigaction = &sigChldHandler;
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
    else if((msgbuffId = msgget(keyMsgBuf,0660)) <0)
    {
        std::cerr << "unable to open msgbuff on Sortie" << std::endl;
    }
    //récupération de la mémoire partager pour le nombre de place dispo
    key_t keyMpPD = ftok(PATH_TO_MP_PLACEDISPO,PROJECT_ID);
    if(keyMpPD<0)
    {
        std::cerr << "unable to get key for MP on " << PATH_TO_MP_PLACEDISPO << std::endl;
    }
    else if((mpPlaceDispoId=shmget(keyMpPD,0,0660)) <0)
    {
        std::cerr << "unable to open MP PD on Sortie" << std::endl;
    }
    //récupération de la mémoire partager représentant le parking
    key_t keyMpP = ftok(PATH_TO_MP_PARKING,PROJECT_ID);
    if(keyMpP<0)
    {
        std::cerr << "unable to get key for MP on " << PATH_TO_MP_PARKING << std::endl;
    }
    else if((mpParkingId=shmget(keyMpP,0,0660)) <0)
    {
        std::cerr << "unable to open MP Parking on Sortie" << std::endl;
    }
    //récupération des sémaphores
    key_t keySem = ftok(PATH_TO_SEM,PROJECT_ID);
    if(keySem<0)
        std::cerr << "unable to get key for semaphores on Sortie" << std::endl;
    else if((semId=semget(keySem,NUMBER_OF_SEM,0660)) <0)
        std::cerr << "unable to open semaphores on Sortie" << std::endl;
}

static void attachSharedMemory()
{
    if((mpPlaceDispo = (int*)shmat(mpParkingId,NULL,0)) == NULL)
    {
        std::cerr << "unable to attach shared memory Parking." << std::endl;
    }
    if((mpParking = (Voiture*)shmat(mpParkingId,NULL,0)) == NULL)
    {
        std::cerr << "unable to attach shared memory PlaceDispo." << std::endl;
    }
}

static void sigChldHandler(int signum,siginfo_t *siginfo,void* ucontext)
{
    list<pid_t>::iterator voiturier = find(listeVoiturier.begin(),listeVoiturier.end(),siginfo->si_pid);
    if(voiturier == listeVoiturier.end())
        return; //si pour une raison quelconque ce n'était pas un voiturier

    int ret;
    waitpid(siginfo->si_pid,&ret,0);
    listeVoiturier.erase(voiturier);
    //vide la place de parking     ->   désactiver car inuitle (qui va aller vérifier ???)
    /*Voiture voitureNull;
    semP(SEMELM_MP_PARKING);
    mpParking[ret] = voitureNull;
    semV(SEMELM_MP_PARKING);*/
    //ajoute une place

    if(*mpPlaceDispo > 0) // s'il y avait déjà des places dispo pas la peine de chercher plus loin on ajoute juste une place disponible
    {
        semP(SEMELM_MP_PLACEDISPO);
        (*mpPlaceDispo)++;
        semV(SEMELM_MP_PLACEDISPO);
    }
    else // sinon en plus de rajouter une place il faut aussi dire quel entrée doit s'ouvrir
    {
        //TODO
        bool waitingAtA, waitingAtP, waitingAtGB;
        Requete rA, rP, rGB;
        waitingAtA  = msgrcv(msgbuffId,&rA, sizeof(Requete),MSGBUF_ID_REQUETE_A, MSG_COPY | IPC_NOWAIT) != -1;
        waitingAtP  = msgrcv(msgbuffId,&rP, sizeof(Requete),MSGBUF_ID_REQUETE_P, MSG_COPY | IPC_NOWAIT) != -1;
        waitingAtGB = msgrcv(msgbuffId,&rGB,sizeof(Requete),MSGBUF_ID_REQUETE_GB,MSG_COPY | IPC_NOWAIT) != -1;
        //On trouve qui est plus prioritaire
        Requete *nextIn = nullptr;
        int semEntree;
        if(waitingAtP)
        {
            nextIn = &rP;
            semEntree = SEMELM_SINC_ENTREE_P;
        }
        if(waitingAtGB)
        {
            if(nextIn==nullptr || (nextIn->heureArrivee > rGB.heureArrivee && rGB.typeUsager==PROF))
            {
                nextIn = &rGB;
                semEntree = SEMELM_SINC_ENTREE_GB;
            }
        }
        if(waitingAtA)
        {
            if(nextIn==nullptr || (nextIn->typeUsager==AUTRE && nextIn->heureArrivee > rA.heureArrivee))
            {
                nextIn = &rA;
                semEntree = SEMELM_SINC_ENTREE_A;
            }
        }
        if(nextIn!= nullptr) //si on a trouver quelqu'un on lui donne l'autorisation pour rentrer
        {
            msgrcv(msgbuffId,NULL,nextIn->type,sizeof(Requete),0); //retire la requette acepter de la boite aux lettre
            semV(semEntree);//donne l'autorisation de rentrer
        }
        else //si le parking était plein mais que personne n'attendait on ajoute une place libre
        {
            semP(SEMELM_MP_PLACEDISPO);
            (*mpPlaceDispo)++;
            semV(SEMELM_MP_PLACEDISPO);
        }
    }
}

static void sigUsr2Handler(int signum)
{
    //desactive sigChldHandler
    struct sigaction resetAction;
    resetAction.sa_handler = SIG_IGN;//pour ignorer les prochains signaux
    sigaction(SIGCHLD,&resetAction,NULL);
    //arrête tout les voiturier un à un
    for(pid_t pid:listeVoiturier)
    {
        kill(pid,SIGUSR2);
        waitpid(pid,NULL,0);
    }
    exit(0); //un peut brutale peut-être
}

static void semP(unsigned short int sem_num)
{
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_flg = 0;
    op.sem_op = -1;
    semop(semId,&op,1);
}

static void semV(unsigned short int sem_num)
{
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_flg = 0;
    op.sem_op = 1;
    semop(semId,&op,1);
}
