cmake_minimum_required(VERSION 3.4)
project(cicq VERSION 1.0)

# C++标准
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(DOCUMENTATION False)

find_package(Curses REQUIRED)

# 包含的头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/cicq)
include_directories(${PROJECT_SOURCE_DIR}/libs/cjson)
include_directories(${PROJECT_SOURCE_DIR}/libs/)
include_directories(${PROJECT_SOURCE_DIR}/libs/ncurses/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/libevent/include)
include_directories(${PROJECT_BINARY_DIR}/libs/libevent/include)
include_directories(${CURSES_INCLUDE_DIR})

# 枚举头文件
file(GLOB_RECURSE INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/cicq/*.h)

# 指定引用的外部库的搜索路径
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libs)

# disable cjson uninstall target
set(ENABLE_CJSON_UNINSTALL False)

# 定义客户端和服务端需要的共用源文件
set(COMMON_SOURCES
    src/json_wrapper.c
)

set(thirdparty_libraries
	cjson
    libevent
)

set(thirdparty_link_libraries
	cjson
    event
)

# 添加子目录
foreach(lib IN LISTS thirdparty_libraries)
	message("[thirdparty] including library ${lib}")
	add_subdirectory("libs/${lib}")
endforeach()

set(link_libraries
	${thirdparty_link_libraries}
    ${CURSES_LIBRARIES}
)

# 生成客户端
add_executable(cicq
    src/client_main.c
    src/chatroom.c
    src/client_com.c
    ${COMMON_SOURCES}
)
# 确保链接到libs目录下编译的libs
target_link_libraries(cicq PRIVATE ${link_libraries})

# 生成服务端
add_executable(cicq-server
    src/server_main.c
    src/server_proc.c
    ${COMMON_SOURCES}
)
# 确保链接到libs目录下编译的libs
target_link_libraries(cicq-server PRIVATE ${link_libraries})

# 生成测试
add_executable(cicq-test
    src/chat.c
    ${COMMON_SOURCES}
)
# 确保链接到libs目录下编译的libs
target_link_libraries(cicq-test PRIVATE ${link_libraries})
