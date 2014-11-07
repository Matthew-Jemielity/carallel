#include <carallel/carallel.h>
#include <stdio.h>

CARALLEL_ENABLED( foo )
{
    CARALLEL( foo,
        for( int i = 0; i < 10; ++i ) { puts( "thread 1" ); }
        puts( "after for in thread 1" );
    );
    CARALLEL( foo,
        for( int i = 0; i < 10; ++i ) { puts( "thread 2" ); }
        puts( "after for in thread 2" );
    );
    CARALLEL( foo,
        for( int i = 0; i < 10; ++i ) { puts( "thread 3" ); }
        puts( "after for in thread 3" );
    );

    puts( "thread main" );

    CARALLEL_RETURN();
}

CARALLEL_ENABLED( baz )
{
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL( baz, puts( "test" ); );
    CARALLEL_RETURN();
}

int main() { foo(); baz(); return 0; }

