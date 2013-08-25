///Author: Hari Pradhan
///Date: 07/19/2013
///Purpose: This program will do following for each incoming connection in the server.
///a.	Fork a child for that connection
///b.	Child will read the filename sent by the client from the socket
///c.	Check for file existence. Send “File does not exist” error back over socket if the file doesn’t exist. Otherwise, open for read
///d.	Read the file contents and send it over the socket
///e.	Send the size of the file to the client so it can check that the download finished successfully.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>

#define LINELENGTH 40
#define BACKLOG 10
#define MYPORT 3490
#define LENGTH 80

//Reference: course ecture notes and examples
int main(int argc, char **argv) {
	int listenfd, newsocketfd;
	FILE *fp;
	char line[LINELENGTH];

	char e;
	struct sockaddr_in myAddr, clientAddr;
	socklen_t sin_size;
	ssize_t n;
	char filePath[LENGTH];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(MYPORT);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(myAddr.sin_zero, '\0', sizeof(myAddr.sin_zero));

	bind(listenfd, (struct sockaddr *)&myAddr, sizeof(myAddr));
	listen(listenfd, BACKLOG);
	struct stat fileStat;
	while(1) {
		sin_size = sizeof(clientAddr);
		printf("\n\nWaiting to connect...\n");
		newsocketfd = accept(listenfd, (struct sockaddr *)&clientAddr, &sin_size);
		printf("Client:%s is connected.\n", inet_ntoa(clientAddr.sin_addr));
		if(!vfork()) {
			close(listenfd);
			while(1){
				bzero(filePath, LENGTH);
				if(recv(newsocketfd, filePath, LENGTH,0) == 0){
					return;
				}
				printf("%s\n",filePath);
				fp = fopen(filePath,"r");
				if(fp == NULL){
					//send(newsocketfd,tempMsg,sizeof(tempMsg),0);
					perror("File doesn't exist");
					exit(1);
				}
				if(stat(filePath, &fileStat) != 0){
					perror("Stat failed");
					exit(1);
				}
				printf("Sending %s to the client\n",filePath);
				bzero(filePath, LENGTH);

				bzero(line,LINELENGTH);
				send(newsocketfd,&fileStat.st_size,sizeof(fileStat.st_size),0);
				printf("size %d\n",fileStat.st_size);

				size_t total=0;
				while(fgets(line,LINELENGTH,fp) != NULL){
					send(newsocketfd, line, strlen(line), 0);
					total += strlen(line);
					bzero(line,LINELENGTH);
				}

				fclose(fp);
				printf("%d bytes transmitted\n",total);
				printf("Completed transfering....\n");
				exit(1);

			}
		}
		close(newsocketfd);
	}

	return 0;
}


