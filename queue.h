#ifndef ___QUEUES___
#define ___QUEUES___
#include <stdlib.h>

typedef struct node
{
    struct node* next;
    void* ptr;
} node;

typedef struct queue
{
    node* head;
    node* tail;
    int count;
} queue;

typedef struct queue_iterator
{
    queue* q;
    node* prev;
    node* cur;
    node* next;
    int just_removed;
} queue_iterator;

queue* create_queue(void);
void enqueue(queue* , void* );
void* dequeue(queue*);
void destroy_queue(queue*);
queue_iterator* queue_iterator_create(queue*);
int queue_iterator_has_next(queue_iterator*);
void* queue_iterator_next(queue_iterator*);
void* queue_iterator_remove(queue_iterator*);
void queue_iterator_destroy(queue_iterator*);
#endif

