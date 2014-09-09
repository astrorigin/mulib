# plib:CMakeLists.txt

cmake_minimum_required( VERSION 2.8 )

project( PLIB )

add_definitions( -ansi -Wall -Wextra -Werror -pthread )

include_directories( BEFORE . )

set( P_FCGI_STDIO off
    CACHE BOOL "Compile with fcgi_stdio.h" )
if ( P_FCGI_STDIO )
    add_definitions( -DP_FCGI_STDIO )
endif()

set( P_MAKE_TESTS off
    CACHE BOOL "Compile tests" )
if ( P_MAKE_TESTS )
    if ( NOT ( CMAKE_BUILD_TYPE STREQUAL Debug ))
        message( FATAL_ERROR "Tests must be made in Debug mode." )
    endif()
endif()

set( P_NO_MEMPOOL off
    CACHE BOOL "Compile without mempool" )
if ( P_NO_MEMPOOL )
    add_definitions( -DP_NO_MEMPOOL )
endif()

set( INC
    p_btree.h
    p_chrono.h
    #p_csv.h
    p_dict.h
    p_dllist.h
    p_fbuf.h
    p_g11n.h
    p_h.h
    p_json.h
    p_llabs.h
    p_memlist.h
    p_mempool.h
    p_mutex.h
    p_shell.h
    p_sllist.h
    p_string.h
    p_strnstr.h
    p_utf8.h
    p_vdict.h
    p_vector.h
    )

set( SRC
    p_btree.c
    p_chrono.c
    #p_csv.c
    p_dict.c
    p_fbuf.c
    p_g11n.c
    p_json.c
    p_llabs.c
    p_memlist.c
    p_mempool.c
    p_shell.c
    p_sllist.c
    p_string.c
    p_strnstr.c
    p_utf8.c
    p_vdict.c
    p_vector.c
    )

if ( NOT P_FCGI_STDIO )
    add_library( plib SHARED ${SRC} )
    install( TARGETS plib LIBRARY DESTINATION lib )
    add_executable( p_i18n p_i18n.c )
    target_link_libraries( p_i18n plib -pthread )
    add_executable( p_l10n p_l10n.c )
    target_link_libraries( p_l10n plib -pthread )
    install( TARGETS p_i18n p_l10n RUNTIME DESTINATION bin )
    if ( P_MAKE_TESTS )
        add_subdirectory( tests )
    endif()
else()
    add_library( plib_fcgi SHARED ${SRC} )
    install( TARGETS plib_fcgi LIBRARY DESTINATION lib )
endif()

install( FILES ${INC} DESTINATION include/plib )

# vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 :
