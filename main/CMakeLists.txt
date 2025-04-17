# Collect all source files in the current directory
file(GLOB MAIN_SOURCES "*.cpp" "*.c")

idf_component_register(SRCS ${MAIN_SOURCES} INCLUDE_DIRS "." "../include")
