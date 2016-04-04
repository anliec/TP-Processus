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
#include <map>
#include <iostream>
#include <algorithm>
#include <time.h>
//------------------------------------------------------ Include personnel
#include "Outils.h"
#include "Sortie.h"
#include "config.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int msgbuffId;
static int mpParkingId;
static int semId;

static Voiture *mpParking;

static std::map<pid_t,int> listeVoiturier;

//------------------------------------------------------- Fonctions privee
static void init();
static void initId();
static void attachSharedMemory();
static void moteur();
static void sigChldHandler(int signum,siginfo_t *siginfo,void* ucontext);
static void sigUsr2Handler(int signum);
static int semVal(int semNum);
static void semP(unsigned short int sem_num, bool saRestart=true);
static void semV(unsigned short int sem_num);



//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void Sortie(int idMsgBuff, int iDMpParking, int idSem)
{
    msgbuffId = idMsgBuff;
    mpParkingId = iDMpParking;
    semId = idSem;

    //phase moteur
    moteur();
}

void Sortie()
{
    //initialisation
    init();

    //phase moteur
    moteur();
}

static void moteur()
{
    while(1)
    {
        cerr << "number of voiturier: " << listeVoiturier.size() << endl;

        CommandeStruct message;
        //appel bloquant, attente d'une demande de sortie
        if(msgrcv(msgbuffId,&message, sizeof(CommandeStruct),MSGBUF_ID_SORTIE,0) == -1)
        {
            continue; // sarestart (sur toutes les erreurs)
        }

        //lance voiturier
        pid_t voiturier = SortirVoiture(message.valeur);
        if(voiturier == -1) //s'il n'y a pas de voiture à cette place on attend une nouvelle demande
        {
            continue;
        }
        //listeVoiturier.push_back(voiturier);
        listeVoiturier[voiturier]=message.valeur;
        //affiche message de sortie
        semP(SEMELM_MP_PARKING);
        Voiture voiture = mpParking[message.valeur];
        semV(SEMELM_MP_PARKING);
        voiture.heureDepart = time(NULL);
        AfficherSortie(voiture.typeUsager,voiture.immatriculation,voiture.heureArrivee, voiture.heureDepart);
        Effacer((TypeZone) message.valeur);
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
    else if((msgbuffId = msgget(keyMsgBuf,DROITS_ACCES)) <0)
    {
        std::cerr << "unable to open msgbuff on Sortie" << std::endl;
    }
    //récupération de la mémoire partager représentant le parking
    key_t keyMpP = ftok(PATH_TO_MP_PARKING,PROJECT_ID);
    if(keyMpP<0)
    {
        std::cerr << "unable to get key for MP on " << PATH_TO_MP_PARKING << std::endl;
    }
    else if((mpParkingId=shmget(keyMpP,0,DROITS_ACCES)) <0)
    {
        std::cerr << "unable to open MP Parking on Sortie" << std::endl;
    }
    //récupération des sémaphores
    key_t keySem = ftok(PATH_TO_SEM,PROJECT_ID);
    if(keySem<0)
        std::cerr << "unable to get key for semaphores on Sortie" << std::endl;
    else if((semId=semget(keySem,NUMBER_OF_SEM,DROITS_ACCES)) <0)
        std::cerr << "unable to open semaphores on Sortie" << std::endl;
}

static void attachSharedMemory()
{
	if((mpParking = (Voiture *)shmat(mpParkingId,NULL,0)) == NULL)
    {
        std::cerr << "unable to attach shared memory Parking." << std::endl;
    }
}

static void sigChldHandler(int signum,siginfo_t *siginfo,void* ucontext)
{
    cerr << "SIGCHLD from pid " << siginfo->si_pid << endl;
    //list<pid_t>::iterator voiturier = find(listeVoiturier.begin(),listeVoiturier.end(),siginfo->si_pid);
    map<pid_t,int>::iterator voiturier = listeVoiturier.find(siginfo->si_pid);
    if(voiturier == listeVoiturier.end())
        return; //si pour une raison quelconque ce n'était pas un voiturier

    int ret = voiturier->second;
    cerr << "wait pid" << endl;
    waitpid(siginfo->si_pid,&ret,0);
    cerr << "pid get" << endl;
    listeVoiturier.erase(voiturier);
    cerr << "erased" << endl;
    /*if(WIFEXITED(ret))
    {
        ret = WEXITSTATUS(ret);
    }
    else
    {
        cerr << "probleme on voiturier, unable to get return code" << endl;
        return; //le voiturier n'a pas quitté normalement, que faire d'autre ?
    }*/
    //vide l'affichage de la place de parking
    Effacer((TypeZone) ret);
    cerr << "effacer" << endl;

    //ajoute une place
    if(semVal(SEMELM_PLACEDISPO) > 0) // s'il y avait déjà des places dispo pas la peine de chercher plus loin on ajoute juste une place disponible
    {
        cerr << "place dispo++" << endl;
        semV(SEMELM_PLACEDISPO);
    }
    else // sinon il faut dire quel entrée doit s'ouvrir (ou ajouter une place si il n'y pas de demandes)
    {
        bool waitingAtA, waitingAtP, waitingAtGB;
        Requete rA, rP, rGB;
        waitingAtA  = msgrcv(msgbuffId,&rA, sizeof(Requete),MSGBUF_ID_REQUETE_A, 0 /*|MSG_COPY*/ | IPC_NOWAIT) != -1;
        std::cerr << "reading on " << msgbuffId << " | " << MSGBUF_ID_REQUETE_A << std::endl;
        if(!waitingAtA)
            cerr << "errno on A  " << errno << std::endl;
        waitingAtP  = msgrcv(msgbuffId,&rP, sizeof(Requete),MSGBUF_ID_REQUETE_P, 0 /*| MSG_COPY*/ | IPC_NOWAIT) != -1;
        std::cerr << "reading on " << msgbuffId << " | " << MSGBUF_ID_REQUETE_P << std::endl;
        if(!waitingAtP)
            cerr << "errno on P  " << errno << std::endl;
        waitingAtGB = msgrcv(msgbuffId,&rGB,sizeof(Requete),MSGBUF_ID_REQUETE_GB,0 /*| MSG_COPY*/ | IPC_NOWAIT) != -1;
        std::cerr << "reading on " << msgbuffId << " | " << MSGBUF_ID_REQUETE_GB << std::endl;
        if(!waitingAtGB)
            cerr << "errno on GB " << errno << std::endl;
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

        if(nextIn != nullptr) //si on a trouver quelqu'un on lui donne l'autorisation pour rentrer
        {
            std::cerr << "Request found on " << nextIn->type << std::endl;
            //reposte les requete refuser dans les boite aux lettre:
            if(nextIn != &rA && waitingAtA)
                msgsnd(msgbuffId,&rA,sizeof(Requete),0);
            if(nextIn != &rP && waitingAtP)
                msgsnd(msgbuffId,&rP,sizeof(Requete),0);
            if(nextIn != &rGB && waitingAtGB)
                msgsnd(msgbuffId,&rGB,sizeof(Requete),0);
            //msgrcv(msgbuffId,NULL,nextIn->type,sizeof(Requete),0); //retire la requette accepter de la boite aux lettre
            semV(semEntree);//donne l'autorisation de rentrer
            std::cerr << "SEMOK" << nextIn->type << std::endl;
        }
        else //si le parking était plein mais que personne n'attendait on ajoute une place libre
        {
            std::cerr << "No request found" << std::endl;
            semV(SEMELM_PLACEDISPO);
        }
    }
}

static int semVal(int semNum)
{
	return semctl(semId, semNum, GETVAL, 0);
}

static void sigUsr2Handler(int signum)
{
    //desactive sigChldHandler
    struct sigaction resetAction;
    resetAction.sa_handler = SIG_IGN;// pour ignorer les prochains signaux
    sigaction(SIGCHLD,&resetAction,NULL);
    //arrête tout les voiturier un à un
    for(std::pair<const int, int> pid:listeVoiturier)
    {
        kill(pid.first,SIGUSR2);
        waitpid(pid.first,NULL,0);
    }
    exit(0); //un peut brutal peut-être
}

static void semP(unsigned short int sem_num, bool saRestart)
{
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_flg = 0;
    op.sem_op = -1;
    int returnValue;
    do{
        returnValue = semop(semId,&op,1);
    }while(returnValue==-1 && saRestart);
}

static void semV(unsigned short int sem_num)
{
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_flg = 0;
    op.sem_op = 1;
    semop(semId,&op,1);
}
