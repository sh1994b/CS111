
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include "SortedList.h"

long *waitForLock;
int opt_yield = 0;
int numThreads = 1;
int numIters = 1;
int numLists = 1;
char *yieldType;
char syncType = 'a';
int opt_sync = 0, yieldflag = 0;
int numElements = 0;
//head is an array of head pointers
SortedList_t **head;
SortedListElement_t **elementsptr;
char* makeRandKey();
void errexit(char* msg, int exitcode);
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
int slock = 0;
void *threadThings(void *tid);
unsigned int hashFunc(const char* key);

int main(int argc, char* argv[])
{
	int long_index = 0;
	unsigned int j = 0;
	int opt = -1;
	struct timespec start, end;
	char testname[20];
	strcpy(testname, "list-");
	int totOps = 0, numElements = 0;
	long runtime = 0, timePerOp = 0;
	opt_yield = 0;	
	static struct option long_options[] =
        {
                {"threads=", optional_argument, NULL, 't'},
                {"iterations=", optional_argument, NULL, 'i'},
                {"yield=", required_argument, NULL, 'y'},
                {"sync=", required_argument, NULL, 's'},
		{"lists=", required_argument, NULL, 'l'},
		{0, 0, 0, 0}
        };
	while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
        {
                switch (opt)
                {
			case 'l':
				numLists = atoi(optarg);
				break;
                        case 't':
                                numThreads = atoi(optarg);
                                break;
                        case 'i':
                                numIters = atoi(optarg);
                                break;
                        case 'y':
				yieldflag = 1;
                                yieldType = optarg;
				for (j = 0; j < strlen(optarg); j++)
				{
				switch(optarg[j])
				{		
				case 'i':
					opt_yield = opt_yield | INSERT_YIELD;
					break;
				case 'd':
					opt_yield = opt_yield | DELETE_YIELD;
					break;
				case 'l':
					opt_yield = opt_yield | LOOKUP_YIELD;
					break;
				default:
					errexit("Unsupported yield option\n", 1);
				}
				}
                                break;
                        case 's':
                                opt_sync = 1;
                                syncType = *optarg;
                                if(syncType != 'm' && syncType != 's')
                                        errexit("Unsupported sync type\n", 1);
                                break;
                        default:
                                errexit("Unrecognized option entered\n", 1);
                }
        }

	if (yieldflag == 1)
	{
		strcat(testname, yieldType);
		strcat(testname, "-");
	}
	else
		strcat(testname, "none-");
	if (opt_sync == 1)
		strcat(testname, &syncType);
	else
		strcat(testname, "none");

	//each item in waitForLock holds the amount of time that thread
	//waited to acquire the lock
	waitForLock = malloc (numThreads * sizeof(long));

		numElements = numThreads * numIters;
	//initialize numLists number of empty lists
	head = malloc(numLists * sizeof(SortedList_t*)); //allocate mem for list heads array
	int l;
	for (l = 0; l < numLists; l++)
	{
		//allocate memory for each sublist head
		head[l] = malloc(sizeof(SortedList_t));
		head[l]->next = head[l];
		head[l]->prev = head[l];
		head[l]->key= NULL;
	}

	//create and initialize required # of list elements	
	elementsptr = malloc(numElements * sizeof(SortedListElement_t*));
	if (elementsptr == NULL)	
		errexit("malloc() failed\n", 1);
	int i = 0;
	for (i = 0; i < numElements; i++)
	{
		elementsptr[i] = malloc(sizeof(SortedListElement_t));
		elementsptr[i]->key = makeRandKey();
		elementsptr[i]->next = NULL;
		elementsptr[i]->prev = NULL;
	}


	clock_gettime(CLOCK_MONOTONIC, &start);

	pthread_t threads[numThreads];
        int rc;
        long t;
	int* threadIDs = malloc(sizeof(int)*numThreads);
        for (t = 0; t < numThreads; t++)
	{
		threadIDs[t] = t;
                rc = pthread_create(&threads[t], NULL, threadThings, (void *) (threadIDs+t));
                if (rc != 0)
                        errexit("Couldn't create thread\n", 1);
        }

	for (t = 0; t < numThreads; t++)
        {
		rc = pthread_join(threads[t], NULL);
                if (rc != 0)
                        errexit("Couldn't join threads\n", 1);
        }
	long waitForLockAvg = 0;
	clock_gettime(CLOCK_MONOTONIC, &end);
	long int length = 0;	
	for (i = 0; i < numLists; i++)
	{
		length = SortedList_length(head[i]);
		if (length != 0)
			errexit("final length not zero\n", 2);
	}
	totOps = numThreads * numIters * 3;
	runtime = (end.tv_sec - start.tv_sec)*1000000000 + end.tv_nsec - start.tv_nsec;  
	timePerOp = runtime / totOps;

	if (syncType == 'm' || syncType == 's')
	{
		long totWaitForLock = 0;
		int k;
		for (k = 0; k < numThreads; k++)
			totWaitForLock += waitForLock[k];
		waitForLockAvg = totWaitForLock / numThreads;
	}

	printf("list-");
	if (opt_yield & INSERT_YIELD)
		printf("i");
	if (opt_yield & DELETE_YIELD)
		printf("d");
	if (opt_yield & LOOKUP_YIELD)
		printf("l");
	if (opt_yield == 0)
		printf("none");
	printf("-");
	if (syncType == 'a')  
		printf("none");
	else
		printf("%c", syncType);

	printf(",%d,%d,%d,%d,%ld,%ld,%ld\n",numThreads, numIters, numLists, totOps, runtime, timePerOp, waitForLockAvg);

	//free(elementsptr);
	return 0;
}

