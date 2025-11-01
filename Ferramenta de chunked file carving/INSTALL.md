# Instruções de Instalação e Compilação

## Pré-requisitos para Windows

### Opção 1: Visual Studio (Recomendado para Windows)

1. **Baixe e instale o Visual Studio Community** (gratuito):
   - https://visualstudio.microsoft.com/pt-br/vs/community/
   
2. **Durante a instalação, selecione**:
   - "Desenvolvimento para desktop com C++"
   - "CMake tools for C++"
   - "Git for Windows"

3. **Após a instalação**:
   ```cmd
   # Abra o "Developer Command Prompt for VS"
   cd "caminho\para\Ferramenta de chunked file carving"
   mkdir build
   cd build
   cmake ..
   msbuild chunked_carver.sln
   ```

### Opção 2: MinGW-w64 + MSYS2

1. **Instale MSYS2**:
   - https://www.msys2.org/
   
2. **Abra o terminal MSYS2 e execute**:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-cmake
   pacman -S make
   ```

3. **Compile o projeto**:
   ```bash
   cd /c/caminho/para/projeto
   mkdir build && cd build
   cmake ..
   make
   ```

### Opção 3: Compilação Manual (se tiver g++ instalado)

```bash
cd "caminho\para\projeto"

# Compilação em uma linha
g++ -std=c++17 -I include src/*.cpp -o chunked_carver.exe

# Ou compilação otimizada
g++ -std=c++17 -O2 -I include src/*.cpp -o chunked_carver.exe -static-libgcc -static-libstdc++
```

## Pré-requisitos para Linux

### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake g++ make
```

### CentOS/RHEL/Fedora:
```bash
# CentOS/RHEL
sudo yum install gcc-c++ cmake make

# Fedora
sudo dnf install gcc-c++ cmake make
```

### Compilação no Linux:
```bash
cd "caminho/para/projeto"
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Pré-requisitos para macOS

```bash
# Instalar Xcode Command Line Tools
xcode-select --install

# Instalar Homebrew (se não tiver)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Instalar dependências
brew install cmake

# Compilar
cd "caminho/para/projeto"
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Testando a Instalação

Após compilar com sucesso:

```bash
# Windows
.\chunked_carver.exe --version
.\chunked_carver.exe --help

# Linux/macOS
./chunked_carver --version
./chunked_carver --help
```

## Solucionando Problemas Comuns

### Erro: 'filesystem' not found
- **Solução**: Use g++ 8.0+ ou Visual Studio 2017+
- **Alternativa**: Adicione `-lstdc++fs` na compilação

### Erro: CMake not found
- **Windows**: Instale CMake do site oficial ou use Visual Studio
- **Linux**: `sudo apt install cmake` ou `sudo yum install cmake`
- **macOS**: `brew install cmake`

### Erro: Compilador não encontrado
- **Windows**: Certifique-se de estar usando o "Developer Command Prompt"
- **Linux**: Instale `build-essential` (Ubuntu) ou `gcc-c++` (CentOS)
- **macOS**: Execute `xcode-select --install`

### Erro de Linking
- Adicione `-static-libgcc -static-libstdc++` para compilação estática
- No Linux, pode precisar de `-lpthread`

## Exemplo de Compilação Manual Completa

```bash
# Windows (PowerShell/CMD)
g++ -std=c++17 -O2 -I include ^
    src/main.cpp ^
    src/file_carver.cpp ^
    src/file_signature.cpp ^
    src/chunk_reader.cpp ^
    src/command_line_parser.cpp ^
    src/logger.cpp ^
    -o chunked_carver.exe ^
    -static-libgcc -static-libstdc++

# Linux/macOS
g++ -std=c++17 -O2 -I include \
    src/main.cpp \
    src/file_carver.cpp \
    src/file_signature.cpp \
    src/chunk_reader.cpp \
    src/command_line_parser.cpp \
    src/logger.cpp \
    -o chunked_carver \
    -lpthread
```

## Verificação Final

Se a compilação foi bem-sucedida, você deve ter:
- Um executável `chunked_carver` (Linux/macOS) ou `chunked_carver.exe` (Windows)
- O programa deve exibir a versão quando executado com `--version`
- O diretório `output` será criado automaticamente quando necessário