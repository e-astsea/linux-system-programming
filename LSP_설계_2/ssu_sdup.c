	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include<sys/stat.h>
	#include<sys/types.h>
	#include<unistd.h>
	#include<dirent.h>
	#include <sys/wait.h>

	#define ARGMAX 5
    #define BUFMAX 1024
    #define PATHMAX 1024



	int split(char* string, char* seperator, char* argv[]);


	int main(void){
	int argc = 0;
    char input[BUFMAX];
	char* argv[ARGMAX];
    
	pid_t pid;
    while (1) {
		char cwd[PATHMAX];
		getcwd(cwd, sizeof(cwd));
		printf("20182586> ");
        
		
        fgets(input, sizeof(input), stdin);
		input[strlen(input) - 1] = '\0';

		argc = split(input, " ", argv);

		if (argc == 0)
			continue;
		if (strcmp(argv[0], "fmd5")==0){
            if (argc != ARGMAX) {                           //입력 인자수 제어
					int status;
					pid=fork();
					if(pid==0){
						char filepath[BUFMAX]="";
						strcat(filepath,cwd);
						strcat(filepath,"/");
						strcat(filepath,"help");

						char *argz[] = {"help", argv[1], argv[2], argv[3],argv[4],(char *) 0};
						execv(filepath, argz);
					}
					if (wait(&status) != pid) { 
							fprintf(stderr, "wait error\n"); 
							exit(1); 
					}
				continue;
        	}
			int status;
			pid=fork();
			if(pid==0){
				char filepath[BUFMAX]="";
				strcat(filepath,cwd);
				strcat(filepath,"/");
				strcat(filepath,"fmd5");

				char *argz[] = {"fmd5", argv[1], argv[2], argv[3],argv[4],(char *) 0};
    			execv(filepath, argz);
			}


			if (wait(&status) != pid) { 
					fprintf(stderr, "wait error\n"); 
					exit(1); 
			}
		}

		else if (strcmp(argv[0], "fsha1")==0){
            if (argc != ARGMAX) {                           //입력 인자수 제어
				int status;
				pid=fork();
				if(pid==0){
					char filepath[BUFMAX]="";
					strcat(filepath,cwd);
					strcat(filepath,"/");
					strcat(filepath,"ssu_help");

					char *argz[] = {"ssu_help", argv[1], argv[2], argv[3],argv[4],(char *) 0};
					execv(filepath, argz);
				}
				if (wait(&status) != pid) { 
						fprintf(stderr, "wait error\n"); 
						exit(1); 
				}
				continue;
        	}
			int status;
			pid=fork();
			if(pid==0){
				char filepath[BUFMAX]="";
				strcat(filepath,cwd);
				strcat(filepath,"/");
				strcat(filepath,"fsha1");

				char *argz[] = {"fsha1", argv[1], argv[2], argv[3],argv[4],(char *) 0};
    			execv(filepath, argz);
			}


			if (wait(&status) != pid) { 
					fprintf(stderr, "wait error\n"); 
					exit(1); 
			}
		}

		else if (!strcmp(argv[0], "help")){

			int status;
			pid=fork();
			if(pid==0){
				char filepath[BUFMAX]="";
				strcat(filepath,cwd);
				strcat(filepath,"/");
				strcat(filepath,"help");

				char *argz[] = {"help", argv[1], argv[2], argv[3],argv[4],(char *) 0};
    			execv(filepath, argz);
			}
			if (wait(&status) != pid) { 
					fprintf(stderr, "wait error\n"); 
					exit(1); 
			}
		} 

		else if (!strcmp(argv[0], "exit")) {
			printf("Prompt End\n");
			break;
		}

		else {
			int status;
			pid=fork();
			if(pid==0){
				char filepath[BUFMAX]="";
				strcat(filepath,cwd);
				strcat(filepath,"/");
				strcat(filepath,"help");

				char *argz[] = {"help", argv[1], argv[2], argv[3],argv[4],(char *) 0};
    			execv(filepath, argz);
			}
			if (wait(&status) != pid) { 
					fprintf(stderr, "wait error\n"); 
					exit(1); 
			}
		}

	}

    }


	// 입력 문자열 토크나이징
	int split(char* string, char* seperator, char* argv[])
	{
		int argc = 0;
		char* ptr = NULL;

		ptr = strtok(string, seperator);
		while (ptr != NULL) {
			argv[argc++] = ptr;
			ptr = strtok(NULL, " ");
		}

		return argc;
	}