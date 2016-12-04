/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Monitor.h"


Monitor::Monitor() 
{
    testlist = new Testlist();
    scheduler = new Scheduler();
    addrgetter = new Addrgetter();
    connector = new Connector();
    sender = new Sender();
    receiver = new Receiver();
}

Monitor::Monitor(const Monitor& orig) 
{
}

Monitor::~Monitor()
{
    if ( testlist )
        delete testlist;
    if ( addrgetter )
        delete addrgetter;
    if ( connector )
        delete connector;
    if ( sender )
        delete sender;
    if ( receiver )
        delete receiver;
}

pthread_cond_t waitAdd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t waitAddMutex = PTHREAD_MUTEX_INITIALIZER;


void* Monitor::waitCInterval(void *arg) 
{    
      Scheduler * scheduler = ((Monitor *)arg)->scheduler;
      Addrgetter * addrgetter = ((Monitor *)arg)->addrgetter;
      HttpMonitor* pmon;
      scheduler->AddTestsToList();
      while (true)
      {
        scheduler-> getNextHost(pmon); 
        if (pmon != NULL )
            addrgetter->AddMonitor(pmon);
      }
    return NULL;
}

void* Monitor::getHosts(void *arg) 
{    
    Addrgetter * addrgetter = ((Monitor *)arg)->addrgetter;
    Connector * connector = ((Monitor *)arg)->connector;
    HttpMonitor* pmon;
    while (true)
    {
        addrgetter->getHostByName(pmon);
        if (pmon != NULL) 
        {
            std::cout<<pmon->_url<<std::endl;
            //addrgetter->RemoveMonitor(pmon);
        }
    }
    return NULL;
}

void* Monitor::makeConnection(void *arg) 
{    
    Connector * connector = ((Monitor *)arg)->connector;
    connector->makeConnection();    
    return NULL;
}

void* Monitor::sendRequest(void *arg) 
{    
    Sender * sender = ((Monitor *)arg)->sender;
    sender->sendRequest();    
    return NULL;
}

void* Monitor::getResponce(void *arg) 
{    
    Receiver * receiver = ((Monitor *)arg)->receiver;
    receiver->getResponce();    
    return NULL;
}

void Monitor::threadInit()
{


    int i = 0;
    int err;
    //pthread_t trid[size];
    pthread_t tcInterval;
    pthread_t tsetHost;
    pthread_t tnameResolve;
   // pthread_t tmkConnection;
   // pthread_t tsRequest;
   // pthread_t tgResponce;

    err = pthread_create(&(tcInterval), NULL, &waitCInterval, (void*)this);
    if (err != 0)
        perror("create thread waitCInterval");

    err = pthread_create(&(tnameResolve), NULL, &getHosts, (void*)this);
    if (err != 0)
        perror("create thread getHosts");

   // err = pthread_create(&(tmkConnection), NULL, &makeConnection, (void*)this);
   // if (err != 0)
   //     perror("create thread makeConnection");

   // err = pthread_create(&(tsRequest), NULL, &sendRequest, (void*)this);
   // if (err != 0)
   //     perror("create thread sendRequest");

   // err = pthread_create(&(tgResponce), NULL, &getResponce, (void*)this);
   // if (err != 0)
   //     perror("create thread getResponce");

    (void) pthread_join(tcInterval, NULL);
    (void) pthread_join(tnameResolve, NULL);
   // (void) pthread_join(tmkConnection, NULL);
   // (void) pthread_join(tsRequest, NULL);
   // (void) pthread_join(tgResponce, NULL);
    return;
}

