#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 100
#define INF 1000000

//arrival time: 0, IOburst_remain: 1, CPUburst_remain: 2, priority: 3, pid: 4
#define ARRIVAL 0
#define IOREMAIN 1
#define CPUREMAIN 2
#define PRIORITY 3
#define PID 4

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

//ready queue 배열 선언
proPointer readyQ[MAX_PROCESS_NUM];
int rQ_front, rQ_rear;

//waiting queue 배열 선언
proPointer waitQ[MAX_PROCESS_NUM];
int wQ_front, wQ_rear;

proPointer termQ[MAX_PROCESS_NUM];
int tQ_front, tQ_rear;

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

void reset_jobQ(int num_process){
  jQ_front = -1;
  jQ_rear = num_process-1;
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

//termination queue 초기화
void init_termQ(){
  tQ_front = -1;
  tQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    termQ[i] = NULL;
  }
}
//termination queue enqueue
void add_termQ(proPointer newP){
  if(tQ_rear == MAX_PROCESS_NUM - 1)
    printf("termQ is FULL");
  else
    termQ[++tQ_rear] = newP;
}
//termination queue dequeue
proPointer poll_termQ(){
  if(tQ_front == tQ_rear)
    printf("termQ is EMPTY");
  else
    return termQ[++tQ_front];
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
  for(int i = rQ_front+1; i <= rQ_rear; i++){
    printf("p%d ", readyQ[i]->pid);
    printf("CPUburst %d, ", readyQ[i]->CPUburst);
    printf("arrival %d, ", readyQ[i]->arrival);
    printf("priority %d\n", readyQ[i]->priority);
  }
  printf("\n");
}


