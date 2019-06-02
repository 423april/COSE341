#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

proPointer poll_Q(queue Q){
  if(Q.front == Q.rear)
    printf("Q is empty");
  else
    return Q.q[++Q.front];
}

// void create_processes(int num_process, int num_IO){
//   //난수 생성
//   srand( (unsigned)time(NULL) );
//
//
// }

int main(int argc, char **argv){
  int num_process, num_IO;
  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Type IO number: ");
  scanf("%d", &num_IO);

  queue jobQ;
  jobQ = init_Q(jobQ);

  printf("%d ", jobQ.front);
  printf("%d \n", jobQ.rear);

  for(int i = 0; i < 5; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = i+1;
    jobQ = add_Q(jobQ, newP);
    printf("front: %d, rear: %d\n", jobQ.front, jobQ.rear);
    for(int j = jobQ.front+1; j <= jobQ.rear; j++){
      printf("p%d ", jobQ.q[j]->pid);
    }
    printf("\n");
  }

  for(int i = 0; i < 5; i++){
    proPointer oldP;
    printf("p%d ", jobQ.q[jobQ.front + 1]->pid);
    oldP = poll_Q(jobQ);
    printf("oldP: %d\n", oldP->pid);

    printf("front: %d, rear: %d\n", jobQ.front, jobQ.rear);
    for(int j = jobQ.front+1; j <= jobQ.rear; j++){
      printf("p%d ", jobQ.q[j]->pid);
    }
    printf("\n");
  }
}
