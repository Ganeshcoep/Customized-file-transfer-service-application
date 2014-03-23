/* ------------------------------------------------------------------------------
Author: Ganesh S Salvi
ACN HW1
-------------------------------------------------------------------------------*/

/*Header files*/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>	


//#define UDP_PORT 4050  //Port Number of Server
//#define TCP_PORT 4051  //Port Number of Server
#define BUFLEN 512    //Length of the data to be sent to the Server

int main(int argc,char *argv[])
{
if(argc!=3)//strcmp(argv[0]," ") == 0||strcmp(argv[1]," ") == 0)
{
  printf("\nPlease pass valid command line parameters...\n");
  return 1;
}

int udp_port=atoi(argv[1]);
int tcp_port=atoi(argv[2]);


printf("Client started.");

struct sockaddr_in my_addr,server_addr;
int sock_id,server_sock_id;
char buf[BUFLEN],buf2[BUFLEN];
socklen_t slen=sizeof(server_addr);

/* setting the server details*/
server_addr.sin_family=AF_INET;
//server_addr.sin_port=htons(UDP_PORT); // short, network byte order
server_addr.sin_port=htons(udp_port); // short, network byte order
server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
if (server_addr.sin_addr.s_addr == INADDR_NONE ) {
     printf("\nBad address."); }

/* socket creation */
sock_id=socket(PF_INET,SOCK_DGRAM,0);
if(sock_id==-1){
   err("\nsocket Creation Error\n");
   return 1;
}

int filesize=0;
while(1)
{
       printf("\nPlease enter command (Type 'quit' and press enter to quit) : ");
       scanf("%[^\n]",buf);
       getchar();
       if(strcmp(buf,"quit") == 0)
       break;
       
       sprintf(buf2,"%s",buf); //copy the command string before passing to tokenizer
     
       const char delimiter[2]=" ";
       char *cmd,*filename,*no_chunks;
       cmd=strtok(buf,delimiter);
      if(strcmp(cmd,"lookup") == 0 || strcmp(cmd,"download") == 0)
      {

             if(strcmp(cmd,"lookup") == 0)
	      {
                      /* check if User has entered second parameter i.e. filename*/
                      filename=strtok(NULL,delimiter);
		      if(filename==NULL){
		      printf("\nThe filename is missing");
		      continue;
                      }
      
		      /* send lookup query to server*/
                       int sentbytes=sendto(sock_id, buf2, BUFLEN, 0, (struct sockaddr*)&server_addr, slen);
		       if (sentbytes==-1)
		           err("\nServer lookup sendtoerror()");
		       if (sentbytes==0)
		           err("\nServer is not listening");
		      
//   		      int size=0;
		      char bytesMsg[50];
		      if (recvfrom(sock_id, bytesMsg, sizeof(bytesMsg), 0, (struct sockaddr*)&server_addr, &slen)==-1){
		      err("\n Server lookup recvfromerror()");
                      continue;
		      }
                      filesize=atoi(bytesMsg);
                      printf("\n%d",filesize);
		           				
             }
            if(strcmp(cmd,"download") == 0)
             {

                      /* check if User has entered second and parameter i.e. filename and no_of_chunks*/
                      filename=strtok(NULL,delimiter);
		      if(filename==NULL){
		      printf("\nThe filename is missing");
		      continue;
		      }
                      no_chunks=strtok(NULL,delimiter);
		      if(no_chunks==NULL){
		      printf("\nPlease enter no. of chunks...");
		      continue;
                      }
             
	       /* send the download command to the server*/
               int sentbytes=sendto(sock_id, buf2, BUFLEN, 0, (struct sockaddr*)&server_addr, slen);
		       if (sentbytes==-1)
		          { err("sendtoerror()");continue;}
		       if (sentbytes==0)
		          { err("Server is not listening");continue;}
	            
               //int filesize=78;
               int no_of_chunks=atoi(no_chunks);
               printf("\nDownloading started...");
               int success=download(filename,filesize,no_of_chunks,tcp_port);
               if(success==1)
	               {
                       printf("\nDownload Successful.");
	               }
               else
		       {
                        printf("\nDownload Failed.");
		       }
            }
      }
     else 
     {
      printf("\nUnknown Command");
      printf("\nPlease use below commnads: \nlookup[SPACE][Filename] OR \ndownload[SPACE][Filename][SPACE][No of chunks] \nNote:Both the commands are case sensitive)");
      continue;
     } 
  
}
}

