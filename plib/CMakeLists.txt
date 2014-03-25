# plib:CMakeLists.txt

cmake_minimum_required( VERSION 2.8 )
project( plib )

add_definitions( -ansi -Wall -Wall -Wextra -Werror )

include_directories( BEFORE . )

set( INC
    p_btree.h
    p_chrono.h
    p_csv.h
    p_dict.h
    p_dllist.h
    p_h.h
    p_llabs.h
    p_memlist.h
    p_mempool.h
    p_mutex.h
    p_sllist.h
    p_string.h
    p_vector.h
    )

set( SRC
    p_btree.c
    p_chrono.c
    p_csv.c
    p_dict.c
    p_llabs.c
    p_memlist.c
    p_mempool.c
    p_sllist.c
    p_string.c
    p_vector.c
    )

add_library( plib SHARED ${SRC} )

install( FILES ${INC} DESTINATION include/plib )
install( TARGETS plib LIBRARY DESTINATION lib )

# vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 :
