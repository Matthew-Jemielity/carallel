#include <carallel/queue.h>

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

void* carallel_queue_get(carallel_queue* const queue) {
  if (NULL == queue->head) { return NULL; }
  carallel_queue_item* const item = queue->head;
  queue->head = item->next;
  void* const result = item->value;
  free(item);
  return result;
}

int carallel_queue_put(carallel_queue* const queue, void* const value) {
  carallel_queue_item* const item = malloc( sizeof( carallel_queue_item ));
  if (NULL == item) { return ENOMEM; }
  item->value = value;
  item->next = NULL;
  if (NULL == queue->head) { queue->head = item; return 0; }
  carallel_queue_item* it;
  for (it = queue->head; NULL != it->next; it = it->next) { continue; }
  it->next = item;
  return 0;
}

