find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        URL_HASH SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

include(FetchContent)
if(NOT TARGET ArbitraryInteger::ArbitraryInteger)
    FetchContent_Declare(arbitrary_integer
        GIT_REPOSITORY https://github.com/AbstractClassroom-CPP/ArbitraryInteger.git
        GIT_TAG eaee528f09a32231ef9841dd2a2c90d2a6ece234 # v4.0.0
    )
    FetchContent_MakeAvailable(arbitrary_integer)
endif()
if(NOT TARGET RationalNumber::RationalNumber)
    FetchContent_Declare(rational_number
        GIT_REPOSITORY https://github.com/AbstractClassroom-CPP/RationalNumber.git
        GIT_TAG c9d841504443b92c476527170d5a1ba4b9ab16cb # v5.0.0
    )
    FetchContent_MakeAvailable(rational_number)
endif()

add_executable(matrix_tests
    MatrixTests.cpp AugmentTests.cpp PowerTests.cpp RrefTests.cpp NumberTypeTests.cpp
)
target_link_libraries(matrix_tests PRIVATE
    MatrixClassDemo::matrix GTest::gtest_main
    ArbitraryInteger::ArbitraryInteger RationalNumber::RationalNumber
)

include(GoogleTest)
gtest_discover_tests(matrix_tests)
