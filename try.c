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
    newQ = add_Q(newQ, newP);
  }
  printQ(newQ);
}

//arrival time을 기준으로 정렬해서 ready queue에 넣어준다.
//type는 arrival time으로 정렬하는 것인지, IOburst_remain으로 정렬하는지 결정한다.
//arrival time: 0, IOburst_remain: 1, CPUburst_remain: 2, priority: 3
proPointer* merge(proPointer list[], int p, int q, int r, int type){
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
  return list;
}//end merge

proPointer* mergesort(proPointer list[], int p, int r, int type){
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
  return list;
}


int main(int argc, char **argv){
  int num_process, num_IO;
  printf("Welcome to CPU Scheduling Simulator!\n");
  printf("Type process number: ");
  scanf("%d", &num_process);
  printf("Type IO number: ");
  scanf("%d", &num_IO);

  create_processes(num_process, num_IO);
  queue clone_jobQ;
  clone_jobQ = cloneQ(job_global, clone_jobQ);
  clone_jobQ = mergesort(clone_jobQ.q, clone_jobQ.front+1, clone_jobQ.rear, 0);
  printQ(clone_jobQ);
  clone_jobQ = mergesort(clone_jobQ.q, clone_jobQ.front+1, clone_jobQ.rear, 2);
  printQ(clone_jobQ);
  clone_jobQ = mergesort(clone_jobQ.q, clone_jobQ.front+1, clone_jobQ.rear, 3);
  printQ(clone_jobQ);
}
