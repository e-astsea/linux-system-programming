    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include<sys/stat.h>
    #include<sys/types.h>
    #include<unistd.h>
    #include<dirent.h>
    #include <openssl/md5.h>
    #include <sys/time.h>
    #include <time.h>
    #include <fcntl.h>
    #define ARGMAX 5

    #define BUFMAX 1024
    #define PATHMAX 1024
    #define makedeletefile "tmpfile.txt"        //bfs 로 읽어온 파일 저장하는 txt파일

    typedef struct node {                       //링크드리스트 data 부분
        int filesize;
        char filehash[BUFMAX];
        char name[BUFMAX];
        char filepath[BUFMAX];
        char file_mtime[BUFMAX];
        char file_atime[BUFMAX];
        struct node *next;
    } Node;

     typedef struct Node // 큐의 노드 부분
    {
        char *data;
        struct Node *next;
    }Node_q;

    typedef struct Queue {
        Node_q* front;
        Node_q* rear;
        int count;
    }Queue;
    
    typedef struct {    // 링크드리스트 부분
        Node *head;     // 리스트의 시작 노드
        Node *tail;     // 리스트의 마지막 노드
        int size;       // 리스트의 노드 갯수
    } List;


    void update_link(List* list);
    void option(List* linklist);
    void makelink(FILE* readfile,List* list);
    void command_fmd5( char* argv[]);
    void read_directory(char* target_dir, Queue* q, char* file_extension, FILE* inputfile, long double minsize, long double maxsize);
    
    double strTodouble(char *str);
    int size_check(char *argv,char *size,char *size_int);

    void initQueue(Queue* queue);
    int isEmpty(Queue *queue);
    void enqueue(Queue *queue, char *data);
    char *dequeue(Queue *queue);

    void printList(List *list);
    void addFirst(List *linkedlist,int filesize, char *filehash, char* filename, char *filepath,char *file_mtime,char *file_atime);
    void createList(List *linkedlist);
    void sortList(List* linkedlist);

    char* extract_name(char* string,char oper);
    char* get_time(time_t stime);
    int md5(char *filename, char *md5str);
    void ssu_runtime(struct timeval *start_time, struct timeval *end_time);
    char * number_comma(int size);
    long double doublesize_calc(char *str,long double num);
    int my_atoi(const char* str);
    int split_file(char* string, char* seperator, char* argv[]);
    int split(char* string, char* seperator, char* argv[]);

    char *startpoint=NULL;
    char *argv4point=NULL;
    char *argvstart=NULL;
    char *argvstartpoint=NULL;
    int print_cnt=0;

    int main(int argc, char* argv[]){
        if(startpoint!=NULL){                           //시작 지점을 파일의 처음 위치로 옮겨
            chdir(startpoint);
        }
        
        char cwd[PATHMAX];                              //시작 지점 구하기
        getcwd(cwd, sizeof(cwd));
        startpoint = cwd;
        argvstart=argv[4];

        if((strcmp(argv[1],"*")!=0)){                   // 확장자 예외처리
            if(argv[1][0]=='*' && argv[1][1]=='.'){     // *, *. 만 받음
            }
            else{
                printf("ERROR: FILE_EXTENSION error\n");
                return 0;
            }
        }

        command_fmd5(argv);                        //argc argv 인자로 주고 fmd5 명령어 함수 호출
    }


    void command_fmd5( char* argv[]){          // argc argv로 명령어   
        char deltxt[BUFMAX];                            // tmp파일 삭제*/
        char cwd[PATHMAX];                              // 파일 경로 저장
        char file_extension[BUFMAX];                    // 파일 확장자 저장
        char target_dir[PATHMAX];                       // 처음에 탐색을 하는 target_directory

        struct stat statbuf;                            // stat구조체
        char minsize_int[BUFMAX]="",maxsize_int[BUFMAX]="";            //kb mb gb 구별
        char minsize_str[3]="",maxsize_str[3]="";                      //kb mb gb 구별
        long double minsize=0, maxsize=0;                          //size 값

        
        if(size_check(argv[2],minsize_str, minsize_int)==-1 || size_check(argv[3],maxsize_str, maxsize_int) ==-1 ){    //세번째 인자 minsize하고 maxsize 분리 argv 2 3
            return; 
        }
        if(strcmp(argv[2],"kb")==0 || strcmp(argv[2],"KB")==0 || strcmp(argv[2],"mb")==0 ||strcmp(argv[2],"MB")==0||strcmp(argv[2],"gb")==0||strcmp(argv[2],"GB")==0) {
            printf("size error\n");
            return;
        }
        if(strcmp(argv[3],"kb")==0 || strcmp(argv[3],"KB")==0 || strcmp(argv[3],"mb")==0 ||strcmp(argv[3],"MB")==0||strcmp(argv[3],"gb")==0||strcmp(argv[3],"GB")==0) {
            printf("size error\n");
            return;
        }

        for(int i=0; i<strlen(minsize_int); i++){                           // 2번째인자 minsize 에러처리
            if((minsize_int[i]<48 || minsize_int[i]>57)){
                if(minsize_int[i]=='.' && i!=strlen(minsize_int)-1){
                    continue;
                }
                if(strcmp(argv[2],"~")==0 ){
                    continue;
                }
                printf("size error\n");
                return ;
            }
        }

        for(int i=0; i<strlen(maxsize_int); i++){                           // 3번째 인자 maxsize 에러처리
            if((maxsize_int[i]<48 || maxsize_int[i]>57)){
                if(maxsize_int[i]=='.' && i!=strlen(maxsize_int)-1){
                    continue;
                }
                if(strcmp(argv[3],"~")==0){
                    continue;
                }
                printf("size error%s\n",argv[3]);
                return ;
            }
        }
        
        if(strrchr(minsize_int,'.')!=NULL){                                 // 실수일 때
            minsize = strTodouble(minsize_int);
            if(minsize==-1){
                return;
            }
        }
        else{
            minsize=my_atoi(minsize_int);                                   // 정수일 때
        }
        minsize = doublesize_calc(minsize_str,minsize);                     //minsize를 kb mb gb 계산해주는과정.

        if(strrchr(maxsize_int,'.')!=NULL){                                 // 실수일 때
            maxsize = strTodouble(maxsize_int);
            if(maxsize==-1){
                return;
            }
        }
        else{
            maxsize = my_atoi(maxsize_int);                                 // 정수일 때
        }
        maxsize = doublesize_calc(maxsize_str,maxsize);                     // maxsize를 kb mb gb 계산해주는 과정

        if(strcmp(argv[2],"~")==0){                                         // 최소크기 ~ 로 입력시 0
            minsize= 0;
        }
        if(strcmp(argv[3],"~")==0){                                         // 최대크기 ~ 로 입력시 long의 최대크기
            maxsize= 4294967295;
        }

        if(argv[4][0]=='~'){     //네 번째 인자 targe_directory
            chdir(getenv("HOME")); 
            getcwd(cwd, sizeof(cwd));
            strcpy(target_dir,cwd);
            char cwd[PATHMAX];
            char *ptr1 = strtok(argv[4], "~");          //~제거
            char *ptr2 = strtok(ptr1, "/");             // / 제거
            while (ptr2 != NULL)                        //ptr이 NULL일때까지
            {
                if(chdir(ptr2)==-1){
                    printf("Error : target_dir\n");
                    return;
                }
                ptr2 = strtok(NULL, "/");               //자른 문자 다음부터 /또자르기
            }
            getcwd(cwd, sizeof(cwd));
            printf("%s",cwd);
            
            argvstartpoint=cwd;
        }
        else{
            chdir(startpoint);
            realpath(argv[4], target_dir);
            argv4point=target_dir;
            chdir(target_dir);
            getcwd(cwd, sizeof(cwd));
            chdir(cwd);
        }


        if (lstat(target_dir, &statbuf) < 0   || !S_ISDIR(statbuf.st_mode)) {       //네번째 인자 에러
            fprintf(stderr, "lstat error for %s\n", argv[4]);
            return;
        }

        struct timeval begin_t, end_t;
	    gettimeofday(&begin_t, NULL);                   //시간 측정 시작

        FILE* inputfile = fopen(makedeletefile,"w+");   // data저장할 file 오픈
        Queue q;
        initQueue(&q);                                  // dir저장할 queue 만들기
        read_directory(target_dir ,&q, argv[1], inputfile,minsize,maxsize);     // 처음입력된 dir 파일 읽어오기

        while(!isEmpty(&q)){                            // 큐에서 하나씩 빼면서 큐 빌때까지 반복
                char *bfs_dir= dequeue(&q);             // 큐에서 dir정보를 dequeue
                read_directory(bfs_dir,&q,argv[1],inputfile,minsize,maxsize);   //dequee한 dir에서 파일 읽어오기
        }
        fclose(inputfile);
   
        List list;                                      //링크드리스트
        createList(&list);                              // 링크드 리스트 열기
        chdir(cwd);                                     
        FILE* readfile = fopen(makedeletefile,"r+");    // data저장한 file 오픈

        makelink(readfile,&list);                       // data저장한 file을 기반으로 링크드리스트연결
        fclose(readfile);

                
        printList(&list);                               // 링크드리스트에있는 file 출력하는 함수
        gettimeofday(&end_t, NULL);                     // 시간 측정 종료

        ssu_runtime(&begin_t,&end_t);                   // 시간 계산

        option(&list);                                  // 옵션

       if(argvstart[0]=='~'){
                strcat(deltxt,argvstartpoint);
                strcat(deltxt,"/");
                strcat(deltxt,makedeletefile);
        }
        else{
                strcat(deltxt,argv4point);
                strcat(deltxt,"/");
                strcat(deltxt,makedeletefile);
        }
       
       unlink(deltxt);                                  // 처음에 만든 data 저장용 파일 삭제
     
     return ;
}

