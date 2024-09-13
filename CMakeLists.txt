cmake_minimum_required(VERSION 3.5.0)
project(SeniorDesignMatchingC++ VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(SOURCES main.cc matchingCode.cc fundamentalMatrix.cc utils.cc)
add_executable(SeniorDesignMatchingC++ ${SOURCES}) 


target_link_libraries( SeniorDesignMatchingC++ ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
