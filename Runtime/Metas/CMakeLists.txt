cmake_minimum_required (VERSION 2.8)
project(lcmake)

# set
set(1_VERSION_MAJOR 1)
set(1_VERSION_MINOR 0)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# build
include(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
check_function_exists(pow USE_MYMATH)

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

option(USE_MYMATH "Use provided math implementation" ON)

# build
if( USE_MYMATH )
	include_directories("${PROJECT_SOURCE_DIR}/math")
	add_subdirectory(./math)
	set(EXTRA_LIBS ${EXTRA_LIBS} math)
endif(USE_MYMATH)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} DIR_SRCS)

add_executable(${PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})

# install
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/config.h" DESTINATION include)

# test
macro(do_test arg1 arg2 result)
	add_test(test_${arg1}_${arg2} ${PROJECT_NAME} ${arg1} ${arg2})
	set_tests_properties(test_${arg1}_${arg2} PROPERTIES PASS_REGULAR_EXPRESSION "is ${result}")
endmacro(do_test)

enable_testing()

add_test(test_usage ${PROJECT_NAME})
set_tests_properties(test_usage PROPERTIES PASS_REGULAR_EXPRESSION "Usage: .* base exponent")

add_test(test_5_2 ${PROJECT_NAME} 5 2)
set_tests_properties(test_5_2 PROPERTIES PASS_REGULAR_EXPRESSION "is 25")

do_test(2 10 1024)
do_test(5 2 25)

# pack
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${1_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${1_VERSION_MINOR}")
include(CPack)

