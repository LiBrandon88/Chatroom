#include "queue.h"

/* standard queuing functions & functionality                                                                                                                                                                                                 
 * From project 4; MODIFIED by Brandon Li for a mutex                                                                                                                                                                                         
*/
queue * create_queue()
{
    queue * q = ( queue * ) malloc( sizeof( queue ) );
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
    return q;
}


void enqueue( queue* q, void* val )
{
    q->count++;
    node * n = ( node * ) malloc( sizeof(node) );
    n->next = NULL;
    n->ptr = val;
    if( q->tail == NULL )
    {
        q->tail = n;
        q->head = n;
    }
    else
    {
        q->tail->next = n;
        q->tail = n;
    }
}

void * dequeue( queue* q )
{
    if( q->count < 1 )
        return NULL;
    q->count--;
    node * n = q->head;
    q->head = n->next;
    if( q->head == NULL )
        q->tail = NULL;
    void * s =  n->ptr;
    free( n );
    return s;
}

void destroy_queue( queue *q )
{
    while( q->count > 0 )
        dequeue( q );
    free( q );

}

queue_iterator * queue_iterator_create( queue *q )
{
    queue_iterator * qitr = ( queue_iterator * ) malloc( sizeof( queue_iterator ) );
    qitr->q = q;
    qitr->cur = NULL;
    qitr->prev = NULL;
    qitr->next = q->head;
    qitr->just_removed = 0;
    return qitr;
}

int queue_iterator_has_next( queue_iterator *qitr )
{
    return ( qitr->next !=NULL );
}

void * queue_iterator_next( queue_iterator *qitr )
{
    if( !queue_iterator_has_next(qitr) )
        return NULL;
    if( !qitr->just_removed )
        qitr->prev = qitr->cur;
    qitr->cur = qitr->next;
    qitr->next = qitr->next->next;
    qitr->just_removed = 0;
    return qitr->cur->ptr;
}

/*remove the current node the iterator points to*/
void * queue_iterator_remove( queue_iterator *qitr )
{
    if( qitr->cur == NULL )
        return NULL; /* can't remove what is not there */
    if( qitr->prev != NULL )
    {
        qitr->prev->next = qitr->next;
        if( qitr->q->tail == qitr->cur )
                qitr->q->tail = qitr->prev;
        }
        else if( qitr->q->head == qitr->cur ) /*no prev, cur must be head, check to be sure*/
        {
                qitr->q->head = qitr->next;
                if( qitr->q->head == NULL )
                        qitr->q->tail = NULL;
        }

    void * ptr = qitr->cur->ptr;
    free( qitr->cur );
    qitr->q->count --;
    qitr->cur = NULL;
    qitr->just_removed=1;
    return ptr;
}

void queue_iterator_destroy( queue_iterator *qitr )
{
    free( qitr );
}

