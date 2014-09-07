#include <p_json.h>

#include <p_dict.h>
#include <p_mempool.h>

const P_CHAR txt[] =
"{"
"\"varname\": 123.321,\n"
"\"testing\": false,\n"
"\"name\": \"Olga\",\n"
"\"age\": 32\n"
"}";

P_INT32
p_json_test( P_VOID )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
#endif
    const P_CHAR* nil = strchr( txt, '\0' );
    P_CHAR* p = (P_CHAR*) txt;
    p_Shell shell, *sh;

#ifndef P_NO_MEMPOOL
    P_ASSERT( p_mempool_init( &mp, 0 ))
    p_mempool_set( &mp );
#endif
    P_ASSERT( p_shell_init( &shell, P_SHELLTYPE_NULL ))

    P_ASSERT( p_json( p, nil, &shell, &p ))
    P_ASSERT( shell.type == P_SHELLTYPE_DICT )

    P_ASSERT( sh = p_dict_get( (p_Dict*)shell.p, "varname" ))
    P_ASSERT( sh->type == P_SHELLTYPE_DBL )
    P_ASSERT( *(P_DBL*)sh->p == 123.321 )

    P_ASSERT( sh = p_dict_get( (p_Dict*)shell.p, "testing" ))
    P_ASSERT( sh->type == P_SHELLTYPE_BOOL )
    P_ASSERT( *(P_BOOL*)sh->p == P_FALSE )

    P_ASSERT( sh = p_dict_get( (p_Dict*)shell.p, "name" ))
    P_ASSERT( sh->type == P_SHELLTYPE_STRING )
    P_ASSERT( !strcmp( (P_CHAR*)((p_String*)sh->p)->data, "Olga" ))

    P_ASSERT( sh = p_dict_get( (p_Dict*)shell.p, "age" ))
    P_ASSERT( sh->type == P_SHELLTYPE_INT32 )
    P_ASSERT( *(P_INT32*)sh->p == 32 )

    p_shell_fini( &shell );
#ifndef P_NO_MEMPOOL
    p_mempool_debug( &mp );
    p_mempool_fini( &mp );
#endif
    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_json_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
