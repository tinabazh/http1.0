set(SOURCE_DIR src)
set(INCLUDE_DIR include/http)
set(SOURCE_LIST
        ${SOURCE_DIR}/handlers.c
        ${SOURCE_DIR}/request.c)
set(HEADER_LIST
        ${INCLUDE_DIR}/handlers.h
        ${INCLUDE_DIR}/request.h)

add_library(http ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(http PUBLIC include)
target_include_directories(http PRIVATE include/http)
