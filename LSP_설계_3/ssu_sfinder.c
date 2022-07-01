#include "ssu_sfinder.h"

char *startpoint_sfinder=NULL;
int listchecker=0;
int trashchecker=0;

void list_opt(int argc, char** argv, char *List_type, char * Category, char *Order);
void fmd5_opt(int argc, char** argv, char *file_extension,char *minsize_str,char *maxsize_str,char *target_directory, char *thread_num_str);
void fmd5_opt_not(int argc, char** argv, char *file_extension,char *minsize_str,char *maxsize_str,char *target_directory);
void trash_opt(int argc, char** argv, char * Category, char *Order);

int split(char* string, char* seperator, char* argv[]);
void command_help(void);
void ssu_find_md5(char *file_extension,char *minsize,char *maxsize,char *target_directory,char *thread_num,List *linkedlist);

        

int main(void)
{
    if(startpoint_sfinder!=NULL){                   //시작 지점을 파일의 처음 위치로 옮기기
            chdir(startpoint_sfinder);
	}
	char cwd[PATHMAX];                              //시작 지점 구하기
	getcwd(cwd, sizeof(cwd));

	List list;                                      //fmd5링크드리스트
	List trash_link;                                //trash링크드리스트
	chdir(getenv("HOME"));                          //trash만들기위한 접근
	mkdir(".Trash",0777);
	chdir(".Trash");                                //trash만들기
	mkdir("info",0777);                             //정보 있는 info 디렉토리만들기
	mkdir("files",0777);                            //삭제된파일 모아놓을 files 디렉토리 만들기
	

	while (1) {
		chdir(cwd);                             //시작위치로 계속 초기화

		int argc = 0;
		char input[STRMAX];                     //fgets로 입력받을공간
		char *argv[ARGMAX];
		memset(input,0, STRMAX);
		memset(argv, 0, ARGMAX);

		char file_extension[STRMAX]=" ",target_directory[STRMAX]=" ",minsize_str[STRMAX]=" ",maxsize_str[STRMAX]=" ",thread_num_str[STRMAX]=" ";
		char List_type[STRMAX]=" ",Category[STRMAX]=" ",Order[STRMAX]=" ",Category_trash[STRMAX]=" ",Order_trash[STRMAX]=" ";
				
		printf("20182586> ");                   //학번프롬포트

		fgets(input, sizeof(input), stdin);     //명령어들 입력받기
		
		input[strlen(input) - 1] = '\0';
		argc = split(input, " ", argv);         //입력받은 값 분해해주기
		
		if (argc == 0)
				continue;

		if (!strcmp(argv[0], "fmd5")){          //fdm5 명령어 입력받았을 경우
				if(argc<=9){                    //인자 에러처리
						memcpy(thread_num_str, "1", STRMAX);
						fmd5_opt_not(argc, argv, file_extension,minsize_str,maxsize_str,target_directory);              //쓰레드 숫자 입력안했을 경우
				}
				else{
						fmd5_opt(argc, argv, file_extension,minsize_str,maxsize_str,target_directory, thread_num_str);  //쓰레드 숫자 입력했을 경우
				}
				createList(&list);                              // 링크드 리스트 열기
				ssu_find_md5(file_extension,minsize_str,maxsize_str,target_directory,thread_num_str,&list);             //링크드리스트랑 입력받은 인자 넘기기
				listchecker=1;                                                                                          //list 함수 호출할때 사용되는 flag
		}
		else if (!strcmp(argv[0], "list")) {    //list 명령어 입력받았을 경우
				if(listchecker!=0){             //fmd5 탐색 한번은 해야함
						list_opt(argc,argv,List_type,Category,Order);   //list 옵션처리
						createList(&list);                              //list 를 위한 링크드리스트만들기
						ssu_list(&list,List_type,Category,Order);       // 함수 호출
				}
		}
		else if (!strcmp(argv[0], "trash")){    //trash 명령어 입력받았을 경우
				trash_opt(argc,argv,Category_trash,Order_trash);        //trash 옵션처리
				createList(&trash_link);                                //trash용 링크드리스트만들기
				trash(&trash_link,Category_trash,Order_trash);          //trash 함수호출
				trashchecker=1;                                         //restore시 trash호출했는지 확인 할 flag
		}
		else if (!strcmp(argv[0], "restore")) { //restroe 명령어 입력받았을 경우
				if(trashchecker==1){            //trash호출 후 호출되었는지 확인
						if(argc<2){             //인자 확인
								fprintf(stderr,"argc error\n");
								continue;
						}
						.(&trash_link,argv[1]);   //함수호출
						createList(&trash_link);        //링크드리스트
						trash(&trash_link,"filename","1");      //리스트 후에 trash 출력하기위함.
				}
		}
		else if (!strcmp(argv[0], "exit")) //exit호출 시 종료
				break;
		else if (!strcmp(argv[0], "help")) {    //help 호출 시 help함수호출
				command_help();
		}
		else
				command_help();         //이외의 입력 help출력
	}
	printf("Prompt End\n");
	return 0;
}

