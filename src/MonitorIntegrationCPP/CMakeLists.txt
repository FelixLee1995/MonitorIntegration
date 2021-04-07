cmake_minimum_required(VERSION 3.17)


#设定vckpg 路径
#SET(CMAKE_TOOLCHAIN_FILE, /to/your/vcpkgpath)


project(MonitorIntegration)
IF(MSVC)
IF(CMAKE_CL_64)
  set(platform x64)
  message("64bit OS: ${platform}")
ELSE(CMAKE_CL_64)
  set(platform x86)
  message("32bit OS")
ENDIF(CMAKE_CL_64)
ENDIF(MSVC)

IF(UNIX)
  message("OS UNIX")
  set(platform x64)
ENDIF(UNIX)
#设定CPP版本
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DUSE_STANDALONE_ASIO)
add_definitions(-DFMT_HEADER_ONLY)
add_definitions(-DMYDLL_EXPORTS)


include_directories(include)
include_directories(dependencies/include)

link_directories(lib)
link_directories(dependencies/lib)


#扫描src目录, 获取.c .cpp 源码文件列表
file(GLOB_RECURSE DIR_SRCS ./src/export/*.cpp)

file(GLOB_RECURSE USE_SRCS ./src/main.cpp)

ADD_LIBRARY(monitor_dll_${platform} SHARED ${DIR_SRCS}  MonitorIntegration.rc)

if(UNIX)
target_link_libraries(monitor_dll_${platform} pthread)
endif(UNIX)

set (LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/runtime)


## 可执行文件输出目录
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/runtime)

##使用源码列表生成可执行文件
ADD_EXECUTABLE(MonitorIntegration ${USE_SRCS})


target_link_libraries(MonitorIntegration monitor_dll_${platform})


#设置msvc编译选项 -- 目标是否使用静态运行时 
IF(MSVC)
  target_compile_options(MonitorIntegration PUBLIC /MT)
  target_compile_options(monitor_dll_${platform} PUBLIC /MT)

ENDIF()
