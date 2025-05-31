cmake_minimum_required(VERSION 3.10)  # Минимальная версия CMake

project(MyProject)  # Название проекта

set(CMAKE_CXX_STANDARD 17)  # Используем C++17

# Добавляем исполняемый файл
add_executable(my_program main.cpp)

# Если есть другие исходные файлы:
# add_executable(my_program main.cpp file1.cpp file2.cpp)