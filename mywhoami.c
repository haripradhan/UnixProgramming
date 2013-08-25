///Name: Hari Pradhan
///Date: 06/10/2013
///Purpose: This program displays the following information.
///	1. Full Name (assume the comment field of the password file contains the full name)
///	2. Login Name
///	3. UID
///	4. GID
///	5. Home Directory
///	6. Default Shell
///	7. Host Name
///	8. Machine

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/utsname.h>
int main(int argc, char* argv[])
{	
	struct passwd* userInfo;
	struct utsname sysInfo;
	char hostname[20];

	uid_t userid = getuid();
	userInfo = getpwuid(userid);
	if(uname(&sysInfo) == -1){
		perror("uname\n");
		exit(1);
	}
	if(gethostname(hostname, sizeof(hostname)) == -1){
		perror("gethostname\n");
		exit(1);
	}
	printf("Full Name: %s\n",userInfo->pw_gecos);	
	printf("Login Name: %s\n",userInfo->pw_name);	
	printf("UID: %d\n",userid);	
	printf("GID: %d\n",userInfo->pw_gid);	
	printf("Home Directory: %s\n",userInfo->pw_dir);	
	printf("Default Shell: %s\n",userInfo->pw_shell);	
	printf("Host Name: %s\n",hostname);	
	printf("Machine: %s\n\n",sysInfo.machine);	
	exit(0);
}