char* makeRandKey()
{
//	srand( time(NULL) );
	int i = 0;
	//each key will have 11 characters (last one null)
	char *randkey = malloc(11 * sizeof(char));
	for (i = 0; i < 10; i++)
	{
		randkey[i] = rand() % 26 + 97; //random char from a to z
	}
	randkey[10] = '\0'; 
	return randkey;
}

void *threadThings(void *tid)
{
	int threadID = * (int *) tid;
	int i = 0;
	//long length = 0;
	SortedListElement_t *temp;
	struct timespec lockStart, lockEnd;
	long waitForLockPerThread;
		
	//acquire lock and get the time it takes to do so
	//insert all elements in single shared list
	unsigned int headNumber;
	for (i = threadID; i < (numIters * numThreads); i += numThreads)
	{
	if (syncType == 'm')
	{
		clock_gettime(CLOCK_MONOTONIC, &lockStart);
		pthread_mutex_lock(&mlock);
		clock_gettime(CLOCK_MONOTONIC, &lockEnd);
		waitForLockPerThread = (lockEnd.tv_sec - lockStart.tv_sec)*1000000000 + lockEnd.tv_nsec - lockStart.tv_nsec;
		waitForLock[threadID] = waitForLockPerThread;
	}

	if (syncType == 's')
	{
		clock_gettime(CLOCK_MONOTONIC, &lockStart);
		while(__sync_lock_test_and_set(&slock, 1))
			;
		clock_gettime(CLOCK_MONOTONIC, &lockEnd);
		waitForLockPerThread = (lockEnd.tv_sec - lockStart.tv_sec)*1000000000 + lockEnd.tv_nsec - lockStart.tv_nsec;
		waitForLock[threadID] = waitForLockPerThread;
	
	}	
		headNumber = hashFunc(elementsptr[i]->key);
		SortedList_insert(head[headNumber], elementsptr[i]);

	if (syncType == 'm')
		pthread_mutex_unlock(&mlock);
	if (syncType == 's')
		__sync_lock_release(&slock);
		
	}

	int j;
	int listLength = 0, templength;
	for (j = 0; j < numLists; j++)
	{

	if (syncType == 'm')
	{
		clock_gettime(CLOCK_MONOTONIC, &lockStart);
		pthread_mutex_lock(&mlock);
		clock_gettime(CLOCK_MONOTONIC, &lockEnd);
		waitForLockPerThread = (lockEnd.tv_sec - lockStart.tv_sec)*1000000000 + lockEnd.tv_nsec - lockStart.tv_nsec;
		waitForLock[threadID] = waitForLockPerThread;
	}

	if (syncType == 's')
	{
		clock_gettime(CLOCK_MONOTONIC, &lockStart);
		while(__sync_lock_test_and_set(&slock, 1))
			;
		clock_gettime(CLOCK_MONOTONIC, &lockEnd);
		waitForLockPerThread = (lockEnd.tv_sec - lockStart.tv_sec)*1000000000 + lockEnd.tv_nsec - lockStart.tv_nsec;
		waitForLock[threadID] = waitForLockPerThread;
	
	}

		templength = SortedList_length(head[j]);
		if (templength == -1)
			errexit("Error in length of sublist\n", 2);
		listLength += templength;

	if (syncType == 'm')
		pthread_mutex_unlock(&mlock);
	if (syncType == 's')
		__sync_lock_release(&slock);
	
	}

	//look up each key and delete it
	for (i = threadID; i < numIters * numThreads; i+=numThreads)
	{

	if (syncType == 'm')
	{
		clock_gettime(CLOCK_MONOTONIC, &lockStart);
		pthread_mutex_lock(&mlock);
		clock_gettime(CLOCK_MONOTONIC, &lockEnd);
		waitForLockPerThread = (lockEnd.tv_sec - lockStart.tv_sec)*1000000000 + lockEnd.tv_nsec - lockStart.tv_nsec;
		waitForLock[threadID] = waitForLockPerThread;
	}

	if (syncType == 's')
	{
		clock_gettime(CLOCK_MONOTONIC, &lockStart);
		while(__sync_lock_test_and_set(&slock, 1))
			;
		clock_gettime(CLOCK_MONOTONIC, &lockEnd);
		waitForLockPerThread = (lockEnd.tv_sec - lockStart.tv_sec)*1000000000 + lockEnd.tv_nsec - lockStart.tv_nsec;
		waitForLock[threadID] = waitForLockPerThread;
	
	}



		headNumber = hashFunc(elementsptr[i]->key);		
		temp = SortedList_lookup (head[headNumber], elementsptr[i]->key);
		if (temp == NULL)
			errexit("Key not found\n", 2);
		SortedList_delete (temp);

	if (syncType == 'm')
		pthread_mutex_unlock(&mlock);
	if (syncType == 's')
		__sync_lock_release(&slock);
	
	}

	pthread_exit(NULL);
}



void errexit(char* msg, int exitcode)
{
        fprintf(stderr, msg);
        exit(exitcode);
}

unsigned int hashFunc(const char* key)
{
	unsigned int val = 3879;
	int i;
	for (i = 0; i < 10; i++)
        	val = (val << 4) + val + key[i];
	return val % numLists;
}
