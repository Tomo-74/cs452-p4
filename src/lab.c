#include "lab.h"
#include <pthread.h>

// Written by AI and me
struct queue {
    pthread_mutex_t mutex;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;

    void** data;
    int capacity;
    int size;
    int shutdown;
};

// Written by AI
queue_t queue_init(int capacity)
{
    // Allocate queue memory
    queue_t q = (queue_t)malloc(sizeof(struct queue));
    if(!q) return NULL;

    // Allocate data memory
    q->data = malloc(capacity * sizeof(void *));
    if(!q->data)
    {
        free(q);
        return NULL;
    }
    
    // Initialize fields
    q->capacity = capacity;
    q->size = 0;
    q->shutdown = false;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->notFull, NULL);
    pthread_cond_init(&q->notEmpty, NULL);

    return q;
}

void queue_destroy(queue_t q)
{
    if(!q) return;

    // Acquire lock
    pthread_mutex_lock(&q->mutex);

    q->shutdown = true;

    // Wake up all waiting threads
    pthread_cond_broadcast(&q->notEmpty);
    pthread_cond_broadcast(&q->notFull);
    pthread_mutex_unlock(&q->mutex);

    // Destroy mutex and condition variables
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->notFull);
    pthread_cond_destroy(&q->notEmpty);

    // Release lock
    pthread_mutex_unlock(&q->mutex);
}

// Written by me
void enqueue(queue_t q, void *data)
{
    if(!q || !data) return;
    
    // Do nothing if shut down
    if(q->shutdown) return;

    // Acquire lock
    pthread_mutex_lock(&q->mutex);

    // Wait until there is room in the queue
    while(q->size == q->capacity)
    {
        if (q->shutdown) return; // If a shutdown occurs while waiting
        pthread_cond_wait(&q->notFull, &q->mutex); // Wait on the condition that queue is not full
    }

    // Add to back of queue
    q->data[q->size] = data;
    q->size++;

    // Signal that queue is guaranteed to be not empty right now
    pthread_cond_signal(&q->notEmpty);

    // Release lock
    pthread_mutex_unlock(&q->mutex);
}

// Written by me
void *dequeue(queue_t q)
{
    if(!q) return;

    // Do nothing if shut down
    if(q->shutdown) return;

    // Acquire lock
    pthread_mutex_lock(&q->mutex);
    
    // Wait until there is an element to remove
    while(q->size == 0)
    {
        if (q->shutdown) return; // If a shutdown occurs while waiting
        pthread_cond_wait(&q->notEmpty, &q->mutex); // Wait on the condition that queue is not empty
    }


    // Remove front element, shift remaining elements left
    for(size_t i = 0; i < q->size - 1; i++)
    {
        q->data[i] = q->data[i+1];
    }
    q->size--;

    // Signal that queue is guaranteed to be not full
    pthread_cond_signal(&q->notFull);

    // Release lock
    pthread_mutex_unlock(&q->mutex);
}

// Written by me
void queue_shutdown(queue_t q)
{
    if(!q) return;
    
    pthread_mutex_lock(&q->mutex);
    q->shutdown = 1;

    // Unblock all threads waiting on this condition variable
    pthread_cond_broadcast(&q->notEmpty);
    pthread_cond_broadcast(&q->notFull);
    pthread_mutex_unlock(&q->mutex);
}

// Written by me
bool is_empty(queue_t q)
{
    return q->size == 0; // Mutex not needed because no data is only being accessed, not changed
}

// Written by me
bool is_shutdown(queue_t q)
{
    return q->shutdown; // Mutex not needed because no data is only being accessed, not changed
}
