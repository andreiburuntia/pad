#include"stdio.h"  
#include"stdlib.h"  
#include"sys/types.h"  
#include"sys/socket.h"  
#include"string.h"  
#include"netinet/in.h"  
#include"netdb.h"
#include"pthread.h"
#include"assert.h"
  
#define PORT 5000 
#define BUF_SIZE 2000 

void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

int GetNumber2(const char *str) {
  int sw=0;
  while (*str) {
    int number;
    if (sscanf(str, "%d", &number) == 1) {
      if(sw==2) return number;
      sw++;
    }
    str++;
  } 
  // No int found
  return -1; 
}
  
void * receiveMessage(void * socket) {
 int sockfd, ret, x, num;
 char msg;
 char temp[10];
 char buffer[BUF_SIZE]; 
 sockfd = (int) socket;
 memset(buffer, 0, BUF_SIZE);  
 for (;;) {
    memset(buffer,0,strlen(buffer));
  ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
  if (ret < 0) {  
   printf("Error receiving data!\n");    
  } else {
    x=GetNumber2(buffer);
    //printf("%d\n",x);
    snprintf(temp,10," /SEND %d ", x);
    removeSubstring(buffer, temp);
    num=strlen(buffer);
   fputs(buffer, stdout);
  }  
 }
}

int main(int argc, char**argv) {  
 struct sockaddr_in addr, cl_addr;  
 int sockfd, ret;  
 char buffer[BUF_SIZE]; 
 char * serverAddr;
 pthread_t rThread;
 
 sockfd = socket(AF_INET, SOCK_STREAM, 0);  
 if (sockfd < 0) {  
  printf("Error creating socket!\n");  
  exit(1);  
 }  
 printf("Socket created...\n");   

 memset(&addr, 0, sizeof(addr));  
 addr.sin_family = AF_INET;  
 addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 addr.sin_port = htons(5000);     

 ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));  
 if (ret < 0) {  
  printf("Error connecting to the server!\n");  
  exit(1);  
 }  
 printf("Connected to the server...\n");  

 memset(buffer, 0, BUF_SIZE);
 printf("Enter your messages one by one and press return key!\n");

 //creating a new thread for receiving messages from the server
 ret = pthread_create(&rThread, NULL, receiveMessage, (void *) sockfd);
 if (ret) {
  printf("ERROR: Return Code from pthread_create() is %d\n", ret);
  exit(1);
 }
char sizeBuf[5];
char newBuf[BUF_SIZE]="";
 while (fgets(buffer, BUF_SIZE, stdin) != NULL) {
    if(buffer[0]=='\\')
    {
        ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
    }
    else{
    strcpy(newBuf,"/SEND ");
    sprintf(sizeBuf, "%d ", strlen(buffer));
    strcat(newBuf, sizeBuf);
    strcat(newBuf, buffer);
    strcat(newBuf, "\0\r\n");
  ret = sendto(sockfd, newBuf, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
}
  if (ret < 0) {  
   printf("Error sending data!\n\t-%s", buffer);  
  }
 }

 close(sockfd);
 pthread_exit(NULL);
 
 return 0;    
}