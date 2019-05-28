#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 30
#define MAX_IO_NUM 60
#define INF 1000000

#define FCFS 1
#define SJF 2
#define SJFPRE 3 //SJF preemptive
#define PRI 4 //Priority non-preemptive
#define RR 5
#define PRIPRE 6 //Priority preemptive

typedef struct IO* IOPointer;
typedef struct IO{
  int pid;
  int IOburst;
//해당 프로세스의 CPUburst_remain 이 얼마일때 IO interrupt 될 것인지.
  int when;
}IO;

//프로세스 구조체
typedef struct process* proPointer;
typedef struct process{
  int pid;
  int CPUburst;
  int IOburst;
  int arrival;
  int priority;
  int CPUburst_remain;
  int IOburst_remain;
  IOPointer IO;
  int waitingTime;
  int turnaroundTime;
  int responseTime;
  //waiting queue에서 기다린 시간
  int waitingQ;
}process;

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

proPointer clonereadyQ[MAX_PROCESS_NUM];
int crQ_front, crQ_rear;

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

//clone ready queue 초기화
void init_clonereadyQ(){
  crQ_front = -1;
  crQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    clonereadyQ[i] = NULL;
  }
}
//clone ready queue enqueue
void add_clonereadyQ(proPointer newP){
  if(crQ_rear == MAX_PROCESS_NUM - 1)
    printf("clonereadyQ is FULL");
  else
    clonereadyQ[++crQ_rear] = newP;
}
//clone ready queue dequeue
proPointer poll_clonereadyQ(){
  if(crQ_front == crQ_rear)
    printf("clonereadyQ is EMPTY");
  else
    return clonereadyQ[++crQ_front];
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
  printf("\n");
}

//알고리즘 여러개 돌릴때 같은 데이터 써야하므로 기존 레디큐를 복사해서 사용한다.
void clone_readyQ(){
  init_clonereadyQ();
  for(int i = 0; i <= rQ_rear; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = readyQ[i]->pid;
    newP->CPUburst = readyQ[i]->CPUburst;
    newP->arrival = readyQ[i]->arrival;
    newP->priority = readyQ[i]->priority;
    newP->CPUburst_remain = readyQ[i]->CPUburst_remain;
    newP->IOburst_remain = readyQ[i]->IOburst_remain;
    newP->IO = readyQ[i]->IO;
    newP->waitingTime = readyQ[i]->waitingTime;
    newP->turnaroundTime = readyQ[i]->turnaroundTime;
    newP->responseTime = readyQ[i]->responseTime;
    newP->IOburst = readyQ[i]->IOburst;
    newP->waitingQ = readyQ[i]->waitingQ;
    add_clonereadyQ(newP);
  }
  for(int i= crQ_front+1; i <= crQ_rear; i++){
    printf("p%d ", clonereadyQ[i]->pid);
  }
  printf("\n");
}

//arrival time을 기준으로 정렬해서 ready queue에 넣어준다.
//type는 arrival time으로 정렬하는 것인지, IOburst_remain으로 정렬하는지 결정한다.
//arrival time: 0, IOburst_remain: 1.
void merge(proPointer list[], int p, int q, int r, int type){
  int n1 = q - p + 1;
  int n2 = r - q ;
 // printf("n1: %d, n2: %d\n", n1, n2);
  proPointer L[n1 + 1];
  proPointer R[n1 + 1];
 // printf("created L and R\n");
  int i, j;
  for(i = 0; i < n1; i++){
    L[i] = list[p + i];
  }
 // printf("L insert til n1 - 1\n");
  proPointer dummy1 = (proPointer)malloc(sizeof(struct process));
  if(type == 0)
    dummy1 -> arrival = INF;
  else
    dummy1 -> IOburst_remain = INF;
  L[n1] = dummy1;
 // printf("dummy interted\n");
  for(j = 0; j < n2; j++){
    R[j] = list[q + 1 + j];
  }
 // printf("R insert til n2 -1 \n");
  proPointer dummy2 = (proPointer)malloc(sizeof(struct process));
  if(type == 0)
    dummy2 -> arrival = INF;
  else
    dummy2 -> IOburst_remain = INF;
  R[n2] = dummy2;
 // printf("dummy inserted\n");
 // printf("L,R init good\n");
  i = 0; j = 0;
  for(int k = p; k <= r; k++){
    if(type == 0){
      if(L[i]->arrival <= R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
    }

    if(type == 1){
      if(L[i]->IOburst_remain <= R[j]->IOburst_remain){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
    }

  }
 // printf("merge %d, %d, %d well\n", p, q, r);
}//end merge

void mergesort(proPointer list[], int p, int r, int type){
  if(type == 0){
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, 0);
      mergesort(list, q+1, r, 0);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, 0);
    }
  }

  if(type == 1){
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, 1);
      mergesort(list, q+1, r, 1);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, 1);
    }
  }
}

