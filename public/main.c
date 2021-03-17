#include <assert.h>
#include <pthread.h>
#include <stdio.h>

int x;
int a;

int ret;

void fac() {
  a = 1;
fac:
  if (x == 0) {
    ret = a;
  } else {
    a = a * x;
    --x;
    goto fac;
  }
}

void *f(void *p) {
  x = 5;
  fac();
  return NULL;
}

int main(void) {
  pthread_t thrd;
  int res = pthread_create(&thrd, NULL, f, NULL);
  assert(res == 0);
  res = pthread_join(thrd, NULL);
  assert(res == 0);
  printf("fac() = %d\n", ret);
  return 0;
}
