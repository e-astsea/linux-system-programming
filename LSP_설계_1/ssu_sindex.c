#include "./ssu_sindex.h"


char *start_point;
char * start_point ;
char *first_arr[1000]={NULL,};
char first_input[1000];
char *ptr;
int dir_size=0;
/*
기능 : 프롬프트로써 계속 나오는 부분
*/
void prompt(){
        printf("20182586> ");
}

/*
기능 : help 입력시 출력되는 부분
*/
void ssu_help(){
    printf("Usage:");
    printf("  > find [FILENAME] [PATH]\n");
    printf("     >> [INDEX] [OPTION ... ]\n");
    printf("  > help\n");
    printf("  > exit\n\n");
    printf("  [OPTION ... ]\n");
    printf("   q : report only when files differ\n");
    printf("   s : report when two files are the same\n");
    printf("   i : ignore case differences in file contents\n");
    printf("   r : recursively compare any subdirectories found\n");
}




/*
기능 : 함수를 통해 필요한 파일들의 세부 내용을 출력하고 구조체에 file정보 저장하는 함수
*/
int file_index=0;
char space[256];
struct find_result{           //file의 내용들이 들어가 있는 구조체
    int index;
    char *file;
    char *filename;
    char *cwd;
    int filesize;
};
struct find_result fr[1023]={0}; 
void print(char *file,char *filename, int filesize,int  mode, int blocksize, int links,int uid,int gid, int access,int change,int modify)
{
    struct tm *t;
    space[file_index]=filesize;

    if(file_index==0){                          //구조체 초기화
        for(int i=0; i<1023; i++){
            fr[i].index=0;
            fr[i].file=NULL;
            fr[i].filename=NULL;
            fr[i].cwd=NULL;
        }
    }
    if((!strcmp(file,filename)) && space[0] == space[file_index]){        //어떤파일 넣고 출력할건지 결정 조건문
        char *cwd_file = getcwd(NULL,2560);
        fr[file_index].index = file_index;
        fr[file_index].file = file;
        fr[file_index].filename = filename;
        fr[file_index].cwd = cwd_file;
        fr[file_index].filesize = filesize;
        strcat(fr[file_index].cwd,"/");
        strcat(fr[file_index].cwd,fr[file_index].filename);               //경로를 절대경로로 합쳐주는 과정

    for(int i=1; i<=file_index; i++){
      if(!strcmp(fr[0].cwd,fr[i].cwd)!=0){
        return 0;
      }
    }
 struct stat fstat;
 if( !(((lstat(file, &fstat)) != 0) || !(S_ISDIR(fstat.st_mode))) ){
     dir_plus(fr[file_index].cwd,0);
     fr[file_index].filesize=dir_size;
     dir_size=0;
      if(fr[0].filesize!=fr[file_index].filesize){
      return 0;
    }
 }
    if(file_index==0 )
      printf("Index Size Mode        Blocks Links UID  GID  Access          Change           Modify         Path\n");
   
        printf("%-6d",file_index);
        printf("%-5d",fr[file_index].filesize);
            printf((S_ISDIR(mode)) ? "d" : "-");
			printf((mode & S_IRUSR) ? "r" : "-");
			printf((mode & S_IWUSR) ? "w" : "-");
			printf((mode & S_IXUSR) ? "x" : "-");
			printf((mode & S_IRGRP) ? "r" : "-");
			printf((mode & S_IWGRP) ? "w" : "-");
			printf((mode & S_IXGRP) ? "x" : "-");
			printf((mode & S_IROTH) ? "r" : "-");
			printf((mode & S_IWOTH) ? "w" : "-");
			printf((mode & S_IXOTH) ? "x" : "-");
        printf("  %-7d",blocksize);
        printf("%-6d",links);
        printf("%-5d",uid);
        printf("%-5d",gid);
        t = localtime(&access);
	    printf("%d-%02d-%02d %02d:%02d  ", t->tm_year-100, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
        t = localtime(&change);
	    printf("%d-%02d-%02d %02d:%02d  ", t->tm_year-100, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
        t = localtime(&modify);
	    printf("%d-%02d-%02d %02d:%02d  ", t->tm_year-100, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
        printf("%s\n", fr[file_index].cwd);
        file_index++;
    }
}


int depth_zero_dir=0;

int dir_plus(char *wd, int depth){   
    
    struct dirent **namelist;                           //디렉토리에 있는 파일 및 목록이 저장될 공간
    int scan;
    
    if(chdir(wd) <0 ){                                  //chdir이 현재 작업경로를 변경하는데 경로가 올바르지 않을때 에러 제어   
        return 0;
    }
    scan = scandir(".",&namelist,NULL,alphasort);       //wd에 있는 dir,file탐색
    for(int i=0; i<scan; i++){                          //scan으로 받은 .경로에 있는 파일과 폴더 개수만큼
        struct stat fstat;                              //파일의 정보를 얻을 수 있는 stat구조체
        
        if((!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name,"..")  ))                 //디렉토리에있는 파일,목록의 이름이 ., ..이면 continue
        continue;
        
                   //파일 정보를 얻어오는 시스템 콜 첫 인자로 파일이름
        lstat(namelist[i]->d_name, &fstat);             //파일 정보를 얻어오는 시스템 콜 첫 인자로 파일이름

        if(((fstat.st_mode & S_IFDIR) != S_IFDIR && (fstat.st_mode & S_IFMT) != S_IFDIR  )) {
          dir_size+=fstat.st_size;
        }
        
        if(((fstat.st_mode & S_IFDIR) == S_IFDIR && (fstat.st_mode & S_IFMT) == S_IFDIR  )) {
                if(depth_zero_dir<(depth-1) || (depth==0)){         //depth만큼 하위디렉토리 검색 0이면 무조건검색
                depth_zero_dir++;
                dir_plus(namelist[i]->d_name,depth);
            }
        }
}
  depth_zero_dir--;                                           //디렉토리의 depth 1 감소시킴 더깊게들어감
    
    chdir("..");                                        //하위디렉토리 이동

    return 0;
}
/*
기능 : 처음 내장명령어 입력하면 처리하는 부분 
        받은 입력을 token으로 나누어서 주소검색,상대경로검색도 가능하게함
*/
int cnt=0;
char *resultpathssu;
char c;
char *first_arr_cmp;
int index_input_cnt=0;
void first_input_func(char *first_arr[]){
        
        if(!strcmp(first_arr[0],"find")){               // 입력 명령어가 find일때 
                if(first_arr[1]==NULL || first_arr[2]==NULL){
                  return 0;
                }    
                first_arr_cmp = first_arr[2];
                first_arr_cmp[strlen(first_arr_cmp)-1]='\0';
                file_index=0;
                char *wd_divide[1000]={NULL,};
                int zero=0;
                int wd_arr_index=0;
                for(int i=0; i<strlen(first_arr[1]); i++){                //만약 filename을 경로로 받았을 경우 토큰분리해주는곳
                  c = first_arr[1][i];
                  if(c =='/'){
                    char *ptrf = strtok(first_arr[1], "/"); 
                      while (ptrf != NULL){    
                      wd_divide[wd_arr_index++] = ptrf;   
                      ptrf = strtok(NULL, "/");          
                      }
                  }  
                }
                  chdir(start_point);
                 int j=0;
                char *resultpath = getcwd(NULL,2560);
                char *cwd_file = getcwd(NULL,2560);

                int flag = 0;
                if(wd_arr_index>0){  
                for(int i=0; i<wd_arr_index-1;i++){
                    chdir(wd_divide[i]);
                     
                    cwd_file = getcwd(NULL,2560);
                    strcat(resultpath,"/");
                    strcat(resultpath,wd_divide[i]);
                    j++;
                   
                }
                zerofind_dfs(resultpath,wd_divide[wd_arr_index-1],print,0);           //filename에 주소값 받았을 경우 0번인덱스 찾는 곳
                zero=1;
                flag = 1;
                }
                else{
                  zerofind_dfs(cwd_file,first_arr[1],print,0);                        //filename에 파일명 받았을 경우 0번 인덱스 찾는 곳
                  zero=0;
                  flag = 1;
                }

                
                if(zero==1){
                 first_arr[1]= wd_divide[wd_arr_index-1];
                }
                index_input_cnt=1;
                chdir(start_point);

                if(flag!=0){                                                          //0번 인덱스랑 같은 1번인덱스 ~ 찾는 것
                ssu_scandir(first_arr_cmp,print,first_arr[1],0);
                if(file_index==1){
                  printf("(None)\n");
                  index_input_cnt=0;
                }
                }
                else if(flag==0){
                  perror("");
                }
        }
        else if(!strcmp(first_arr[0],"exit\n")){        // 입력 명령어가 exit 일때
            printf("Prompt End\n");
        }
        else if(!strcmp(first_arr[0],"\n")){            //입력 명령어가 엔터키일때 (개행)
        }
        else{
            ssu_help();                                 //이외 명령어 실행 시 자동으로 help를 실행시킨 것과 동일한 결과 출력
        }
}

/*
기능 : 0번인덱스파일만 따로 찾아주는 부분
*/
int depth_zero =0;
void zerofind_dfs(char *wd, char*filename,void (*print)(char *,char*,int *,int *,int *,int *,int *,int *,int *,int*,int*),int depth){
  if(!strcmp(wd,"proc")){
        return 0;
    }                  
    struct dirent **namelist;                           //디렉토리에 있는 파일 및 목록이 저장될 공간
    if(chdir(wd) <0 ){                                  //chdir이 현재 작업경로를 변경하는데 경로가 올바르지 않을때 에러 제어   

        return 0;
    }
    int scan;
    int dirsize=0;
    scan = scandir(wd,&namelist,NULL,alphasort);       //wd에 있는 dir,file탐색
    for(int i=0; i<scan; i++){                          //scan으로 받은 .경로에 있는 파일과 폴더 개수만큼
        struct stat fstat;                              //파일의 정보를 얻을 수 있는 stat구조체
        
        if((!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name,"..")  ))                 //디렉토리에있는 파일,목록의 이름이 ., ..이면 continue
        continue;
        
        lstat(namelist[i]->d_name, &fstat);             //파일 정보를 얻어오는 시스템 콜 첫 인자로 파일이름
        if (S_ISLNK (fstat.st_mode)  ){
        continue;
        }
        print(namelist[i]->d_name, filename, (int *)fstat.st_size, (int *)fstat.st_mode,(int *)fstat.st_blocks,(int *)fstat.st_nlink,(int *)fstat.st_uid,(int *)fstat.st_gid,(int *)fstat.st_atime,(int *)fstat.st_ctime,(int *)fstat.st_mtime);                //print data출력 함수 lstat에서 찾은 data의 세부정보를 출력하기 위함
        if(((fstat.st_mode & S_IFDIR) == S_IFDIR && (fstat.st_mode & S_IFMT) == S_IFDIR  )) {
            if(depth_zero<(depth-1) || (depth==0)){         //depth만큼 하위디렉토리 검색 0이면 무조건검색
                depth_zero++;
                zerofind_dfs(namelist[i]->d_name,filename,print, depth);
            }
        }
    }
    
    depth_zero--;                                           //디렉토리의 depth 1 감소시킴 더깊게들어감
    
    chdir("..");                                        //하위디렉토리 이동

    return 0;
} 




/*
기능 : 인자로 받은 디렉토리,하부 디렉토리에있는 모든 파일 검색 , 0번인덱스랑 같은 1번인덱스이후부터 찾아줌
*/
static int depth_ssu_scandir = 0; //깊이 체크
//wd 는 파일경로 ,print-데이터출력해주는함수,파일 이름 , 깊이가 0이면 맨하위까지 탐방
int ssu_scandir(char *wd, void (*print)(char *,char*,int *,int *,int *,int *,int *,int *,int *,int*,int*),char *filename, int depth){
    if(!strcmp(wd,"proc")){
        return 0;
    }                 
    struct dirent **namelist;                           //디렉토리에 있는 파일 및 목록이 저장될 공간
    if(chdir(wd) <0 ){                                  //chdir이 현재 작업경로를 변경하는데 경로가 올바르지 않을때 에러 제어   

        perror("");
        return 1;
    }
    int scan;
    scan = scandir(".",&namelist,NULL,alphasort);       //인자로 넘겨진 .경로에 있는 파일과 폴더 목록을 3번째 인자에서 filtering 할수 있고 alphasort 기본 정렬함수 이용

    for(int i=0; i<scan; i++){                          //scan으로 받은 .경로에 있는 파일과 폴더 개수만큼
        struct stat fstat;                              //파일의 정보를 얻을 수 있는 stat구조체
        if((!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name,"..") ))                 //디렉토리에있는 파일,목록의 이름이 ., ..이면 continue
        continue;

        lstat(namelist[i]->d_name, &fstat);            //파일 정보를 얻어오는 시스템 콜 첫 인자로 파일이름, 두번째 인자에 저장
        if (S_ISLNK (fstat.st_mode)  ){
        continue;
        }
        print(namelist[i]->d_name, filename, (int *)fstat.st_size, (int *)fstat.st_mode,(int *)fstat.st_blocks,(int *)fstat.st_nlink,(int *)fstat.st_uid,(int *)fstat.st_gid,(int *)fstat.st_atime,(int *)fstat.st_ctime,(int *)fstat.st_mtime);                //print data출력 함수 lstat에서 찾은 data의 세부정보를 출력하기 위함
                
        if(((fstat.st_mode & S_IFDIR) == S_IFDIR && (fstat.st_mode & S_IFMT) == S_IFDIR  )) {
            if(depth_ssu_scandir<(depth-1) || (depth==0)){         //depth만큼 하위디렉토리 검색 0이면 무조건검색
                depth_ssu_scandir++;
                ssu_scandir(namelist[i]->d_name, print,filename, depth);
            }
        }
    }
    depth_ssu_scandir--;                                           //디렉토리의 depth 1 감소시킴 더깊게들어감
    chdir("..");                                        //하위디렉토리 이동
    return 0;
}



