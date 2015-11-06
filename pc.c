#define MAX 1


#include<stdio.h>
#include "mythreads.h"



pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

int buffer[MAX];
int populate = 0;
int use = 0;
int count = 0;
int loops =2;

void put(int value) {
				printf("putting\n");
				buffer[populate] = value;
				populate = ( populate + 1) % MAX;
				count++;
}

int get() {
				printf("getting\n");
				int tmp = buffer[use];
				use = (use + 1) % MAX;
				count--;
				return tmp;
}

void *producer(void* args) {
				int i;
				for (i = 0; i < loops; i++) {
								Pthread_mutex_lock(&mutex); // p1
								while (count == MAX) // p2
												pthread_cond_wait(&empty, &mutex); // p3
								put(i); // p4
								pthread_cond_signal(&fill); // p5
								Pthread_mutex_unlock(&mutex); // p6
				}
}

void *consumer(void *args) {
				int i;
				for (i = 0; i < loops; i++) {
								Pthread_mutex_lock(&mutex); // c1
								while (count == 0) // c2
												pthread_cond_wait(&fill, &mutex); // c3
								int tmp = get(); // c4
								pthread_cond_signal(&empty); // c5
								Pthread_mutex_unlock(&mutex); // c6
								printf("%d\n", tmp);
				}
}

int main(){
	pthread_t p,c;
	Pthread_create(&p,NULL,producer,NULL);
	Pthread_create(&c,NULL,consumer,NULL);
	Pthread_join(p,NULL);
	Pthread_join(c,NULL);
}
