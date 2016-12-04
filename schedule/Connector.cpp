/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Connector.h"

Connector::Connector() 
{
}

Connector::Connector(const Connector& orig) 
{
}

Connector::~Connector() 
{
}

bool Connector::makeConnection()
{

        int rc=0;
        int ret=0;
        int fd_count=0;
        fd_set fset;
        struct timeval startTime, endTime, result; 
        struct timeval timeout = {this->minTimeout(), 0};
        bool fail = false;

        while (true )
        {

           ret = select(_maxFd + 1, NULL, &fset, NULL, &timeout);

           if (ret < 0)
           {
             if(errno == EINTR)  
                 continue;
             //_logger.addToLog(" NOK -- Select for connection: " + strerror(errno) + "\n ");
           }
           break;
        }

        while (true)
        {
           for(auto it =testlist->begin(); it != testlist->end(); ++it)
           {
                int valopt;
                socklen_t lon = sizeof(int); 
                //if (FD_ISSET((*it).getSock(), &fset))
                if ((*it).getSock() != -1)
                {
                   gettimeofday(&startTime, NULL);

                   rc = connect((*it).getSock(), ((*it).getAInf())->ai_addr, ((*it).getAInf())->ai_addrlen); 

                   gettimeofday(&endTime, NULL);
                   timeDiff(&result, &endTime, &startTime);
                   if (timeDiff(&(*it)._timeout, &(*it)._timeout, &result) <= 0)
                   {
                       _logger.addToLog("testing( "+(*it)._tid + ") " +(*it)._url + " NOK -- Timeout on sending request\n ");
                       FD_CLR((*it).getSock(), &fset);
                       break;
                   }

                   if (rc < 0 && (errno == EINPROGRESS || errno == EINTR))
                       continue;

                    if (rc == 0) 
                    {
                         ++ fd_count;
                         if (getsockopt((*it).getSock(), SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
                         {
                             FD_CLR((*it).getSock(), &fset);
                             _logger.addToLog("testing( "+ (*it)._tid + ") " +(*it)._url + " NOK -- Getsocksetoption after connection: " + strerror(errno) + "\n ");
                            continue;
                         } 

                         if(valopt)
                         {
                             FD_CLR((*it).getSock(), &fset);
                             _logger.addToLog("testing( "+(*it)._tid + ") " +(*it)._url + " NOK --  Error in connection" + strerror(valopt) + " \n ");
                            continue;
                         }

                         std::cout<<(*it).getSock()<<" is connected"<<std::endl;
                         FD_SET((*it).getSock(), &fset);
                         FD_CLR((*it).getSock(), &fset);
                    }
                }
           }
           if (fd_count == testlist->size())
                break;
         }

        return fail;
}

void Connector::AddMonitor(HttpMonitor* pmon)
{
   testlist->emplace_back(*pmon);
}

void Connector::maxFd(int fd)
{
  _maxFd = (fd > _maxFd) ? fd : _maxFd; 
}

long Connector::minTimeout() const 
{
    long min = 0;
    long timeoutS;
    for (std::vector<HttpMonitor>::const_iterator it = testlist->begin() ; it != testlist->end(); ++it) 
    {

        timeoutS =(*it)._timeout.tv_sec + (*it)._timeout.tv_usec/1000;
        min = min < timeoutS ? timeoutS : min;  
    }
    return min;
}

long Connector::timeDiff( struct timeval* result, struct timeval* end, struct timeval * start) 
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