//arrival time을 기준으로 정렬해서 ready queue에 넣어준다.
//type는 arrival time으로 정렬하는 것인지, IOburst_remain으로 정렬하는지 결정한다.
//arrival time: 0, IOburst_remain: 1, CPUburst_remain: 2, priority: 3, pid: 4
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
    case ARRIVAL:
      dummy1 -> arrival = INF;
      break;
    case IOREMAIN:
      dummy1 -> IOburst_remain = INF;
      break;
    case CPUREMAIN:
      dummy1 -> CPUburst_remain = INF;
      dummy1 -> arrival = INF;
      dummy1 -> pid = INF;
      break;
    case PRIORITY:
      dummy1 -> priority = INF;
      dummy1 -> CPUburst_remain = INF;
      dummy1 -> arrival = INF;
      dummy1 -> pid = INF;
      break;
    case PID:
      dummy1 -> pid = INF;
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
    case ARRIVAL:
      dummy2 -> arrival = INF;
      break;
    case IOREMAIN:
      dummy2 -> IOburst_remain = INF;
      break;
    case CPUREMAIN:
      dummy2 -> CPUburst_remain = INF;
      dummy2 -> arrival = INF;
      dummy2 -> pid = INF;
      break;
    case PRIORITY:
      dummy2 -> priority = INF;
      dummy2 -> CPUburst_remain = INF;
      dummy2 -> arrival = INF;
      dummy2 -> pid = INF;
      break;
    case PID:
      dummy2 -> pid = INF;
      break;
    default:
      printf("merge: unknown type received\n");
      break;
  }
  R[n2] = dummy2;

  i = 0; j = 0;
  for(int k = p; k <= r; k++){
    switch(type){
      case ARRIVAL:
      if(L[i]->arrival <= R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case IOREMAIN:
      if(L[i]->IOburst_remain <= R[j]->IOburst_remain){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case CPUREMAIN:
      if(L[i]->CPUburst_remain < R[j]->CPUburst_remain){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->CPUburst_remain == R[j]->CPUburst_remain && L[i]->arrival < R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->CPUburst_remain == R[j]->CPUburst_remain && L[i]->arrival == R[j]->arrival && L[i]->pid < R[j]->pid){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case PRIORITY:
      if(L[i]->priority < R[j]->priority){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->priority == R[j]->priority && L[i]->CPUburst_remain < R[j]->CPUburst_remain){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->priority == R[j]->priority && L[i]->CPUburst_remain == R[j]->CPUburst_remain && L[i]->arrival < R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->priority == R[j]->priority && L[i]->CPUburst_remain == R[j]->CPUburst_remain && L[i]->arrival == R[j]->arrival && L[i]->pid < R[j]->pid){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case PID:
      if(L[i]->pid <= R[j]->pid){
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
    case ARRIVAL:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, ARRIVAL);
      mergesort(list, q+1, r, ARRIVAL);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, ARRIVAL);
    }
    break;

    case IOREMAIN:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, IOREMAIN);
      mergesort(list, q+1, r, IOREMAIN);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, IOREMAIN);
    }
    break;

    case CPUREMAIN:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, CPUREMAIN);
      mergesort(list, q+1, r, CPUREMAIN);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, CPUREMAIN);
    }
    break;

    case PRIORITY:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, PRIORITY);
      mergesort(list, q+1, r, PRIORITY);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, PRIORITY);
    }
    break;

    case PID:
    if(p < r){
  //	  printf("p: %d, r: %d\n", p, r);
      int q = (p+r)/2;
      mergesort(list, p, q, PID);
      mergesort(list, q+1, r, PID);
    //  printf("merge %d-%d and %d-%d\n", p, q, q+1, r);
      merge(list, p, q, r, PID);
    }
    break;

    default:
    printf("mergesort: unknown type\n");
    break;
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
void create_processes(int num_process){

	//난수 생성
  srand( (unsigned)time(NULL) );

  //job queue 초기화
  init_jobQ();

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
  void waiting(int type){
    if(!isEmpty(wQ_front, wQ_rear)){
      for(int i = wQ_front + 1; i <= wQ_rear; i++){
        waitQ[i]->IOburst_remain--;
        if(waitQ[i]->IOburst_remain == 0){
          printf("\n<IO finished> p%d CPUburst_remain: %d\n", waitQ[i]->pid, waitQ[i]->CPUburst_remain);
          add_readyQ(poll_waitQ());
          //waiting queue는 남아있는 IOburst time 오름차순으로 정렬한다.
          mergesort(waitQ, wQ_front+1, wQ_rear, 1);
          //readyQ는 해당 우선순위에 부합하게 오름차순 정렬한다.
          //arrival time은 넣은 그대로가 순서가 되므로 따로 정렬해주지 않는다.
          if(type == ARRIVAL) return;
          else
            mergesort(readyQ, rQ_front+1, rQ_rear, type);
        }
      }
    }
  }

  void evaluation(){
    //evaluation
    //termination queue를 pid순으로 정렬
    mergesort(termQ, tQ_front+1, tQ_rear, 4);
    int num = tQ_rear - tQ_front;
    int sumwT = 0;
    int sumtT = 0;
    int sumrT = 0;
    double avgwT = 0;
    double avgtT = 0;
    double avgrT = 0;
    for(int i = tQ_front+1; i <= tQ_rear ; i++){
      printf("pid: %d, waiting time: %d, turnaround time: %d, response time: %d\n",
        termQ[i]->pid, termQ[i]->waitingTime, termQ[i]->turnaroundTime, termQ[i]->responseTime);
        sumwT += termQ[i]->waitingTime;
        sumtT += termQ[i]->turnaroundTime;
        sumrT += termQ[i]->responseTime;
    }
    avgwT = (double)sumwT/num;
    avgtT = (double)sumtT/num;
    avgrT = (double)sumrT/num;
    printf("avgwT: %f, avgtT: %f, avgrT: %f\n", avgwT, avgtT, avgrT);
  }

  proPointer preempt(proPointer runP, int type){
    proPointer next;
    switch(type){
      case CPUREMAIN:
      if(readyQ[rQ_front+1]->CPUburst_remain < runP->CPUburst_remain){
        next = poll_readyQ();
      }
      else if(readyQ[rQ_front+1]->CPUburst_remain == runP->CPUburst_remain && readyQ[rQ_front+1]->arrival < runP->arrival){
        next = poll_readyQ();
      }
      else if(readyQ[rQ_front+1]->CPUburst_remain == runP->CPUburst_remain && readyQ[rQ_front+1]->arrival == runP->arrival && readyQ[rQ_front+1]->pid < runP->pid){
        next = poll_readyQ();
      }
      else{
        next = runP;
      }
      break;

      case PRIORITY:
      if(readyQ[rQ_front+1]->priority < runP->priority){
        next = poll_readyQ();
      }
      else if(readyQ[rQ_front+1]->priority == runP->priority && readyQ[rQ_front+1]->CPUburst_remain < runP->CPUburst_remain){
        next = poll_readyQ();
      }
      else if(readyQ[rQ_front+1]->priority == runP->priority && readyQ[rQ_front+1]->CPUburst_remain == runP->CPUburst_remain && readyQ[rQ_front+1]->arrival < runP->arrival){
        next = poll_readyQ();
      }
      else if(readyQ[rQ_front+1]->priority == runP->priority && readyQ[rQ_front+1]->CPUburst_remain == runP->CPUburst_remain && readyQ[rQ_front+1]->arrival == runP->arrival && readyQ[rQ_front+1]->pid < runP->pid){
        next = poll_readyQ();
      }
      else{
        next = runP;
      }
      break;
      default:
        printf("<ERROR> preemption type wrong");
      break;
    }
    if(next != runP){
      printf("\npreemtpion from p%d ", runP->pid);
      add_readyQ(runP);
      printf("to p%d\n", next->pid);
    }
    return next;
  }

//선입선출
void FCFS_alg(int num_process){
  printf("\n********************start FCFS algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );
  //jobQ arrival time 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();
  //ready, wait initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){
    if(isEmpty(jQ_front, jQ_rear) != 1){
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
          add_readyQ(poll_jobQ());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      waiting(ARRIVAL);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      wait(runP->pid);
      waiting(ARRIVAL);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;
      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }
    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d, CPUburst_remain: %d\n", runP->pid, runP->IOburst, runP->CPUburst_remain);
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1) runP = poll_readyQ();
      else runP = NULL;
    }
    }/////else
  }/////for process
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  printf("\n");
  evaluation();
}/////FCFS_alg