void job2ready(){
	//printQ_job();
  mergesort(jobQ, jQ_front+1, jQ_rear, 0);
  //printQ_job();
  init_readyQ();
  //printf("jQ front %d, rear %d\n", jQ_front, jQ_rear);
  for(int i = 0; i <= jQ_rear; i++){
    //printf("i: %d\n", i);
	   proPointer temp = (proPointer)malloc(sizeof(struct process));
   	temp = poll_jobQ();
	 //printf("jQ front %d, rear %d\n", jQ_front, jQ_rear);
	 // printf("pid: %d, arrival: %d\n",temp->pid, temp->arrival );
	  add_readyQ(temp);
  }
  //printf("ready front %d rear %d\n", rQ_front, rQ_rear);

  printQ_ready();
}

//arrival time을 기준으로 정렬해서 ready queue에 넣어준다.
void merge_when(IOPointer list[], int p, int q, int r){
  int n1 = q - p + 1;
  int n2 = r - q ;
 // printf("n1: %d, n2: %d\n", n1, n2);
  IOPointer L[n1 + 1];
  IOPointer R[n1 + 1];
 // printf("created L and R\n");
  int i, j;
  for(i = 0; i < n1; i++){
    L[i] = list[p + i];
  }
 // printf("L insert til n1 - 1\n");
  IOPointer dummy1 = (IOPointer)malloc(sizeof(struct IO));
  dummy1 -> when = INF;
  L[n1] = dummy1;
 // printf("dummy interted\n");
  for(j = 0; j < n2; j++){
    R[j] = list[q + 1 + j];
  }
 // printf("R insert til n2 -1 \n");
  IOPointer dummy2 = (IOPointer)malloc(sizeof(struct IO));
  dummy2 -> when = INF;
  R[n2] = dummy2;
 // printf("dummy inserted\n");
 // printf("L,R init good\n");
  i = 0; j = 0;
  for(int k = p; k <= r; k++){
    if(L[i]->when <= R[j]->when){
      list[k] = L[i];
      i++;
    }
    else{
      list[k] = R[j];
      j++;
    }
  }
 // printf("merge %d, %d, %d well\n", p, q, r);
}//end merge

