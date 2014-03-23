/* ------------------------------------------------------------------------------
Author: Ganesh S Salvi 
ACN HW1
-------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h> // For INADDR_ANY and sockaddr_in
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>	//for itoa call
#include <pthread.h>	
//#define UDP_PORT 4050
//#define TCP_PORT 4051
#define BUFLEN 512
#define BACKLOG 100     // how many pending connections queue will hold used for listen

int sendAll(int s, char *buf, int *len);
void *download(void *new_sock_id);

int main(int argc,char *argv[])
{

if(argc!=3)//strcmp(argv[0]," ") == 0||strcmp(argv[1]," ") == 0)
{
  printf("\nPlease pass valid command line parameters...\n");
  return 1;
}

int udp_port=atoi(argv[1]);
int tcp_port=atoi(argv[2]);

/*Get the details of the Server*/
struct hostent *server;
server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 0;//exit(0);
    }

printf("Server is running on UDP port %d TCP port %d.\n",udp_port,tcp_port);


/* Setup Server for UDP connection*/

struct sockaddr_in my_addr,client_addr;
int sock_id,client_sock_id;
char buf[BUFLEN];
socklen_t slen=sizeof(client_addr);

my_addr.sin_family=AF_INET;
//my_addr.sin_port=htons(UDP_PORT); // short, network byte order
my_addr.sin_port=htons(udp_port); // short, network byte order
my_addr.sin_addr.s_addr=htonl(INADDR_ANY);

/* Create socket for UDP */
sock_id=socket(PF_INET,SOCK_DGRAM,0);
if(sock_id==-1){
   err("\nsocket Error");
   return 1;
}

/*Bind socket for UDP*/
int bind_status=bind(sock_id,(struct sockaddr*)&my_addr,sizeof(my_addr));
if(bind_status==-1){
   err("\nsocket bind Error");
   return 1;
}

/* Setup Server for TCP connection*/
struct sockaddr_in tcp_my_addr,tcp_client_addr;
int tcp_sock_id,tcp_client_sock_id;
char tcp_buf[BUFLEN];
socklen_t tcp_slen=sizeof(tcp_client_addr);
tcp_my_addr.sin_family=AF_INET;
//tcp_my_addr.sin_port=htons(TCP_PORT); // short, network byte order
tcp_my_addr.sin_port=htons(tcp_port); // short, network byte order
tcp_my_addr.sin_addr.s_addr=htonl(INADDR_ANY);

/* Create socket for TCP */
tcp_sock_id=socket(PF_INET,SOCK_STREAM,0);
if(tcp_sock_id==-1){
   err("\nsocket Error");
   return 1;
}

/*Bind socket for TCP*/
int tcp_bind_status=bind(tcp_sock_id,(struct sockaddr*)&tcp_my_addr,sizeof(tcp_my_addr));
if(tcp_bind_status==-1){
   err("\nsocket bind Error");
   return 1;
}

if(listen(tcp_sock_id,10)==-1){
printf("\nlistenerror");
return 1;
}

