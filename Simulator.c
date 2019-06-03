#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 30
#define MAX_IO_NUM 60
#define INF 1000000

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
  int waitingTime;
  int turnaroundTime;
  int responseTime;

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

proPointer cjobQ[MAX_PROCESS_NUM];
int cjQ_front, cjQ_rear;

proPointer clonereadyQ[MAX_PROCESS_NUM];
int crQ_front, crQ_rear;

int eval[6];

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

//cjob queue 초기화
void init_cjobQ(){
  cjQ_front = -1;
  cjQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    cjobQ[i] = NULL;
  }
}
//cjob queue enqueue
void add_cjobQ(proPointer newP){
  if(cjQ_rear == MAX_PROCESS_NUM - 1)
    printf("cjobQ is FULL");
  else
    cjobQ[++cjQ_rear] = newP;
}
//cjob queue dequeue
proPointer poll_cjobQ(){
  if(cjQ_front == cjQ_rear)
    printf("cjobQ is EMPTY");
  else
    return cjobQ[++cjQ_front];
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
  printf("\nreadyQ: ");
  for(int i = 0; i < (rQ_rear - rQ_front); i++){
    printf("p%d ", readyQ[i]->pid);
    printf("CPUburst %d, ", readyQ[i]->CPUburst);
    printf("arrival %d, ", readyQ[i]->arrival);
    printf("priority %d\n", readyQ[i]->priority);
  }
  printf("\n");
}

void printQ_cloneready(){
  printf("\nclone readyQ: \n");
  for(int i = crQ_front+1; i <= crQ_rear; i++){
    printf("p%d ", clonereadyQ[i]->pid);
    printf("CPUburst %d, ", clonereadyQ[i]->CPUburst);
    printf("cpu remain %d, ", clonereadyQ[i]->CPUburst_remain);
    printf("arrival %d, ", clonereadyQ[i]->arrival);
    printf("priority %d\n", clonereadyQ[i]->priority);
  }
  printf("\n");
}

void clone_jobQ(){
  init_cjobQ();
  for(int i = jQ_front+1; i <= jQ_rear; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = jobQ[i]->pid;
    newP->CPUburst = jobQ[i]->CPUburst;
    newP->arrival = jobQ[i]->arrival;
    newP->priority = jobQ[i]->priority;
    newP->CPUburst_remain = jobQ[i]->CPUburst_remain;
    newP->IOburst = jobQ[i]->IOburst;
    newP->IOburst_remain = jobQ[i]->IOburst_remain;
    newP->waitingTime = jobQ[i]->waitingTime;
    newP->turnaroundTime = jobQ[i]->turnaroundTime;
    newP->responseTime = jobQ[i]->responseTime;
    add_cjobQ(newP);
  }

}

//알고리즘 여러개 돌릴때 같은 데이터 써야하므로 기존 레디큐를 복사해서 사용한다.
void clone_readyQ(){
  init_clonereadyQ();
  for(int i = rQ_front+1; i <= rQ_rear; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = readyQ[i]->pid;
    newP->CPUburst = readyQ[i]->CPUburst;
    newP->arrival = readyQ[i]->arrival;
    newP->priority = readyQ[i]->priority;
    newP->CPUburst_remain = readyQ[i]->CPUburst_remain;
    newP->IOburst_remain = readyQ[i]->IOburst_remain;
    newP->waitingTime = readyQ[i]->waitingTime;
    newP->turnaroundTime = readyQ[i]->turnaroundTime;
    newP->responseTime = readyQ[i]->responseTime;
    newP->IOburst = readyQ[i]->IOburst;
    add_clonereadyQ(newP);
  }
  printQ_cloneready();
}

