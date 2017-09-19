cmake_minimum_required (VERSION 3.1)
project (pokepronouns)

add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

include_directories(vendor/libtwittercpp/src vendor/yaml-cpp/include)
add_executable(pokepronouns pokepronouns.cpp)
set_property(TARGET pokepronouns PROPERTY CXX_STANDARD 11)
set_property(TARGET pokepronouns PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(pokepronouns yaml-cpp twitter++)