int download(char *filename,int filesize,int no_of_chunks,int tcp_port)
{

/* Create sockets for TCP connections*/
/*-------------------------------- */
struct sockaddr_in my_addr,server_addr;
int sock_id[no_of_chunks],server_sock_id;
char buf[BUFLEN],buf2[BUFLEN];
socklen_t slen=sizeof(server_addr);

//setting the server details
server_addr.sin_family=AF_INET;
//server_addr.sin_port=htons(TCP_PORT); 
server_addr.sin_port=htons(tcp_port);
server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
if (server_addr.sin_addr.s_addr == INADDR_NONE ) {
     printf("\nBad address."); }

/* socket creation for K chunks*/
int i;
for (i=0;i<no_of_chunks;i++)
{
	sock_id[i]=socket(PF_INET,SOCK_STREAM,0);
	if(sock_id[i]==-1){
	   err("\nsocket Creation Error\n");
   	return 0;
	}
}

/* Connect to the Server*/
for (i=0;i<no_of_chunks;i++)
{
  if (connect(sock_id[i],(struct sockaddr*)&server_addr, slen) < 0) 
	{
	      error("\nERROR connecting");
	   	return 0;
	}
}

/*Divide the requests into K parts*/
int cntt=0;
int K=no_of_chunks;
int each_req_size=filesize/K;
int start_index=0,end_index=0;
int cnt=0;
int No_of_bytes[no_of_chunks-1];

while(end_index<filesize-1)
{
	if(cnt!=0) 
	start_index=end_index+1;

	end_index=start_index+each_req_size-1;

	//last chunk can have more data or less data e.g if Filesize is 78 and no of chunks is 5 then 5th chunk will have 18 bytes and if n		o of chunks is 4 then 4th chunk will have 18 bytes.
	if(cnt==K-1)
	end_index=filesize-1;
        char buff[BUFLEN]="abc\0";
	sprintf(buff, "%s %d %d ",filename, start_index, end_index);
//	printf("\nBuffer to be sent: %s size id :%d",buff,strlen(buff));
	No_of_bytes[cnt]=end_index-start_index+1;

	/* Send the Request for file transfer*/

	int m=send(sock_id[cnt], buff, strlen(buff),0);
	if(m==-1)
        {
	printf("\n error in send");
        return 0;
        }
        if(m!=strlen(buff))
        {
	printf("\n error in send");
        return 0;
        }
	cnt++;
	printf("\nChunk request %d sent %s %d %d",cnt,filename,start_index,end_index);
}

int rv,n;
fd_set readfds;
struct timeval tv;
char *filedata[no_of_chunks-1];

int a=0;
for(a=0;a<no_of_chunks;a++)
filedata[a]=(char*)malloc(sizeof(char)*No_of_bytes[a]); 

// the n param in select()
n = sock_id[no_of_chunks-1] + 1;
//printf("\nlast sock id is %d and N is %d",sock_id[no_of_chunks-1],n);

// wait until either socket has data ready to be recv()d (timeout 10.5 secs)

int count=0;
while(count<no_of_chunks)
{

tv.tv_sec = 10;
tv.tv_usec = 500000;
FD_ZERO(&readfds);

// add descriptors to the set
for(a=0;a<no_of_chunks;a++)
{
FD_SET(sock_id[a], &readfds);

}


rv = select(n, &readfds, NULL, NULL, &tv);
if (rv == -1)
{
    perror("\nselect"); // error occurred in select()
}
else if (rv == 0) 
{
    printf("\nTimeout occurred!  No data after 10.5 seconds.\n");
}
else
{
	// one or both of the descriptors have data
	for(a=0;a<no_of_chunks;a++)
	{
		 if (FD_ISSET(sock_id[a], &readfds))
		 {
	                //printf("\nData Received from socket %d  and count is",sock_id[a]);
	           
			 int rcvcnt=receiveAllData(sock_id[a],filedata[a],No_of_bytes[a]);
			// int rcvcnt=recv(sock_id[j], filedata[j],No_of_bytes[j], 0);
		   	 if(rcvcnt==-1)
		   	 {
	   		  printf("\n receiveALlerror");
		   	  return 0;
			 }
	                 count++;
		         //printf("\nData Received from Server : %s cnt is %d and Received all? 0-success %d",filedata[a],count,rcvcnt);
	           
	
	         }
	}//end of for
}//end of else
}//end of while

/* Write all the data recieved to the file*/
 FILE *fp;
 fp = fopen(filename, "wb");
 
	 for(a=0;a<no_of_chunks;a++)
	{
         
         if(a==0)
         fseek(fp, 0, SEEK_SET);	 
         else
         fseek(fp, 0, SEEK_END);	 

	 fwrite(filedata[a], sizeof(char),No_of_bytes[a], fp); 
	}
 fclose(fp);


/* Close the sockets*/
for (i=0;i<no_of_chunks;i++)
{
  close(sock_id[i]);
}

return 1;
}

int receiveAllData(int sockid, char *buf, int No_Of_Bytes)
{
    int totalreceived = 0; // how many bytes we've received
    int bytesleft = No_Of_Bytes; // how many we have left to receive
    int n;

    while(totalreceived < No_Of_Bytes) {
        n = recv(sockid, buf+totalreceived, bytesleft, 0);
        if (n == -1) { break; }
        totalreceived += n;
        bytesleft -= n;
    }

    No_Of_Bytes = totalreceived; // return number actually received here

     return n==-1?-1:0; // return -1 on failure, 0 on success
}
