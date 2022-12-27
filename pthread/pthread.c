#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define THREAD_NUM 4
#define TASK_NUM 100

typedef struct {
  int a, b;
} Task;

Task taskQueue[256];
int taskCount = 0;
int finished = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void executeTask(Task *task) {
  int result = task->a + task->b;
  printf("The sum of %d and %d is %d\n", task->a, task->b, result);
}

void submitTask(Task task) {
  pthread_mutex_lock(&mutexQueue);
  taskQueue[taskCount] = task;
  taskCount++;
  pthread_mutex_unlock(&mutexQueue);
  pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
  while (1) {
    Task task;

    if (taskCount == 0 && finished) break;
    pthread_mutex_lock(&mutexQueue);
    while (taskCount == 0) {
      pthread_cond_wait(&condQueue, &mutexQueue);
    }
    task = taskQueue[0];
    int i;
    for (i = 0; i < taskCount - 1; i++) {
      taskQueue[i] = taskQueue[i + 1];
    }
    taskCount--;
    pthread_mutex_unlock(&mutexQueue);
    executeTask(&task);
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
    Task t = {
      .a = rand() % 100,
      .b = rand() %100
    };
    submitTask(t);
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
