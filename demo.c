#include <carallel/carallel.h>
#include <stdio.h>

CARALLELIZE( foo,
    puts( "before thread 1" );
    CARALLEL( foo )
    {
        for( int i = 0; i < 10; ++i ) { puts( "thread 1" ); }
        puts( "after for in thread 1" );
    }
    puts( "before thread 2" );
    CARALLEL( foo )
    {
        for( int i = 0; i < 10; ++i ) { puts( "thread 2" ); }
        puts( "after for in thread 2" );
    }
    puts( "before thread 3" );
    CARALLEL( foo )
    {
        for( int i = 0; i < 10; ++i ) { puts( "thread 3" ); }
        puts( "after for in thread 3" );
    }
    puts( "thread main" );
)

CARALLELIZE( bar,
    CARALLEL( bar ) puts( "bar 1" );
    CARALLEL( bar ) puts( "bar 2" );
    CARALLEL( bar ) puts( "bar 3" );
    CARALLEL( bar ) puts( "bar 4" );
)

int main() { foo(); bar(); return 0; }

