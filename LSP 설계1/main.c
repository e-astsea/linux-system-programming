#include "./ssu_sindex.h"

#define SECOND_TO_MICRO 1000000;

//시간 측정 함수 원형
void ssu_runtime(struct timeval *start_time, struct timeval *end_time);  //마이크로초 단위의 시간 측정 함수



int main(int argc, char *argv[]){

    struct timeval start_time, end_time; 
    gettimeofday(&start_time,NULL);         //시작 시간


    ssu_sindex();


    gettimeofday(&end_time,NULL);           //종료 시간
    ssu_runtime(&start_time,&end_time);     //시간 측정 함수
    
    return 0;
}



//마이크로초 단위의 시간 측정 함수
void ssu_runtime(struct timeval *start_time, struct timeval *end_time){
    end_time-> tv_sec -= start_time -> tv_sec; //끝난 시간 - 시작 시간

    if(end_time -> tv_usec < start_time -> tv_usec){
        end_time -> tv_sec--;
        end_time -> tv_usec += SECOND_TO_MICRO;
    } // 만약 start의 usec가 더 크다면 end의 1sec 을 usec로 변환해서 계산

    end_time -> tv_usec -= start_time -> tv_usec; //끝난 마이크로초 - 시작 마이크로초
    printf("Runtime: %ld:%06ld(sec:usec)\n", end_time->tv_sec, end_time->tv_usec); // 출력

}