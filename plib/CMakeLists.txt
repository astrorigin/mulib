# Plib:plib/CMakeLists.txt

cmake_minimum_required( VERSION 2.8 )

project( PLIB )

add_definitions( -ansi -Wall -Wextra -Werror -pthread )

include_directories( BEFORE . )

set( P_FCGI_STDIO off
    CACHE BOOL "Compile with fcgi_stdio.h" )
if ( P_FCGI_STDIO )
    add_definitions( -DP_FCGI_STDIO )
endif()

set( P_MAKE_STATIC off
    CACHE BOOL "Make static archive library" )

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

find_package( PostgreSQL MODULE )
if ( PostgreSQL_FOUND )
    include_directories( ${PostgreSQL_INCLUDE_DIRS} )
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
    p_postgres.h
    p_regmatch.h
    p_shell.h
    p_sllist.h
    p_strdup.h
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
    #p_postgres.c
    p_regmatch.c
    p_shell.c
    p_sllist.c
    p_strdup.c
    p_string.c
    p_strnstr.c
    p_utf8.c
    p_vdict.c
    p_vector.c
    )

if ( NOT P_FCGI_STDIO )
    if ( P_MAKE_STATIC )
        add_library( plib_s STATIC ${SRC} )
        install( TARGETS plib_s LIBRARY DESTINATION lib )
        if ( PostgreSQL_FOUND )
            add_library( plib_postgres_s STATIC p_postgres.c )
            install( TARGETS plib_postgres_s LIBRARY DESTINATION lib )
        endif()
    endif()
    add_library( plib SHARED ${SRC} )
    install( TARGETS plib LIBRARY DESTINATION lib )
    if ( PostgreSQL_FOUND )
        add_library( plib_postgres SHARED p_postgres.c )
        install( TARGETS plib_postgres LIBRARY DESTINATION lib )
    endif()
    add_executable( p_i18n p_i18n.c )
    target_link_libraries( p_i18n plib -pthread )
    add_executable( p_l10n p_l10n.c )
    target_link_libraries( p_l10n plib -pthread )
    install( TARGETS p_i18n p_l10n RUNTIME DESTINATION bin )
    if ( P_MAKE_TESTS )
        add_subdirectory( tests )
    endif()
else()
    if ( P_MAKE_STATIC )
        add_library( plib_fcgi_s STATIC ${SRC} )
        install( TARGETS plib_fcgi_s LIBRARY DESTINATION lib )
    endif()
    add_library( plib_fcgi SHARED ${SRC} )
    install( TARGETS plib_fcgi LIBRARY DESTINATION lib )
endif()

install( FILES ${INC} DESTINATION include/plib )

# vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 :
