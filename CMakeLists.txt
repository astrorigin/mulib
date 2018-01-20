# mulib:CMakeLists.txt

cmake_minimum_required(VERSION 3.7)

project(mulib)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:\\Program Files (x86)\\mulib")
  else()
    set(CMAKE_INSTALL_PREFIX "/usr/local")
  endif()
endif()

set(M_MAKE_STATIC off CACHE BOOL "Make static archive library")
set(M_MAKE_TESTS off CACHE BOOL "Compile tests")
set(M_NO_MEMPOOL off CACHE BOOL "Omit memory pools")

if(M_NO_MEMPOOL)
  add_definitions(-DM_NO_MEMPOOL)
endif()

set(M_TRACE_MODE off CACHE BOOL "Enable traces (global)")
mark_as_advanced(M_TRACE_MODE)

set(M_TRACE_ARRAY off CACHE BOOL "Enable Array traces")
mark_as_advanced(M_TRACE_ARRAY)
set(M_TRACE_BTREE off CACHE BOOL "Enable BTree traces")
mark_as_advanced(M_TRACE_BTREE)
set(M_TRACE_DICT off CACHE BOOL "Enable Dict traces")
mark_as_advanced(M_TRACE_DICT)
set(M_TRACE_MEMCNT off CACHE BOOL "Enable MemCnt traces")
mark_as_advanced(M_TRACE_MEMCNT)
set(M_TRACE_MEMPOOL off CACHE BOOL "Enable MemPool traces")
mark_as_advanced(M_TRACE_MEMPOOL)
set(M_TRACE_SLLIST off CACHE BOOL "Enable SLList traces")
mark_as_advanced(M_TRACE_SLLIST)

if(M_TRACE_MODE)
  add_definitions(-DM_TRACE_MODE)
  if(M_TRACE_ARRAY)
    add_definitions(-DM_TRACE_ARRAY)
  endif()
  if(M_TRACE_BTREE)
    add_definitions(-DM_TRACE_BTREE)
  endif()
  if(M_TRACE_DICT)
    add_definitions(-DM_TRACE_DICT)
  endif()
  if(M_TRACE_MEMCNT)
    add_definitions(-DM_TRACE_MEMCNT)
  endif()
  if(M_TRACE_MEMPOOL)
    add_definitions(-DM_TRACE_MEMPOOL)
  endif()
  if(M_TRACE_SLLIST)
    add_definitions(-DM_TRACE_SLLIST)
  endif()
endif()

if(MSVC)
  add_definitions(-DM_MAKE_DLL -D_CRT_SECURE_NO_WARNINGS)
else()
  add_definitions(-std=c99 -Wall -Wextra -Werror -pthread)
  if(NOT WIN32)
    add_definitions(-fPIC)
  endif()
endif()

include_directories(BEFORE .)

set(INC
  m_array.h
  m_btree.h
  m_btree_priv.h
  m_dict.h
  m_dict_priv.h
  m_h.h
  m_llabs.h
  m_memcnt.h
  m_memcnt_priv.h
  m_mempool.h
  m_mempool_priv.h
  m_mutex.h
  m_sllist.h
  m_strdup.h
  m_string.h
  m_strnstr.h
  m_utf8.h)

set(SRC
  m_array.c
  m_btree.c
  m_dict.c
  m_llabs.c
  m_memcnt.c
  m_mempool.c
  m_sllist.c
  m_strdup.c
  m_string.c
  m_strnstr.c
  m_utf8.c)

if(NOT MSVC)
  list(APPEND INC
    m_workerpool.h)
  list(APPEND SRC
    m_workerpool.c)
endif()

if(M_MAKE_STATIC)
  add_library(mu STATIC ${SRC})
else()
  add_library(mu SHARED ${SRC})
endif()

if(NOT MSVC)
  target_link_libraries(${LIBNAM} rt)
endif()

if(MSVC)
  install(TARGETS ${LIBNAM} RUNTIME DESTINATION lib)
else()
  install(TARGETS ${LIBNAM} LIBRARY DESTINATION lib)
endif()

install(FILES ${INC} DESTINATION include/mu)

if(M_MAKE_TESTS)
  add_subdirectory(tests)
endif()

# vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 :
