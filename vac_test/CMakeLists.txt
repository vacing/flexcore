CMAKE_MINIMUM_REQUIRED( VERSION 3.0  )
PROJECT( flexcore_test  )

set(FLEXCORE_LIB_PATH "../")
include_directories(${FLEXCORE_LIB_PATH})
link_directories(${FLEXCORE_LIB_PATH}/build/flexcore/)
link_libraries(flexcore pthread)

add_executable(main main.cpp)
add_executable(integ integ.cpp)
add_executable(node_port node_port.cpp)


