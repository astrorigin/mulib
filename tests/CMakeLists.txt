# mulib:tests/CMakeLists.txt

include_directories(BEFORE ..)

add_executable(m_array_test m_array_test.c)
add_executable(m_btree_output m_btree_output.c)
add_executable(m_btree_test m_btree_test.c)
add_executable(m_dict_test m_dict_test.c)
add_executable(m_sllist_test m_sllist_test.c)
add_executable(m_string_test m_string_test.c)


target_link_libraries(m_array_test mu)
target_link_libraries(m_btree_output mu)
target_link_libraries(m_btree_test mu)
target_link_libraries(m_dict_test mu)
target_link_libraries(m_sllist_test mu)
target_link_libraries(m_string_test mu)

# vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 :
