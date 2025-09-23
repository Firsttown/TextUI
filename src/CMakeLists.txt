cmake_minimum_required(VERSION 3.10)
project(TextUI_Library)

# Установка стиля C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим исходные файлы библиотеки
file(GLOB_RECURSE TEXTUI_LIB_SOURCES
    "core/*.cpp"
    "widgets/*.cpp"
    "window/*.cpp"
    "input/*.cpp"
    "platform/*.cpp"
)

# Исключаем файлы, которые не нужно компилировать
list(FILTER TEXTUI_LIB_SOURCES EXCLUDE REGEX ".*_new\.cpp$")

# Создаем библиотеку
add_library(TextUI SHARED ${TEXTUI_LIB_SOURCES})

# Включаем заголовочные файлы библиотеки
target_include_directories(TextUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Устанавливаем стандартные флаги компиляции
if(MSVC)
    target_compile_options(TextUI PRIVATE /W4)
else()
    target_compile_options(TextUI PRIVATE -Wall -Wextra)
endif()

# Устанавливаем точку входа для Windows
if(WIN32)
    set_target_properties(TextUI PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
