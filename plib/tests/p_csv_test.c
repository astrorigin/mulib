#include <p_csv.h>

P_INT32
p_csv_test()
{
    P_CHAR* txt = (P_CHAR*)"aa b \nd e\tf\n";
    p_CsvContext csv;
    P_INT32 res;
    P_CHAR* st, *nd;
    P_CHAR buf[24];

    p_csv_init( &csv, txt, NULL, NULL );

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_WORD )

    p_csv_word( &csv, &st, &nd );
    strncpy( buf, st, nd-st+1 );
    buf[ nd-st+1 ] = '\0';
    P_ASSERT( !strcmp( "aa", buf ) )

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_WORD )

    p_csv_word_copy( &csv, buf, 24 );
    P_ASSERT( !strcmp( buf, "b" ) )

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_EOL )

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_WORD )

    p_csv_line_copy( &csv, buf, 24 );
    P_ASSERT( !strcmp( buf, "d e\tf" ) )

    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_csv_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
