#ifndef CARALLEL_H__
# define CARALLEL_H__

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

# include <assert.h>
# include <carallel/queue.h>
# include <pthread.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdlib.h>

/**
 * \def CARALLEL_ENABLED
 * \brief Creates a function that can use parallel code blocks.
 * \param name Name of the function to be created.
 * \remarks Creates helper functions using provided name as UID.
 *
 * Several wrappers are created, using provided name for unique
 * identification. Main purpose of the wrappers is:
 * 1. create method: void <name>( void );
 * 2. create thread method that can be passed to pthread_create;
 * 3. create data structure holding UID for code block and a
 *    pointer to queue containing IDs of created threads.
 * The actual function, which the user will define after calling
 * this macro has several additional arguments:
 * a) UID - unique identifier for parallelized code block
 * b) q - pointer to pthread IDs queue
 * c) main - boolean variable telling whether execution is
 *           in the main thread or parallel threads
 * The 'main' argument is necessary to stop parallel threads
 * from spawning additional threads when they check for code
 * block to execute.
 * For the parallelism to work, the code block parallelized must
 * be reachable at all times, or at least to hit the same
 * conditions as when pthread_create was called. This limits
 * functionality to simple cases. Perhaps less trivial code could
 * be refactored into simpler one to accomodate carallel.
 * Similarly, this version only supports methods taking no
 * arguments and returning nothing. Though with sufficient macro
 * magic adding more features should be possible.
 */
# define CARALLEL_ENABLED( name ) \
    typedef struct { \
        uint32_t UID; \
        carallel_queue_t * q; \
    } carallel_##name##_arg_t; \
    \
    void carallel_ready_##name( \
        uint32_t const UID, \
        carallel_queue_t * const q, \
        bool const main \
    ); \
    void name( void ) \
    { \
        carallel_queue_t q = { NULL }; \
        carallel_ready_##name( __LINE__, &q, true );\
    } \
    \
    void * carallel_thread_##name( void * const p ) \
    { \
        carallel_##name##_arg_t * const a = p; \
        assert( NULL != a ); \
        carallel_ready_##name( a->UID, a->q, false ); \
        free( a ); \
        return NULL; \
    } \
    \
    void carallel_ready_##name( \
        uint32_t const UID, \
        carallel_queue_t * const q, \
        bool const main \
    )

/**
 * \def CARALLEL_RETURN
 * \brief Waits for threads to finish.
 *
 * The main method should wait for all parallelized code blocks
 * to finish execution before exiting. This macro does just that.
 * Since threads are created with pthread_create and pthread_t IDs
 * are held in a queue, here code execution can call pthread_join
 * to wait for every thread to finish. This macro should be used
 * any time a parallelizable method returns, otherwise running
 * threads will be cancelled and will quit before they can finish,
 * leading to inconsistent program state.
 */
# define CARALLEL_RETURN() \
    do { \
        while( NULL != q->head ) \
        { \
            pthread_t * id = carallel_queue_get( q ); \
            pthread_join( *id, NULL ); \
            free( id ); \
        } \
    } while( false )

/**
 * \def CARALLEL
 * \brief Causes code block defined within to execute in new thread.
 * \param name Name of the method to which the code block belongs.
 * \param ... Code block to be parallelized.
 * \remarks Code block should be legal inside a method's scope.
 *
 * This macro uses line in which it's used as unique identifier for
 * a block of code, which it will attempt to run in separate thread.
 * Setting up this parallelization allocates memory for internal
 * data structures. If that allocation fails, the macro bails out
 * without creating a separate thread. Preprocessor macro __LINE__
 * allows to have unique identifier inside a single compilation unit.
 * The macro basically creates an if() { ... } block. This tests
 * whether exectution is in the main thread and the UID passed is
 * the same as the one belonging to the block. If so, the code inside
 * that if block is executed, after which the method returns. This
 * limits usage to quite simple scenarios. On the other hand, perhaps
 * code could be refactored to accomodate carallel. With sufficient
 * macro wizardry some obstacles may be removed and functionality
 * extended.
 */
# define CARALLEL( name, ... ) \
    do { \
        if(( false == main ) && ( __LINE__ == UID )) \
        { \
            __VA_ARGS__ \
            return; \
        } else if( true == main ) { \
            pthread_t * id = malloc( sizeof( pthread_t )); \
            carallel_##name##_arg_t * p = \
                malloc( sizeof( carallel_##name##_arg_t )); \
            p->UID = __LINE__; \
            p->q = q; \
            if(( NULL == id ) || ( NULL == p )) \
            { free( id ); free( p ); break; } \
            if( 0 == pthread_create( id, NULL, carallel_thread_##name, p )) \
            { carallel_queue_put( q, id ); } \
        } \
    } while( false )

#endif /* CARALLEL_H__ */

