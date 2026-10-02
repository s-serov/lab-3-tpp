cmake_minimum_required(VERSION 3.20)
project(Lab3 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(UNICODE _UNICODE)
add_compile_options(/utf-8)

# Завдання 1: Калькулятор f(x, y) = x * y + 3
add_executable(Task1 WIN32 task1.cpp)
target_link_libraries(Task1 PRIVATE user32 gdi32)

# Завдання 2: Векторна графіка з меню (Заміна об'єкта)
add_executable(Task2 WIN32 task2.cpp)
target_link_libraries(Task2 PRIVATE user32 gdi32)