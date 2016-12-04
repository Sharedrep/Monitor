/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: root
 *
 * Created on October 12, 2016, 6:09 PM
 */
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include<pthread.h>
#include<unistd.h>
#include <signal.h>
#include "Monitor.h"
#include <execinfo.h>
#include <unistd.h>


void catch_all_signals(volatile int n)
{
    char buf[1024] = { 0};
    switch(n)
    {
    case SIGHUP:
    case SIGSEGV:
    //case SIGINT:
    case SIGQUIT:
    case SIGTERM:
    case SIGILL:
    case SIGABRT:
    case SIGFPE:
    case SIGALRM:
    case SIGUSR1:
    case SIGUSR2:
    case SIGBUS:
    case SIGPROF:
    case SIGSYS:
        {
            printf(buf,"Signal %d Received, monitis will be terminated.\n", n);


            void *bt[128];
            int bt_size;
            char **bt_strings;

            // get the calling stack addresses
            bt_size = backtrace(bt,sizeof(bt)/sizeof(void *));

            // get the symbols for the calling stack addresses if applicable
            bt_strings = (char**)backtrace_symbols(bt,sizeof(bt)/sizeof(void *));
            int bti;
            for(bti = 0; bti < bt_size; ++bti)
            {
                printf(buf, 1023, " %p: %s\n", bt[bti], bt_strings[bti]);
            }

            exit(0);
        }
        break;

    default:
        {
            printf(buf,"Signal Received: %d\n",n);
        }
    }
} 

int CatchSignals()
{
    struct sigaction ignaction;
    ignaction.sa_flags = 0;
    ignaction.sa_handler = SIG_IGN;
    sigemptyset(&(ignaction.sa_mask));

	if(sigaction(SIGPIPE, &ignaction, NULL) < 0) 
    {
        printf("Can not ignor SIGPIPE signal." );
        return 0;
    }

	struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = catch_all_signals;
    sigemptyset(&(action.sa_mask));

    if(sigaction(SIGSEGV, &action, NULL) < 0)
    {
        printf("Can not set SIGSEGV signal handler." );
        return 0;;
    }
    if(sigaction(SIGHUP, &action, NULL) < 0)
    {
        printf("Can not set SIGHUP signal handler." );
        return 0;;
    }
	//if(sigaction(SIGINT, &action, NULL) < 0)
    //{
    //    printf("Can not set SIGINT signal handler." );
    //    return 0;;
    //}
    if(sigaction(SIGQUIT, &action, NULL) < 0)
    {
        printf("Can not set SIGQUIT signal handler." );
        return 0;;
    }
    if(sigaction(SIGTERM, &action, NULL) < 0)
    {
        printf("Can not set SIGTERM signal handler." );
        return 0;;
    }
    if(sigaction(SIGILL, &action, NULL) < 0)
    {
        printf("Can not set SIGILL signal handler." );
        return 0;;
    }
    if(sigaction(SIGABRT, &action, NULL) < 0)
    {
        printf("Can not set SIGABRT signal handler." );
        return 0;;
    }
    if(sigaction(SIGFPE, &action, NULL) < 0)
    {
        printf("Can not set SIGFPE signal handler." );
        return 0;;
    }
    if(sigaction(SIGALRM, &action, NULL) < 0)
    {
        printf("Can not set SIGALRM signal handler." );
        return 0;;
    }
    if(sigaction(SIGUSR1, &action, NULL) < 0)
    {
        printf("Can not set SIGUSR1 signal handler." );
        return 0;;
    }
    if(sigaction(SIGUSR2, &action, NULL) < 0)
    {
        printf("Can not set SIGUSR2 signal handler." );
        return 0;;
    }

    if(sigaction(SIGBUS, &action, NULL) < 0)
        printf("Can not set SIGBUS signal handler." );

    if(sigaction(SIGPROF, &action, NULL) < 0)
        printf("Can not set SIGPROF signal handler." );

    if(sigaction(SIGSYS, &action, NULL) < 0)
        printf("Can not set SIGSYS signal handler." );

    return 1;
}

using namespace std;

int main(int argc, char** argv) {

	if(!CatchSignals())
	   return 0;
    Monitor monitor;
    monitor.threadInit();

    return 0;
    
}


