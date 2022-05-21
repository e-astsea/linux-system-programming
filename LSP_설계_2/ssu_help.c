#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>

int main(){
    printf("Usage:\n");
    printf("  > fmd5/fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n");
    printf("     >> [SET_INDEX][OPTION ... ]\n");
    printf("        [OPTION ... ]\n");
    printf("        d [LIST_IDX] : delete [LIST_IDX] file\n");
    printf("        i : ask for confirmation before delete\n");
    printf("        f : force delete except the recently modified file\n");
    printf("        t : force move to Trash except the recently modified file\n") ;
    printf("  > help\n");
    printf("  > exit\n\n");

}