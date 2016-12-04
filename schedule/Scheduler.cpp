/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Scheduler.h"


Scheduler::Scheduler() 
{
}

Scheduler::Scheduler(const Scheduler& orig) 
{
}

Scheduler::~Scheduler()
{
}

pthread_cond_t waitCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t waitMutex = PTHREAD_MUTEX_INITIALIZER;

void* Scheduler::getNextHost(HttpMonitor*& pmon) 
{    
    time_t sleeptime;
    struct timeval currentTime;
    struct timespec timeout;
    int i= 0;

    auto it = testMap.begin();
    while (it !=testMap.end()) 
    {
        time_t curTime = time(NULL);
        if((*it).first <= curTime)
        {
            pmon = (*it).second;
            pmon->nextcheck = curTime + pmon->_cInterval;
            it = testMap.erase(it);
            testMap.insert( std::make_pair(pmon->nextcheck, pmon));
            break;
        }
        else 
        {
            pmon = NULL;
            sleeptime = (*it).first - curTime;
            break;
        }
    }
    if ( pmon  != NULL) 
    {
        _logger.addToLog(pmon->_url + "\n");
    } else 
    {
        gettimeofday(&currentTime, NULL);
        timeout.tv_sec = currentTime.tv_sec + sleeptime; 
        timeout.tv_nsec = 1000 * currentTime.tv_usec;
        pthread_cond_timedwait(&waitCond, &waitMutex, &timeout);
    }
    return NULL;
}

void Scheduler::AddTestsToList()
{
    
    std::string test;
    std::string delimiter = "->";
    std::string tid, url, port;
    int timeout; 
    long cInterval;
    size_t pos = 0;
    std::ifstream file;
    file.open ("res/tests");
    while(getline(file, test)) 
    {
        pos = test.find(delimiter);
        tid = test.substr(0, pos);
        test.erase(0, pos + delimiter.length());

        pos= test.find(delimiter);
        url = test.substr(0, pos);
        test.erase(0, pos + delimiter.length());

        pos= test.find(delimiter);
        port = test.substr(0, pos);
        test.erase(0, pos + delimiter.length());

        pos= test.find(delimiter);
        timeout = atoi(test.substr(0, pos).c_str());
        test.erase(0, pos + delimiter.length());

        pos= test.find(delimiter);
        cInterval =atoi(test.substr(0, pos).c_str());
        test.erase(0, pos + delimiter.length());
        time_t cInt = time(NULL) + cInterval;
        testMap.insert(std::make_pair(cInt, new HttpMonitor(tid, url, port , timeout, cInterval))) ;

    }

    file.close();
    
}    
