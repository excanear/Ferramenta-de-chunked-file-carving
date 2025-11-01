cmake_minimum_required(VERSION 3.12)
project(ChunkedFileCarver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Definir diretórios
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Incluir diretórios de cabeçalho
include_directories(${INCLUDE_DIR})

# Listar arquivos fonte
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${INCLUDE_DIR}/*.h")

# Criar executável
add_executable(chunked_carver ${SOURCES} ${HEADERS})

# Configurações específicas do compilador
if(MSVC)
    target_compile_options(chunked_carver PRIVATE /W4)
else()
    target_compile_options(chunked_carver PRIVATE -Wall -Wextra -pedantic)
endif()

# Configurações de Debug/Release
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Adicionar threads para suporte a multithreading
find_package(Threads REQUIRED)
target_link_libraries(chunked_carver Threads::Threads)

# Configuração para criar diretório de saída
add_custom_command(TARGET chunked_carver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
    "${CMAKE_CURRENT_SOURCE_DIR}/output")