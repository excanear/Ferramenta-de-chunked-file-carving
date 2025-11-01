#!/bin/bash

# Script de build para Linux/macOS

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_TYPE="${1:-Release}"
CLEAN="${2:-false}"

echo "=== Chunked File Carver Build Script ==="
echo "Build Type: $BUILD_TYPE"

# Limpa build anterior se solicitado
if [ "$CLEAN" = "clean" ] && [ -d "$BUILD_DIR" ]; then
    echo "Limpando build anterior..."
    rm -rf "$BUILD_DIR"
fi

# Cria diretório de build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configura CMake
echo "Configurando projeto com CMake..."
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Compila projeto
echo "Compilando projeto..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Compilação concluída com sucesso!"

# Verifica se o executável foi criado
EXECUTABLE_PATH="$BUILD_DIR/chunked_carver"
if [ -f "$EXECUTABLE_PATH" ]; then
    echo "Executável criado: $EXECUTABLE_PATH"
    
    # Executa versão para verificar
    echo "Verificando executável..."
    "$EXECUTABLE_PATH" --version
else
    echo "Erro: Executável não encontrado em $EXECUTABLE_PATH"
    exit 1
fi

echo "Build script concluído!"