//arrival time을 기준으로 정렬해서 ready queue에 넣어준다.
//type는 arrival time으로 정렬하는 것인지, IOburst_remain으로 정렬하는지 결정한다.
//arrival time: 0, IOburst_remain: 1, CPUburst_remain: 2, priority: 3
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
  switch (type) {
    case 0:
      dummy1 -> arrival = INF;
      break;
    case 1:
      dummy1 -> IOburst_remain = INF;
      break;
    case 2:
      dummy1 -> CPUburst_remain = INF;
      break;
    case 3:
      dummy1 -> priority = INF;
      break;
    default:
      printf("merge: unknown type received\n");
      break;
  }
  L[n1] = dummy1;
 // printf("dummy interted\n");
  for(j = 0; j < n2; j++){
    R[j] = list[q + 1 + j];
  }
 // printf("R insert til n2 -1 \n");
  proPointer dummy2 = (proPointer)malloc(sizeof(struct process));
  switch (type) {
    case 0:
      dummy2 -> arrival = INF;
      break;
    case 1:
      dummy2 -> IOburst_remain = INF;
      break;
    case 2:
      dummy2 -> CPUburst_remain = INF;
      break;
    case 3:
      dummy2 -> priority = INF;
      break;
    default:
      printf("merge: unknown type received\n");
      break;
  }
  R[n2] = dummy2;

  i = 0; j = 0;
  for(int k = p; k <= r; k++){
    switch(type){
      case 0:
      if(L[i]->arrival <= R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case 1:
      if(L[i]->IOburst_remain <= R[j]->IOburst_remain){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case 2:
      if(L[i]->CPUburst_remain <= R[j]->CPUburst_remain){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case 3:
      if(L[i]->priority <= R[j]->priority){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      default:
      printf("unknown type\n");
      break;
    }
  }
  free(dummy1);
  free(dummy2);
}//end merge

void mergesort(proPointer list[], int p, int r, int type){
  switch(type){
    case 0:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, 0);
      mergesort(list, q+1, r, 0);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, 0);
    }
    break;

    case 1:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, 1);
      mergesort(list, q+1, r, 1);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, 1);
    }
    break;

    case 2:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, 2);
      mergesort(list, q+1, r, 2);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, 2);
    }
    break;

    case 3:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, 3);
      mergesort(list, q+1, r, 3);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, 3);
    }
    break;

    default:
    printf("mergesort: unknown type\n");
    break;
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
	   proPointer temp = jobQ[i];

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
    newP->IOburst = 0;
    newP->IOburst_remain = 0;

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
      //중복제거의 노력
     for(int i = 1; i < j; i++){
       if(newIO->pid == ioQ[i]->pid && newIO->when == ioQ[i]->when){
         newIO->pid = (newIO->pid + 1) % num_process + 1;
       }
     }

      add_ioQ(newIO);
      printf("pid: %d, IOburst: %d, when %d\n", newIO->pid, newIO->IOburst, newIO->when);
     }
      printf("IO assigned\n");

  }
  //한 프로세스를 실행하는 동안 다른 프로세스들의 waiting time을 +1 해주는 함수
  void wait(int pid){
    for(int i = rQ_front+1; i <= rQ_rear; i++){
      if(readyQ[i]->pid != pid){
        readyQ[i]->waitingTime++;
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

//IO busrt 얼마나 했는지 매 타임  -1 해주고,
//IOburst_remain == 0 되면 내보내준다.
  void waiting(int nowTime, int type){
    if(!isEmpty(wQ_front, wQ_rear)){
      for(int i = wQ_front + 1; i <= wQ_rear; i++){
        waitQ[i]->IOburst_remain--;
        if(waitQ[i]->IOburst_remain == 0){
	         //printf("waiting exit: p%d, CPU remain: %d\n", newP->pid, newP->CPUburst_remain);
          //waiting queue는 남아있는 IOburst time 오름차순으로 정렬한다.
          add_readyQ(poll_waitQ());
          mergesort(waitQ, wQ_front+1, wQ_rear, 1);
          // printf("clone ready queue: ");
          // for(int i = crQ_front+1; i <= crQ_rear; i++){
          //   printf("p%d ", clonereadyQ[i]->pid);
          // }
          // printf("\n");
          //해당 우선순위에 부합하게 오름차순 정렬한다.
          //arrival time은 넣은 그대로가 순서가 되므로 따로 정렬해주지 않는다.
          if(type == 0) return;
          else
            mergesort(readyQ, rQ_front+1, rQ_rear, type);
          // printf("clone ready queue: ");
          // for(int i = crQ_front+1; i <= crQ_rear; i++){
          //   printf("p%d ", clonereadyQ[i]->pid);
          // }
          // printf("\n");
        }
      }
    }
  }

  void evaluation(int wT[], int tT[], int rT[], int alg){
    //evaluation
    int num = rQ_rear - rQ_front;
    int sumwT = 0;
    int sumtT = 0;
    int sumrT = 0;
    double avgwT, avgtT, avgrT;
    for(int i = 0; i < rQ_rear - rQ_front; i++){
      printf("pid: %d, waiting time: %d, turnaround time: %d, response time: %d\n",
        i+1, wT[i], tT[i], rT[i]);
        sumwT += wT[i];
        sumtT += tT[i];
        sumrT += rT[i];
    }
    avgwT = (double)sumwT/num;
    avgtT = (double)sumtT/num;
    avgrT = (double)sumrT/num;
    printf("avgwT: %f, avgtT: %f, avgrT: %f\n", avgwT, avgtT, avgrT);
  }

//선입선출
void FCFS_alg(int num_process, int num_IO){
  printf("\n********************start FCFS algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );
  //jobQ arrival time 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, 0);
  printQ_job();
  //ready, wait initialize
  init_readyQ();
  init_waitQ();
  //각 프로세스의 평가시간 넣어둘 배열 선언
  int wT[jQ_rear - jQ_front];
  int tT[jQ_rear - jQ_front];
  int rT[jQ_rear - jQ_front];
  //현재 시간 나타내는 변수
  int nowTime = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){
    if(!isEmpty(jQ_front, jQ_rear)){
      while(jobQ[jQ_front+1]->arrival == nowTime)
        add_readyQ(poll_jobQ());
        printQ_ready();
    }

    if(!isEmpty(rQ_front, rQ_rear) && runP == NULL){
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
    printf("bb ");
    }
    else if(runP==NULL && !isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
      waiting(nowTime, 0);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      wait(runP->pid);
      waiting(nowTime, 0);

        if(runP->CPUburst_remain == 0){
          check++;
          runP = NULL;
        }
      //random IO. 95% 확률로 IO 발생.
      if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
        runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
        runP->IOburst_remain = runP->IOburst;
        printf("\n<IO interrupt!>p%d, IOburst: %d, CPUburst_remain: %d\n", runP->pid, runP->IOburst, runP->CPUburst_remain);
        add_waitQ(runP);
        mergesort(waitQ, wQ_front+1, wQ_rear, 1);
        if(!isEmpty(rQ_front, rQ_rear)) runP = poll_readyQ();
        else runP = NULL;
      }
    }/////else
  }/////for process
}/////FCFS_alg


//preemption 없는 SJF 알고리즘.
//CPU_remain이 가장 작은 것부터 실행
void SJF_alg(int num_IO){
  printf("\n**************start non-preemptive SJF algorithm:**************\n");

  //job queue는 arrival 오름차순으로 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, 0);
  clone_jobQ();
  // printf("%d, %d", cjQ_front, cjQ_rear);
  // for(int i = cjQ_front+1; i <= cjQ_rear; i++){
  //   printf("%d ", cjobQ[i]->pid);
  // }
  // printf("\n");

//레디큐 init
  init_clonereadyQ();

  //wait queue 초기화
  init_waitQ();

  int wT[rQ_rear - rQ_front];
  int tT[rQ_rear - rQ_front];
  int rT[rQ_rear - rQ_front];
  //현재 시간 나타내는 변수
  int nowTime = 0;

  proPointer runP;
  printf("%d ", runP->pid);
  proPointer inP;

  do{
    for(int i = cjQ_front+1; i <= cjQ_rear; i++){
      if(cjobQ[cjQ_front+1]->arrival == nowTime){
        inP = poll_cjobQ();
        add_clonereadyQ(inP);
        printf("%d ", inP->pid);
        mergesort(clonereadyQ, crQ_front+1, crQ_rear, 2);
      }
    }

    if(!isEmpty(crQ_front, crQ_rear)){
      runP = poll_clonereadyQ();
      printf("\n new process polled! p%d\n", runP->pid);
      printf("clone ready queue: ");
      for(int i = crQ_front+1; i <= crQ_rear; i++){
        printf("p%d ", clonereadyQ[i]->pid);
      }
      printf("\n");
    }

    do{

      //CPU에서 실행중인 프로세스가 없으면 bb를 출력한다.
      if(runP == NULL){
        printf("bb ");
        //다른 프로세스들 웨이팅 타임 더해준다.
        if(!isEmpty(crQ_front, crQ_rear));
          wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        if(!isEmpty(wQ_front, wQ_rear));
          waiting(nowTime, 0);
      }

      else{
        printf("p%d ", runP->pid);
        //해당 프로세스의 CPUburst_remain -1해준다.
        runP->CPUburst_remain--;

        //다른 프로세스들 웨이팅 타임 더해준다.
        if(!isEmpty(crQ_front, crQ_rear));
          wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        if(!isEmpty(wQ_front, wQ_rear));
          waiting(nowTime, 0);


        //실행 마치면 turnaroundTime 계산한다.
        if(runP->CPUburst_remain == 0){
          runP->turnaroundTime = nowTime - runP->arrival + 1;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(runP->CPUburst == runP->CPUburst_remain+1){
          runP->responseTime = nowTime - runP->arrival;
        }


        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        for(int i = 0; i < num_IO; i++){
          if(ioQ[i]->pid == runP->pid){
            if(ioQ[i]->when == runP->CPUburst - runP->CPUburst_remain){
              runP->IOburst = ioQ[i]->IOburst;
              runP->IOburst_remain = ioQ[i]->IOburst;
              add_waitQ(runP);
              printf("waitP: p%d, IOburst remain: %d\n", runP->pid, runP->IOburst_remain);
              //IOburst_remain 순으로 정렬.
              mergesort(waitQ, wQ_front+1, wQ_rear, 1);
            if(!isEmpty(crQ_front, crQ_rear)){
              runP = poll_clonereadyQ();
              printf("after waitQ process: p%d\n", runP->pid);
              printf("clone ready queue: ");
              for(int i = crQ_front+1; i <= crQ_rear; i++){
                printf("p%d ", clonereadyQ[i]->pid);
              }
              printf("\n");
            }else{
              printf("next is blank\n");
              runP = NULL;
              printf("NULL\n");
            }
            break;
            }
          }
        }

      }/////else
      nowTime++;
      if(cjobQ[cjQ_front+1]->arrival == nowTime){
        inP = poll_cjobQ();
        add_clonereadyQ(inP);
        mergesort(clonereadyQ, crQ_front+1, crQ_rear, 2);
      }
    }while( runP->CPUburst_remain > 0 || runP == NULL);
    wT[runP->pid - 1] = runP->waitingTime;
    tT[runP->pid - 1] = runP->turnaroundTime;
    rT[runP->pid - 1] = runP->responseTime;
    runP = NULL;
  }while(!isEmpty(crQ_front, crQ_rear) || !isEmpty(wQ_front, wQ_rear));
  printf("\n");
  evaluation(wT, tT, rT, 1);
}

//preemption 없는 priority 알고리즘.
//CPU_remain이 가장 작은 것부터 실행
void PRI_alg(int num_IO){
  printf("\n***********start non-preemptive priority algorithm:************\n");

  //job queue는 arrival 오름차순으로 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, 0);
  clone_jobQ();

//레디큐 initialize
  init_readyQ();

  //wait queue 초기화
  init_waitQ();

  int wT[rQ_rear - rQ_front];
  int tT[rQ_rear - rQ_front];
  int rT[rQ_rear - rQ_front];
  //현재 시간 나타내는 변수
  int nowTime = 0;

  proPointer runP;

  do{
    if(cjobQ[cjQ_front+1]->arrival == nowTime){
      runP = poll_cjobQ();
      add_clonereadyQ(runP);
      mergesort(clonereadyQ, crQ_front+1, crQ_rear, 3);
    }
    if(!isEmpty(crQ_front, crQ_rear)){
      runP = poll_clonereadyQ();
      printf("\n new process polled! p%d\n", runP->pid);
      printf("clone ready queue: ");
      for(int i = crQ_front+1; i <= crQ_rear; i++){
        printf("p%d ", clonereadyQ[i]->pid);
      }
      printf("\n");
    }

    do{

      //CPU에서 실행중인 프로세스가 없으면 bb를 출력한다.
      if(nowTime < runP->arrival || runP == NULL){
        printf("bb ");
        //다른 프로세스들 웨이팅 타임 더해준다.
        wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime, 3);
      }

      else{
        printf("p%d ", runP->pid);
        //해당 프로세스의 CPUburst_remain -1해준다.
        runP->CPUburst_remain--;

        //다른 프로세스들 웨이팅 타임 더해준다.
        wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime, 3);


        //실행 마치면 turnaroundTime 계산한다.
        if(runP->CPUburst_remain == 0){
          runP->turnaroundTime = nowTime - runP->arrival + 1;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(runP->CPUburst == runP->CPUburst_remain+1){
          runP->responseTime = nowTime - runP->arrival;
        }


        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        for(int i = 0; i < num_IO; i++){
          if(ioQ[i]->pid == runP->pid){
            if(ioQ[i]->when == runP->CPUburst - runP->CPUburst_remain){
              runP->IOburst = ioQ[i]->IOburst;
              runP->IOburst_remain = ioQ[i]->IOburst;
              add_waitQ(runP);
              printf("waitP: p%d, IOburst remain: %d\n", runP->pid, runP->IOburst_remain);
              //IOburst_remain 순으로 정렬.
              mergesort(waitQ, wQ_front+1, wQ_rear, 1);
            if(!isEmpty(crQ_front, crQ_rear)){
              runP = poll_clonereadyQ();
              printf("after waitQ process: p%d\n", runP->pid);
              printf("clone ready queue: ");
              for(int i = crQ_front+1; i <= crQ_rear; i++){
                printf("p%d ", clonereadyQ[i]->pid);
              }
              printf("\n");
            }else{
              printf("next is blank\n");
              runP = NULL;
              printf("NULL\n");
            }
            break;
            }
          }
        }

      }/////else
      nowTime++;
      if(cjobQ[cjQ_front+1]->arrival == nowTime){
        runP = poll_cjobQ();
        add_clonereadyQ(runP);
        mergesort(clonereadyQ, crQ_front+1, crQ_rear, 3);
      }
    }while(runP == NULL || runP->CPUburst_remain > 0);
    wT[runP->pid - 1] = runP->waitingTime;
    tT[runP->pid - 1] = runP->turnaroundTime;
    rT[runP->pid - 1] = runP->responseTime;
    runP = NULL;
  }while(!isEmpty(crQ_front, crQ_rear) || !isEmpty(wQ_front, wQ_rear));
  printf("\n");
  evaluation(wT, tT, rT, 2);
}

//preemption 있는 SJF 알고리즘.
//CPU_remain이 가장 작은 것부터 실행
void PRESJF_alg(int num_IO){
  printf("\n************start preemptive SJF algorithm****************\n");

  //job queue는 arrival 오름차순으로 정렬
  clone_jobQ();
  mergesort(cjobQ, cjQ_front+1, cjQ_rear, 0);

//레디큐 initialize.
  init_clonereadyQ();

  //wait queue 초기화
  init_waitQ();

  int wT[rQ_rear - rQ_front];
  int tT[rQ_rear - rQ_front];
  int rT[rQ_rear - rQ_front];
  //현재 시간 나타내는 변수
  int nowTime = 0;

  proPointer inP;
  proPointer runP;

  do{
    //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
    if(cjobQ[cjQ_front+1]->arrival == nowTime){
      inP = poll_cjobQ();
      if(runP != NULL && runP->CPUburst_remain > inP->CPUburst_remain){
        add_clonereadyQ(runP);
        runP = inP;
      }else{
        add_clonereadyQ(inP);
        mergesort(clonereadyQ, crQ_front+1, crQ_rear, 2);
      }
    }
    if(!isEmpty(crQ_front, crQ_rear)){
      runP = poll_clonereadyQ();
      printf("\n new process polled! p%d\n", runP->pid);
      printf("clone ready queue: ");
      for(int i = crQ_front+1; i <= crQ_rear; i++){
        printf("p%d ", clonereadyQ[i]->pid);
      }
      printf("\n");
    }

    do{
      //아직 아무 프로세스도 도착하지 않았을때
      //CPU에서 실행중인 프로세스가 없으면 bb를 출력한다.
      if(runP == NULL){
        printf("bb ");
        //다른 프로세스들 웨이팅 타임 더해준다.
        if(!isEmpty(crQ_front, crQ_rear))
          wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        if(!isEmpty(wQ_front, wQ_rear))
          waiting(nowTime, 2);
      }

      else{
        printf("p%d ", runP->pid);
        //해당 프로세스의 CPUburst_remain -1해준다.
        runP->CPUburst_remain--;

        //다른 프로세스들 웨이팅 타임 더해준다.
        wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime, 2);


        //실행 마치면 turnaroundTime 계산한다.
        if(runP->CPUburst_remain == 0){
          runP->turnaroundTime = nowTime - runP->arrival + 1;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(runP->CPUburst == runP->CPUburst_remain+1){
          runP->responseTime = nowTime - runP->arrival;
        }


        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        for(int i = 0; i < num_IO; i++){
          if(ioQ[i]->pid == runP->pid){
            if(ioQ[i]->when == runP->CPUburst - runP->CPUburst_remain){
              runP->IOburst = ioQ[i]->IOburst;
              runP->IOburst_remain = ioQ[i]->IOburst;
              add_waitQ(runP);
              printf("waitP: p%d, IOburst remain: %d\n", runP->pid, runP->IOburst_remain);
              //IOburst_remain 순으로 정렬.
              mergesort(waitQ, wQ_front+1, wQ_rear, 1);
            if(!isEmpty(crQ_front, crQ_rear)){
              runP = poll_clonereadyQ();
              printf("after waitQ process: p%d\n", runP->pid);
              printf("clone ready queue: ");
              for(int i = crQ_front+1; i <= crQ_rear; i++){
                printf("p%d ", clonereadyQ[i]->pid);
              }
              printf("\n");
            }else{
              printf("next is blank\n");
              runP = NULL;
              printf("NULL\n");
            }
            break;
            }
          }
        }

      }/////else
      nowTime++;

      //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
      if(cjobQ[cjQ_front+1]->arrival == nowTime){
        inP = poll_cjobQ();
        if(runP != NULL && runP->CPUburst_remain > inP->CPUburst_remain){
          add_clonereadyQ(runP);
          runP = inP;
        }else{
          add_clonereadyQ(inP);
          mergesort(clonereadyQ, crQ_front+1, crQ_rear, 2);
        }
      }
    }while(runP == NULL || runP->CPUburst_remain > 0);
    wT[runP->pid - 1] = runP->waitingTime;
    tT[runP->pid - 1] = runP->turnaroundTime;
    rT[runP->pid - 1] = runP->responseTime;
    runP = NULL;
    //free(runP);
    //proPointer runP = (proPointer)malloc(sizeof(struct process));
    // runP = poll_clonereadyQ();
    // printf("\n new process polled! p%d\n", runP->pid);
  }while(!isEmpty(crQ_front, crQ_rear) || !isEmpty(wQ_front, wQ_rear));
  printf("\n");

  evaluation(wT, tT, rT, 3);
}

//preemption 있는 Priority 알고리즘.
//우선순위 번호가 가장 작은 것부터 실행(숫자가 작을수록 우선순위 높음)
void PREPRI_alg(int num_IO){
  printf("\n************start preemptive Priority algorithm****************\n");

  //job queue는 arrival 오름차순으로 정렬
  clone_jobQ();
  mergesort(cjobQ, cjQ_front+1, cjQ_rear, 0);

//레디큐 initialize.
  init_clonereadyQ();

  //wait queue 초기화
  init_waitQ();

  int wT[rQ_rear - rQ_front];
  int tT[rQ_rear - rQ_front];
  int rT[rQ_rear - rQ_front];
  //현재 시간 나타내는 변수
  int nowTime = 0;

  proPointer inP;
  proPointer runP;

  do{
    //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
    if(cjobQ[cjQ_front+1]->arrival == nowTime){
      inP = poll_cjobQ();
      if(runP != NULL && runP->priority > inP->priority){
        add_clonereadyQ(runP);
        runP = inP;
      }else{
        add_clonereadyQ(inP);
        mergesort(clonereadyQ, crQ_front+1, crQ_rear, 3);
      }
    }
    if(!isEmpty(crQ_front, crQ_rear)){
      runP = poll_clonereadyQ();
      printf("\n new process polled! p%d\n", runP->pid);
      printf("clone ready queue: ");
      for(int i = crQ_front+1; i <= crQ_rear; i++){
        printf("p%d ", clonereadyQ[i]->pid);
      }
      printf("\n");
    }

    do{
      //아직 아무 프로세스도 도착하지 않았을때
      //CPU에서 실행중인 프로세스가 없으면 bb를 출력한다.
      if(runP == NULL){
        printf("bb ");
        //다른 프로세스들 웨이팅 타임 더해준다.
        if(!isEmpty(crQ_front, crQ_rear))
          wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        if(!isEmpty(wQ_front, wQ_rear))
          waiting(nowTime, 3);
      }

      else{
        printf("p%d ", runP->pid);
        //해당 프로세스의 CPUburst_remain -1해준다.
        runP->CPUburst_remain--;

        //다른 프로세스들 웨이팅 타임 더해준다.
        wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime, 3);


        //실행 마치면 turnaroundTime 계산한다.
        if(runP->CPUburst_remain == 0){
          runP->turnaroundTime = nowTime - runP->arrival + 1;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(runP->CPUburst == runP->CPUburst_remain+1){
          runP->responseTime = nowTime - runP->arrival;
        }


        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        for(int i = 0; i < num_IO; i++){
          if(ioQ[i]->pid == runP->pid){
            if(ioQ[i]->when == runP->CPUburst - runP->CPUburst_remain){
              runP->IOburst = ioQ[i]->IOburst;
              runP->IOburst_remain = ioQ[i]->IOburst;
              add_waitQ(runP);
              printf("waitP: p%d, IOburst remain: %d\n", runP->pid, runP->IOburst_remain);
              //IOburst_remain 순으로 정렬.
              mergesort(waitQ, wQ_front+1, wQ_rear, 1);
            if(!isEmpty(crQ_front, crQ_rear)){
              runP = poll_clonereadyQ();
              printf("after waitQ process: p%d\n", runP->pid);
              printf("clone ready queue: ");
              for(int i = crQ_front+1; i <= crQ_rear; i++){
                printf("p%d ", clonereadyQ[i]->pid);
              }
              printf("\n");
            }else{
              printf("next is blank\n");
              runP = NULL;
              printf("NULL\n");
            }
            break;
            }
          }
        }

      }/////else
      nowTime++;

      //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
      if(cjobQ[cjQ_front+1]->arrival == nowTime){
        inP = poll_cjobQ();
        if(runP != NULL && runP->priority > inP->priority){
          add_clonereadyQ(runP);
          runP = inP;
        }else{
          add_clonereadyQ(inP);
          mergesort(clonereadyQ, crQ_front+1, crQ_rear, 3);
        }
      }
    }while(runP == NULL || runP->CPUburst_remain > 0);
    wT[runP->pid - 1] = runP->waitingTime;
    tT[runP->pid - 1] = runP->turnaroundTime;
    rT[runP->pid - 1] = runP->responseTime;
    runP = NULL;
  }while(!isEmpty(crQ_front, crQ_rear) || !isEmpty(wQ_front, wQ_rear));
  printf("\n");

  evaluation(wT, tT, rT, 4);
}

//Round Robin 알고리즘.
//arrival이 가장 작은 것부터 실행
//time quantum = 3(default);
void RR_alg(int num_IO, int tq){
  printf("\n************start Round Robin algorithm****************\n");

  //job queue는 arrival 오름차순으로 정렬
  clone_jobQ();
  mergesort(cjobQ, cjQ_front+1, cjQ_rear, 0);

//레디큐 initialize.
  init_clonereadyQ();

  //wait queue 초기화
  init_waitQ();

  int wT[rQ_rear - rQ_front];
  int tT[rQ_rear - rQ_front];
  int rT[rQ_rear - rQ_front];
  //현재 시간 나타내는 변수
  int nowTime = 0;

  proPointer inP;
  proPointer runP;

  do{
    //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
    if(cjobQ[cjQ_front+1]->arrival == nowTime){
      inP = poll_cjobQ();
      if(runP != NULL && nowTime%tq == 0){
        add_clonereadyQ(runP);
        runP = inP;
      }else{
        add_clonereadyQ(inP);
        mergesort(clonereadyQ, crQ_front+1, crQ_rear, 0);
      }
    }
    if(!isEmpty(crQ_front, crQ_rear)){
      runP = poll_clonereadyQ();
      printf("\n new process polled! p%d\n", runP->pid);
      printf("clone ready queue: ");
      for(int i = crQ_front+1; i <= crQ_rear; i++){
        printf("p%d ", clonereadyQ[i]->pid);
      }
      printf("\n");
    }

    do{
      //아직 아무 프로세스도 도착하지 않았을때
      //CPU에서 실행중인 프로세스가 없으면 bb를 출력한다.
      if(runP == NULL){
        printf("bb ");
        //다른 프로세스들 웨이팅 타임 더해준다.
        if(!isEmpty(crQ_front, crQ_rear))
          wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        if(!isEmpty(wQ_front, wQ_rear))
          waiting(nowTime, 0);
      }

      else{
        printf("p%d ", runP->pid);
        //해당 프로세스의 CPUburst_remain -1해준다.
        runP->CPUburst_remain--;

        //다른 프로세스들 웨이팅 타임 더해준다.
        wait(runP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime, 0);


        //실행 마치면 turnaroundTime 계산한다.
        if(runP->CPUburst_remain == 0){
          runP->turnaroundTime = nowTime - runP->arrival + 1;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(runP->CPUburst == runP->CPUburst_remain+1){
          runP->responseTime = nowTime - runP->arrival;
        }


        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        for(int i = 0; i < num_IO; i++){
          if(ioQ[i]->pid == runP->pid){
            if(ioQ[i]->when == runP->CPUburst - runP->CPUburst_remain){
              runP->IOburst = ioQ[i]->IOburst;
              runP->IOburst_remain = ioQ[i]->IOburst;
              add_waitQ(runP);
              printf("waitP: p%d, IOburst remain: %d\n", runP->pid, runP->IOburst_remain);
              //IOburst_remain 순으로 정렬.
              mergesort(waitQ, wQ_front+1, wQ_rear, 1);
            if(!isEmpty(crQ_front, crQ_rear)){
              runP = poll_clonereadyQ();
              printf("after waitQ process: p%d\n", runP->pid);
              printf("clone ready queue: ");
              for(int i = crQ_front+1; i <= crQ_rear; i++){
                printf("p%d ", clonereadyQ[i]->pid);
              }
              printf("\n");
            }else{
              printf("next is blank\n");
              runP = NULL;
              printf("NULL\n");
            }
            break;
            }
          }
        }

      }/////else
      nowTime++;

      //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
      if(cjobQ[cjQ_front+1]->arrival == nowTime){
        inP = poll_cjobQ();
        if(runP != NULL &&  nowTime%tq == 0){
          add_clonereadyQ(runP);
          runP = inP;
        }else{
          add_clonereadyQ(inP);
          mergesort(clonereadyQ, crQ_front+1, crQ_rear, 0);
        }
      }
    }while(runP == NULL || runP->CPUburst_remain > 0);
    wT[runP->pid - 1] = runP->waitingTime;
    tT[runP->pid - 1] = runP->turnaroundTime;
    rT[runP->pid - 1] = runP->responseTime;
    runP = NULL;
  }while(!isEmpty(crQ_front, crQ_rear) || !isEmpty(wQ_front, wQ_rear));
  printf("\n");

  evaluation(wT, tT, rT, 5);
}



int main(int argc, char **argv){
  int num_process, num_IO, tq;

  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Type IO number: ");
  scanf("%d", &num_IO);
  printf("Time quantum: ");
  scanf("%d", &tq);

  create_processes(num_process, num_IO);
  //job2ready();
  FCFS_alg(num_process, num_IO);
  //SJF_alg(num_IO);
  //PRI_alg(num_IO);
  //PRESJF_alg(num_IO);
  //PREPRI_alg(num_IO);
  //RR_alg(num_IO, tq);


  return 0;
}
