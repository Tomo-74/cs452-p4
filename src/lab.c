#include "lab.h"
#include <pthread.h>

// Written by AI
struct queue {
    int* data;
    int capacity;
    int size;
};

// Written by AI
queue_t queue_init(int capacity)
{
    // Allocate queue memory
    queue_t q = (queue_t)malloc(sizeof(struct queue));
    if(q == NULL) return NULL;

    // Allocate data memory
    q->data = (int*)malloc(capacity * sizeof(int));
    if(q->data == NULL)
    {
        free(q);
        return NULL;
    }
    
    // Initialize fields
    q->capacity = capacity;
    q->size = 0;
    
    return q;
}

// Written by me
void enqueue(queue_t q, int data)
{
    static pthread_mutex_t enq_mutex = PTHREAD_MUTEX_INITIALIZER;

    while(q->size == q->capacity)
    {
        // Wait until there is room in the queue
    }

    // Acquire lock
    pthread_mutex_lock(&enq_mutex);

    // Add to back of queue
    q->data[q->size] = data;
    q->size++;

    // Release lock
    pthread_mutex_unlock(&enq_mutex);
}

// Written by me
void *dequeue(queue_t q)
{
    static pthread_mutex_t deq_mutex = PTHREAD_MUTEX_INITIALIZER;

    while(q->size == 0)
    {
        // Wait until there is an element to remove
    }

    // Acquire lock
    pthread_mutex_lock(&deq_mutex);

    // Remove front element, shift remaining elements left
    for(size_t i = 0; i < q->size - 1; i++)
    {
        q->data[i] = q->data[i+1];
    }
    q->data[q->size-1] = -1; // can't overwrite as NULL
    q->size--;

    // Release lock
    pthread_mutex_unlock(&deq_mutex);
}
