///Name: Hari Pradhan
///Date: 07/06/2013
///Purpose: This program will generate the list of  producer threads (as defined by the user)
///which create random numbers that will be stored into a circular queue and the other set of  
///consumer threads that will remove numbers from the queue.
///Steps:
///1. Enter the number of producer.
///2. Enter the number of consumer.
///3. Enque and deque in the circular queue.
/// Assumption: SIZE of cirulcar Queue = 5 and Random number generated within the range of 1 to 50.
/// The producer sleeps for a random amount of time (between 1 and 3 seconds), generate a single random integer 
///while the consumer sleeps for a random amount of time (between 1 and 3 seconds), consume a single random integer.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define SIZE 5
#define MAX_NUM 50

struct CircularQueue{
	int rear;
	int front;
	int queue[SIZE];
	int full;
	int empt;
};

struct Thread{
	int id;
	pthread_t thread;
};

pthread_mutex_t mutex;
struct CircularQueue *cq;

/// <summary>
/// Initialize the parameters of cirular queue
/// </summary>
/// <param name="cq">CircularQueue</param>
void initialize(struct CircularQueue *cq){
	cq->rear = -1;
	cq->front = -1;
	cq->full = 0;
	cq->empt = 1;
}

/// <summary>
/// Enqueue the data into the circular queue
/// </summary>
/// <param name="cq">CircularQueue</param>
/// <param name="data">data to be enqueued</param>
void enqueue(struct CircularQueue *cq,int data){
	cq->rear = (cq->rear + 1) % SIZE;
	cq->queue[cq->rear] = data;
	//QUEUE full condition
	if(cq->rear == cq->front || (cq->front == -1 && cq->rear == SIZE - 1)){
		cq->full = 1;
	}
	if(cq->empt == 1){
		cq->empt = 0;
	}

}

/// <summary>
/// Dequeue the data from the circular queue
/// </summary>
/// <param name="cq">CircularQueue</param>
/// <return> data dequeued </return>
int dequeue(struct CircularQueue *cq){
	cq->front = (cq->front + 1) % SIZE;
	//QUEUE empty condition
	if(cq->front == cq->rear || cq->rear == -1){
		cq->empt = 1;
	}
	if(cq->full == 1){
		cq->full = 0;
	}
	return cq->queue[cq->front];
}

/// <summary>
/// Starts producing the data and inserts into the circular queue.
/// </summary>
/// <param name="producerThread">Producer thread</param>
void produce(void *producerThread){
	struct Thread *producer = (struct Thread*) producerThread;
	//Initialize seed value for rand_r in seed and random value to an interger value of  producer->id ;
	int s = producer->id; 
	int seedForRandomNum = producer->id;
	while(1){
		sleep(rand_r(&s) % 3 + 1);
		int data = rand_r(&seedForRandomNum) % MAX_NUM + 1;
		pthread_mutex_lock(&mutex);
		if(!cq->full){
			enqueue(cq,data);
			printf("Producer[%d] produces data[%d]: %d\n",producer->id,cq->rear,data);	
		}else{
			printf("Queue is Full\n");
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

/// <summary>
/// Starts consuming the data fromt the circular queue.
/// </summary>
/// <param name="consumerThread">Consumer thread</param>
void consume(void *consumerThread){
	int conData;
	struct Thread *consumer= (struct Thread*) consumerThread;
	//Initialize seed value for rand_r in seed and random value to an interger value of  consumer->id ;
	int s = consumer->id;
	while(1){
		sleep(rand_r(&s) % 3 + 1);
		pthread_mutex_lock(&mutex);
		if(!cq->empt){
			conData = dequeue(cq);
			printf("\t\t\t\tConsumer[%d] consumes data[%d]: %d \n",consumer->id,cq->front,conData);
		}else{
			printf("\t\t\t\tQueue is Emtpy\n");
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

int main(){
	int i;
	int producerCount;
	int consumerCount;
	cq = (struct CircularQueue *)malloc(sizeof(struct CircularQueue));
	initialize(cq);

	printf("Enter the number of Producers: ");
	scanf("%d",&producerCount);
	printf("Enter the number of Consumers: ");
	scanf("%d",&consumerCount);

	struct Thread *pThreadProducer = (struct Thread*)malloc(sizeof(struct Thread)*producerCount);
	struct Thread *pThreadConsumer = (struct Thread*)malloc(sizeof(struct Thread)* consumerCount);

	if(pthread_mutex_init(&mutex, NULL) != 0){
		perror("Error on mutex initialization");
		exit(1);
	}

	for(i = 0; i < producerCount; i++){
		pThreadProducer[i].id = i;
		if(pthread_create(&pThreadProducer[i].thread,NULL, (void *)&produce, (void*) &pThreadProducer[i]) != 0){
			perror("Error on producer thread creation");
			exit(1);
		}
	}

	for(i =0; i < consumerCount; i++){
		pThreadConsumer[i].id = i;
		if(pthread_create(&pThreadConsumer[i].thread,NULL,(void *) &consume, (void*) &pThreadConsumer[i]) != 0){
			perror("Error on consumer thread creation");
			exit(1);
		}
	}

	for(i = 0; i < producerCount; i++){
		pthread_join(pThreadProducer[i].thread,NULL);
	}

	for(i =0; i < consumerCount; i++){
		pthread_join(pThreadConsumer[i].thread, NULL);
	}

	pthread_mutex_destroy(&mutex);
	free(pThreadProducer);
	free(pThreadConsumer);
	free(cq);
	return 0;
}


