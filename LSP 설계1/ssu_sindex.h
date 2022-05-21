#ifndef HEADER_H
#define HEADER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>//마이크로초 계산 헤더
#include<unistd.h>  //마이크로초 계산 헤더
#include<dirent.h> //scandir 헤더
#include <sys/stat.h>

#include <time.h>


void prompt();
void ssu_help();
void first_input_func(char *first_arr[]);
void ssu_sindex();


    
int scandir(const char *dirp, struct dirent *** namelist,
            int(*filter)(const struct dirent *),
            int(*compar)(const struct dirent**, const struct dirent **));
void ssu_find(char first_arr[],char first_arr2[]);

#endif