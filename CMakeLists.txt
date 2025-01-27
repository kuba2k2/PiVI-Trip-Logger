cmake_minimum_required(VERSION 3.20)

project(projekt C)

find_package(SQLite3 REQUIRED)

file(GLOB SOURCES "src/*.c" "src/**/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "src/")
target_link_libraries(${PROJECT_NAME} PUBLIC SQLite::SQLite3 pthread)
