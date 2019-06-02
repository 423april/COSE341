#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 30
#define MAX_IO_NUM 60
#define INF 1000000

//프로세스 구조체
typedef struct process{
  int pid;
  int CPUburst;
  int IOburst;
  int arrival;
  int priority;
  int CPUburst_remain;
  int IOburst_remain;
  int waitingTime;
  int turnaroundTime;
  int responseTime;
}process;
typedef struct process* proPointer;

typedef struct IO* IOPointer;
typedef struct IO{
  int pid;
  int IOburst;
//해당 프로세스의 CPUburst_remain 이 얼마일때 IO interrupt 될 것인지.
  int when;
}IO;

typedef struct queue{
  proPointer q[MAX_IO_NUM];
  int front;
  int rear;
}queue;

queue init_Q(queue Q){
  Q.front = -1;
  Q.rear = -1;
  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    Q.q[i] = NULL;
  }
  return Q;
}

queue add_Q(queue Q, proPointer newP){
  if(Q.rear == MAX_PROCESS_NUM - 1){
    printf("Q is full");
    return Q;
  }
  else{
    Q.q[++Q.rear] = newP;
    return Q;
  }
}

queue poll_Q(queue Q){
  if(Q.front == Q.rear){
    printf("Q is empty");
    return Q;
  }
  else{
    //proPointer pop;
    /*pop = Q.q[*/++Q.front/*]*/;
    return Q;
  }
}

queue job_global;
//IOPointer io_global[MAX_IO_NUM];

void create_processes(int num_process, int num_IO){
  //난수 생성
  srand( (unsigned)time(NULL) );

  //job queue 전역으로 초기화
  job_global = init_Q(job_global);

  for(int i = 0; i < num_process; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = i+1;
    newP->CPUburst = rand() % 25 + 2; //CPU burst time 2 ~ 26
    newP->IOburst = 0;
    newP->arrival = rand() % (num_process + 10);
    newP->priority = rand() % num_process + 1;
    newP->CPUburst_remain = newP -> CPUburst;
    newP->IOburst_remain = 0;
    newP->waitingTime = 0;
    newP->turnaroundTime = 0;
    newP->responseTime = 0;

    //job queue에 넣어준다. 순서는 pid 오름차순.
    job_global = add_Q(job_global, newP);
  }
}

void printQ(queue Q){
  for(int i = Q.front+1; i <= Q.rear; i++){
    printf("pid: %d ", Q.q[i]->pid);
    printf("CPUburst: %d, ", Q.q[i]->CPUburst);
    printf("arrival: %d, ", Q.q[i]->arrival);
    printf("priority: %d\n", Q.q[i]->priority);
  }
}

int main(int argc, char **argv){
  int num_process, num_IO;
  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Type IO number: ");
  scanf("%d", &num_IO);

  create_processes(num_process, num_IO);
  printQ(job_global);
}
