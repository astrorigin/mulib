#include <p_utf8.h>

#if 0 /* unused */
static void
showbits(int x)
{
    int i;
    for(i=(sizeof(int)*8)-1; i>=0; i--)
        (x&(1<<i))?putchar('1'):putchar('0');

	printf("\n");
}
#endif

static void
showbitsc(char x)
{
    int i;
    for(i=(sizeof(char)*8)-1; i>=0; i--)
        (x&(1<<i))?putchar('1'):putchar('0');

	printf("\n");
}

static void
showbitsstr( char* s )
{
    size_t i;
    size_t n = strlen( s );
    for ( i = 0; i < n; ++i )
        showbitsc( *s++ );
}

P_INT32
p_utf8_test( P_VOID )
{
    char buf[5];

    P_ASSERT( sizeof( char ) == 1 )
    P_ASSERT( sizeof( int ) == 4 )

    /* ascii */
    P_ASSERT( p_utf8_octetnum( 0x45 ) == 1 )
    printf( "ASCII 'E': %s\n", p_utf8_encode( 0x45, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* Some latin-1 */
    P_ASSERT( p_utf8_octetnum( 0xE2 ) == 2 )
    printf( "French 'â': %s\n", p_utf8_encode( 0xE2, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* hebrew */
    P_ASSERT( p_utf8_octetnum( 0x5D0 ) == 2 )
    printf( "Hebrew aleph: %s\n", p_utf8_encode( 0x5D0, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* samaritan */
    P_ASSERT( p_utf8_octetnum( 0x800 ) == 3 )
    printf( "Samaritan alaf: %s\n", p_utf8_encode( 0x800, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* Linear b syllabe KA */
    P_ASSERT( p_utf8_octetnum( 0x1000F ) == 4 )
    printf( "Syllabe KA: %s\n", p_utf8_encode( 0x1000F, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_utf8_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
