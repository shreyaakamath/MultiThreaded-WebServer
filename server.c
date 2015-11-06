#include "cs537.h"
#include "request.h"
#include "mythreads.h"

pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

int poolSz;
int bufferSz;

int *buffer;
int populate = 0;
int use = 0;
int count = 0;


void put(int value) {
				//printf("putting\n");
				buffer[populate] = value;
				populate = ( populate + 1) % bufferSz;
				count++;
}

int get() {
				//printf("getting\n");
				int tmp = buffer[use];
				use = (use + 1) % bufferSz;
				count--;
				return tmp;
}

void getargs(int *port, int *threads,int *buffers,int argc, char *argv[])
{
    if (argc != 4) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *buffers = atoi(argv[3]);
}

void* consumer(void *args){
	while(1){
		int connfd;
		Pthread_mutex_lock(&mutex);
		while(count == 0)
			pthread_cond_wait(&fill, &mutex);
		connfd=get();
		Pthread_mutex_unlock(&mutex);
		requestHandle(connfd);
		Close(connfd);
	}
}

void thread_pool_create(int size, int buf_size){
	int i;
	pthread_t pool[size];
	for(i=0;i<size;i++){
		pthread_create(&pool[i],NULL,consumer,NULL);
	}
	poolSz = size;
	bufferSz = buf_size;
	buffer = (int*)malloc(buf_size * sizeof(int));
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, threads, buffers;
    struct sockaddr_in clientaddr;

    getargs(&port, &threads, &buffers, argc, argv);
    thread_pool_create(threads,buffers);

    listenfd = Open_listenfd(port);

    while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

		Pthread_mutex_lock(&mutex);
		if(count == bufferSz){
			Pthread_mutex_unlock(&mutex);
			Close(connfd);
		}
		else {
			put(connfd);
			pthread_cond_signal(&fill);
			Pthread_mutex_unlock(&mutex);
		}
    }

}


    


 