//shortest job first!! non-preemptive.
void SJF_alg(int num_process){
  printf("\n********************start SJF algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );
  //jobQ arrival 정렬
  reset_jobQ(num_process);
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();
  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){

    if(isEmpty(jQ_front, jQ_rear) != 1){
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
          add_readyQ(poll_jobQ());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      //CPUburst_remain 낮은 순서대로 정렬. 우선순위 같으면 arrival time 순서. arrival time도 같으면 pid순서.
      mergesort(readyQ, rQ_front+1, rQ_rear, CPUREMAIN);
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      waiting(CPUREMAIN);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      wait(runP->pid);
      waiting(CPUREMAIN);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;
      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }
    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d\n", runP->pid, runP->IOburst);
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1) runP = poll_readyQ();
      else runP = NULL;
    }
    }/////else
  }/////for process
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  printf("\n");
  evaluation();
  reset_jobQ(num_process);
}/////SJF_alg


//Priority!! non-preemptive.
void PRI_alg(int num_process){
  printf("\n********************start Priority algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );
  //jobQ arrival 정렬
  reset_jobQ(num_process);
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();
  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){

    if(isEmpty(jQ_front, jQ_rear) != 1){
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
          add_readyQ(poll_jobQ());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      //우선순위 높은 순서대로 정렬. 우선순위 같으면 arrival time 순서. arrival time도 같으면 pid순서.
      //숫자 작을수록 우선순위 높다.
      mergesort(readyQ, rQ_front+1, rQ_rear, PRIORITY);
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      waiting(PRIORITY);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      wait(runP->pid);
      waiting(PRIORITY);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;
      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }
    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d\n", runP->pid, runP->IOburst);
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1) runP = poll_readyQ();
      else runP = NULL;
    }
    }/////else
  }/////for process
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  printf("\n");
  evaluation();
  reset_jobQ(num_process);
}/////PRI_alg

