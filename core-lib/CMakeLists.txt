set(SOURCE_DIR src)
set(INCLUDE_DIR include/core-lib)
set(SOURCE_LIST
        ${SOURCE_DIR}/util.c
        )
set(HEADER_LIST
        ${INCLUDE_DIR}/util.h
        ${INCLUDE_DIR}/objects.h
        ${INCLUDE_DIR}/api_functions.h
        )

add_library(core-lib ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(core-lib PUBLIC include)

find_library(MEM_MANAGER mem_manager REQUIRED)
target_link_libraries(core-lib PUBLIC ${MEM_MANAGER})
