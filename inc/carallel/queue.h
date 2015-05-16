/**
 * \file carallel_queue.h
 * \author Matthew Jemielity matthew.jemielity@gmail.com
 * \brief Helper functions used by carallel macros.
 *
 * Carallel macros create new threads. Because main method
 * can exit before the threads are finished, it's necessary
 * to join all the created threads. For that, carallel uses
 * queue, into which newly created thread IDs are put.
 * Whenever function exits, it should use proper finish
 * macro. The macro will use the queue entries to join all
 * threads.
 */

#ifndef CARALLEL_QUEUE_H__
# define CARALLEL_QUEUE_H__

/**
 * \typedef carallel_queue_item_t
 *
 * Queue item holding pointer to arbitrary memory location
 * and link to next item in the queue.
 */
typedef struct carallel_queue_item_s carallel_queue_item;

/**
 * \typedef carallel_queue_t
 *
 * Describes a queue. Holds pointer to head of the queue
 * (first item).
 */
typedef struct { carallel_queue_item* head; } carallel_queue;

/**
 * \struct carallel_queue_item
 *
 * Describes carallel queue's item
 */
struct carallel_queue_item_s
{
  void* value; /**< pointer to arbitrary memory location */
  carallel_queue_item* next; /**< pointer to next queue item */
};

/**
 * \func carallel_queue_get
 * \param queue Pointer to carallel queue.
 * \return Pointer to memory location held in first item in the queue.
 * \remarks Takes a single item off the queue, frees memory used by it.
 */
void* carallel_queue_get(carallel_queue* const queue);

/**
 * \func carallel_queue_put
 * \param queue Pointer to carallel queue.
 * \param value Pointer to arbitrary memory to be stored in the queue.
 * \return Returns 0 on success, ENOMEM if item allocation fails.
 * \remarks Allocates memory for new item put on the queue.
 */
int carallel_queue_put(carallel_queue* const queue, void* const value);

#endif /* CARALLEL_QUEUE_H__ */

