
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "lib.h"

int main( int argc,  char *argv[] ) {
    printf("FixME\n");
    //return 0;
    XINE::Lib lib;
    QString str = QString::fromLatin1( argv[1] );
    lib.play( str );
    for (;;) sleep( 60 );
    return 0;
}
