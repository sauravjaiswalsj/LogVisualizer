cmake_minimum_required(VERSION 3.10)
project(backend)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Find required packages
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SQLite3 REQUIRED)

# Add Crow
add_subdirectory(include/Crow)
include_directories(${CMAKE_SOURCE_DIR}/include/Crow/include)


include_directories(/usr/local/homebrew/Cellar/boost/1.78.0_1/include)
link_directories(/usr/local/homebrew/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Crow/include)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cpp_redis/includes)

add_executable(backend src/main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Crow::Crow
    Threads::Threads
    Boost::system
    Boost::filesystem
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    ${SQLite3_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SQLite3_INCLUDE_DIRS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_PREFIX_PATH "/usr/local")
include(CPack)


#