
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sched.h>

int numThreads = 1;
int numIters = 1;
long long counter = 0;
void add(long long *pointer, long long value);
void madd(long long *pointer, long long value);
void sadd(long long *pointer, long long value);
void cadd(long long *pointer, long long value);
void *callAdder();
void errexit(char* msg, int exitcode);
int opt_yield = 0, opt_sync = 0;
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
int slock = 0;
char syncType;
	
int main(int argc, char* argv[])
{
	int long_index = 0;
	int opt = -1;
	struct timespec start, end;
	char testname[20];
	strcpy(testname, "add-");
	int totOps = 0;
	long runtime = 0, timePerOp = 0;
	static struct option long_options[] =
	{
        	{"threads=", optional_argument, NULL, 't'},
		{"iterations=", optional_argument, NULL, 'i'},
		{"yield", no_argument, NULL, 'y'},
		{"sync=", required_argument, NULL, 's'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
	{
        	switch (opt)
        	{
                	case 't':
                        	numThreads = atoi(optarg);
                        	break;
			case 'i':
				numIters = atoi(optarg);
				break;
			case 'y':
				opt_yield = 1;
				strcat(testname, "yield-");
				break;
			case 's':
				opt_sync = 1;
				syncType = *optarg;
				if(syncType != 'm' && syncType != 's' \
				&& syncType != 'c')
					errexit("Unsupported sync type\n", 1);  
				break;
                	default:
                        	errexit("Unrecognized option entered\n", 1);
        	}
	}

	if (opt_sync)
		strcat(testname, &syncType);
	else
	strcat(testname, "none");
	
	
	clock_gettime(CLOCK_MONOTONIC, &start);

	pthread_t threads[numThreads];
	int rc;
	long t;
	for (t = 0; t < numThreads; t++)
	{	
		rc = pthread_create(&threads[t], NULL, callAdder, NULL);
		if (rc != 0)
			errexit("Couldn't create thread\n", 1);
	}
	for (t = 0; t < numThreads; t++)
	{	
		rc = pthread_join(threads[t], NULL);
		if (rc != 0)
			errexit("Couldn't join threads\n", 1);
	}
	
	clock_gettime(CLOCK_MONOTONIC, &end);

	

	totOps = numThreads * numIters * 2;
	runtime = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	timePerOp = runtime / totOps;
	printf("%s,%d,%d,%d,%ld,%ld,%lld\n", testname, numThreads, \
		numIters, totOps, runtime, timePerOp, counter);

return 0;
}

void add(long long *pointer, long long value) 
{
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
}

void madd(long long *pointer, long long value)
{
	pthread_mutex_lock(&mlock);
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
	pthread_mutex_unlock(&mlock);
}
void sadd(long long *pointer, long long value)
{
	while(__sync_lock_test_and_set(&slock, 1))
		;	//lock spin lock
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
	__sync_lock_release(&slock);	//unlock spin lock
}
void cadd(long long *pointer, long long value)
{
	long long oldval;
	do
	{
		oldval = counter;
		if (opt_yield)
			sched_yield();
	} while (__sync_val_compare_and_swap(pointer, oldval, oldval+value) != oldval);
}

void *callAdder()
{
	int i;
	for (i = 0; i < numIters; i++)
        {
                if (syncType == 'm')
                        madd(&counter, 1);
		else if (syncType == 's')
			sadd(&counter, 1);
		else if (syncType == 'c')
			cadd(&counter, 1);
		else
			add(&counter, 1);
	}
	for (i = 0; i < numIters; i++)
        {
                if (syncType == 'm')
                        madd(&counter, -1);
		else if (syncType == 's')
			sadd(&counter, -1);
		else if (syncType == 'c')
			cadd(&counter, -1);
		else
			add(&counter, -1);
	}
	return 0; //I get a warning without this: why?
}

void errexit(char* msg, int exitcode)
{
	fprintf(stderr, msg);
	exit(exitcode);
}

