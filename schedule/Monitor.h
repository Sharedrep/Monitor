
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include<pthread.h>
#include<unistd.h>
#include<vector>
#include <sys/time.h>
#include "HttpMonitor.h"
#include "TestList.h"
#include "Sender.h"
#include "Receiver.h"
#include "Connector.h"
#include "Addrgetter.h"
#include "Scheduler.h"

#ifndef MONITOR_H
#define MONITOR_H

class Monitor {
public:
    Monitor();
    Monitor(const Monitor& orig);
    virtual ~Monitor();
    static void* waitCInterval(void *arg);
    static void* setHost(void *arg);
    static void* getHosts(void *arg);
    static void* makeConnection(void *arg);
    static void* sendRequest(void *arg);
    static void* getResponce(void *arg);
    void threadInit();
private:
    Scheduler * scheduler;
    Testlist * testlist;
    Addrgetter * addrgetter;
    Connector* connector;
    Sender* sender;
    Receiver* receiver;

};

#endif /* MONITOR_H */

