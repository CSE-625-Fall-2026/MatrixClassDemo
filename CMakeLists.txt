cmake_minimum_required(VERSION 3.16)

project(
    MatrixClassDemo
    VERSION 2.0.0
    DESCRIPTION "A small matrix class for teaching sequential implementations"
    LANGUAGES CXX
)

include(GNUInstallDirs)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

add_library(matrix_class_demo INTERFACE)
add_library(MatrixClassDemo::matrix ALIAS matrix_class_demo)
set_target_properties(matrix_class_demo PROPERTIES EXPORT_NAME matrix)

target_compile_features(matrix_class_demo INTERFACE cxx_std_17)
target_include_directories(
    matrix_class_demo
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(
    MATRIX_CLASS_DEMO_BUILD_EXAMPLE
    "Build the MatrixClassDemo example"
    ${PROJECT_IS_TOP_LEVEL}
)

if(MATRIX_CLASS_DEMO_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    add_subdirectory(test)
endif()

install(
    TARGETS matrix_class_demo
    EXPORT MatrixClassDemoTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    EXPORT MatrixClassDemoTargets
    FILE MatrixClassDemoTargets.cmake
    NAMESPACE MatrixClassDemo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatrixClassDemo
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MatrixClassDemoConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MatrixClassDemoConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MatrixClassDemoConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatrixClassDemo
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/MatrixClassDemoConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/MatrixClassDemoConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MatrixClassDemo
)
