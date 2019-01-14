
#include "SortedList.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sched.h>

int opt_yield;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	if (list == NULL || element == NULL)
	{
		fprintf(stderr, "NULL pointer as argument\n");
		exit(2);
	}
        SortedList_t *curr = list->next;
 	if (opt_yield & INSERT_YIELD)
                sched_yield();
	
        while (curr != list)
        {
                if (strcmp(element->key, curr->key) > 0) //move to next node
                {
                        curr = curr->next;
                }
                else            //found place for element: between past & curr
                        break;
        }
	
	element->prev = curr->prev;
	element->next = curr;
	curr->prev->next = element;
	curr->prev = element;
		
}

int SortedList_delete( SortedListElement_t *element)
{
	if ((element->prev->next != element) || (element->next->prev != element))
		return 1;
	if (opt_yield & DELETE_YIELD)
		sched_yield();
	element->prev->next = element->next;
	element->next->prev = element->prev;
	free(element);
	return 0;	
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
	SortedList_t *curr = list->next;
	if (opt_yield & LOOKUP_YIELD)
		sched_yield();
	while (curr != list)
	{
		if (strcmp(curr->key, key) == 0)
			return curr;
		else
			curr = curr->next;
	}	
	return NULL;
}

int SortedList_length(SortedList_t *list)
{
	SortedList_t *curr = list->next;
	int len = 0;
	if (opt_yield & LOOKUP_YIELD)
		sched_yield();
	while (curr != list)
	{
		if( curr->prev->next != curr || curr->next->prev != curr)
			return -1;
		len = len + 1;
		curr = curr->next;
	}
	return len;
}
