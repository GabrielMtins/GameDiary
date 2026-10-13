cmake_minimum_required(VERSION 3.16)

project(backlog)

file(GLOB_RECURSE all_SRCS
		"${PROJECT_SOURCE_DIR}/src/*.c"
        )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_package(Curses REQUIRED)

# Executável
add_executable(${PROJECT_NAME}
	${all_SRCS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SQLite::SQLite3
	${CURSES_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
	include/
	${CURSES_INCLUDE_DIRS}
	${SQLITE3_INCLUDE_DIRS}
)

# Flags de compilação (opcional, mas recomendado)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
)
