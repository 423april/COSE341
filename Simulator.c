#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 30
#define MAX_IO_NUM 60
#define INF 1000000

//프로세스 구조체
typedef struct process* proPointer;
typedef struct process{
  int pid;
  int CPUburst;
  //int IOburst;
  int arrival;
  int priority;
  int CPUburst_remain;
}process;

typedef struct IO* IOPointer;
typedef struct IO{
  int pid;
  int IOburst;
  int when; //해당 프로세스의 CPUburst_remain 이 얼마일때 IO interrupt 될 것인지.
}IO;

//job queue를 최대 프로세스 개수만큼 선언
proPointer jobQ[MAX_PROCESS_NUM];
int jQ_front, jQ_rear;

//IO 담아놓은 배열을 MAX_IO_NUM만큼 선언
IOPointer ioQ[MAX_IO_NUM];
int ioQ_front, ioQ_rear;

//ready queue 배열 선언
proPointer readyQ[MAX_PROCESS_NUM];
int rQ_front, rQ_rear;

//waiting queue 배열 선언
proPointer waitQ[MAX_PROCESS_NUM];
int wQ_front, wQ_rear;

//job queue 초기화
void init_jobQ(){
  jQ_front = -1;
  jQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    jobQ[i] = NULL;
  }
}
//job queue enqueue
void add_jobQ(proPointer newP){
  if(jQ_rear == MAX_PROCESS_NUM - 1)
    printf("jobQ is FULL");
  else
    jobQ[++jQ_rear] = newP;
}
//job queue dequeue
proPointer poll_jobQ(){
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
void add_ioQ(IOPointer newIO){
  if(ioQ_rear == MAX_PROCESS_NUM - 1)
    printf("ioQ is FULL");
  else
    ioQ[++ioQ_rear] = newIO;
}
//IO queue dequeue
IOPointer poll_ioQ(){
  if(ioQ_front == ioQ_rear)
    printf("ioQ is EMPTY");
  else
    return ioQ[++ioQ_front];
}

//ready queue 초기화
void init_readyQ(){
  rQ_front = -1;
  rQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    readyQ[i] = NULL;
  }
}
//ready queue enqueue
void add_readyQ(proPointer newP){
  if(rQ_rear == MAX_PROCESS_NUM - 1)
    printf("readyQ is FULL");
  else
    readyQ[++rQ_rear] = newP;
}
//ready queue dequeue
proPointer poll_readyQ(){
  if(rQ_front == rQ_rear)
    printf("readyQ is EMPTY");
  else
    return readyQ[++rQ_front];
}

//wait queue 초기화
void init_waitQ(){
  wQ_front = -1;
  wQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    waitQ[i] = NULL;
  }
}
//wait queue enqueue
void add_waitQ(proPointer newP){
  if(wQ_rear == MAX_PROCESS_NUM - 1)
    printf("waitQ is FULL");
  else
    waitQ[++wQ_rear] = newP;
}
//wait queue dequeue
proPointer poll_waitQ(){
  if(wQ_front == wQ_rear)
    printf("waitQ is EMPTY");
  else
    return waitQ[++wQ_front];
}

void printQ_job(){
  for(int i = 0; i < (jQ_rear - jQ_front); i++){
    printf("p%d , ", jobQ[i]->pid);
    printf("CPUburst %d, ", jobQ[i]->CPUburst);
    printf("arrival %d, ", jobQ[i]->arrival);
    printf("priority %d\n", jobQ[i]->priority);
  }
}

void printQ_ready(){
  for(int i = 0; i < (rQ_rear - rQ_front); i++){
    printf("p%d ", readyQ[i]->pid);
  }
}

//arrival time을 기준으로 정렬해서 ready queue에 넣어준다.
void merge(proPointer list[], int p, int q, int r){
  int n1 = q - p + 1;
  int n2 = r - q;
  proPointer L[n1 + 1];
  proPointer R[n1 + 1];
  int i, j;
  for(i = 0; i < n1; i++){
    L[i] = list[p + i - 1];
  }
  L[n1]->arrival = INF;
  for(j = 0; j < n2; j++){
    R[j] = list[q + j];
  }
  R[n2]->arrival = INF;
  i = 0; j = 0;
  for(int k = p; k <= r; k++){
    if(L[i]->arrival <= R[j]->arrival){
      list[k] = L[i];
      i++;
    }
    else{
      list[k] = R[j];
      j++;
    }
  }
}//end merge

void mergesort(proPointer list[], int p, int r){
  if(p < r){
    int q = (p+r)/2;
    mergesort(list, p, q);
    mergesort(list, q+1, r);
    merge(list, p, q, r);
  }
}

void job2ready(){
	printQ_job();
  mergesort(jobQ, jQ_front, jQ_rear);
  printQ_job();
  init_readyQ();
  for(int i = 0; i < (jQ_rear - jQ_front); i++){
    add_readyQ(poll_jobQ());
  }
  // printf("init temp\n");
  // proPointer temp;
  // printf("inited temp\n");
  // printQ_job();
  // printf("jQ_rear: %d\n", jQ_rear);
  // for (int i = 0; i <= jQ_rear; i++)
  //  {  printf("i: %d, ", i);
  //      for (int j = 0; j <= jQ_rear; j++)
  //      { printf("j: %d, ", j);
  //          if (jobQ[j]->arrival  > jobQ[j + 1]-> arrival)
  //          {
  //              temp        = jobQ[j];
  //              jobQ[j]     = jobQ[j + 1];
  //              jobQ[j + 1] = temp;
	//        for(int k = 0; k < (jQ_rear-jQ_front); k++){
	// 	      printf("p%d ", jobQ[k]->pid);
	//        }
	//       printf("\n");
  //          }
  //      }
  //  }
  // printQ_job();
  //  init_readyQ();
  //  for(int i = 0; i <= jQ_rear; i++){
  //    add_readyQ(poll_jobQ());
  //  }
  //  printQ_ready();
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
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = i+1;
    newP->CPUburst = rand() % 25 + 1; //CPU burst time 1 ~ 25
    newP->arrival = rand() % (num_process + 10);
    newP->priority = rand() % num_process + 1;
    newP->CPUburst_remain = newP -> CPUburst;

    //job queue에 넣어준다. 순서는 pid 오름차순.
    add_jobQ(newP);
  }
  printQ_job();
  for(int j = 0; j < num_IO; j++){
	  IOPointer newIO = (IOPointer)malloc(sizeof(struct IO));
      newIO->pid = rand() % num_process + 1;
      newIO->IOburst = rand() % 10 + 1; //IO burst time 1~10
      // 1 <= when < CPUburst 이어야한다.
      newIO->when = rand() % (jobQ[newIO->pid - 1]->CPUburst - 1) + 1;
      add_ioQ(newIO);
      printf("pid: %d, IOburst: %d, when %d\n", newIO->pid, newIO->IOburst, newIO->when);
  //   if(j != 0){
  //    for(int k = 0; k < j; k++){
  //       해당 프로세스가 같은 시간에 다른 IO burst 있는지 확인하고 있으면, 다시 선택.
  //      if(ioQ[k]->pid == whichP && ioQ[k]->when == when) continue;
	// printf("IO same place\n");
	// add_ioQ(newIO);
  //      count++;
  //     }
     }
     for(int i = 0; i < (ioQ_rear - ioQ_front); i++){
       printf("ioQ: ");
       printf("p%d , IOburst %d, when %d\n", ioQ[i]->pid, ioQ[i]->IOburst, ioQ[i]->when);
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
  job2ready();

  return 0;
}