void mergesort_when(IOPointer list[], int p, int r){
  if(p < r){
//	  printf("p: %d, r: %d\n", p, r);
    int q = (p+r)/2;
    mergesort_when(list, p, q);
    mergesort_when(list, q+1, r);
  //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
    merge_when(list, p, q, r);
  }
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
    newP->CPUburst = rand() % 25 + 2; //CPU burst time 2 ~ 26
    newP->arrival = rand() % (num_process + 10);
    newP->priority = rand() % num_process + 1;
    newP->CPUburst_remain = newP -> CPUburst;
    newP->waitingTime = 0;
    newP->turnaroundTime = 0;
    newP->responseTime = 0;
    newP->waitingQ = 0;
    newP->IOburst = 0;
    newP->IOburst_remain = 0;
    newP->IO = NULL;

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
      //프로세스 상세에 가장 먼저 일어나는 IO를 표시해준다.
      if(jobQ[newIO->pid - 1]->IO == NULL){
        jobQ[newIO->pid - 1]->IO = newIO;
        jobQ[newIO->pid - 1]->IOburst = newIO->IOburst;
        jobQ[newIO->pid - 1]->IOburst_remain = newIO->IOburst;
      }else if(jobQ[newIO->pid - 1]->IO->when > newIO->when){
        jobQ[newIO->pid - 1]->IO = newIO;
        jobQ[newIO->pid - 1]->IOburst = newIO->IOburst;
        jobQ[newIO->pid - 1]->IOburst_remain = newIO->IOburst;
      }
      printf("IO assigned\n");
     }
     //io를 when 오름차순으로 정렬한다.
     mergesort_when(ioQ, ioQ_front+1, ioQ_rear);
     for(int i = 0; i < (ioQ_rear - ioQ_front); i++){
       printf("ioQ: ");
       printf("p%d , IOburst %d, when %d\n", ioQ[i]->pid, ioQ[i]->IOburst, ioQ[i]->when);
     }
  }
  //한 프로세스를 실행하는 동안 다른 프로세스들의 waiting time을 +1 해주는 함수
  void wait(proPointer list[], int front, int rear, int pid){
    for(int i = front+1; i <= rear; i++){
      if(i != pid-1){
        list[i]->waitingTime++;
      }
    }
  }
//큐의 front, rear index 넣으면 해당 큐가 비었는지 알려준다.
  int isEmpty(int front, int rear){
    if(front == rear)
      return 1; //true;
    else
      return 0;//false;
  }

//waiting queue에서 얼마나 기다리고 있는지 매 타임 +1 해주고,
//waitingQ time이 IOburst와 같아지면 내보낸다.
  void waiting(int nowTime){
    if(!isEmpty(wQ_front, wQ_rear)){
      for(int i = wQ_front + 1; i <= wQ_rear; i++){
        waitQ[i]->waitingQ++;
        if(waitQ[i]->waitingQ == waitQ[i]->IOburst){
          proPointer newP = (proPointer)malloc(sizeof(struct process));
          newP = poll_waitQ();
          newP->arrival = nowTime;
          add_clonereadyQ(newP);
          //mergesort(waitQ, wQ_front + 1, wQ_rear, 1);
        }
      }
    }
  }

//선입선출
void FCFS_alg(){
  printf("start FCFS algorithm: \n");
  //레디큐를 복사한다. //현재 arrival time 오름차순 정렬되어있다.
  clone_readyQ();

//wait queue 초기화
  init_waitQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;


  //레디큐는 도착시간 순으로 정렬되어있다.
  do{
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP = poll_clonereadyQ();

    do{
      if(nowTime < newP->arrival){
        printf("bb ");
      }
      else{
        //해당 프로세스의 CPUburst_remain -1해준다.
        newP->CPUburst_remain--;
        printf("p%d ", newP->pid);
        //다른 프로세스들 웨이팅 타임 더해준다.
        wait(clonereadyQ, crQ_front, crQ_rear, newP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime);
        //실행 마치면 turnaroundTime 계산한다.
        if(newP->CPUburst_remain == 0){
          newP->turnaroundTime = nowTime - newP->arrival;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(newP->CPUburst == newP->CPUburst_remain){
          newP->responseTime = nowTime - newP->arrival;
        }

        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        if(newP->IO != NULL){
          if(newP->IO->when == newP->CPUburst - newP->CPUburst_remain){
            IOPointer nowIO = (IOPointer)malloc(sizeof(struct IO));
            nowIO = poll_ioQ();
            newP->IOburst = nowIO->IOburst;
            add_waitQ(newP);
            mergesort(waitQ, wQ_front+1, wQ_rear, 1);
            free(newP);
	    newP = (proPointer)malloc(sizeof(struct process));
            newP = poll_clonereadyQ();
            continue;
          }
        }

      }/////else
      nowTime++;
    }while(newP->CPUburst_remain >= 0);
    printf("finished one segment, start next\n");
    printf("p%d at CPU\n", newP->pid);
  }while(isEmpty(crQ_front, crQ_rear));
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
  FCFS_alg();

  return 0;
}
