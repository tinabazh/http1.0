set(SOURCE_DIR src)
set(INCLUDE_DIR include/core-lib)
set(SOURCE_LIST
        ${SOURCE_DIR}/receiver.c
        ${SOURCE_DIR}/util.c
        )
set(HEADER_LIST
        ${INCLUDE_DIR}/api_functions.h
        ${INCLUDE_DIR}/objects.h
        ${INCLUDE_DIR}/receiver.h
        ${INCLUDE_DIR}/util.h
        )

add_library(core-lib ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(core-lib PUBLIC include)
target_include_directories(core-lib PRIVATE include/core-lib)

find_library(MEM_MANAGER mem_manager REQUIRED)
target_link_libraries(core-lib PUBLIC ${MEM_MANAGER})
