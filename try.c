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

void init_Q(queue Q){
  Q.front = -1;
  Q.rear = -1;
  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    Q.q[i] = NULL;
  }
  return Q;
}

void add_Q(queue Q, proPointer newP){
  if(Q.rear == MAX_PROCESS_NUM - 1){
    printf("Q is full");
  }
  else{
    Q.q[++Q.rear] = newP;
  }
}

proPointer poll_Q(queue Q){
  if(Q.front == Q.rear){
    printf("Q is empty");
    return;
  }
  else{
    return Q.q[++Q.front];
  }
}

queue reset_Q(queue Q, int num_process){
  Q.front = -1;
  Q.rear = num_process-1;
  return Q;
}

queue job;
queue ready;
queue wait;
IOPointer ioQ[MAX_IO_NUM];

void create_processes(int num_process, int num_IO){
  //난수 생성
  srand( (unsigned)time(NULL) );

  //job queue 전역으로 초기화
  init_Q(job);

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
    add_Q(job, newP);
  }
  for(int j = 0; j < num_IO; j++){
    IOPointer newIO = (IOPointer)malloc(sizeof(struct IO));
      newIO->pid = rand() % num_process + 1;
      newIO->IOburst = rand() % 10 + 1; //IO burst time 1~10
      // 1 <= when < CPUburst 이어야한다.
      newIO->when = rand() % (job.q[newIO->pid - 1]->CPUburst - 1) + 1;
      //중복제거의 노력
     for(int i = 1; i < j; i++){
       if(newIO->pid == ioQ[i]->pid && newIO->when == ioQ[i]->when){
         newIO->pid = (newIO->pid + 1) % num_process + 1;
       }
     }

     ioQ[j] = newIO;
      printf("pid: %d, IOburst: %d, when %d\n", newIO->pid, newIO->IOburst, newIO->when);
     }
      printf("IO assigned\n");
}

