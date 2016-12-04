/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Addrgetter.h"
#include <ctime>

Addrgetter::Addrgetter(): reqs(0), nreqs(0) 
{
}

Addrgetter::Addrgetter(const Addrgetter& orig) 
{
    _maxFd = orig._maxFd; 
    nreqs= orig.nreqs; 
    testMap = orig.testMap;
    hostMap = orig.hostMap;
    for (int i= 0; i < nreqs; ++i) 
    {
        reqs[i] = orig.reqs[i];
    }
}

Addrgetter::~Addrgetter()
{
}

int Addrgetter::getHostByName(HttpMonitor * pmon) 
{
   struct addrinfo *aInf = NULL;
   struct addrinfo * res = NULL;
   int sock = -1;
   bool fail = false;

   struct timespec timeout = {this->minTimeout(), 0};

  while (true)
  {
     if (nreqs > 1) 
     {
         int ret = getaddrinfo_a(GAI_NOWAIT, reqs, nreqs, NULL);
         if ( ret < 0)
             std::cout<<"Failed getaddrinfo:"<<strerror(errno)<<std::endl;
     }

     for (size_t j = 0; j < nreqs; j++)
     {
         int remain = nreqs-j;
         int ret = gai_suspend(reqs, remain, &timeout);
         if (ret < 0 && (errno == EAI_AGAIN || errno == EAI_INTR))
            continue;
         for (int i = 0; i < remain; i++)
         {
             int rc = gai_error(reqs[i]);
             if (rc == EAI_INPROGRESS)
                 continue;
             if (rc == 0)
             {
                 res = reqs[i]->ar_result;
                 for (aInf = res; aInf; aInf = aInf->ai_next)
                 {
                     sock = socket(aInf->ai_family, aInf->ai_socktype,aInf->ai_protocol);
                     if (sock < 0)
                         continue;
                     break;
                 }
                 if (sock < 0){
                   fail = true;
                   break;
                 } 
                long arg;
                if((arg = fcntl(sock, F_GETFL, NULL)) < 0 )
                {
                    _logger.addToLog("testing( "+ hostMap[reqs[i]]->_tid + ") " + hostMap[reqs[i]]->_url + " NOK -- Get socket access mode: " + strerror(errno) + "\n ");
                    break;         
                } 
                arg |= O_NONBLOCK; 

                if( fcntl(sock, F_SETFL, arg) < 0)
                {
                    _logger.addToLog("testing( "+ hostMap[reqs[i]]->_tid + ") " + hostMap[reqs[i]]->_url + " NOK -- Set socket access mode: " + strerror(errno) + "\n ");
                }
                std::cout<<sock<<" is opened"<<std::endl;
                //hostMap[reqs[i]]->setSock(sock);
                //hostMap[reqs[i]]->setAInf(aInf);
                pmon = hostMap[reqs[i]];
                RemoveMonitor(pmon);
                reqs[i] = reqs[remain-1];
                reqs[remain-1] = NULL;
                maxFd(sock);
                break;
             }
         }
     }
   
   break;
}
  if (fail)
      std::cout<<"Failed getaddrinfo for all hosts"<<std::endl;

  // free(reqs);
   return 0;
}

void Addrgetter::AddMonitor(HttpMonitor*& pmon) 
{
   struct addrinfo *aInf = NULL;
   struct addrinfo * res = NULL;
   std::string path ;
   std::string domain ;
   reqs = (gaicb**)realloc(reqs, nreqs + 1  * sizeof(gaicb*));

    if (pmon != NULL)
    {
        pmon->parseUrl(domain, path);
        //reqs[nreqs] = (gaicb*)calloc(1, sizeof(gaicb));
        reqs[nreqs]->ar_name = strdup(domain.c_str());
        reqs[nreqs]->ar_service = strdup(pmon->_port.c_str());
        testMap[pmon] = reqs[nreqs];
        hostMap[reqs[nreqs]] = pmon;
        ++ nreqs;
    }
}

void Addrgetter::RemoveMonitor(HttpMonitor* pmon) 
{
    for (auto it = testMap.begin() ; it != testMap.end(); ++it)
    {    
        if (it->first->_tid == pmon->_tid) 
        {
            it = testMap.erase(it);
            break;
        }
    }
}

void Addrgetter::maxFd(int fd)
{
  _maxFd = (fd > _maxFd) ? fd : _maxFd; 
}

long Addrgetter::minTimeout() const
{
    long min = 0;
    long timeoutS;
    for (auto it = testMap.begin() ; it != testMap.end(); ++it) 
    {

        timeoutS =it->first->_timeout.tv_sec + it->first->_timeout.tv_usec/1000;
        min = min < timeoutS ? timeoutS : min;  
    }
    return min;
}

long Addrgetter::timeDiff( struct timeval* result, struct timeval* end, struct timeval * start) 
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
