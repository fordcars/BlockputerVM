set(SOURCES
	main.cpp
	VM.cpp
)

set(HEADERS
    getopt.h
	VM.hpp
)

add_executable(
	BlockputerVM
	${SOURCES}
	${HEADERS}
)

target_include_directories(BlockputerVM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})