
message(STATUS "in loglib show PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# 搜索源文件
file(GLOB SOURCES "src/*.cpp")

# 创建静态库
add_library(loglib STATIC ${SOURCES})

# 设置包含目录
message(STATUS "in loglib show CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")

target_include_directories(loglib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)


# 设置库输出目录
message(STATUS "in loglib show CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
set_target_properties(loglib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