//preemption 있는 SJF 알고리즘.
//CPU_remain이 가장 작은 것부터 실행
void PRESJF_alg(int num_process){
  printf("\n********************start preemptive SJF algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );
  //jobQ arrival 정렬
  reset_jobQ(num_process);
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);

  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){

    if(isEmpty(jQ_front, jQ_rear) != 1){
      //해당 시간에 도착한 프로세스 모두 레디큐로 옮겨줌.
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime){
          add_readyQ(poll_jobQ());
        }//////if process arrived.
      }////for every process in jobQ
      //preemption
      //레디큐를 CPUburst_remain 순으로 정렬한 후, 현재 실행중인 프로세스와 비교해서 CPUburst_remain이 작으면 프로세스를 바꿔준다.
      //tie breaking은 arrival, pid순서.
      if(isEmpty(rQ_front, rQ_rear) != 1 && runP != NULL){
        mergesort(readyQ, rQ_front+1, rQ_rear, CPUREMAIN);
        if(readyQ[rQ_front+1]->CPUburst_remain < runP->CPUburst_remain){
          preempt(runP, CPUREMAIN);
        }
      }/////if readyQ is not empty and runP is not null
    }////if jobQ is not empty

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      //CPUburst_remain 낮은 순서대로 정렬. 우선순위 같으면 arrival time 순서. arrival time도 같으면 pid순서.
      mergesort(readyQ, rQ_front+1, rQ_rear, CPUREMAIN);
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      waiting(CPUREMAIN);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      wait(runP->pid);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;
      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }
      //preemption
      //waiting 종료 후에 레디큐를 CPUburst_remain 순으로 정렬한 후,
      //tie breaking은 arrival, pid순서.
      waiting(CPUREMAIN);
      mergesort(readyQ, rQ_front+1, rQ_rear, CPUREMAIN);
      if(runP != NULL && isEmpty(rQ_front, rQ_rear) != 1 && readyQ[rQ_front+1]->CPUburst_remain < runP->CPUburst_remain){
        preempt(runP, CPUREMAIN);
      }


    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d\n", runP->pid, runP->IOburst);
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1) runP = poll_readyQ();
      else runP = NULL;
    }
    }/////else
  }/////for process
  printf("\n");
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  evaluation();
  reset_jobQ(num_process);
}/////PRESJF_alg//

//preemption 있는 Priority 알고리즘.
//우선순위 번호가 가장 작은 것부터 실행(숫자가 작을수록 우선순위 높음)
void PREPRI_alg(int num_process){
  printf("\n********************start preemptive PRIORITY algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );
  //jobQ arrival 정렬
  reset_jobQ(num_process);
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);

  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){

    if(isEmpty(jQ_front, jQ_rear) != 1){
      //해당 시간에 도착한 프로세스 모두 레디큐로 옮겨줌.
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
          add_readyQ(poll_jobQ());
      }
      //레디큐를 priority 순으로 정렬한 후, 현재 실행중인 프로세스와 비교해서 priority이 작으면 프로세스를 바꿔준다.
      if(isEmpty(rQ_front, rQ_rear) != 1 && runP != NULL){
        mergesort(readyQ, rQ_front+1, rQ_rear, PRIORITY);
        if(readyQ[rQ_front+1]->priority < runP->priority){
          runP = preempt(runP, PRIORITY);
        }
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      //priority 낮은 순서대로 정렬. tie breaking은 CPUburst_remain, arrival, pid 순서.
      mergesort(readyQ, rQ_front+1, rQ_rear, PRIORITY);
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      waiting(PRIORITY);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      wait(runP->pid);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;
      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }
      //waiting 종료 후에 레디큐를 CPUburst_remain 순으로 정렬한 후,
      //priority 낮은 순서대로 정렬. tie breaking은 CPUburst_remain, arrival, pid 순서.
      waiting(PRIORITY);
      mergesort(readyQ, rQ_front+1, rQ_rear, PRIORITY);
      if(runP != NULL && isEmpty(rQ_front, rQ_rear) != 1){
        runP = preempt(runP, PRIORITY);
      }


    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d\n", runP->pid, runP->IOburst);
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1) runP = poll_readyQ();
      else runP = NULL;
    }
    }/////else
  }/////for process
  printf("\n");
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  evaluation();
  reset_jobQ(num_process);
}/////PREPRI_alg//