void trash_opt(int argc, char** argv, char * Category, char *Order){	//trash option 인자 처리 함수
    char opt=0;
    optind =1;	//option index 초기화
    while((opt = getopt(argc, argv, "c:o:")) != -1){		//c~ o~ 만 받음
		switch(opt){
			case 'c':
					memcpy(Category, optarg, STRMAX);
					break;
			case 'o':
					memcpy(Order, optarg, STRMAX);
					break;
			}
    }
}

void list_opt(int argc, char** argv, char *List_type, char * Category, char *Order){	//list option 인자 처리 함수
	char opt=0;
	optind =1;
	while((opt = getopt(argc, argv, "l:c:o:")) != -1){		//l~ c~ o~ 만 받음
		switch(opt){
			case 'l':
					memcpy(List_type, optarg, STRMAX);
					break;
			case 'c':
					memcpy(Category, optarg, STRMAX);
					break;
			case 'o':
					memcpy(Order, optarg, STRMAX);
					break;
			}
	}
}

void fmd5_opt(int argc, char** argv, char *file_extension,char *minsize_str,char *maxsize_str,char *target_directory, char *thread_num_str){	//fmd5 옵션 처리 함수
    char opt=0;
    optind =1;
    while((opt = getopt(argc, argv, "e:l:h:d:t:")) != -1){	//e~ l~ h~ d~ t~ 만 받음
		switch(opt){
			case 'e':
					memcpy(file_extension, optarg, STRMAX);
					break;
			case 'l':
					memcpy(minsize_str, optarg, STRMAX);
					break;
			case 'h':
					memcpy(maxsize_str, optarg, STRMAX);
					break;
			case 'd':
					memcpy(target_directory, optarg, STRMAX);
					break;
			case 't':
					memcpy(thread_num_str, optarg, STRMAX);
					break;
		}
	}
}

void fmd5_opt_not(int argc, char** argv, char *file_extension,char *minsize_str,char *maxsize_str,char *target_directory){		//fdm5의 쓰레드입력없을시 함수
    char opt;
    optind =1;
    while((opt = getopt(argc, argv, "e:l:h:d:")) != -1){	//e~ l~ h~ d~만 받음
		switch(opt){
			case 'e':
					memcpy(file_extension, optarg, STRMAX);
					break;
			case 'l':
					memcpy(minsize_str, optarg, STRMAX);
					break;
			case 'h':
					memcpy(maxsize_str, optarg, STRMAX);
					break;
			case 'd':
					memcpy(target_directory, optarg, STRMAX);
					break;
		}
	}
}


int split(char* string, char* seperator, char* argv[])		//입력값 seperator 로 분리해주는 함수
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

void command_help(void){		//도움말 명령어 함수
    printf("Usage:\n");
    printf("  > fmd5/fsha1 -e [FILE_EXTENSION] -l [MINSIZE] -h [MAXSIZE] -d [TARGET_DIRECTORY] -t [THREAD_NUM]\n");
    printf("     >> delete -l [SET_INDEX] -d [OPTARG] -i -f -t\n");
    printf("  > trash -c [CATEGORY] -o [ORDER]\n");
    printf("  > restore [RESTORE_INDEX]\n");
    printf("  > help\n");
    printf("  > exit\n\n");
}


void createList(List *linkedlist) {                     // 링크드리스트를 만들어지는 함수
	linkedlist->head = (Node*)malloc(sizeof(Node));
	linkedlist->tail = (Node*)malloc(sizeof(Node));
	linkedlist->head->next = linkedlist->tail;          // head와 tail 연결해줌
	linkedlist->tail->next = linkedlist->tail;
	linkedlist->size=0;
}
