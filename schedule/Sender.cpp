/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Sender.h"

Sender::Sender() 
{
}

Sender::Sender(const Sender& orig) 
{
}

Sender::~Sender() 
{
}

bool Sender::sendRequest() 
{

        int rc = 0;
        int ret = 0;
        int fd_count = 0;
        fd_set fset;
        bool fail = false;
        struct timeval startTime, endTime, result; 
        struct timeval timeout = {this->minTimeout(), 0};
        std::string path ;
        std::string domain ;
	    std::string request; 


        while (true) 
        {
            ret = select(_maxFd + 1, NULL, &fset, NULL, &timeout);

            if(ret == 0)
                break;

            if(ret < 0 && errno != EINTR || errno != EAGAIN)
            {
                //_logger.addToLog("NOK -- Select on sending: " + strerror(errno) + "\n ");
                break;
            } 
        }

        while(fd_count != testlist->size())
        {
           for(auto it = testlist->begin(); it != testlist->end(); ++it)
           {
               if ((*it).getSock() != -1 && FD_ISSET((*it).getSock(), &fset))
               {
                  (*it).parseUrl(domain, path);
	              request = "GET " + path + " HTTP/1.1\r\nHost: " + domain + "\r\nConnection: close\r\n\r\n";

                  gettimeofday(&startTime, NULL);

                  rc = send((*it).getSock(), request.c_str(),  strlen(request.c_str()), 0);

                  gettimeofday(&endTime, NULL);
                  timeDiff(&result, &endTime, &startTime);
                  if (timeDiff(&(*it)._timeout, &(*it)._timeout, &result) <= 0)
                  {
                      ++fd_count;
                      FD_CLR((*it).getSock(), &fset);
                      _logger.addToLog("testing( "+ (*it)._tid + ") " + (*it)._url + " NOK -- Timeout on sending request\n ");
                      continue;
                  }

                  if(rc < 0)
                  {
                      if(errno = EINTR)
                          continue;
                      _logger.addToLog("testing( "+(*it)._tid + ") " + (*it)._url + " NOK -- Send: " + strerror(errno) + "\n ");
                      ++fd_count;
                      FD_CLR((*it).getSock(), &fset);
                      continue;
                  }     
                  request.erase(request.begin(), request.begin() + rc);
                  if (request.empty())
                  {
                      ++fd_count;
                      FD_SET((*it).getSock(), &fset);
                      FD_CLR((*it).getSock(), &fset);
                      std::cout<<"Request is sent for "<<(*it).getSock()<<std::endl;
                  }   
              }
           }
        }
        return fail;
}

void Sender::AddMonitor(HttpMonitor* pmon) 
{
   testlist->emplace_back(*pmon);
}

void Sender::maxFd(int fd) 
{
  _maxFd = (fd > _maxFd) ? fd : _maxFd; 
}

long Sender::minTimeout() const 
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

long Sender::timeDiff( struct timeval* result, struct timeval* end, struct timeval * start) 
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