while(1)
{
      if (recvfrom(sock_id, buf, BUFLEN, 0, (struct sockaddr*)&client_addr, &slen)==-1)
         err("\nrecvfrom()");
     
      const char delimiter[2]=" ";
      char *cmd,*filename;
      cmd=strtok(buf,delimiter);

      if(strcmp(cmd,"lookup") == 0)
      {
      
      filename=strtok(NULL,delimiter);
      
      char *cwd="./Data";
      struct stat buff;
      DIR *dir;
      struct dirent *ent;
      int flag=0;
      long size=0;
      if ((dir = opendir (cwd)) != NULL) {

       	 char bytesMsg[256];  //used to send reply to client
         while ((ent = readdir (dir)) != NULL) 
         {
         	if(strcmp(filename,ent->d_name) == 0)
         	{
		 flag=1; // File found
	         stat (ent->d_name,&buff);
                 size = (long)buff.st_size;
	         printf("\nUDP look-up query received for file %s. Returning query response :%d",filename,size);
	         sprintf(bytesMsg, "%ld", size);
	         //printf("%s\n", bytesMsg);
	         if (sendto(sock_id, bytesMsg, sizeof(bytesMsg), 0, (struct sockaddr*)&client_addr, slen)==-1)
	         err("\nsendto()");
        	 break; // File found and response sent to client so break the loop
        	 }

         }//while
         if(flag==0) //File not found
	 { 
         	printf("\nUDP look-up query received for file %s.",filename);
         	printf("Returning query response :%d",size);
		sprintf(bytesMsg, "%ld", size);
//         	sprintf(bytesMsg, "%s","FNF");
	       	if (sendto(sock_id, bytesMsg, sizeof(bytesMsg), 0, (struct sockaddr*)&client_addr, slen)==-1)
         	err("\nsendto()");
       
         }
         

         closedir (dir);
         } else {
          //could not open directory 
          perror ("");
         }
      }else {
      		if(strcmp(cmd,"download") == 0)
		      {
                       char *filename=strtok(NULL,delimiter);
                       char *no_chunks=strtok(NULL,delimiter);
                       long no_of_chunks=atoi(no_chunks);

                       //Call lookup first if lookup is successful then start download
                       //printf("%s size %ld",filename,no_of_chunks);
                       struct sockaddr_storage client_addrs[no_of_chunks];
                       int new_sock_id[no_of_chunks];
                       int i;
                       int flag=1;
                       for (i=0;i<no_of_chunks;i++)
                       {
                       socklen_t addr_size=sizeof(client_addrs[i]);
                       new_sock_id[i] = accept(tcp_sock_id,(struct sockaddr*)&client_addrs[i],&addr_size);
                       //printf("\n Sock Id: %d",new_sock_id[i]);
                       if(new_sock_id[i]==-1)
                       flag=0;
                       }
                       printf("\nTCP connection received.");
                       if(flag==1)
                       {
                       		pthread_t thread[no_of_chunks];
	                       int rc[no_of_chunks];
        	               for (i=0;i<no_of_chunks;i++)
        	               {
        	               rc[i] = pthread_create(&thread[i], NULL, download,(void *) &new_sock_id[i]);
        	               pthread_join(thread[i],NULL);
        	               }
               		       printf("\nFile sent to the client Successfully...");       
		      }
                     }
		else 
		     { 
                      printf("\nUnknown Command");
		      printf("\nPlease use commnads: lookup or download (both are case sensitive)");
                     }
                      
      }
printf("\n");
}//while loop
 
close(sock_id);
close(tcp_sock_id);

pthread_exit(NULL);

return 0;
}


int sendAll(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n=0;
//    printf("\nThe socket id from SendALL is %d %s %d",s,buf,*len);

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
  //  printf("\nThe no. of bytes sent from SendALL is %d ",n);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

void *download(void *new_sock_id)
{
int *sock_id=(int *) new_sock_id;

//printf("\nThread STARTED");

int i;
char buf[2000];
FILE *fp;
char *buff;

//printf("\nSock id: %d", *sock_id);

bzero(buf,2000);
if(recv(*sock_id, buf, sizeof(buf),0)==-1)
printf("\n buferror");

//printf("\nReceived : %s",buf);


//Parse the request 
const char delimiter[2]=" ";
      char *filename;
      int start_index=0,end_index=0;
      filename=strtok(buf,delimiter);
      char *start=strtok(NULL,delimiter);
      char *end=strtok(NULL,delimiter);
      start_index=atoi(start);
      end_index=atoi(end);
      int No_of_bytes=end_index-start_index+1;
//printf("\nFilename:%s start_index:%d end_index:%dNo_of_bytes:%d",filename,start_index,end_index,No_of_bytes);
printf("\nChunk request of %d bytes in ""%s"" at offset %d.",No_of_bytes,filename,start_index);

char filepath[1000];
char *folderpath="./Data/";
sprintf(filepath, "%s%s",folderpath,filename);

fp = fopen(filepath, "rb");
if (fp == NULL) 
{
    printf("\nFile not found!\n");
    return 0;
}
else
{
	buff=(char*)malloc(sizeof(char)*No_of_bytes); 
	fseek(fp, start_index-1, SEEK_SET);
        bzero(buff,No_of_bytes);
	fread(buff, sizeof(char), No_of_bytes, fp); 
//      printf("\nBuffer read from file: %s",buff);
        buff[No_of_bytes]='\0';
        sleep(2);
	int m=sendAll(*sock_id, buff,&No_of_bytes);
	if(m==-1)
	printf("\n error");
	fclose(fp);
}

free(buff);
//printf("\nThread finished : %s",buf);
//printf("\n");
pthread_exit(NULL);
}