void option(List* linklist){
    
     while(1){
        int flag=0;
            if(print_cnt!=0){                   //print할 list가 있을때. 출력
            char gettmp[BUFMAX]=" ";
            char* arr[10];                      // 입력 값 저장할 공간
            int count=0;                        // 몇개 입력했는지 count
            printf("\n>> ");                    // >> 출력
            fgets(gettmp,BUFMAX,stdin);         // 입력값 불러오기
            gettmp[strlen(gettmp) - 1] = '\0';

            if(strcmp(gettmp,"exit")==0){       // exit 시 에러제어
                printf(">> Back to Prompt\n");

                return;
            }

            count = split(gettmp," ",arr);      // 입력 값 공백으로 split

            if(count > 3 || count < 2){         // 3개초과 2개미만은 skip
                continue;
            }

            Node *node = linklist->head;

            if(strcmp(arr[1],"d")==0 && count == 3){                    //d옵션 && 3개 입력
                if(my_atoi(arr[0])<=0 || my_atoi(arr[0])>print_cnt){    //인자값 초과되거나 잘못되면 continue
                    continue;
                }   
                int set_count = 1;                                      //set개수 count 해줌
                int set_content_count =1;                               //set내부에 내용물 개수 count

                while (node != linklist->tail) {                            // 링크드리스트 끝까지 탐색
                    if(strcmp(node->filehash,node->next->filehash)!=0){ // 파일해쉬값이 다르면 set달라지므로 count ++
                        if(set_count == my_atoi(arr[0])){               // setcount가 입력값과 같다면
                            while(1){
                                if(set_content_count == (my_atoi(arr[2]))){
                                    if(strcmp(node->filehash,node->next->filehash)!=0){ 

                                    }
                                        char temp[BUFMAX]="";           
                                        Node *delNode = node->next;     //노드삭제하는 부분
                                        node->next = delNode->next;

                                        strcat(temp,delNode->filepath); //파일삭제위해서 경로 받아오는 부분
                                        strcat(temp,"/");
                                        strcat(temp,delNode->name);

                                        unlink(temp);                   //파일 unlink로 삭제
                                        free(delNode);

                                        printf("\"%s\" has been deleted in #%d\n",temp,set_count);
                                        memset(temp,0,BUFMAX);
                                        break;
                                    }
                                    set_content_count++;
                                    node = node->next;
                                    if(strcmp(node->filehash,node->next->filehash)!=0){
                                        break;
                                    }
                                }
                        }
                    set_count++;
                    }
                node = node->next;
                }
            }

            else if(strcmp(arr[1],"i")==0 && count ==2){                    //i옵션 , 입력 2개
                if(my_atoi(arr[0])<=0 || my_atoi(arr[0])>print_cnt){        // 잘못된 입력 처리
                    continue;
                }
                int set_count = 1;
                Node *delNode_i;
                while (node != linklist->tail) {        //링크드리스트 처음부터 탐색    
                    if(strcmp(node->filehash,node->next->filehash)!=0){     //해시달라지면 count 증가
                        if(set_count == my_atoi(arr[0])){
                                while(1){
                                    char temp[BUFMAX]="";
                                    char temp_i[BUFMAX]="";
                                    delNode_i=node;
                                    node=node->next;
                                    
                                    strcat(temp,delNode_i->next->filepath);
                                    strcat(temp,"/");
                                    strcat(temp,delNode_i->next->name);


                                    printf("Delete \"%s\"? [y/n] ",temp);
                                    char input[BUFMAX]="";
                                    fgets(input,BUFMAX,stdin);
                                    input[strlen(input) - 1] = '\0';

                                    if(strcmp(input,"y")==0 || strcmp(input,"Y")==0){   //입력값이 y Y면 삭제하는 코드
                                        strcat(temp_i,delNode_i->next->filepath);
                                        strcat(temp_i,"/");
                                        strcat(temp_i,delNode_i->next->name);
                                        
                                        node = delNode_i;
                                        delNode_i = delNode_i->next;
                                        char delhash[BUFMAX]="";
                                        strcat(delhash,delNode_i->filehash);
                                        node->next = delNode_i->next;

                                        unlink(temp_i);
                                       
                                        free(delNode_i);

                                        if(node->next == linklist->tail){
                                            break;
                                        }
                                        if(strcmp(delhash,node->next->filehash) !=0 ){
                                            break;
                                        }
                                    }
                                    else if(strcmp(input,"n")==0||strcmp(input,"N")==0){    //입력값이 N n 이면 삭제하지 않음
                                        if(strcmp(node->filehash,node->next->filehash) !=0 ){
                                            break;
                                        }
                                        continue;
                                    }
                                    else{
                                        flag=1;                     //에러처리
                                        printf("type error\n");
                                        break;
                                    }           
                                }
                            }
                            set_count++;
                        }
                node = node->next;
                }
            }
            
            else if(strcmp(arr[1],"f")==0 && count ==2){            // f입력과 2개만입력시
                if(my_atoi(arr[0])<=0 || my_atoi(arr[0])>print_cnt){// 에러처리
                    continue;
                }
                int foption_cnt=1;
                Node *delNode_f2;

                while (node != linklist->tail) {    //링크드리스트 처음부터 끝까지탐색
                     if(strcmp(node->filehash,node->next->filehash) != 0){
                         if(foption_cnt == my_atoi(arr[0])){        //입력값하고 세트값이 같을 때
                            delNode_f2=node;                        
                            node =node->next;

                            while(1){
                                char temp[BUFMAX]="";
                                if( node->next == linklist->tail){      //끝이면 break
                                    break;
                                }
                                if(strcmp(node->filehash,node->next->filehash) != 0){       //filehash가 달라지면 다른세트이므로 break
                                    break;
                                }
                                if( strcmp(node->filehash,node->next->filehash) == 0){
                                        for(int i=1; i<=strlen(node->file_mtime);i++){      //file mtime중 큰값이 무엇인지 찾는 반복문
                                            if((node->file_mtime)[i]==(node->next->file_mtime)[i] && i!=strlen(node->file_mtime)){      
                                                if(strcmp(node->filehash,node->next->filehash) != 0){       //filehash가 달라지면 다른세트이므로 break
                                                        break;
                                                }
                                                continue;
                                            }

                                            else if((node->file_mtime)[i]<(node->next->file_mtime)[i]){     //node 보다 node->next의 시간이 최근일경우
                                                node = delNode_f2;
                                                delNode_f2 = delNode_f2->next;                          // node를 뒤따라오던 delNode와 node 위치변경
                                                    
                                                strcat(temp,delNode_f2->filepath);
                                                strcat(temp,"/");
                                                strcat(temp,delNode_f2->name);

                                                node->next = delNode_f2->next;
                                                unlink(temp);                                           //파일삭제
                                                node=node->next;
                                                if(strcmp(node->filehash,node->next->filehash) != 0 || node->next == linklist->tail){
                                                    break;
                                                }
                                                break;
                                            }
                                            else if((node->file_mtime)[i]>(node->next->file_mtime)[i] || i==strlen(node->file_mtime)){  //node의 mtime이 더빠를경우
                                                Node *delNode_f1;
                                                delNode_f1 = node->next;
                                                strcat(temp,delNode_f1->filepath);
                                                strcat(temp,"/");
                                                strcat(temp,delNode_f1->name);
                                                    
                                                node->next = delNode_f1->next;
                                                unlink(temp);                                                       //파일 삭제
                                                free(delNode_f1);
                                                if(strcmp(node->filehash,node->next->filehash) != 0){
                                                    break;
                                                }
                                                break;
                                            }
                                        }
                                        continue;
                                }

                                if( node->next == linklist->tail){              //파일 끝이면 break
                                      break;
                                }

                                delNode_f2 = node;                          //delnode는 node를 뒤따라오는 뒤에있는 함수
                                node =node->next;
                                
                                if(strcmp(node->filehash,node->next->filehash) != 0){
                                    break;
                                }                                
                            }
                        printf("Left file in #%d : %s/%s (%s)\n",my_atoi(arr[0]), node->filepath,node->name,node->file_mtime);
                        }
                    foption_cnt++;
                    }
                    if( node->next == linklist->tail){      //파일끝이면 break
                        break;
                    }
                    delNode_f2 = node;
                    node =node->next;
                } 
            }

            else if(strcmp(arr[1],"t")==0 && count ==2){                   // t옵션과 2개 입력
                if(my_atoi(arr[0])<=0 || my_atoi(arr[0])>print_cnt){
                    continue;
                }
                char directoryPath[BUFMAX] = "";
                strcpy(directoryPath,startpoint);
                strcat(directoryPath,"/");
                strcat(directoryPath,"Trash");
	            mkdir( directoryPath ,0777);                               // 쓰레기통 777옵션으로 생성

                int foption_cnt=1;
                Node *delNode_f2;
                while (node != linklist->tail) {
                     if(strcmp(node->filehash,node->next->filehash) != 0){
                         if(foption_cnt == my_atoi(arr[0])){                //입력값하고 세트값이 같을 때
                            delNode_f2=node;
                            node =node->next;

                            while(1){
                                char temp[BUFMAX]="";
                                if( node->next == linklist->tail){              // 끝이면 break
                                    break;
                                }
                                if(strcmp(node->filehash,node->next->filehash) != 0){      //filehash가 달라지면 다른세트이므로 break     
                                    break;
                                }
                                if( strcmp(node->filehash,node->next->filehash) == 0){
                                    for(int i=1; i<=strlen(node->file_mtime);i++){          //file mtime중 큰값이 무엇인지 찾는 반복문
                                        if((node->file_mtime)[i]==(node->next->file_mtime)[i] && i!=strlen(node->file_mtime)){
                                            if(strcmp(node->filehash,node->next->filehash) != 0){       //filehash가 달라지면 다른세트이므로 break
                                            break;
                                            }
                                            continue;
                                        }
                                            else if((node->file_mtime)[i]<(node->next->file_mtime)[i]){ //node 보다 node->next의 시간이 최근일경우
                                                    node = delNode_f2;
                                                    delNode_f2 = delNode_f2->next;
                                                    
                                                    strcat(temp,delNode_f2->filepath);
                                                    strcat(temp,"/");
                                                    strcat(temp,delNode_f2->name);

                                                    node->next = delNode_f2->next;

                                                    
                                                    char copyfilename[BUFMAX]="";
                                                    strcat(copyfilename,directoryPath);
                                                    strcat(copyfilename,"/");
                                                    strcat(copyfilename,delNode_f2->name);
                                                    link(temp,copyfilename);                         //새로운 링크파일 쓰레기통에 만듬

                                                    unlink(temp);                                       //원본 파일의 link를 끊어준다.
                                                    node=node->next;
                                                    if(strcmp(node->filehash,node->next->filehash) != 0 || node->next == linklist->tail){
                                                        break;
                                                    }
                                                    break;
                                            }
                                            else if((node->file_mtime)[i]>(node->next->file_mtime)[i] || i==strlen(node->file_mtime)){      //node의 mtime이 더빠를경우
                                                Node *delNode_f1;
                                                delNode_f1 = node->next;
                                                strcat(temp,delNode_f1->filepath);
                                                strcat(temp,"/");
                                                strcat(temp,delNode_f1->name);
                                                    
                                                node->next = delNode_f1->next;

                                                char copyfilename[BUFMAX]="";
                                                strcat(copyfilename,directoryPath);
                                                strcat(copyfilename,"/");
                                                strcat(copyfilename,delNode_f1->name);
                                                link(temp,copyfilename);             //새로운 링크파일 쓰레기통에 생성
                                                unlink(temp);                           // 원래있던 파일 링크없애기
                                                free(delNode_f1);
                                                if(strcmp(node->filehash,node->next->filehash) != 0){
                                                    break;
                                                }
                                                break;
                                            }
                                        }
                                        continue;
                                }

                                if( node->next == linklist->tail){
                                      break;
                                }
                                delNode_f2 = node;                  //delnode는 node를 뒤따라오는 node
                                node =node->next;
                                
                                if(strcmp(node->filehash,node->next->filehash) != 0){
                                    break;
                                }
                            }
                            printf("All files in #%d have moved to Trash except \"%s/%s\" (%s)\n",my_atoi(arr[0]), node->filepath,node->name,node->file_mtime);
                        }
                        foption_cnt++;
                    }
                    if( node->next == linklist->tail){
                        break;
                    }
                    delNode_f2 = node;
                    node =node->next;
                } 
            }
            else{
                printf("input error");
                continue;
            }    
        }

        update_link(linklist);                      //옵션으로 바뀐링크를 업데이트해주는 함수

        if(linklist->head->next == linklist->tail){
            return;
        }

        if(flag==0)
        printList(linklist);
    }
}
    //옵션 후 바뀐 링크드리스트 업데이트 해주는 함수
    void update_link(List* list){       
        Node *node = list->head;
        int j = 1;
        char tmp[BUFMAX]="";
            while (node != list->tail) {
                if(strcmp(node->filehash,node->next->filehash)!=0 && strcmp(node->next->filehash,node->next->next->filehash)!=0){
                        Node *delNode = node->next;
                        node->next = delNode->next;
                        free(delNode);
                        break;
                }
                node = node->next;
            }
    }



    //txt에저장된 파일을 대상으로 연결리스트를 만드는 과정
    void makelink(FILE* readfile, List* list){                          

        long long fzero=0, fstart=0, fmid=0;        // file읽을때 위치 저장해놓을 변수
        fseek(readfile,0,SEEK_SET);                 // 파일의 처음부터 시작

        while(!feof(readfile)){     //파일이 끝날때 까지
            char check[BUFMAX],size[BUFMAX],name[BUFMAX],hash[BUFMAX],path[BUFMAX],mtime[BUFMAX],mtime_t[BUFMAX],atime[BUFMAX],atime_t[BUFMAX];
            char tmp[BUFMAX]=" ";
            char* tmpsave[10];      // 파일내부 정보를 | 로 split한 값 저장할 공간

            fzero=ftell(readfile);
                        
            fgets(tmp,sizeof(tmp),readfile);        //파일로부터 한줄씩 읽어옴
            tmp[strlen(tmp) - 1] = '\0';
            int count = split_file(tmp, "|",tmpsave);
        
            if(feof(readfile)!=0){                  //에러처리
                return;
            }
            
            
            strcpy(check ,tmpsave[0]);              //스플릿된 0번 파일이 중복된게 있는지 확인하는 flag값 check
            int check_int = my_atoi(check);         //check 값 str을 int형으로 변환
            if(check_int==1){                       //에러처리
                continue;
            }
            
            strcpy(size ,tmpsave[1]);               //스플릿된 1번 값은 size
            strcpy(name ,tmpsave[2]);               //스플릿된 2번 값은 name
            strcpy(hash,tmpsave[3]);                //스플릿된 3번 값은 hash
            strcpy(path,tmpsave[4]);                //스플릿된 4번 값은 path
            strcpy(mtime,tmpsave[5]);               //스플릿된 5번 값은 mtime
            strcpy(atime,tmpsave[6]);               //스플릿된 6번 값은 atime
            int size_int = my_atoi(size);           //size 값 str을 int형으로 변환
            fstart=ftell(readfile);                 //한줄 읽고 반복문 탐색하기 전 위치 저장
            int cnt=0;
            while(!feof(readfile)){
                
                char tmp_c[BUFMAX]=" ";
                char* tmpsave_c[10];                 // 파일내부 정보를 | 로 split한 값 저장할 공간
                char check_c[BUFMAX],size_c[BUFMAX];
                char name_c[BUFMAX]=" ",hash_c[BUFMAX],path_c[BUFMAX],mtime_c[BUFMAX],mtime_t_c[BUFMAX],atime_c[BUFMAX],atime_t_c[BUFMAX];
                    
                fmid=ftell(readfile);               // 두번째 반복문 돌기 전 위치 저장. 돌아올수도있음
                
                fgets(tmp_c,sizeof(tmp_c),readfile);    // 처음 반복문에서 fgets한 값과 같은 값 찾는 과정
                if(cnt==0){
                    cnt=1;
                    if(feof(readfile)!=0){                  //에러처리
                    printf("%s",tmp_c);
                    return;
                    }
                }
                tmp_c[strlen(tmp_c) - 1] = '\0';
                int cnt = split_file(tmp_c, "|",tmpsave_c);
                
                strcpy(check_c ,tmpsave_c[0]);          // 스플릿된 0번 파일이 중복된게 있는지 확인하는 flag값 check
                
                int check_int_c = my_atoi(check_c);     // check 값 str을 int형으로 변환
                
                if(check_int_c==1){
                        continue;
                }
            
                strcpy(size_c ,tmpsave_c[1]);           //스플릿된 1번 값은 size
                strcpy(name_c ,tmpsave_c[2]);           //스플릿된 2번 값은 name
                strcpy(hash_c,tmpsave_c[3]);            //스플릿된 3번 값은 hash
                strcpy(path_c,tmpsave_c[4]);            //스플릿된 4번 값은 path
                strcpy(mtime_c,tmpsave_c[5]);           //스플릿된 5번 값은 mtime
                strcpy(atime_c,tmpsave_c[6]);           //스플릿된 6번 값은 atime
                int size_int_c = my_atoi(size_c);       //size값을 int로전환
                
                    if(strcmp(hash,hash_c)==0 && check_int_c ==0){          // 해쉬값이 처음비교하는 줄과 같고 check_c가 0일때 linklist에 넣어줌
                        fseek(readfile,fmid, SEEK_SET);
                        fputs("1",readfile);                                // 첫반복문에있는 비교하는 줄 check 1로 바꾸어줌
                        fseek(readfile,fzero, SEEK_SET);
                        fputs("1",readfile);                                // 두번째 반복문에 있는 비교되는 줄 check 1로 바꾸어줌
                        if(check_int == 0){
                            addFirst(list,size_int,hash,name,path,mtime,atime);     // check되지 않았으면 첫반복문 비교하는줄 링크드리스트에넣어줌
                        }
                        fseek(readfile,fmid, SEEK_SET);
                        addFirst(list,size_int_c,hash_c,name_c,path_c,mtime_c,atime_c); // 두번째 반복문에 있는 비교되는 줄 링크드리스트에넣어줌
                        check_int =1; 
                    }
                    if(feof(readfile)!=0){
                        
                        fseek(readfile,fstart, SEEK_SET);
                        break;
                    }
            }
        }
        
        return;
    }


    // 디렉토리를 bfs로 읽어서 Queue에 저장 후 txt파일에 정규파일 저장
    void read_directory(char* target_dir, Queue* q, char* file_extension, FILE* inputfile, long double minsize, long double maxsize){
        int fd;
        int count;
        struct stat statbuf;
        struct dirent** namelist;
        char filename[PATHMAX];
        char filenamehash[PATHMAX];

        if(chdir(target_dir) <0 ){                                  
            return ;
        }

        strcpy(filename,target_dir);    // 시작 dir 보기사
        count = scandir(".", &namelist, NULL, alphasort);   //scandir로 dir탐색
        char *path;

        for (int i = 0; i < count; i++) {
            
            if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))        // 파일 or 디렉토리가 . .. 이면 skip
                continue;
            if(!strcmp(namelist[i]->d_name,"proc") || !strcmp(namelist[i]->d_name,"run") || !strcmp(namelist[i]->d_name,"sys")){        // 파일 or 디렉토리가 sys, run, proc 이면 skip
                continue ;
            }     
        
            if ((lstat(namelist[i]->d_name, &statbuf) < 0) &&  (!access(namelist[i]->d_name, F_OK)) ) {         //lstat 이 잘못되거나 권한없으면 에러 출력
                fprintf(stderr, "lstat error for %s\n", namelist[i]->d_name);
                exit(1);
            }

            
            if (S_ISLNK (statbuf.st_mode)  ){           //링크파일 skip
                continue;
            }
            if (!S_ISREG(statbuf.st_mode) && !S_ISDIR(statbuf.st_mode)  ){      //정규파일 and dir아니면 skip
                continue;
            }
            
            path = getcwd(NULL, BUFMAX);    // 현위치 찾기
            if(S_ISDIR(statbuf.st_mode)){   //디렉토리인것만 큐에넣기
                if(strcmp("Trash",namelist[i]->d_name)==0){ //휴지통 continue
                      continue;
                }
                strcat(path,"/");
                strcat(path,namelist[i]->d_name);
                enqueue(q, path);
                
            }
            
            else if (S_ISREG(statbuf.st_mode)){                            //일반파일들
                if(strcmp(makedeletefile,namelist[i]->d_name)==0 || statbuf.st_size==0){    //size가 0이거나 tmpfile.txt파일이면 skip.
                    continue;
                }

                if(statbuf.st_size< minsize || statbuf.st_size > maxsize){                  //size가 최소보다작고 최대보다 크면 skip
                    continue;
                }

                if(strcmp(file_extension,"*")==0){                                          //파일확장자가 * 이면 모든파일 탐색
                    md5(namelist[i]->d_name,filenamehash);                                  //맞는파일들의 MD5를 탐색함
                    fprintf(inputfile,"%d|%d",0,(int)statbuf.st_size);                      //들어갔는지안들어갔는지 확인시켜주는 FLAG,SIZE 넣음
                    fputs("|",inputfile);                                                   //구분자
                    fputs(namelist[i]->d_name,inputfile);                                   //이름,해시,경로,mtime,atime을 파일에 적음
                    fputs("|",inputfile);
                    fputs(filenamehash,inputfile);
                    fputs("|",inputfile);
                    fputs(path,inputfile);
                    fputs("|",inputfile);
                    fputs(get_time(statbuf.st_mtime),inputfile);
                    fputs("|",inputfile);
                    fputs(get_time(statbuf.st_atime),inputfile);
                    fputs("\n",inputfile);
                }
                else if(strcmp(extract_name(file_extension,'.') , extract_name(namelist[i]->d_name,'.'))==0){       //파일확장자가 *. 이면 맞는 확장자 탐색
                    char* fn = strrchr(namelist[i]->d_name, '.');
                    if(fn==NULL){
                        continue;
                    }
                    md5(namelist[i]->d_name,filenamehash);                                                          //맞는파일들의 MD5를 탐색함
                    fprintf(inputfile,"%d|%d",0,(int)statbuf.st_size);                                             //들어갔는지안들어갔는지 확인시켜주는 FLAG,SIZE 넣음
                    fputs("|",inputfile);                                                                           //구분자
                    fputs(namelist[i]->d_name,inputfile);                                                           //이름,해시,경로,mtime,atime을 파일에 적음
                    fputs("|",inputfile);
                    fputs(filenamehash,inputfile);
                    fputs("|",inputfile);
                    fputs(path,inputfile);
                    fputs("|",inputfile);
                    fputs(get_time(statbuf.st_mtime),inputfile);
                    fputs("|",inputfile);
                    fputs(get_time(statbuf.st_atime),inputfile);
                    fputs("\n",inputfile);
                }
            }
        }
        for(int idx = 0; idx < count; idx++) {  //FREE
                free(namelist[idx]); 
        }
            free(namelist);
        
    }





    double strTodouble(char *str){                                              //받은 문자열을 double형 숫자로 바꿔주는 함수
        double num1=0,num2=0;
        int flag=0;
            for(int i=0; i<strlen(str); i++){    
                if(str[i]!='.' && (str[i]<48 && str[i]>57) && str[i]!='~'){     //숫자, 소수점, ~ 가 아니면 에러처리
                    printf("type error\n");
                    return -1;
                }
                if(flag==2 || str[0]=='.'){
                    printf("type error\n");
                    return -1;
                }
                if(str[i]=='.')
                    flag++;
            }

            for(int i=0; str[i]!='.';i++){                                      // 소수부하고 정수부하고 따로 나누어서 계산
                num1 *=10;
                num1 +=(str[i]-'0');
            }

            for(int i=0; i<strlen(str); i++){
                if(str[i]=='.'){
                    for(int k=strlen(str)-1; str[k]!='.';k--){
                        num2+=(str[k]-'0');
                        num2*=0.1;
                    }
                break;
                }
            }
            return num1+num2;                                                   // 소수부 정수부 나눈 값을 합쳐줌
    }

    int size_check(char *argv,char *size,char *size_int){                               //입력으로 받은 size를 kb,KB,mb,MB,gb,GB 를 단위로 입력받는지 확인   
        int tmp=0;
            for(int i=0; argv[i] != '\0'; i++){
                if ((argv[i] >= 'a' && argv[i] <= 'z') || (argv[i] >= 'A' && argv[i] <= 'Z')){      // 입력은 모두 알파벳이여야함.
                    if(strcmp(&argv[i],"kb")==0 ||  strcmp(&argv[i],"KB")==0)   //kb,KB일때 
                    strcat(size, &argv[i]);
                    else if(strcmp(&argv[i],"mb")==0 ||  strcmp(&argv[i],"MB")==0)  //mb,MB일때
                    strcat(size, &argv[i]);
                    else if(strcmp(&argv[i],"gb")==0 ||  strcmp(&argv[i],"GB")==0)  //gb,GB 일때
                    strcat(size, &argv[i]);
                    else{
                        printf("ERROR: Size error\n");
                        return -1;
                    }
                    break;
                }
                else{
                    size_int[tmp]=argv[i];
                    tmp++;
                }
            }
    }

    //큐 부분
    void initQueue(Queue* queue) {                          // 큐를 만드는 함수
        queue->front = NULL;
        queue->rear = NULL;
        queue->count = 0;
    }

    int isEmpty(Queue *queue)                               // 큐가 비었는지 확인하는 함수
    {
        return queue->count == 0;   
    }

    void enqueue(Queue *queue, char *data){                 // 큐에 값을 넣어주는 엔큐 함수
        Node_q *newNode = (Node_q *)malloc(sizeof(Node_q));
        newNode->data = data;
        newNode->next = NULL;
        if(isEmpty(queue)){
            queue->front = newNode;
        }
        else{
            queue->rear->next = newNode;                            
        }
        queue->rear = newNode;                              // 뒤에 새로운 노드를 추가함
        queue->count++;
    }

    char* dequeue(Queue *queue){                            // 큐에 들어간 정보를 빼내는 함수 FIFO
        char *dat;
        Node_q *ptr = (Node_q *)malloc(sizeof(Node_q));
        if( isEmpty(queue)){
            printf("Empty Queue");
            return 0;
        }
        ptr = queue->front;
        dat = ptr->data;
        queue->front = ptr->next;
        free(ptr);
        queue->count--;
        return dat;
    }


    //링크드리스트 부분
    void createList(List *linkedlist) {                     // 링크드리스트를 만들어지는 함수
        linkedlist->head = (Node*)malloc(sizeof(Node));
        linkedlist->tail = (Node*)malloc(sizeof(Node));
        linkedlist->head->next = linkedlist->tail;          // head와 tail 연결해줌
        linkedlist->tail->next = linkedlist->tail;
        linkedlist->size=0;
    }


    
    void addFirst(List *linkedlist,int filesize, char *filehash, char* name, char *filepath,char *file_mtime,char *file_atime){             // 링크드리스트에 data를 추가하는 함수.
        Node *newNode = (Node*)malloc(sizeof(Node));                                                                                        // size, hash, name, path, mtime, atime을 추가함
        strcpy(newNode->filehash,filehash);                                             // filehash의 값을 새로운노드의 구조체에 저장
        strcpy(newNode->name,name);
        strcpy(newNode->filepath,filepath);
        strcpy(newNode->file_mtime,file_mtime);
        strcpy(newNode->file_atime,file_atime);
        newNode->filesize = filesize;

        Node *p = (Node*)malloc(sizeof(Node));  
            for(p=linkedlist->head; p!=linkedlist->tail; p=p->next){                    // 링크드리스트에 data를 추가할 때 size단위로 정렬하고                   
                if(p->next->filesize > newNode->filesize){                              // 경로로 정렬함
                        newNode->next = p->next;
                        p->next = newNode;
                        linkedlist->size++;
                        break;
                }

                else if(p->next->filesize == newNode->filesize ){                       // 만약 data들의 filesize가 같다면 path의 ascii순으로 정렬함
                    if(p->next->filepath > newNode->filepath){                          // path 별로 구분
                        newNode->next = p->next;
                        p->next = newNode;
                        linkedlist->size++;
                        break;
                    }
                    else if(newNode->filesize==0){ 
                        newNode->next = p->next;
                        p->next = newNode;
                        linkedlist->size++;
                        break;
                    }     
                }
                else if(p->next==linkedlist->tail){                                     // 만약 next가 링크드리스트 마지막부분이면 마지막에 data를 추가함
                    newNode->next = p->next;
                        p->next = newNode;
                        linkedlist->size++;
                        break;
                }
            }
    }


    void printList(List *linkedlist) {                                        // 링크드리스트를 출력하는 함수
        Node *node = linkedlist->head->next;
        print_cnt = 0;                                                        // print하는 인덱스 값
        int j = 0;                                                            // flag 값으로 출력할 것이 있으면 0 없으면 1로 없으면 no 출력
        char tmp[BUFMAX]="";
            while (node != linkedlist->tail) {
                if(strcmp(tmp, node->filehash)!=0){
                    printf("\n----Identical files #%d (%s bytes - %s) ----\n",++print_cnt,number_comma(node->filesize),node->filehash);
                    strcpy(tmp,node->filehash);
                    j=1;
                }
                
                printf("[%d] %s/%s (mtime : %s) (atime : %s) \n", j++,node->filepath,node->name,node->file_mtime,node->file_atime);
                node = node->next;
            }
        if(j==0){
            if(argvstart[0]=='~'){
                printf("No duplicate in %s",argvstartpoint);
            }
            else
                printf("No duplicate in %s",argv4point);
        }
    }



    char* extract_name(char* string,char oper)                          // string 으로부터 파일의 name을 추출하는 함수
    {
        char* filename = strrchr(string, oper);
        return filename ? filename + 1 : string;
    }

    char* get_time(time_t stime)                                        // stime을 우리가 아는 시간으로 변환하는 함수
    {
        char* time = (char*)malloc(sizeof(char) * BUFMAX);
        struct tm *tm;
        tm = localtime(&stime);
        sprintf(time, "%04d-%02d-%02d %02d:%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
        return time;
    }


    
  int md5(char *filename, char *str)                                  //md5 함수
    {
        MD5_CTX ctx;
        struct stat statbuf;
        char *hash;
        unsigned char buf[16];

        int fd = open(filename, O_RDONLY);
        if (fd < 0)
        {
            return fd;
        }
        fstat(fd, &statbuf);                                            // filename의 file을 오픈하고 fstat정보 얻기
        hash = (char *)malloc(statbuf.st_size);                         // hash 값 저장할 곳
        read(fd, hash, statbuf.st_size);                                

        MD5_Init(&ctx);                                                 // md5 사용법
        MD5_Update(&ctx, hash, statbuf.st_size);                        // md5 사용법
        MD5_Final(buf, &ctx);                                           // md5 사용법
        for (int i = 0; i < sizeof(buf); ++i)
        {
            sprintf(str+(i*2),"%02x", buf[i]);
        }
        free(hash);
        close(fd);
        return 1;
    }

    void ssu_runtime(struct timeval *start_time, struct timeval *end_time){         // 실행시간을 계산하는 함수
        end_time-> tv_sec -= start_time -> tv_sec;                                  //끝난 시간 - 시작 시간

        if(end_time -> tv_usec < start_time -> tv_usec){                            // 만약 start의 usec가 더 크다면 end의 1sec 을 usec로 변환해서 계산
            end_time -> tv_sec--;
            end_time -> tv_usec += 1000000;
        } 

        end_time -> tv_usec -= start_time -> tv_usec;                               //끝난 마이크로초 - 시작 마이크로초
        printf("\nSearching time: %ld:%06ld(sec:usec)\n\n", end_time->tv_sec, end_time->tv_usec); // 출력

    }

    char * number_comma(int size){                                                  // byte숫자를 1000단위로 ,를 찍어줘서 나누는 함수
        static char comma_tmp[BUFMAX];
        memset(comma_tmp, 0, BUFMAX);
        char tmp[BUFMAX];
        int  c=0;
        sprintf(tmp,"%d",size);
        for(int index = 0; index <strlen(tmp); index++){
            if(index !=0 && index % 3 == (strlen(tmp)%3)){                  // 3개 마다 ,를 찍는 곳
                comma_tmp[c++] =',';
            }
            comma_tmp[c++] = tmp[index];
        }       
        return comma_tmp;
    }



    long double doublesize_calc(char *str, long double num){            // size를 KB,MB,GB에 나눠서 BYTE 로 계산해주는 함수
        if(strcmp(str,"kb")==0 || strcmp(str,"KB")==0)
                num*=1024;
        else if(strcmp(str,"mb")==0 || strcmp(str,"MB")==0)
                num*=(1024*1024);
        else if(strcmp(str,"gb")==0 || strcmp(str,"GB")==0)
                num*=(1024*1024*1024);

        return num;
    }

    int my_atoi(const char* str){                                       // string을 정수로 바꿔주는 함수 
        int a=0,i;
        for(i=0; str[i]!=0; ++i){
            a =10*a + str[i]-'0';
        }
        return a;
    }

    int split_file(char* string, char* seperator, char* argv[])         // string을 seperator 단위로 쪼개는 함수
    {
        int argc = 0;
        char* ptr = NULL;
        ptr = strtok(string, seperator);
        while (ptr != NULL) {
            argv[argc++] = ptr;
            ptr = strtok(NULL, "|");
        }
        return argc;
    } 

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