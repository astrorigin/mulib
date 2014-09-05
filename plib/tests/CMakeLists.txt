#plib:tests/CMakeLists.txt

include_directories( BEFORE .. )

add_executable( p_btree_test p_btree_test.c )
add_executable( p_csv_test p_csv_test.c )
add_executable( p_dict_test p_dict_test.c )
add_executable( p_g11n_test p_g11n_test.c )
add_executable( p_mempool_test p_mempool_test.c )
add_executable( p_mempool_test_01 p_mempool_test_01.c )
add_executable( p_mempool_test_02 p_mempool_test_02.c )
add_executable( p_shell_test p_shell_test.c )
add_executable( p_string_test p_string_test.c )
add_executable( p_utf8_test p_utf8_test.c )
add_executable( p_vector_test p_vector_test.c )

target_link_libraries( p_btree_test plib -pthread )
target_link_libraries( p_csv_test plib -pthread )
target_link_libraries( p_dict_test plib -pthread )
target_link_libraries( p_g11n_test plib -pthread )
target_link_libraries( p_mempool_test plib -pthread )
target_link_libraries( p_mempool_test_01 plib -pthread )
target_link_libraries( p_mempool_test_02 plib -pthread )
target_link_libraries( p_shell_test plib -pthread )
target_link_libraries( p_string_test plib -pthread )
target_link_libraries( p_utf8_test plib -pthread )
target_link_libraries( p_vector_test plib -pthread )

# vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 :
