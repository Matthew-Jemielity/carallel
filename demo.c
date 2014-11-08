#include <carallel/carallel.h>
#include <stdio.h>

CARALLEL_ENABLED( foo,
    puts( "before thread 1" );
    CARALLEL( foo,
        for( int i = 0; i < 10; ++i ) { puts( "thread 1" ); }
        puts( "after for in thread 1" );
    );
    puts( "before thread 2" );
    CARALLEL( foo,
        for( int i = 0; i < 10; ++i ) { puts( "thread 2" ); }
        puts( "after for in thread 2" );
    );
    puts( "before thread 3" );
    CARALLEL( foo,
        for( int i = 0; i < 10; ++i ) { puts( "thread 3" ); }
        puts( "after for in thread 3" );
    );
    puts( "thread main" );
)

int main() { foo(); return 0; }

