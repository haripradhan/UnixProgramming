///Author: Hari Pradhan
///Date: 07/19/2013
///Purpose: This program would attempt to connect to the server with given ip address 
///and retrieve the file named ?remotefile? and save a copy of it on the local machine
///in the file named ?localfile?

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define MYPORT 3490
#define LENGTH 40

////Reference: course ecture notes and examples
int main(int argc, char **argv) {
	int sockfd, numbytes;

	char remoteFile[80];
	char localFile[80];
	int n;	
	FILE *fp;
	char line[LENGTH];

	struct sockaddr_in myDestinationSocketAddr;

	if(argc != 4) {
		printf("Usage: netcopy ip_addresss remotefile localfile\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	myDestinationSocketAddr.sin_family = AF_INET;
	myDestinationSocketAddr.sin_port = htons(MYPORT);
	myDestinationSocketAddr.sin_addr.s_addr = inet_addr(argv[1]);
	//myDestinationSocketAddr.sin_addr.s_addr = INADDR_ANY;
	memset(myDestinationSocketAddr.sin_zero, '\0', sizeof(myDestinationSocketAddr.sin_zero));

	//Connect to the server socket
	if(connect(sockfd,(struct sockaddr*)&myDestinationSocketAddr, sizeof(myDestinationSocketAddr)) == -1){
		perror("Connection failed");
		exit(1);
	}
	strcpy(remoteFile,"");
	strcat(remoteFile,argv[2]);
	strcpy(localFile,"");
	strcat(localFile,argv[3]);
	//Send remote file path to the server
	printf("file is : %s\n",remoteFile);
	if(send(sockfd, &remoteFile, sizeof(remoteFile), 0) == -1){
		perror("Send failed");
		exit(1);
	}

	//Open file to write
	fp = fopen(localFile,"w");
	if(fp == NULL){
		perror("File error");
		exit(1);
	}

	int count = 0;
	size_t fileSize;
	recv(sockfd,&fileSize,sizeof(fileSize),0);
	printf("size : %d\n",fileSize);

	//receive content of the file
	size_t totalSizeDownloaded=0;
	bzero(line, LENGTH);	
	while((n = recv(sockfd, line, LENGTH,0)) > 0){ 
		line[n] = '\0';
		fputs(line,fp);
		totalSizeDownloaded += n;
		bzero(line, LENGTH);
	}

	if(totalSizeDownloaded == fileSize){
		printf("Download finished successfully.\n");
	}else{
		printf("Download not completed successfully.\n");
	}
	fclose(fp);
	close(sockfd);
	return 0;
}

