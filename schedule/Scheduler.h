
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

#ifndef SCHEDULER_H
#define SCHEDULER_H

class Scheduler {
public:
    Scheduler();
    Scheduler(const Scheduler& orig);
    virtual ~Scheduler();
    void* getNextHost(HttpMonitor*& pmon);
    void  AddTestsToList();
private:
    std::multimap<time_t, HttpMonitor*> testMap;

public:
    Logger _logger;

};

#endif /* SCHEDULER_H */

