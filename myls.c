///Name: Hari Pradhan
///Date: 06/10/2013
///Purpose: This program lists the files and/or sub-directories under the given directory. It can be used as follows:
///   myls [options] [files/directories]	
/// It supports three options:
/// -s   lists the file size in number of blocks
/// -l   gives long listing like ls ?l
/// -R  list subdirectories recursively

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

//Global flag array for options
//flag[0] = 1 if -s is provided else 0
//flag[1] = 1 if -l is provided else 0
//flag[2] = 1 if -R is provided else 0
int flag[] = {0, 0, 0};
long long totalBlockCount = 0;

void displayContentListing(int,struct stat);
void getContent(char*);
void listEntriesRecursive(char*);

/// <summary>
/// Gets files and/or directories under the given directory.
/// </summary>
/// <param name="dirPath">Directory path.</param>
void getContent(char* dirPath){
	char* entryName;
	struct dirent* direntry;	
	DIR* dir = opendir(dirPath);
	if(dir == NULL){
		perror("opendir");
		exit(1);
	}
	totalBlockCount = 0;
	while((direntry = readdir(dir))!= NULL){
		entryName = direntry -> d_name;
		if(entryName[0] != '.'){
			struct stat entryInfo;
			char* path = (char*)malloc(strlen(dirPath)+strlen(entryName)+1);
			strcpy(path,"");
			strcat(path,dirPath);
			strcat(path,entryName);
			int statVal;
			if((statVal = stat(path,&entryInfo)) == 0){
				//Displays the information about the entry based on option -l, -s
				displayContentListing(statVal,entryInfo);
			}
			free(path);
			//Set font color to magenta if an entry is a directory
			if(S_ISDIR(entryInfo.st_mode)){
				printf("\033[0;35m");
			}
			printf("\t%s\n ",entryName);
			//Set font color back to default
			printf("\033[0m");
		}
	}
	//Set font color to green for displaying total block count
	printf("\033[0;32m");
	printf("\tTotal: %lld\n",totalBlockCount);
	//Set font color back to default.
	printf("\033[0m");

	if(closedir(dir) == -1){
		perror("closedir");
		exit(1);
	}
}

/// <summary>
/// List the subdirectories recursively.
/// </summary>
/// <param name="dirPath">Directory path.</param>
void listEntriesRecursive(char* dirPath){

	printf("\%s:\n",dirPath);
	//Get the content of entries under dirPath directory
	getContent(dirPath);
	printf("\n\n");

	char* entryName;
	char* path;
	struct dirent* direntry;
	struct stat entryInfo;

	DIR* dir = opendir(dirPath);
	if(dir == NULL){
		perror("opendir");
		exit(1);
	}

	while((direntry = readdir(dir))!= NULL){
		entryName = direntry -> d_name;
		if(entryName[0] != '.'){
			path = (char*)malloc(strlen(dirPath)+strlen(entryName)+2);
			strcpy(path,"");
			strcat(path,dirPath);
			strcat(path,entryName);
			if(stat(path,&entryInfo) == 0){
				if(S_ISDIR(entryInfo.st_mode)){
					strcat(path,"/");
					//Recursively list the entries of the subdirectory
					listEntriesRecursive(path);
				}
			}
			free(path);	
		}
	}
	if(closedir(dir) == -1){
		perror("closedir");
		exit(1);
	}

}

