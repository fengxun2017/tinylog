set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

#重新定义当前目标的源文件的__FILE__宏
function(redefine_file_macro targetname)
    #获取当前目标的所有源文件
    get_target_property(source_files "${targetname}" SOURCES)
    #遍历源文件
    foreach(sourcefile ${source_files})
        #获取当前源文件的编译参数
        message(STATUS "file:${sourcefile}")
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        #获取当前文件的绝对路径
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        #将绝对路径中的项目路径替换成空,得到源文件相对于项目路径的相对路径
        string(REPLACE ${PROJECT_SOURCE_DIR}/ "${PROJECT_NAME}/" relpath ${filepath})
        #将我们要加的编译参数(__FILE__定义)添加到原来的编译参数里面
        list(APPEND defs "__FILE__=\"${relpath}\"")
        #重新设置源文件的编译参数
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs}
            )

    endforeach()
endfunction()


# 指定CMake编译最低要求版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "-O1 -msse2 -std=c++11")
add_definitions(-Wno-builtin-macro-redefined)

# 给项目命名
PROJECT(tinlog)

# 收集c/c++文件并赋值给变量SRC_LIST_CPP  ${PROJECT_SOURCE_DIR}代表工程根目录
FILE(GLOB SRC_LIST_CPP ${PROJECT_SOURCE_DIR}/../src/*.cpp)

message(STATUS "current_dir:${PROJECT_SOURCE_DIR}")


# 指定头文件目录
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../inc)
# INCLUDE_DIRECTORIES(/gperftools/include)

FILE(GLOB SRC_test_async_logging  ${PROJECT_SOURCE_DIR}/test_async_logging.cpp)
FILE(GLOB SRC_test_buffer_queue  ${PROJECT_SOURCE_DIR}/test_buffer_queue.cpp)
FILE(GLOB SRC_test_logging  ${PROJECT_SOURCE_DIR}/test_logging.cpp)


add_library(log_lib STATIC ${SRC_LIST_CPP})


#add_library(profiler STATIC IMPORTED)
#set_property(TARGET profiler PROPERTY IMPORTED_LOCATION /gperftools/lib/libprofiler.a)

# 设置单元测试可执行文件输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output_bin)
message(STATUS "unittest_out:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# 单元测试可执行文件生成
add_executable(test_async_logging ${SRC_test_async_logging})
redefine_file_macro(test_async_logging)
target_link_libraries(test_async_logging log_lib)

add_executable(test_buffer_queue ${SRC_test_buffer_queue})
redefine_file_macro(test_buffer_queue)
target_link_libraries(test_buffer_queue log_lib)

add_executable(test_logging ${SRC_test_logging})
redefine_file_macro(test_logging)
target_link_libraries(test_logging log_lib)


#cmake -D CMAKE_C_COMPILER=/opt/compiler/gcc-8.2/bin/gcc -D CMAKE_CXX_COMPILER=/opt/compiler/gcc-8.2/bin/g++ ..