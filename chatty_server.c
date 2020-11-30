#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"

#define MAX_CLIENTS 100
#define MAX_BUFFER 1024
#define MAXNAME_LENGTH 30

clock_t start, end;
double cpu_time_used;

/*                                                                                                                                                                                                                                            
Chatroom stuct containing important data for the chatroom                                                                                                                                                                                     
*/
typedef struct {
    //fd_set serverReadFds;                                                                                                                                                                                                                   
    int socketFd;
    int numClients;
    pthread_mutex_t *clientMutex;
    pthread_mutex_t *msgMutex;

    queue *msgQueue;
    queue *clientQueue;
} chatroom;

//Can add more functions later, such as indicators for being blocked, friended, etc                                                                                                                                                           
/* Class representing client/user AKA Cathty*/
typedef struct{
        int sockfd;
        chatroom *room;
        char name[30];
      //  char name[MAXNAME_LENGTH];                                                                                                                                                                                                          
} client;
/*                                                                                                                                                                                                                                            
Message struct                                                                                                                                                                                                                                
*/
typedef struct {
  int sender; //socket id                                                                                                                                                                                                                     
  char msg[MAX_BUFFER];
  char name[30];
  int type; //type of messafe, -1 for private, 0 for everyone but sender, 1 to all:from server                                                                                                                                                
  //could include time & etc in future                                                                                                                                                                                                        
} message;


//Trims the string                                                                                                                                                                                                                            
void strTrim (char* s, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n                                                                                                                                                                                                   
    if (s[i] == '\n') {
      s[i] = '\0';
      break;
    }
  }
}

//Concatenates the name with the message                                                                                                                                                                                                      
void buildMessage(char *fullMsg, char *name, char *msg)
{

  memset(fullMsg, 0, MAX_BUFFER);
  strcpy(fullMsg, name);
  strcat(fullMsg, ": ");
  strcat(fullMsg, msg);
  strcat(fullMsg, "\n");
}

//function to send messages                                                                                                                                                                                                                   
//toALL -1 for private message, 0 for normal, 1 for to everyone                                                                                                                                                                               
void sendMessage(char *msg,char *name, int sockfd, chatroom *room, int toAll) {
  pthread_mutex_lock(room->clientMutex);
  char buffer[MAX_BUFFER];
  char msgBuffer[MAX_BUFFER+30];


  buildMessage(msgBuffer, name, msg);
  queue_iterator *clientIt = queue_iterator_create(room->clientQueue);
  while(queue_iterator_has_next(clientIt)) {

      client* c = queue_iterator_next(clientIt);
      if(toAll == 1) {
         sprintf(buffer, "Sent to %s\n", c->name);
         printf("%s", buffer);
        write(c->sockfd, msgBuffer, strlen(msgBuffer));
      }
      else if(!(c->sockfd == sockfd) && (toAll == 0)) {
        //Normal message                                                                                                                                                                                                                      
        sprintf(buffer, "Sent to %s\n", c->name);
        printf("%s", buffer);
        write(c->sockfd, msgBuffer, strlen(msgBuffer));
      }
      else if((toAll == -1) && (c->sockfd == sockfd)) {
        //private                                                                                                                                                                                                                             
        write(c->sockfd, msgBuffer, strlen(msgBuffer));
        break;
      }
  }
  pthread_mutex_unlock(room->clientMutex);
  end = clock();
  cpu_time_used = ((double) (end - start))/CLOCKS_PER_SEC;;
  printf("Time to send message to everyone: %f s\n", cpu_time_used);

}
/*                                                                                                                                                                                                                                            
* Thread to handle message sending                                                                                                                                                                                                            
*/
void *messageHandler(void *room) {
  chatroom *r = (chatroom *)room;
  queue * msgQ = r->msgQueue;
  //  printf("Create message thread \n");       
    while(1){
     while(msgQ -> count == 0) {
       sleep(1);
       //future implentation; p thread condition                                                                                                                                                                                              
     }
     // printf("Message handler bout to send \n");                                                                                                                                                                                            

     message *m = (message*)dequeue(msgQ);
     //          printf("Message handler bout to send \n");                                                                                                                                                                                   
     sendMessage(m->msg,m->name, m->sender,r, m->type);
   }

}

//Removes the socket from the list of active client sockets and closes it                                                                                                                                                                     
void removeClient(chatroom *room, int clientSocketFd)
{
    pthread_mutex_lock(room->clientMutex);
      queue_iterator* cIt = queue_iterator_create(room->clientQueue);
      while(queue_iterator_has_next(cIt)) {
        client *c = (client*)queue_iterator_next(cIt);
        if(c->sockfd == clientSocketFd) {
          close(clientSocketFd);
          queue_iterator_remove(cIt);
          room->numClients = room->numClients-1;
          break;
        }
      }
    pthread_mutex_unlock(room->clientMutex);
}
