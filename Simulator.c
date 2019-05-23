#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 30
#define MAX_IO_NUM 60

//프로세스 구조체
typedef struct process process;
typedef struct process{
  int pid;
  int CPUburst;
  //int IOburst;
  int arrival;
  int priority;
  int CPUburst_remain;
}process;

typedef struct IO IO;
typedef struct IO{
  int pid;
  int IOburst;
  int when; //해당 프로세스의 CPUburst_remain 이 얼마일때 IO interrupt 될 것인지.
}

//job queue를 최대 프로세스 개수만큼 선언
process jobQ[MAX_PROCESS_NUM];
int jQ_front, jQ_rear;

//IO 담아놓은 배열을 MAX_IO_NUM만큼 선언
IO ioQ[MAX_IO_NUM];
int ioQ_front, ioQ_rear;

//job queue 초기화
void init_jobQ(){
  jQ_front = -1;
  jQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    jobQ[i] = NULL;
  }
}
//job queue enqueue
void add_jobQ(process newP){
  if(jQ_rear == MAX_PROCESS_NUM - 1)
    printf("jobQ is FULL");
  else
    jobQ[++jQ_rear] = newP;
}
//job queue dequeue
process poll_jobQ(){
  if(jQ_front == jQ_rear)
    printf("jobQ is EMPTY");
  else
    return jobQ[++jQ_front];
}

//IO queue 초기화
void init_ioQ(){
  ioQ_front = -1;
  ioQ_rear = -1;

  for(int i = 0; i < MAX_IO_NUM; i++){
    ioQ[i] = NULL;
  }
}

//IO queue enqueue
void add_ioQ(IO newIO){
  if(ioQ_rear == MAX_PROCESS_NUM - 1)
    printf("ioQ is FULL");
  else
    ioQ[++ioQ_rear] = newIO;
}
//IO queue dequeue
ioQ poll_ioQ(){
  if(ioQ_front == ioQ_rear)
    printf("ioQ is EMPTY");
  else
    return ioQ[++ioQ_front];
}


/*
create_processes
input:
      num_process: 실행시킬 프로세스의 총 개수
      num_IO: 발생할 IO의 총 개수
입력 받은 값에 따라 랜덤으로 프로세스와 IO의 속성값을 결정한 뒤,
프로세스 id 오름차순으로 job queue에 넣어준다.
*/
void create_processes(int num_process, int num_IO){
  //난수 생성
  srand( (unsigned)time(NULL) );

  //job queue 초기화
  init_jobQ();
  //io queue 초기화
  init_ioQ();

  for(int i = 0; i < num_process; i++){
    process newP = (process)malloc(sizeof(struct process));
    newP -> pid = i+1;
    newP -> CPUburst = rand() % 25 + 1; //CPU burst time 1 ~ 25
    newP -> arrival = rand() % (num_process + 10);
    newP -> priority = rand() % num_process + 1;
    newP -> CPUburst_remain = newP -> CPUburst;

    //job queue에 넣어준다. 순서는 pid 오름차순.
    add_jobQ(newP);
  }

  for(int j = 0; j < num_IO; j++){
    IO newIO = (IO)malloc(sizeof(struct IO));
    do{
      int whichP = rand() % num_process + 1;
      int IOburst = rand() % 10 + 1; //IO burst time 1~10
      // 1 <= when < CPUburst 이어야한다.
      int when = rand() % (jobQ[whichP]->CPUburst - 1) + 1;
      for(int k = 0; k < j; k++){
        //해당 프로세스가 같은 시간에 다른 IO burst 있는지 확인하고 있으면, 다시 선택.
        if(ioQ[k]->pid == whichP && ioQ[k]->when == when) continue;
        add_ioQ(newIO);
      }
    }while(ioQ[j] == NULL);
  }

}


int main(int argc, char **argv){
  int num_process, num_IO;

  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Type IO number: ");
  scanf("%d", &num_IO);

  return 0;
}
