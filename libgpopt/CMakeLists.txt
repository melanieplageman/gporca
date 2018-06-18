# Copyright (c) 2015, Pivotal Software, Inc. All Rights Reserved.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libgpcost/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libnaucrates/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../server/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../server/include)

# Some tests use C99 long long integer constants, but the C++ dialect may not
# explicitly include C99. We supress such errors here.
check_cxx_compiler_flag("-Wno-long-long"
                        COMPILER_HAS_WNO_LONG_LONG)
if (COMPILER_HAS_WNO_LONG_LONG)
    set_source_files_properties(../server/src/unittest/dxl/statistics/CStatisticsTest.cpp
                                ../server/src/unittest/dxl/statistics/CBucketTest.cpp
                                ../server/src/unittest/dxl/statistics/CPointTest.cpp
                                ../server/src/unittest/dxl/statistics/CHistogramTest.cpp
                                ../server/src/unittest/dxl/statistics/CMCVTest.cpp
                                ../server/src/unittest/dxl/statistics/CJoinCardinalityTest.cpp
                                ../server/src/unittest/dxl/statistics/CFilterCardinalityTest.cpp
                                ../server/src/unittest/gpopt/base/CConstraintTest.cpp
                                ../server/src/unittest/gpopt/metadata/CPartConstraintTest.cpp
                                PROPERTIES COMPILE_FLAGS "-Wno-long-long")
endif()

# Generate version-number header.
configure_file(version.h.in
        ${PROJECT_BINARY_DIR}/libgpopt/include/libgpopt/version.h)

# for the generated config.h file.
include_directories(${PROJECT_BINARY_DIR}/libgpos/include/)

file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl
                       ${CMAKE_CURRENT_SOURCE_DIR}/../server/include/*.inl)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/../server/src/*.cpp)

# Add headers to make them visible in some IDEs (Clion, VS, Xcode)
list(APPEND srcs ${hdrs})

add_library(gpopt ${srcs})

target_link_libraries(gpopt
                      gpdbcost
                      naucrates
                      ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
                      gpos
                      ${XERCES_LIBRARIES}
                      ${GPORCA_SOLARIS_EXTRA_LIBS})

set_target_properties(gpopt PROPERTIES
                      SOVERSION ${GPORCA_ABI_VERSION}
                      VERSION ${GPORCA_VERSION_STRING})

install(TARGETS gpopt DESTINATION lib)
install(DIRECTORY include/gpopt DESTINATION include)
install(FILES "${PROJECT_BINARY_DIR}/libgpopt/include/libgpopt/version.h"
        DESTINATION include/gpopt)
