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

#define FCFS 0
#define SJF 1
#define PRI 2
#define PRESJF 3
#define PREPRI 4
#define RR 5
#define MULTI 6

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
  int timequantum;
  int waitingTime;
  int turnaroundTime;
  int responseTime;

}process;

//처음에 받은 프로세스 정보.
proPointer GjobQ[MAX_PROCESS_NUM];
int GjQ_front, GjQ_rear;

//ready queue 배열 선언
proPointer readyQ[MAX_PROCESS_NUM];
int rQ_front, rQ_rear;

//waiting queue 배열 선언
proPointer waitQ[MAX_PROCESS_NUM];
int wQ_front, wQ_rear;

proPointer termQ[MAX_PROCESS_NUM];
int tQ_front, tQ_rear;

//각 스케줄링 알고리즘마다 이용할 jobQ.
proPointer jobQ[MAX_PROCESS_NUM];
int jQ_front, jQ_rear;

proPointer ready2Q[MAX_PROCESS_NUM];
int r2Q_front, r2Q_rear;

double wT[7] = {0};
double tT[7] = {0};
double rT[7] = {0};
double util[7] = {0};


//job queue 초기화
void init_GjobQ(){
  GjQ_front = -1;
  GjQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    GjobQ[i] = NULL;
  }
}
//job queue enqueue
void add_GjobQ(proPointer newP){
  if(GjQ_rear == MAX_PROCESS_NUM - 1)
    printf("GjobQ is FULL");
  else
    GjobQ[++GjQ_rear] = newP;
}
//job queue dequeue
proPointer poll_GjobQ(){
  if(GjQ_front == GjQ_rear)
    printf("GjobQ is EMPTY");
  else
    return GjobQ[++GjQ_front];
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
void printQ_Gjob(){
  for(int i = 0; i < (GjQ_rear - GjQ_front); i++){
    printf("p%d , ", GjobQ[i]->pid);
    printf("CPUburst %d, ", GjobQ[i]->CPUburst);
    printf("arrival %d, ", GjobQ[i]->arrival);
    printf("priority %d\n", GjobQ[i]->priority);
  }
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

//background ready queue 초기화
void init_ready2Q(){
  r2Q_front = -1;
  r2Q_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    ready2Q[i] = NULL;
  }
}
//background ready queue enqueue
void add_ready2Q(proPointer newP){
  if(r2Q_rear == MAX_PROCESS_NUM - 1)
    printf("ready2Q is FULL");
  else
    ready2Q[++r2Q_rear] = newP;
}
//background ready queue dequeue
proPointer poll_ready2Q(){
  if(r2Q_front == r2Q_rear)
    printf("ready2Q is EMPTY");
  else
    return ready2Q[++r2Q_front];
}

void clone_jobQ(){
  for(int i = GjQ_front+1; i <= GjQ_rear; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = GjobQ[i]->pid;
    newP->CPUburst = GjobQ[i]->CPUburst;
    newP->arrival = GjobQ[i]->arrival;
    newP->priority = GjobQ[i]->priority;
    newP->CPUburst_remain = GjobQ[i]->CPUburst_remain;
    newP->timequantum = GjobQ[i]->timequantum;
    newP->waitingTime = GjobQ[i]->waitingTime;
    newP->turnaroundTime = GjobQ[i]->turnaroundTime;
    newP->responseTime = GjobQ[i]->responseTime;
    newP->IOburst = GjobQ[i]->IOburst;
    newP->IOburst_remain = GjobQ[i]->IOburst_remain;

    //job queue에 넣어준다. 순서는 pid 오름차순.
    add_jobQ(newP);
  }
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
void create_processes(int num_process, int tq){

	//난수 생성
  srand( (unsigned)time(NULL) );

  //job queue 초기화
  init_GjobQ();

  for(int i = 0; i < num_process; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = i+1;
    newP->CPUburst = rand() % 25 + 2; //CPU burst time 2 ~ 26
    newP->arrival = rand() % (num_process + 10);
    newP->priority = rand() % num_process + 1;
    newP->CPUburst_remain = newP -> CPUburst;
    newP->timequantum = tq;
    newP->waitingTime = 0;
    newP->turnaroundTime = 0;
    newP->responseTime = 0;
    newP->IOburst = 0;
    newP->IOburst_remain = 0;

    //job queue에 넣어준다. 순서는 pid 오름차순.
    add_GjobQ(newP);
  }
  printQ_Gjob();

  }
  //한 프로세스를 실행하는 동안 다른 프로세스들의 waiting time을 +1 해주는 함수
  void wait(int pid){
    for(int i = rQ_front+1; i <= rQ_rear; i++){
      if(readyQ[i]->pid != pid){
        readyQ[i]->waitingTime++;
      }
    }
      for(int i = r2Q_front+1; i <= r2Q_rear; i++){
        if(ready2Q[i]->pid != pid){
          ready2Q[i]->waitingTime++;
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

  //wait queue dequeue
  //해당 pid를 가진 걸 빼주고, 나머지는 땡겨준다.
  proPointer poll_waitQ(int pid){
    proPointer res;
    if(wQ_front == wQ_rear)
      printf("waitQ is EMPTY");
    else{
      for(int i = wQ_front+1; i <= wQ_rear; i++){
        if(waitQ[i]->pid == pid && i == wQ_rear){
          wQ_rear--;
          return waitQ[i];
        }
        else if(waitQ[i]->pid == pid){
          res = waitQ[i];
          for(int j = i+1; j <= wQ_rear; j++){
            waitQ[j-1] = waitQ[j];
          }
          wQ_rear--;
          return res;
        }
      }
    }
  }

//IO busrt 얼마나 했는지 매 타임  -1 해주고,
//IOburst_remain == 0 되면 내보내준다.
  void waiting(int type){
    if(!isEmpty(wQ_front, wQ_rear)){
      for(int i = wQ_front + 1; i <= wQ_rear; i++){
        waitQ[i]->IOburst_remain--;
        if(waitQ[i]->IOburst_remain == 0){
          printf("\n<IO finished> p%d CPUburst_remain: %d\n", waitQ[i]->pid, waitQ[i]->CPUburst_remain);
          add_readyQ(poll_waitQ(waitQ[i]->pid));
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

  void evaluation(int type){
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
    wT[type] = avgwT;
    tT[type] = avgtT;
    rT[type] = avgrT;
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

  void TotalEval(){
    double avg = 0;
    printf("\n*********************total evaluation*****************\n");
    for(int i = 0; i < 7; i++){
      switch (i) {
       case FCFS: printf("FCFS:                "); break;
        case SJF: printf("SJF:                 "); break;
        case PRI: printf("PRIORITY:            "); break;
     case PRESJF: printf("PREEMPTIVE SJF:      "); break;
     case PREPRI: printf("PREEMPTIVE PRIORITY: "); break;
         case RR: printf("RR:                  "); break;
      case MULTI: printf("MULTILEVEL QUEUE:    "); break;
        default: break;
      }
      avg = (wT[i]+tT[i]+rT[i])/3;
      printf("waiting Time: %f, turnaround Time: %f, response Time: %f, evaluation Time average: %f, CPU utilization: %f\n", wT[i], tT[i], rT[i], avg, util[i]);
    }
  }

//선입선출
void FCFS_alg(int num_process){
  printf("\n********************start FCFS algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival time 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();
  //ready, wait initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
  double cpuutil = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;
//모든 프로세스 끝날 때까지
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
    //cpu에서 실행중인 것 없음. 대기하는 프로세스도 없음.
    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
      idle++;
    }
    //cpu에서 실행중인 것 없음. 대기하는 프로세스 있음.
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
      waiting(ARRIVAL);
    }
    //cpu에서 실행중인 프로세스 있음.
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
  evaluation(FCFS);
  printf("%d", nowTime);
  cpuutil = (nowTime-idle)/nowTime;
  util[FCFS] = cpuutil;
  printf("CPU utilization: %f\n", cpuutil);

}/////FCFS_alg


//shortest job first!! non-preemptive.
void SJF_alg(int num_process){
  printf("\n********************start SJF algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();
  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
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
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
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
  evaluation(SJF);
  util[SJF] = (nowTime-idle)/nowTime;
  printf("CPU utilization: %f\n", util[SJF]);
}/////SJF_alg


//Priority!! non-preemptive.
void PRI_alg(int num_process){
  printf("\n********************start Priority algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();
  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
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
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
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
  evaluation(PRI);
  util[PRI] = (nowTime-idle)/nowTime;
  printf("CPU utilization: %f\n", util[PRI]);
}/////PRI_alg

//preemption 있는 SJF 알고리즘.
//CPU_remain이 가장 작은 것부터 실행
void PRESJF_alg(int num_process){
  printf("\n********************start preemptive SJF algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();

  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
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
          runP = preempt(runP, CPUREMAIN);
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
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
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
        runP = preempt(runP, CPUREMAIN);
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
  evaluation(PRESJF);
  util[PRESJF] = (nowTime-idle)/nowTime;
  printf("CPU utilization: %f\n", util[PRESJF]);
}/////PRESJF_alg//

//preemption 있는 Priority 알고리즘.
//우선순위 번호가 가장 작은 것부터 실행(숫자가 작을수록 우선순위 높음)
void PREPRI_alg(int num_process){
  printf("\n********************start preemptive PRIORITY algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();

  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
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
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
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
  evaluation(PREPRI);
  util[PREPRI] = (nowTime-idle)/nowTime;
  printf("CPU utilization: %f\n", util[PREPRI]);
}/////PREPRI_alg//

// //Round Robin 알고리즘.
// //arrival이 가장 작은 것부터 실행
// //time quantum = 3(default);
void RR_alg(int num_process, int tq){
  printf("\n********************start ROUND ROBIN algorithm********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();

  //ready, wait, termination initialize
  init_readyQ();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
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
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
      waiting(ARRIVAL);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      runP->timequantum--;
      wait(runP->pid);
      waiting(ARRIVAL);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;

      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }

      if(runP != NULL && runP->timequantum == 0){
        runP->timequantum = tq;
        runP->arrival = nowTime;
        add_readyQ(runP);
        runP = poll_readyQ();
      }

    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d\n", runP->pid, runP->IOburst);
      runP->timequantum = tq;
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1) runP = poll_readyQ();
      else runP = NULL;
    }

    }/////else
  }/////for process
  printf("\n");
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  evaluation(RR);
  util[RR] = (nowTime-idle)/nowTime;
  printf("CPU utilization: %f\n", util[RR]);
}/////RR_alg



void MULTI_Q(int num_process, int tq){
  printf("\n********************start Scheduling with MULTILEVEL QUEUE********************\n");
  //난수 생성
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  //jobQ arrival 정렬
  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  printQ_job();

  //ready, wait, termination initialize
  init_readyQ();
  init_ready2Q();
  init_waitQ();
  init_termQ();

  //현재 시간 나타내는 변수
  int nowTime = 0;
  int idle = 0;
  //몇개의 프로세스가 종료했는지 기록
  int check = 0;
  //cpu에 할당되는 프로세스
  proPointer runP = NULL;

  int whichQ = 0; //0: front, 1: background

  for(nowTime = 0; check < num_process; nowTime++){
    if(isEmpty(jQ_front, jQ_rear) != 1){
      //해당 시간에 도착한 프로세스 모두 레디큐로 옮겨줌.
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime && jobQ[i]->priority <= 0.5 * num_process)
          add_readyQ(poll_jobQ());
        else if(jobQ[i]->arrival == nowTime && jobQ[i]->priority > 0.5 * num_process)
          add_ready2Q(poll_jobQ());
      }
    }

    //background Q에서 너무 오래 기다린 프로세스는 front로 올려준다.
    if(isEmpty(r2Q_front, r2Q_rear) != 1){
      mergesort(ready2Q, r2Q_front+1, r2Q_rear, ARRIVAL);
      for(int i = r2Q_front+1; i <= r2Q_rear; i++){
        if(ready2Q[i]->waitingTime > 20)
          add_readyQ(poll_ready2Q());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      runP = poll_readyQ();
      whichQ = 0;
    }

    //forward Q가 다 비어야 background Q 실행가능
    if(isEmpty(rQ_front, rQ_rear)==1 && isEmpty(r2Q_front, r2Q_rear)!=1 && runP == NULL){
      runP = poll_ready2Q();
      whichQ = 1;
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("bb ");
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("bb ");
      idle++;
      waiting(ARRIVAL);
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->CPUburst_remain--;
      runP->timequantum--;
      wait(runP->pid);
      waiting(ARRIVAL);

      if(runP->CPUburst_remain+1 == runP->CPUburst) runP->responseTime = nowTime - runP->arrival;

      if(runP->CPUburst_remain == 0){
        runP->turnaroundTime = (nowTime+1) - runP->arrival;
        add_termQ(runP);
        check++;
        runP = NULL;
      }

      if(runP != NULL && runP->timequantum == 0){
        runP->timequantum = tq;
        (whichQ == 0)?add_readyQ(runP):add_ready2Q(runP);
        runP = NULL;
      }

    //random IO. 5% 확률로 IO 발생.
    if(runP != NULL && runP->CPUburst_remain > 0 && runP->CPUburst > runP->CPUburst_remain && rand() % 100 >= 95){
      runP->IOburst = rand() % 10 + 1; //IOburst는 1~10;
      runP->IOburst_remain = runP->IOburst;
      printf("\n<IO interrupt!>p%d, IOburst: %d\n", runP->pid, runP->IOburst);
      runP->timequantum = tq;
      add_waitQ(runP);
      mergesort(waitQ, wQ_front+1, wQ_rear, IOREMAIN);
      if(isEmpty(rQ_front, rQ_rear)!=1){
        runP = poll_readyQ();
        whichQ = 0;
      }
      else if(isEmpty(r2Q_front, r2Q_rear) != 1){
        runP = poll_ready2Q();
        whichQ = 1;
      }
      else runP = NULL;
    }
    }/////else
  }/////for process
  printf("\n");
  //내용물은 그대로. front, rear가 가리키는 인덱스만 초기상태로 바꿔줌.
  evaluation(MULTI);
  util[MULTI] = (nowTime-idle)/nowTime;
  printf("CPU utilization: %f\n", util[MULTI]);
}


int main(int argc, char **argv){
  int num_process, tq;

  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Time quantum: ");
  scanf("%d", &tq);

  create_processes(num_process, tq);
  FCFS_alg(num_process);
  SJF_alg(num_process);
  PRI_alg(num_process);
  PRESJF_alg(num_process);
  PREPRI_alg(num_process);
  RR_alg(num_process, tq);
  MULTI_Q(num_process, tq);
  TotalEval();

  return 0;
}
