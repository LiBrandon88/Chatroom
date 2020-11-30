#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_MSG 1024

char name[30];
int sockfd;


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

void setUpChatter() {

  printf("Please enter your name: ");
  fgets(name, 30, stdin);

  while(strlen(name) > 30 || strlen(name) < 2){
                printf("Name must be less than 30 and more than 2 characters.\n");
    printf("Please enter your name: ");
    fgets(name, 30, stdin);
  }

  //other profile things                                                                                                                                                                                                                      
}

void *chat(void) {

  char message[MAX_MSG] = {};

  while(1) {
    fgets(message, MAX_MSG, stdin);
    strTrim(message, strlen(message));
    if (strcmp(message, "EXIT") == 0) {
      printf("Goodbye\n");
      send(sockfd, message, strlen(message), 0);
      sleep(2);
      break;
    } else {

      //    sprintf(buffer, "%s: %s\n", name, message);                                                                                                                                                                                       
    send(sockfd, message, strlen(message), 0);
    }
    bzero(message, MAX_MSG);

  }

  exit(1);
}
void *listener(void) {

  char message[MAX_MSG] = {};

  while (1) {
    int read = recv(sockfd, message, MAX_MSG, 0);
    message[read] = '\0';
    if (read > 0)
      printf("%s", message);
    else if (read == 0)
      break;
    else {
    }
    // -1                                                                                                                                                                                                                                     
  }
    memset(message, 0, sizeof(message));
    return 0;
}

/*                                                                                                                                                                                                                                            
 * Main function: expected to be called with two arguments: IP address and Port                                                                                                                                                               
 */
int main(int argc, char **argv){
  int port;                       /*server port */
  char *ip;
  struct sockaddr_in servAddr;  /* Local address */

  if(argc != 3){
    //If user did not define port and IP                                                                                                                                                                                                      
      printf("Not enough arg\n");
  }

  ip = argv[1];
  port = atoi(argv[2]);
  /* Setting up the socker */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(ip);
  servAddr.sin_port = htons(port);

  setUpChatter();

  if(connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
     printf("CANT CONNECT");
    return EXIT_FAILURE;
  }
  send(sockfd, name, 30, 0);

  pthread_t chatterThread;
  pthread_t listenerThread;
  pthread_create(&chatterThread, NULL, (void *) &chat, NULL);
  pthread_create(&listenerThread, NULL, (void *) &listener, NULL);
  pthread_join(chatterThread, NULL);
  pthread_join(listenerThread, NULL);
}