queue cloneQ(queue oldQ, queue newQ){
  newQ = init_Q(newQ);
  for(int i = oldQ.front+1; i <= oldQ.rear; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = oldQ.q[i]->pid;
    newP->CPUburst = oldQ.q[i]->CPUburst;
    newP->IOburst = oldQ.q[i]->IOburst;
    newP->arrival = oldQ.q[i]->arrival;
    newP->priority = oldQ.q[i]->priority;
    newP->CPUburst_remain = oldQ.q[i]->CPUburst_remain;
    newP->IOburst_remain = oldQ.q[i]->IOburst_remain;
    newP->waitingTime = oldQ.q[i]->waitingTime;
    newP->turnaroundTime = oldQ.q[i]->turnaroundTime;
    newP->responseTime = oldQ.q[i]->responseTime;

    //job queue에 넣어준다. 순서는 pid 오름차순.
    add_Q(newQ, newP);
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

//한 프로세스를 실행하는 동안 다른 프로세스들의 waiting time을 +1 해주는 함수
void waitT(int pid){
  for(int i = ready.front+1; i <= ready.rear; i++){
    if(ready.q[i]->pid != pid){
      ready.q[i]->waitingTime++;
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
  if(!isEmpty(wait.front, wait.rear)){
    for(int i = wait.front + 1; i <= wait.rear; i++){
      wait.q[i]->IOburst_remain--;
      if(wait.q[i]->IOburst_remain == 0){
        proPointer newP = poll_Q(wait);
         //printf("waiting exit: p%d, CPU remain: %d\n", newP->pid, newP->CPUburst_remain);
        //waiting queue는 남아있는 IOburst time 오름차순으로 정렬한다.
        mergesort(wait.q, wait.front+1, wait.rear, 1);
        add_Q(ready, newP);
        // printf("clone ready queue: ");
        // for(int i = crQ_front+1; i <= crQ_rear; i++){
        //   printf("p%d ", clonereadyQ[i]->pid);
        // }
        // printf("\n");
        //해당 우선순위에 부합하게 오름차순 정렬한다.
        //arrival time은 넣은 그대로가 순서가 되므로 따로 정렬해주지 않는다.
        if(type == 0) return;
        else
          mergesort(ready.q, ready.front+1, ready.rear, type);
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
  int num = ready.rear - ready.front;
  int sumwT = 0;
  int sumtT = 0;
  int sumrT = 0;
  double avgwT, avgtT, avgrT;
  for(int i = 0; i < ready.rear - ready.front; i++){
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
void FCFS_alg(int num_IO){
  printf("\n******************start FCFS algorithm:*********************\n");
  //jobQ를 arrival time 오름차순으로 정렬
  mergesort(job.q, job.front+1, job.rear, 0);
  printQ(job);
  //readyQ 초기화
  init_Q(ready);
//wait queue 초기화
  init_Q(wait);

  int wT[ready.rear - ready.front];
  int tT[ready.rear - ready.front];
  int rT[ready.rear - ready.front];
  //현재 시간 나타내는 변수
  int nowTime = 0;

  //레디큐는 도착시간 순으로 정렬되어있다.
  proPointer inP = NULL;
  proPointer newP = NULL;

  do{
    if(!isEmpty(job.front, job.rear)){
      if(job.q[job.front+1]->arrival == nowTime){
        inP = job.q[++job.front];
        add_Q(ready, inP);
      }
    }
    if(!isEmpty(ready.front, ready.rear)){
      newP = poll_Q(ready);
    }

    do{
      //프로세스 도착 안함.
      if(newP == NULL && isEmpty(ready.front, ready.rear) && isEmpty(wait.front, wait.rear)){
        //printf("bb ");
      }
      //레디큐 비어있고, 프로세스 다 웨이팅큐에 있음.
      else if(newP == NULL && isEmpty(ready.front, ready.rear) && !isEmpty(wait.front, wait.rear)){
        //printf("bb ");
        waiting(nowTime, -1);
      }
      else{
        printf("p%d ", newP->pid);
        //해당 프로세스의 CPUburst_remain -1해준다.
        newP->CPUburst_remain--;

        //다른 프로세스들 웨이팅 타임 더해준다.
        waitT(newP->pid);
        //웨이팅 큐에서 기다리는 프로세스들 IOburst_remain 업데이트.
        waiting(nowTime, 0);


        //실행 마치면 turnaroundTime 계산한다.
        if(newP->CPUburst_remain == 0){
          newP->turnaroundTime = nowTime - newP->arrival + 1;
        }
        //처음 response 했을때까지 레디큐에서 기다린 시간.
        if(newP->CPUburst == newP->CPUburst_remain+1){
          newP->responseTime = nowTime - newP->arrival;
        }


        //현재 시간이 IO가 일어나야 한다면 waitQ에 해당 프로세스를 넣는다.
        for(int i = 0; i < num_IO; i++){
          if(ioQ[i]->pid == newP->pid){
            if(ioQ[i]->when == newP->CPUburst - newP->CPUburst_remain){
              newP->IOburst = ioQ[i]->IOburst;
              newP->IOburst_remain = ioQ[i]->IOburst;
              add_Q(wait, newP);
              //printf("waitP: p%d, IOburst remain: %d\n", newP->pid, newP->IOburst_remain);
              //IOburst_remain 순으로 정렬.
              mergesort(wait.q, wait.front+1, wait.rear, 1);
            if(!isEmpty(ready.front, ready.rear)){
              newP = poll_Q(ready);
              //printf("after waitQ process: p%d\n", newP->pid);
              //printf("clone ready queue: ");
              // for(int i = crQ_front+1; i <= crQ_rear; i++){
              //   printf("p%d ", clonereadyQ[i]->pid);
              // }
              // printf("\n");
            }else{
              //printf("next is blank\n");
              newP = NULL;
              //printf("NULL\n");
            }
            }
          }
          break;
        }

      }/////else
      nowTime++;
    }while(newP == NULL || newP->CPUburst_remain > 0);
    wT[newP->pid - 1] = newP->waitingTime;
    tT[newP->pid - 1] = newP->turnaroundTime;
    rT[newP->pid - 1] = newP->responseTime;
    newP = NULL;
  }while(!isEmpty(ready.front, ready.rear) || !isEmpty(wait.front, wait.rear));
  printf("\n");
  evaluation(wT, tT, rT, 0);
}


int main(int argc, char **argv){
  int num_process, num_IO;
  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Type IO number: ");
  scanf("%d", &num_IO);

  create_processes(num_process, num_IO);
  printQ(job);
  //FCFS_alg(num_IO);

}
