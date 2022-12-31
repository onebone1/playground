#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define THREAD_NUM 4
#define TASK_NUM 10
#define DATA_NUM 10

int taskCount = 0;
int finished = 0;
int taskQueue[TASK_NUM];
int data[DATA_NUM];

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void executeTask(int index) {
  usleep(5000);
  pthread_mutex_lock(&mutexQueue);
  if (index == 0 || index == 1) {
    data[index] = 1;
  } else {
    data[index] = data[index - 1] + data[index - 2];
  }
  printf("Fib(%d): %d\n", index, data[index]);
  pthread_mutex_unlock(&mutexQueue);
}

void submitTask(int i) {
  pthread_mutex_lock(&mutexQueue);
  taskQueue[taskCount] = i;
  taskCount++;
  pthread_mutex_unlock(&mutexQueue);
  pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
  while (1) {
    if (taskCount == 0 && finished) break;
    pthread_mutex_lock(&mutexQueue);
    while (taskCount == 0) {
      pthread_cond_wait(&condQueue, &mutexQueue);
    }
    int index = taskQueue[0];
    int i;
    for (i = 0; i < taskCount - 1; i++) {
      taskQueue[i] = taskQueue[i + 1];
    }
    taskCount--;
    pthread_mutex_unlock(&mutexQueue);
    executeTask(index);
  }
  pthread_exit(NULL);
}

int main() {
  pthread_t th[THREAD_NUM];
  pthread_mutex_init(&mutexQueue, NULL);
  pthread_cond_init(&condQueue, NULL);
  int i;
  for (i = 0; i < THREAD_NUM; i++) {
    if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
      perror("Failed to create the thread");
    }
  }

  srand(time(NULL));
  for (i = 0; i < TASK_NUM; i++) {
    submitTask(i);
    if (i == TASK_NUM - 1) {
      pthread_mutex_lock(&mutexQueue);
      finished = 1;
      pthread_mutex_unlock(&mutexQueue);
    }
  }

  for (i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(th[i], NULL) != 0) {
      perror("Failed to join the thread");
    }
  }
  pthread_mutex_destroy(&mutexQueue);
  pthread_cond_destroy(&condQueue);
  return 0;
}