/*
기능 : 0번인덱스 파일과 fl2_num 파일과 diff 해주는 함수와 출력
      비교 -> 검증 -> 출력 과정
*/
void diff(int fl2_num){
    FILE *fp1, *fp2;
    char *f1[100], *f2[100];
    fp1 = fopen(fr[0].cwd, "r+");
		fp2 = fopen(fr[fl2_num].cwd, "r+");
    int m=1;
    int n=1;
    char str1[1024],str2[1024];

    while(fgets(str1, sizeof(str1), fp1) != NULL){       //f1에 파일 라인 넣어주는 과정
          f1[0] = (char *)malloc( sizeof(str1));

	       	f1[m] = (char *)malloc( sizeof(str1));
        	strcpy(f1[m], str1);
        	m++;
    	}


    	while(fgets(str2, sizeof(str2), fp2) != NULL){     //f2에 파일 라인 넣어주는 과정
      f2[0] = (char *)malloc( sizeof(str2));
        	f2[n] = (char *)malloc(sizeof(str2));
        	strcpy(f2[n], str2);
        	n++;
    	}

    int f1_result[m];                                     //result 배열은 결과값을 저장하는 곳이고 check 함수는 검중을 위한 배열
    int f2_result[n];
    int f1_check[m];
    int f2_check[n];

    for(int i=1; i<m; i++){
      f1_result[i]=0;
      f1_check[i]=0;
    }

    for(int i=1; i<n; i++){
      f2_result[i]=0;
      f2_check[i]=0;
    }

  // 비교 과정

    while(1){                                              // 먼저 f1 이랑 f2같은지 확인후 같은 행값 넣어주고 같지 않으면 0 넣는곳
      for(int i=1; i<m; i++){                             // f1파일 비교
        for(int j=1; j<n; j++){ 
          for(int k=1; k<i; k++){
              if(f1_result[k]==j){
                j++;
              }
            }
          if(strcmp(f1[i],f2[j])==0){
           
            f1_result[i]=j;
            f1_check[i]=j;       
            break;
          }
          else{
            f1_result[i]=0;
            f1_check[i]=0;
                }
                        }
      }


      for(int i=1; i<n; i++){                         //먼저 f1 이랑 f2같은지 확인후 같은 행값 넣어주고 같지 않으면 0 넣는곳
        for(int j=1; j<m; j++){                       // f1파일 비교
          for(int k=1; k<i; k++){
              if(f2_result[k]==j){
                j++;
              }
            }
          if(strcmp(f2[i],f1[j])==0){
            
            f2_result[i]=j;
            f2_check[i]=j; 
            break;
          }
          else{
            f2_result[i]=0;
            f2_check[i]=0;
          }
        }
      }
      break;
    }

    //temp는 result 복사본으로 수정해도 되는 data
    int f1_temp[m];    
    int f2_temp[n];


    for(int i=1; i<m; i++){
      f1_temp[i]=f1_result[i];
    }
    for(int i=1; i<m; i++){
      f2_temp[i]=f2_result[i];
    }

    //검증 단계
     for(int i=1; i<m; i++){                       //같은거체크 하고 f1이면 f2check를 보면서 f1배열에 있는 값이 맞는것인지 확인하는 곳  개행과
          if(strcmp(f1[i],f2[f1_result[i]])==0 ){   // 잘못 들어간 data들을 확인함
            
            f2_check[f1_temp[i]]=0;
          for(int k=f1_temp[i]; k>0; k--){
                  if(f2_check[k]!=0){

                    if(strcmp(f1[f2_check[k]],"\n")==0){
                      continue;
                    }

                    else if(strcmp(f2[f2_check[k]],"\n")!=0){
                              f2_result[f1_result[i]]=0;

                      f1_result[i]=0;
                      break;
                    }
                  }  
          }
        }
        else continue;
    }

    int max=0;
    for(int i=1; i<=m; i++){                                // 같은 것을 검증하는 단계에서 개행의 처리과정
      if(f1_result[i]!=0 && max<f1_result[i]){

        max=f1_result[i];
      }
      if(f1_result[i]<max){
        
        f2_result[f1_result[i]]=0;
        f1_result[i]=0;
      }
    }
  
int cnt=1;
for(int i=1; i<n; i++){
      if(f2_result[i]==0){
        cnt++;
      }
   }



    
   int all_result[m+cnt];                   // 검증후 all _result f1_result 와 f2_result 합쳐주는 곳 
   for(int i=1; i<=m+cnt-1; i++){           // f1_result 에 f2의 겹치지않는 인덱스를 넣어준다. f1은 양수로 f2는 음수로 넣어준다.
      all_result[i]=0;
   }
  int limit = m+cnt-1;
      int z=0;

  while(z<=limit){
   if(z==(limit)){
      break;
    }
    for(int i=1; i<=m; i++){
      if(f1_result[i]!=0){
        for(int j=1; j<=f1_result[i]; j++){         
          if(f2_result[j]==0){
            f2_result[j]=1;
            all_result[z]=j;
            z++;
            if(z==(m+cnt-2)){
      break;
    }
          }          
        }
        all_result[z]=0;
            z++;
            if(z==(m+cnt-2)){
      break;
    }
        
      }
      else{
        all_result[z]=-i;
        z++;
        if(z==(m+cnt-2)){
      break;
        }
      }
    }
    break;
  }

  //합쳐진 결과물인 allresult배열을 a,c,d에 맞춰서 출력해주는 곳
  int a=0;
  int b=0;
  int c=0;
  int d=0;
  int line=0;
  while(1){
    a=b;
    c=d;
    while(1){
    if(all_result[a]==0 && all_result[b]==0 &&all_result[c]==0 &&all_result[d]==0 ){
      a++;    //a는 f1의 시작부분
      b++;    //b는 f1의 끝부분
      c++;    //c는 f2의 시작부분
      d++;    //d는 f2의 끝부분
    }
    else
    break;
    }
  for (int i=line; i<=limit; i++){
    line++;

    if(all_result[i]==0 && all_result[a]<m && all_result[a]<n && all_result[c]<n &&all_result[d]<n ){
      if( all_result[a]>0 && all_result[b]>0 &&all_result[c]>0 && all_result[d-1]>0){             //a옵션일 때 조건
        if(a==b &&c==d-1){
          printf("%da%d\n",a,c);                                                                  //%d a %d
          for(int i=c; i<=c; i++){
            printf("> %s",f2[i]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
        }
        else if(a==b&& c!=d-1){
          printf("%da%d,%d\n",a,all_result[c],all_result[d-1]);                                    //%d a %d,%d
          for(int i=c+1; i<=d; i++){
            printf("> %s",f2[i]);
          }
         if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
        }
        b=d;
          break;   
      }
      else if(all_result[a]<0 && all_result[b-1]<0 &&all_result[c]>0 && all_result[d-1]>0){             //c옵션 조건
                  
        if(a==b-1 && c==d-1){
           printf("%dc%d\n",-all_result[a],all_result[c]);                                        //%d c %d
           for(int i=a; i<=a; i++){
            printf("< %s",f1[-all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
          printf("---\n");
          for(int i=c; i<=c; i++){
            printf("> %s",f2[all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
        }
        else if(a!=b-1 && c==d-1){
          printf("%d,%dc%d\n",-all_result[a],-all_result[b-1],all_result[c]);                     //%d,%d c %d
          for(int i=a; i<=b-1; i++){
            printf("< %s",f1[-all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
          printf("---\n");
          for(int i=c; i<=c; i++){
            printf("> %s",f2[all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
        }
        else if(a==b-1 && c!=d-1){
          printf("%dc%d,%d\n",-all_result[a],all_result[c],all_result[d-1]);                      //%d c %d,%d
          for(int i=a; i<=b-1; i++){
            printf("< %s",f1[-all_result[i]]);
          }
         if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
          printf("---\n");
          for(int i=c; i<=d-1; i++){
            
            printf("> %s",f2[all_result[i]]);

          }
         if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
        }
        else if(a!=b-1 && c!=d-1){
          printf("%d,%dc%d,%d\n",-all_result[a],-all_result[b-1],all_result[c],all_result[d-1]);   //%d,%d c %d,%d
          for(int i=a; i<=b-1; i++){
            printf("< %s",f1[-all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
          printf("---\n");
          for(int i=c; i<=d-1; i++){
            printf("> %s",f2[all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
          
        }
        
        b=d;
        break;
      }
      else if(all_result[a]<0 && all_result[b-1]<0 &&all_result[c-1]<0 && all_result[d-1]<0){      //d옵션 조건
        if(a==b-1 && c-1==d-1){
          printf("%dd%d\n",-all_result[a],all_result[c]);                                         //%d d %d
           for(int i=a; i<=a; i++){
            printf("< %s",f1[-all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
        }
        else if(a!=b-1 && c-1==d-1){
          printf("%d,%dd%d\n",-all_result[a],-all_result[b-1],all_result[c]);                     //%d,%d d %d
           for(int i=a; i<=b-1; i++){
            printf("< %s",f1[-all_result[i]]);
          }
          if(d == limit-1){
              printf("\n\\ No newline at end of file\n");
            }
          
        }
      }
      break;
    }
      if(all_result[i]<0){                                                                      //음수인 것은 f1_result에 있는 것 이기에 b,c,d의 인덱스를 증가시킨다.
      b++;
      c++;
      d++;

    }
    
    else if(all_result[i]>0){                                                                    //양수인 것은 f2_result에 있는 것 이기에 d의 인덱스를 증가시킨다.
      if(d!=limit-1)
      d++;
    }
  }
  if(line==limit+1){
    break;
  }
  }
}

/*
기 능 : 선택한 인덱스의 파일의 option을 넣을 수 있다. q랑 s만 구현된 모습
*/
void option(int fl2_num, char *opt){
  FILE *fp1, *fp2;
    char *f1[100], *f2[100];
    fp1 = fopen(fr[0].cwd, "r+");
		fp2 = fopen(fr[fl2_num].cwd, "r+");
    int m=1;
    int n=1;
    char str1[1024],str2[1024];

    while(fgets(str1, sizeof(str1), fp1) != NULL){       //m은 fp1의 라인개수
          f1[0] = (char *)malloc( sizeof(str1));

	       	f1[m] = (char *)malloc( sizeof(str1));
        	strcpy(f1[m], str1);
        	m++;
    	}

    	while(fgets(str2, sizeof(str2), fp2) != NULL){     //n은 fp2의 라인개수
      f2[0] = (char *)malloc( sizeof(str2));
        	f2[n] = (char *)malloc(sizeof(str2));
        	strcpy(f2[n], str2);
        	n++;
    	}
          int min=0;
    if(m>n){
      min=n;
    }
    else
      min=m;


    if(strcmp(opt,"q\n")==0){                                                   //q를 입력했을 때 옵션
     for(int i=1; i<=min-1; i++){
      if(strcmp(f1[i],f2[i])!=0){
          printf("Files %s and %s differ\n",fr[0].filename,fr[fl2_num].cwd);
          break;
        }
     } 
    }

   else if(strcmp(opt,"s\n")==0){                                               //s를 입력했을 때 옵션
      for(int i=1; i<=min-1; i++){
            if(strcmp(f1[i],f2[i])==0 && i==(min-1)){
                printf("Files %s and %s are identical\n",fr[0].filename,fr[fl2_num].cwd);
              }
            else if(strcmp(f1[i],f2[i])!=0)
                break;
     } 
    }

}

 int numm=0;

void dir_diff(char * wd){                       //디렉토리를 diff 하는 함수
  struct stat fstat;  
struct dir1_result{           
    char *filename;
    int size;

    char *cwd;
    char *cwd_cnt;
};
struct dir1_result dr1[1023]={0}; 

struct dir2_result{           
    char *filename;
    int size;
    char *cwd;
    char *cwd_cnt;
};
struct dir2_result dr2[1023]={0}; 

  //0번인덱스와 선택한 인덱스 디렉토리 파일 받아오는 곳.
char *r1path ;
  int dir1_count = 0;
    struct dirent** namelist = NULL;
    dir1_count = scandir(fr[0].cwd, &namelist, NULL, alphasort);
    for(int i=0; i<dir1_count; i++){
      dr1[i].filename = NULL;
      dr1[i].size =0;
      dr1[i].cwd = NULL;
      dr1[i].cwd_cnt = NULL;
    }
    for (int i = 0; i < dir1_count; i++) {
      if((!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name,"..") ))
        continue;
      lstat(namelist[i]->d_name, &fstat);
      dr1[i].filename = namelist[i]->d_name;
      chdir(fr[0].cwd);
      r1path = getcwd(NULL,2560);
        dr1[i].cwd = r1path;
        dr1[i].cwd_cnt = r1path;
                      strcat(dr1[i].cwd,"/");
                      strcat(dr1[i].cwd,dr1[i].filename);
  
    }
  

char *r2path ;
    int dir2_count = 0;
    struct dirent** name_list = NULL;
    dir2_count = scandir(wd, &name_list, NULL, alphasort);
      for(int i=0; i<dir2_count; i++){
      dr2[i].filename = NULL;
      dr2[i].size =0;
      dr2[i].cwd = NULL;
      dr2[i].cwd_cnt = NULL;
    }
    for (int i = 0; i < dir2_count; i++) {
      if((!strcmp(name_list[i]->d_name, ".")) || (!strcmp(name_list[i]->d_name,"..") ))
        continue;
         chdir(fr[numm].cwd);
      dr2[i].filename = name_list[i]->d_name;
      r2path = getcwd(NULL,2560);
        dr2[i].cwd = r2path;
        dr2[i].cwd_cnt = r2path;
                      strcat(dr2[i].cwd,"/");
                      strcat(dr2[i].cwd,dr2[i].filename);

    }

    int max=0;
      if(dir2_count<=dir1_count){
      max=dir1_count;
    }
    else
      max=dir2_count;
    int j=1;
    int i=1;
    struct stat dir_info;
     while(1){                                      //디렉토리 하위 파일과 디렉토리에 관련하여 출력하는곳
       i++;j++;
                      if(i==dir1_count){
                      
                        for(int x=j; x<dir2_count; x++){
                          printf("Only in %s : %s\n",dr2[x].cwd_cnt, dr2[x].filename);
                        }
                                 break;
                        }
                        else if(j==dir2_count){
                          for(int k=i; k<dir1_count; k++){
                          printf("Only in %s : %s\n",dr1[k].cwd_cnt,dr1[k].filename);
                        }
                      
                      break;
                        }

                      
                      if(strcmp(dr1[i].filename,dr2[j].filename)==0 ){
                        fr[0].cwd = dr1[i].cwd;
                        fr[numm].cwd = dr2[j].cwd;

                      
                       if( (!(((lstat(fr[0].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode)))) &&(!(((lstat(fr[numm].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode)))) ){
                       // 둘다 디렉토리일경우
                       printf("Common subdirectories: %s and %s\n", fr[0].cwd, fr[numm].cwd);
                          
                          continue;
                        }
                       if( (((lstat(fr[0].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) && !(((lstat(fr[numm].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) ){
                         //fr0은 일반파일 fr1은디렉토리파일일때
                          printf("File %s is a regular file while file %s is directory\n",fr[0].cwd,fr[numm].cwd);
                          
                          continue;
                       }
                       if( !(((lstat(fr[0].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) && (((lstat(fr[numm].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) ){
                         //fr0은 디렉토리파일 fr1은 일반파일일때
                          printf("File %s is a directory while file %s is regular file\n",fr[0].cwd, fr[numm].cwd);
                          
                          continue;
                       }
                      
                        if( (((lstat(fr[0].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) && (((lstat(fr[numm].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) ){     //둘다 일반파일이면 
                        //둘다 정규파일일 경우 && 내용이 같을 경우
                         FILE *fp1, *fp2;
                            char *f1[100], *f2[100];
                            fp1 = fopen(fr[0].cwd, "r+");
                            fp2 = fopen(fr[numm].cwd, "r+");
                            int m=0;
                            int n=0;
                            char str1[1024],str2[1024];

                          while(fgets(str1, sizeof(str1), fp1) != NULL){      
                              f1[m] = (char *)malloc( sizeof(str1));
                              strcpy(f1[m], str1);
                              m++;
                          }

                          while(fgets(str2, sizeof(str2), fp2) != NULL){     
                              f2[n] = (char *)malloc( sizeof(str2));
                              strcpy(f2[n], str2);
                              n++;
                          }
                          for(int i=0; i<m; i++){
                             if(strcmp(f1[i],f2[i])==0 && i==m-1){ //둘다 정규파일일 경우 && 내용이 같을 경우
                                break;
                            }
                             else if(strcmp(f1[i],f2[i])!=0){  //둘다 정규파일인데 내용이 다를경우 출력
                              printf("diff %s %s\n", fr[0].cwd, fr[numm].cwd);
                              diff(numm);
                              break;
                            }

                            
                          }



                        }
                      }
                      else{
                        
                        int cntj =j;
                        for(int k=cntj; k<dir2_count; k++){            //dir1에는 있으나 2에는 없는 것
                          if( k == (dir2_count-1) && strcmp(dr1[i].filename,dr2[k].filename)!=0){
                            printf("Only in %s : %s\n", dr1[i].cwd_cnt,dr1[i].filename);
                            j--;
                          }
                        }

                        int cnti=i;
                        if(i<dir2_count){
                        for(int k=cnti; k<dir1_count; k++){            //dir2에는 있으나 1에는 없는 것
                          if(!strcmp(dr2[i].filename,dr1[k].filename)&&k==dir1_count-1){
                            printf("Only in %s : %s\n", dr2[i].cwd_cnt,dr2[i].filename);
                            i--;
                          }
                        }
                        }
     }

               
                  
      }

    }



void ssu_sindex(){
    while(1){
        first_arr[0]=NULL;
        first_arr[1]=NULL;
        first_arr[2]=NULL;
        chdir(start_point);                             // 시작위치를 고정시켜준다.
        start_point =getcwd(NULL,2560);
        prompt();                                       // 프롬포트 출력  ex)20182586>
        fgets(first_input,sizeof(first_input),stdin);   // 내장명령어 입력
        

        //fgets로 받은 문자열 공백 단위로 자르기
        ptr = strtok(first_input, " "); 
        int first_arr_index=0;
        while (ptr != NULL){                            // 자른 문자열이 나오지 않을 때까지 반복
        first_arr[first_arr_index++] = ptr;             // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
        ptr = strtok(NULL, " ");                        // 다음 문자열을 잘라서 포인터를 반환
        }


        //처음 입력된 값으로 내장명령어 실행하는 부분
        first_input_func(first_arr);
        if(!strcmp(first_arr[0],"exit\n"))              //exit 일 경우 break
            break;
        else if(!strcmp(first_arr[0],"\n"))             //공백일경우 프롬프트 재출력
            continue;


        if(!strcmp(first_arr[0],"find")&& fr[0].filename == first_arr[1] && fr[1].filename == first_arr[1] && first_arr[2]!=NULL){         
               
                char ch;
                struct stat dir_info;
                char *index_input_arr[1000]={NULL,};
                char index_input[1000];
                if(index_input_cnt==0){
                  perror(" ");
                }
                else{
                printf(">> ");


                if( !(((lstat(fr[0].cwd, &dir_info)) != 0) || !(S_ISDIR(dir_info.st_mode))) ){      //0번인덱스 디렉토리일때
                  scanf("%d%c",&numm,&ch);
                  if(fr[numm].file == NULL){
                    perror(" ");
                    continue;
                  }
                  
                  dir_diff(fr[numm].cwd);                        //디렉토리 diff 하는 곳
                }
                else{
                fgets(index_input,sizeof(index_input),stdin);
                ptr = strtok(index_input, " "); 
                int index_input_index=0;

                while (ptr != NULL){                            // 자른 문자열이 나오지 않을 때까지 반복
                index_input_arr[index_input_index++] = ptr;             // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
                ptr = strtok(NULL, " ");                        // 다음 문자열을 잘라서 포인터를 반환
                }

                int num = atoi(index_input_arr[0]);
                index_input[strlen(index_input) - 1] = '\0';
                                
                  
                if(index_input_arr[1]!=NULL){
                  option(num,index_input_arr[1]);             //옵션 출력

                }
                else
                 diff(num);                                     //diff 함수 출력
                  
                }
                }
        }


    }
}