// //Round Robin 알고리즘.
// //arrival이 가장 작은 것부터 실행
// //time quantum = 3(default);
// void RR_alg(int tq){
//   printf("\n************start Round Robin algorithm****************\n");
//
//   //job queue는 arrival 오름차순으로 정렬
//   clone_jobQ();
//   mergesort(cjobQ, cjQ_front+1, cjQ_rear, 0);
//
// //레디큐 initialize.
//   init_clonereadyQ();
//
//   //wait queue 초기화
//   init_waitQ();
//
//   int wT[rQ_rear - rQ_front];
//   int tT[rQ_rear - rQ_front];
//   int rT[rQ_rear - rQ_front];
//   //현재 시간 나타내는 변수
//   int nowTime = 0;
//
//   proPointer inP;
//   proPointer runP;
//
//   do{
//     //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
//     if(cjobQ[cjQ_front+1]->arrival == nowTime){
//       inP = poll_cjobQ();
//       if(runP != NULL && nowTime%tq == 0){
//         add_clonereadyQ(runP);
//         runP = inP;
//       }else{
//         add_clonereadyQ(inP);
//         mergesort(clonereadyQ, crQ_front+1, crQ_rear, 0);
//       }
//     }
//     if(!isEmpty(crQ_front, crQ_rear)){
//       runP = poll_clonereadyQ();
//       printf("\n new process polled! p%d\n", runP->pid);
//       printf("clone ready queue: ");
//       for(int i = crQ_front+1; i <= crQ_rear; i++){
//         printf("p%d ", clonereadyQ[i]->pid);
//       }
//       printf("\n");
//     }
//
//     do{
//       //아직 아무 프로세스도 도착하지 않았을때
//       //CPU에서 실행중인 프로세스가 없으면 bb를 출력한다.
//       if(runP == NULL){
//         printf("bb ");
//         //다른 프로세스들 웨이팅 타임 더해준다.
//         if(!isEmpty(crQ_front, crQ_rear))
//           wait(runP->pid);
//         //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
//         if(!isEmpty(wQ_front, wQ_rear))
//           waiting(0);
//       }
//
//       else{
//         printf("p%d ", runP->pid);
//         //해당 프로세스의 CPUburst_remain -1해준다.
//         runP->CPUburst_remain--;
//
//         //다른 프로세스들 웨이팅 타임 더해준다.
//         wait(runP->pid);
//         //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
//         waiting(0);
//
//
//         //실행 마치면 turnaroundTime 계산한다.
//         if(runP->CPUburst_remain == 0){
//           runP->turnaroundTime = nowTime - runP->arrival + 1;
//         }
//         //처음 response 했을때까지 레디큐에서 기다린 시간.
//         if(runP->CPUburst == runP->CPUburst_remain+1){
//           runP->responseTime = nowTime - runP->arrival;
//         }
//
//
//
//
//       }/////else
//       nowTime++;
//
//       //지금 들어온 프로세스의 remain time이 현재 수행중인 프로세스의 remain time보다 작으면 preempt.
//       if(cjobQ[cjQ_front+1]->arrival == nowTime){
//         inP = poll_cjobQ();
//         if(runP != NULL &&  nowTime%tq == 0){
//           add_clonereadyQ(runP);
//           runP = inP;
//         }else{
//           add_clonereadyQ(inP);
//           mergesort(clonereadyQ, crQ_front+1, crQ_rear, 0);
//         }
//       }
//     }while(runP == NULL || runP->CPUburst_remain > 0);
//     wT[runP->pid - 1] = runP->waitingTime;
//     tT[runP->pid - 1] = runP->turnaroundTime;
//     rT[runP->pid - 1] = runP->responseTime;
//     runP = NULL;
//   }while(!isEmpty(crQ_front, crQ_rear) || !isEmpty(wQ_front, wQ_rear));
//   printf("\n");
//
//   evaluation(wT, tT, rT);
// }



int main(int argc, char **argv){
  int num_process, tq;

  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Time quantum: ");
  scanf("%d", &tq);

  create_processes(num_process);
  //FCFS_alg(num_process);
  //SJF_alg(num_process);
  //PRI_alg(num_process);
  PRESJF_alg(num_process);
  PREPRI_alg(num_process);
  //RR_alg(num_IO, tq);


  return 0;
}
