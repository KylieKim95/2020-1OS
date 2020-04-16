#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

int main(){

	struct passwd *u_info;
	int n_buf = 0;
	char uid[128] = {0x0,};
	char pid[128] = {0x0,};
	char uname[128] = {0x0,};
	char fname[128] = {0x0,};
	FILE *fp;

	printf("Enter (1)The user name,(2)The file name(Must be seperated by ' (single space)') :");
	scanf("%s %s", &uname, &fname);

	if(u_info = getpwnam(uname)){

		sprintf(uid,"%d", u_info->pw_uid);
		sprintf(pid,"%d", how_can_I_get_this); // didn't complete
		printf("uid: %s, pid:%s. file: %s\n", uid, pid, fname); 
		
		fp = fopen("hello.txt","r+");
		fprintf(fp,"%s %s %s",uid, pid, fname);
		fclose(fp);
		
	
	}else{

		printf("\nError! Please check the username.\n");

	}

	return 0;
}                
