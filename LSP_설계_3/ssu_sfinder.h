#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/time.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include<pthread.h>
#include<errno.h>

#define STRMAX 10000 
#define ARGMAX 20
#define NAMEMAX 255
#define PATHMAX 4096
#define HASHMAX 33
#define BUFMAX 1024

typedef struct node {                       //링크드리스트 data 부분
    int filesize;
    char filehash[BUFMAX];
    char name[BUFMAX];
    char filepath[BUFMAX];
    char file_mtime[BUFMAX];
    char file_atime[BUFMAX];
    char file_uid[BUFMAX];
    char file_gid[BUFMAX];
    char file_mode[BUFMAX];
    char file_date[BUFMAX];
    char file_time[BUFMAX];
    struct node *next;
} Node;

typedef struct {    // 링크드리스트 부분
    Node *head;     // 리스트의 시작 노드
    Node *tail;     // 리스트의 마지막 노드
    int size;       // 리스트의 노드 갯수
} List;

void ssu_list_sort(List *list, char * category,int flag);
void createList(List *linkedlist);

void trash(List *trashlink,char *category, char *order);
void ssu_list(List *list, char *List_type, char *Category, char * Order);
void restore(List *list, char *idx);
