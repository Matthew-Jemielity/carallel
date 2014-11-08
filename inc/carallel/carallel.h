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
 * The actual function, which the user will define when calling
 * this macro has several additional arguments:
 * a) UID - unique identifier for parallelized code block
 * b) q - pointer to pthread IDs queue
 * c) main - boolean variable telling whether execution is
 *           in the main thread or parallel threads
 * The 'main' argument is necessary to stop parallel threads
 * from spawning additional threads when they check for code
 * block to execute.
 * The parallelism uses Duff's Device, inserting cases into the
 * code. The code defined by user when calling this macro is
 * therefore executed in a switch statement. The macro uses a
 * GCC-only predefined preprocessor macro: __COUNTER__. Without
 * it, user would have to pass a unique value to CARALLEL macros.
 * The __COUNTER__ macro is also supported by clang and Visual
 * Studio. The parallelism should work, even if code block isn't
 * certain to be reached by normal program flow, because cases
 * in switch statements are basically hidden gotos. Other factor
 * is that this version only supports methods taking no arguments
 * and returning nothing. Though with sufficient macro magic
 * adding more features should be possible.
 */
# define CARALLEL_ENABLED( name, ... ) \
    typedef struct { \
        int64_t UID; \
        carallel_queue_t * q; \
    } carallel_##name##_arg_t; \
    void carallel_ready_##name( \
        int64_t const UID, \
        carallel_queue_t * const q, \
        bool const main \
    ); \
    void name( void ) \
    { \
        carallel_queue_t q = { NULL }; \
        carallel_ready_##name( -1, &q, true ); \
    } \
    void * carallel_thread_##name( void * const p ) \
    { \
        carallel_##name##_arg_t * const a = p; \
        assert( NULL != a ); \
        carallel_ready_##name( a->UID, a->q, false ); \
        free( a ); return NULL; \
    } \
    void carallel_ready_##name( \
        int64_t const UID, \
        carallel_queue_t * const q, \
        bool const main \
    ) \
    { \
        switch( UID ) \
        { default: __VA_ARGS__ } \
        if( true == main ) \
        { \
            while( NULL != q->head ) \
            { \
                pthread_t * id = carallel_queue_get( q ); \
                pthread_join( *id, NULL ); \
                free( id ); \
            } \
        } \
    }

/**
 * \def CARALLEL
 * \brief Causes code block defined within to execute in new thread.
 * \param name Name of the method to which the code block belongs.
 * \param ... Code block to be parallelized.
 * \remarks Code block should be legal inside a method's scope.
 *
 * This macro uses GCC's __COUNTER__ macro as unique identifier for
 * a block of code, which it will attempt to run in separate thread.
 * Setting up this parallelization allocates memory for internal
 * data structures. If that allocation fails, the macro bails out
 * without creating a separate thread. Preprocessor macro __COUNTER__
 * allows to have unique identifier inside a single compilation unit.
 * The macro basically creates an case in a switch statement. This
 * tests whether exectution is in the main thread and the UID passed is
 * the same as the one belonging to the block. If so, the code inside
 * that if block is executed, after which the method returns. This may
 * limit usage in not-so trivial scenarios.  With sufficient macro
 * wizardry some obstacles may be removed and functionality extended.
 */
# define CARALLEL( name, ... ) \
    case __COUNTER__: \
        do { \
            int64_t const uid = __COUNTER__ - 1; \
            if(( false == main ) && ( uid == UID )) \
            { __VA_ARGS__ return; } \
            if( false == main ) { break; } \
            pthread_t * id = malloc( sizeof( pthread_t )); \
            carallel_##name##_arg_t * p = \
                malloc( sizeof( carallel_##name##_arg_t )); \
            p->UID = uid; p->q = q; \
            if(( NULL == id ) || ( NULL == p )) \
            { free( id ); free( p ); break; } \
            if( 0 == pthread_create( \
                id, NULL, carallel_thread_##name, p \
            )) { carallel_queue_put( q, id ); } \
        } while( false )

#endif /* CARALLEL_H__ */

