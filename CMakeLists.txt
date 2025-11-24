cmake_minimum_required(VERSION 3.10)

project(ISS_Test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.3.2
)
FetchContent_MakeAvailable(Catch2)

include(Catch)

set(SOURCES
    src/BinaryPacketReader.cpp
    src/PacketReceiver.cpp
    src/PacketReader.cpp
    src/TextPacketReader.cpp
    src/Utils.cpp)

set(HEADERS
    include/BinaryPacketReader.h
    include/ICallback.h
    include/IReceiver.h
    include/PacketReceiver.h
    include/PacketReader.h
    include/TextPacketReader.h
    include/Types.h
    include/Utils.h)

add_executable(main ${SOURCES} ${HEADERS} src/main.cpp)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(tests tests/tests.cpp ${SOURCES} ${HEADERS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()
catch_discover_tests(tests)
