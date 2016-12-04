/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "TestList.h"

Testlist::Testlist() 
{
}

Testlist::Testlist(const Testlist& orig) 
{
}

Testlist::~Testlist() 
{
}

void Testlist::getTests()
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
void Testlist::waitCInterval(time_t &sleeptime , HttpMonitor* &pmon) 
{

     int i= 0;
     struct timespec timeout;
     struct timeval currentTime;
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
            _logger.addToLog(pmon->_url + "\n");
            break;
        }
        else 
        {
            pmon = NULL;
            auto range = testMap.equal_range((*it).first);
            for (auto i = range.first; i != range.second; ++i) 
            {
                sleeptime = sleeptime > (*it).first - curTime ? (*it).first - curTime : sleeptime;   
            }
            break;
        }
     }

}

long Testlist::timeDiff( struct timeval* result, struct timeval* end, struct timeval * start) 
{
  long first =start->tv_sec*1000000 + start->tv_usec; 
  long last =end->tv_sec*1000000 + end->tv_usec; 
  long diff = last -first;
  if (diff < 0)
      return diff;
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;
  return diff;
      
}
