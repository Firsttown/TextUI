cmake_minimum_required(VERSION 3.10)
project(TextUI_Examples)

# Установка стиля C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим исходные файлы фреймворка
file(GLOB_RECURSE TEXTUI_SOURCES 
    "${CMAKE_SOURCE_DIR}/src/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/widgets/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/window/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/input/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/platform/*.cpp"
)

# Исключаем файлы, которые не нужно компилировать
list(FILTER TEXTUI_SOURCES EXCLUDE REGEX ".*_new\.cpp$")

# Добавляем исполняемый файл для примера
add_executable(TextUI_Example main.cpp)

# Подключаем исходные файлы фреймворка
target_sources(TextUI_Example PRIVATE ${TEXTUI_SOURCES})

# Включаем заголовочные файлы фреймворка
target_include_directories(TextUI_Example PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Устанавливаем стандартные флаги компиляции
if(MSVC)
    target_compile_options(TextUI_Example PRIVATE /W4)
else()
    target_compile_options(TextUI_Example PRIVATE -Wall -Wextra)
endif()

# Устанавливаем точку входа для Windows
if(WIN32)
    set_target_properties(TextUI_Example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
