# Script de build para Windows (PowerShell)

param(
    [string]$BuildType = "Release",
    [switch]$Clean,
    [switch]$UseNinja
)

$PROJECT_ROOT = $PSScriptRoot
$BUILD_DIR = Join-Path $PROJECT_ROOT "build"

Write-Host "=== Chunked File Carver Build Script ===" -ForegroundColor Green
Write-Host "Build Type: $BuildType" -ForegroundColor Yellow

# Limpa build anterior se solicitado
if ($Clean -and (Test-Path $BUILD_DIR)) {
    Write-Host "Limpando build anterior..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BUILD_DIR
}

# Cria diretório de build
if (-not (Test-Path $BUILD_DIR)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}

Set-Location $BUILD_DIR

try {
    # Configura CMake
    Write-Host "Configurando projeto com CMake..." -ForegroundColor Yellow
    
    $cmake_args = @(
        ".."
        "-DCMAKE_BUILD_TYPE=$BuildType"
    )
    
    if ($UseNinja) {
        $cmake_args += "-G", "Ninja"
    }
    
    $cmake_process = Start-Process -FilePath "cmake" -ArgumentList $cmake_args -NoNewWindow -Wait -PassThru
    
    if ($cmake_process.ExitCode -ne 0) {
        throw "Erro na configuração do CMake"
    }
    
    # Compila projeto
    Write-Host "Compilando projeto..." -ForegroundColor Yellow
    
    if ($UseNinja) {
        $build_process = Start-Process -FilePath "ninja" -NoNewWindow -Wait -PassThru
    } else {
        $build_process = Start-Process -FilePath "cmake" -ArgumentList @("--build", ".", "--config", $BuildType) -NoNewWindow -Wait -PassThru
    }
    
    if ($build_process.ExitCode -ne 0) {
        throw "Erro na compilação"
    }
    
    Write-Host "Compilação concluída com sucesso!" -ForegroundColor Green
    
    # Verifica se o executável foi criado
    $executable_name = if ($IsWindows) { "chunked_carver.exe" } else { "chunked_carver" }
    $executable_path = Join-Path $BUILD_DIR $executable_name
    
    if (Test-Path $executable_path) {
        Write-Host "Executável criado: $executable_path" -ForegroundColor Green
        
        # Executa versão para verificar
        Write-Host "Verificando executável..." -ForegroundColor Yellow
        & $executable_path --version
    } else {
        Write-Warning "Executável não encontrado em $executable_path"
    }
    
} catch {
    Write-Error "Erro durante o build: $_"
    exit 1
} finally {
    Set-Location $PROJECT_ROOT
}

Write-Host "Build script concluído!" -ForegroundColor Green