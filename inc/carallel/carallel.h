/**
 * \file carallel.h
 * \author Matthew Jemielity matthew.jemielity@gmail.com
 * \brief Main header for carallel parallelization macros.
 *
 * Carallel macros provide a way to quickly and easily
 * parallelize blocks of code inside methods. Currently
 * only very basic functionality is implemented.
 * Methods that can have parallelized code blocks must
 * have declaration like: void foo( void ).
 * See demo.c for sample usage of the macros.
 * Last, but not least, have fun with this project.
 */

#ifndef CARALLEL_H__
# define CARALLEL_H__

# include <carallel/queue.h>

# include <pthread.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdlib.h>

/**
 * \def CARALLEL_JOIN____
 * \brief Internal macro for pasting two tokens together.
 * \param X First token.
 * \param Y Second token.
 */
#define CARALLEL_JOIN____(X, Y) X ## Y
/**
 * \def CARALLEL_JOIN
 * \brief Creates label usable with goto by pasting two tokens together.
 * \param X First token.
 * \param Y Second token.
 */
#define CARALLEL_JOIN(X, Y) CARALLEL_JOIN____(X, Y)

/**
 * \def CARALLELIZE
 * \brief Creates a function that can use parallel code blocks.
 * \param NAME Name of the function to be created.
 * \param ... Source code of the function NAME.
 * \remarks Creates helper functions using provided name as UID.
 *
 * Several wrappers are created, using provided name for unique
 * identification. Main purpose of the wrappers is:
 * 1. create method: void <name>( void );
 * 2. create thread method that can be passed to pthread_create;
 * 3. create data structure holding UID for code block and a
 *  pointer to queue containing IDs of created threads.
 * The actual function, which the user will define when calling
 * this macro has several additional arguments:
 * a) arg - unique identifier for parallelized code block
 * b) q - pointer to pthread IDs queue
 * The parallelism is inspired by Duff's Device, inserting cases
 * into the code. The code defined by user when calling this macro
 * is therefore executed in a switch statement. The macro uses a
 * GCC-only predefined preprocessor macro: __COUNTER__. Without
 * it, user would have to pass a unique value to CARALLEL macros.
 * The __COUNTER__ macro is also supported by clang and Visual
 * Studio. The parallelism should work, even if code block isn't
 * certain to be reached by normal program flow, because cases
 * in switch statements are basically hidden gotos. Other factor
 * is that this version only supports methods taking no arguments
 * and returning nothing. Though with sufficient macro magic
 * adding more features should be possible. Currently I'm looking
 * for a way to remove the __VA_ARGS__ and allow user to define
 * a block of code after the expanded CARALLELIZE macro.
 */
# define CARALLELIZE(NAME, ...) \
  typedef struct { int64_t uid; } carallel_ ## NAME ## _arg; \
  static void carallelized_ ## NAME( \
    carallel_ ## NAME ## _arg const arg, \
    carallel_queue* const q \
  ); \
  void NAME(void) { \
    carallel_queue q = { NULL }; \
    carallelized_ ## NAME((carallel_ ## NAME ## _arg) { .uid = -1 }, &q); \
    while (NULL != q.head) { \
      pthread_t* id = carallel_queue_get(&q); \
      pthread_join(*id, NULL); \
    } \
  } \
  static void* carallel_thread_ ## NAME(void* const p) { \
    carallel_ ## NAME ## _arg* const a = p; \
    carallelized_ ## NAME(*a, NULL); \
    return NULL; \
  } \
  static void carallelized_ ## NAME( \
    carallel_ ## NAME ## _arg const arg, \
    carallel_queue * const q \
  ) { switch (arg.uid) { default: __VA_ARGS__ } }

/**
 * \def CARALLEL_UID____
 * \brief Causes code block defined within to execute in new thread.
 * \param NAME Name of the method to which the code block belongs.
 * \param UID Unique identifier for parallelized code block.
 * \remarks Code block should be legal inside a method's scope.
 * \warning Code block must be self-contained, i.e. not reference outside vars
 *
 * This macro MUST be followed by a block of code or a single statement.
 * This macro uses a unique identifier for a block of code, which it
 * will attempt to run in separate thread. Setting up parallelization
 * uses uniquely named static variables to hold thread id and struct
 * of arguments passed to wrapper function. This is necessary because
 * trying to use normal stack variables would inevitably run into
 * scoping issues, which in turn would mean another set of limitations
 * on usage. The macro basically creates an case in a switch statement.
 * This tests whether execution is in the main thread and the UID
 * passed is the same as the one belonging to the block. If so, the
 * code inside that if block is executed, after which the method returns.
 * This may limit usage in not-so-trivial scenarios. With sufficient
 * macro wizardry some obstacles may be removed and functionality
 * extended.
 */
# define CARALLEL_UID____(NAME, UID) \
  case UID: \
    if (NULL != q) { \
      static pthread_t id; \
      static carallel_ ## NAME ## _arg p = { .uid = UID }; \
      int const t = pthread_create(&id, NULL, carallel_thread_ ## NAME, &p); \
      if (0 == t) { carallel_queue_put( q, &id ); } \
    } else if ((NULL == q) && (UID != arg.uid)) { break; } else \
      if (true) { \
        goto CARALLEL_JOIN( carallel_ ## NAME ## _user_, UID ); \
      } else while( true ) if( true ) { return; } else \
        CARALLEL_JOIN( carallel_ ## NAME ## _user_, UID ):
        /* user code block here */

/**
 * \def CARALLEL
 * \brief Gives unique ID to a code block (per compilation unit).
 * \param NAME Name of carallel-enabled method to which the block belongs.
 * \remark Code block should be legal inside a method's scope.
 * \warning Code block must be self-contained, i.e. not reference outside vars.
 *
 * This macro uses GCC's __COUNTER__ macro as unique identifier for
 * a block of code, which it will attempt to run in separate thread.
 * Preprocessor macro __COUNTER__ allows to have unique identifier
 * inside a single compilation unit.
 * This macro MUST be followed by a valid code block or a signle statement.
 */
# define CARALLEL(NAME) CARALLEL_UID____(NAME, __COUNTER__)
/* user code block here */

#endif /* CARALLEL_H__ */

