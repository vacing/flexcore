CMAKE_MINIMUM_REQUIRED( VERSION 3.0  )
PROJECT( flexcore_test  )

set(FLEXCORE_LIB_PATH "../")
include_directories(${FLEXCORE_LIB_PATH})
link_directories(${FLEXCORE_LIB_PATH}/build/flexcore/)

add_executable(main main.cpp)
target_link_libraries(main flexcore)
target_link_libraries(main pthread)


add_executable(integ integ.cpp)
target_link_libraries(integ flexcore)
target_link_libraries(integ pthread)