/// <summary>
/// Displays the information about the entry based on option -l, -s.
/// </summary>
/// <param name="statVal">Stat value.</param>
/// <param name="entryInfo">entryInfo.</param>
void displayContentListing(int statVal,struct stat entryInfo){
	//Displays number of blocks for an entry if option -s is set
	totalBlockCount += (long long) entryInfo.st_blocks/2;

	if(flag[0] == 1){
		if(statVal == 0){
			printf("\t%lld",(long long) entryInfo.st_blocks/2);
		}
	}
	//Displays detailed info about an entry if option -l is set (i.e. long listing)
	if(flag[1] == 1){
		char permissionmode[10];
		int i;
		struct passwd* userInfo;
		struct group* groupInfo;
		char* modifiedTime;
		modifiedTime = (char*)malloc(50);
		for(i = 0; i < 10; i++){
			permissionmode[i] = '-';
		}
		if(statVal == 0){
			if(S_ISDIR(entryInfo.st_mode))
				permissionmode[0] = 'd';
			else if(S_ISCHR(entryInfo.st_mode))
				permissionmode[0] = 'c';
			else if(S_ISBLK(entryInfo.st_mode))
				permissionmode[0] = 'b';
			else if(S_ISLNK(entryInfo.st_mode))
				permissionmode[0] = 'l';
			else if(S_ISFIFO(entryInfo.st_mode))
				permissionmode[0] = 'f';
			else if(S_ISSOCK(entryInfo.st_mode))
				permissionmode[0] = 's';
			else //regular files
				permissionmode[0] = '-';

			if((entryInfo.st_mode & S_IRUSR) !=0)
				permissionmode[1] = 'r';
			if((entryInfo.st_mode & S_IWUSR) != 0)
				permissionmode[2] = 'w';
			if((entryInfo.st_mode & S_IXUSR) != 0)
				permissionmode[3] = 'x';
			if((entryInfo.st_mode & S_IRGRP) != 0)
				permissionmode[4] = 'r';
			if((entryInfo.st_mode & S_IWGRP) != 0)
				permissionmode[5] = 'w';
			if((entryInfo.st_mode & S_IXGRP) != 0)
				permissionmode[6] = 'x';
			if((entryInfo.st_mode & S_IROTH) != 0)
				permissionmode[7] = 'r';
			if((entryInfo.st_mode & S_IWOTH) != 0)
				permissionmode[8] = 'w';
			if((entryInfo.st_mode & S_IXOTH) != 0)
				permissionmode[9] = 'x';
			userInfo = getpwuid(entryInfo.st_uid);
			groupInfo = getgrgid(entryInfo.st_gid);
			strftime(modifiedTime,50,"%h %e %g %H:%M",localtime(&entryInfo.st_mtime));
			printf("\t");
			for(i=0; i< 10; i++){
				printf("%c",permissionmode[i]);
			}
			printf("\t%ld\t%s\t%s\t%d\t%s\t",
				(long)entryInfo.st_nlink,userInfo->pw_name,groupInfo->gr_name,entryInfo.st_size,modifiedTime);
		}
		free(modifiedTime);
	}
}

/// <summary>
/// Starts listing.
/// </summary>
/// <param name="dirPath">Directory path.</param>
void startListing(char* dirPath){
	struct stat entryInfo;
	int statVal = stat(dirPath,&entryInfo);
	if(statVal == -1){
		perror("Could not access");
		exit(1);
	}
	//Checks if the path provided via argument is file 
	if(!S_ISDIR(entryInfo.st_mode)){
		displayContentListing(statVal,entryInfo);
		printf("\t%s\n ",dirPath);
	}else{ //if the path is directory
		if(dirPath[strlen(dirPath)-1] != '/'){
                	strcat(dirPath,"/");
                }
		//Check for recursive flag (i.e. whether -R option is set or not)
		if(flag[2] == 1){
			listEntriesRecursive(dirPath);
		}else{
			printf("%s:\n",dirPath);
			getContent(dirPath);
			printf("\n");
		}
	}
}

int main(int argc,char* argv[]){
	int optVal;
	while((optVal = getopt(argc,argv,"slR")) != -1){
		switch(optVal){
		case 's':
			flag[0] = 1;
			break;
		case 'l':
			flag[1] = 1;
			break;
		case 'R':
			flag[2] = 1;
			break;
		case '?':
			printf("Usage: myls [slR] [files and/or directories]\n");
			exit(1);		
		}
	}
	//if no directory or file is provided, long list using  the current directory (default one)
	if(optind == argc){
		startListing("./");
	}else{
		//Sets the directory/file path if it is given via arguments
		int i;
		for(i = optind; i < argc; i++){
			char *dirPath = (char*) malloc(strlen(argv[i])+1);
			strcpy(dirPath,argv[i]);			
			startListing(dirPath);
			free(dirPath);
		}
	}
	exit(0);
}
