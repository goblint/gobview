#include <pthread.h>
#include <stdio.h>

int x, a, ret;
int results[] = {1, 2};

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void fac() {
rec:
  if (x <= 0) {
    ret = a;
  } else {
    a *= x;
    --x;
    goto rec;
  }
}

void *f(void *p) {
  int *params = p;
  int lock;
  if (lock) {
    pthread_mutex_lock(&mtx);
  }
  x = params[0];
  a = 1;
  fac();
  results[params[1]] = ret;
  if (lock) {
    pthread_mutex_unlock(&mtx);
  }
  return NULL;
}

int main(void) {
  pthread_t t1, t2;
  pthread_create(&t1, NULL, f, &(int[2]){5, 0});
  pthread_create(&t2, NULL, f, &(int[2]){10, 1});
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  printf("fac(5) = %d\n", results[0]);
  printf("fac(10) = %d\n", results[1]);
  int *p;
  *p = 42;
}
