#include <carallel/queue.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

void * carallel_queue_get( carallel_queue_t * const queue )
{
    if( NULL == queue->head ) { return NULL; }
    carallel_queue_item_t * const item = queue->head;
    queue->head = item->next;
    void * const result = item->value;
    free( item );
    return result;
}

int carallel_queue_put(
    carallel_queue_t * const queue,
    void * const value
)
{
    carallel_queue_item_t * const item =
        malloc( sizeof( carallel_queue_item_t ));
    if( NULL == item ) { return ENOMEM; }
    item->value = value;
    item->next = NULL;
    if( NULL == queue->head ) { queue->head = item; return 0; }
    carallel_queue_item_t * iterator;
    for(
        iterator = queue->head;
        NULL != iterator->next;
        iterator = iterator->next
    ) { continue; }
    iterator->next = item;
    return 0;
}

