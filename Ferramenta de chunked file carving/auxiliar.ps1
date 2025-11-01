# Exemplo de script de teste para a ferramenta de file carving

param(
    [string]$TestImage = "test_image.dd",
    [string]$OutputDir = "test_output"
)

Write-Host "=== Script de Teste - Chunked File Carver ===" -ForegroundColor Green

# Verifica se o executável existe
$executable = ".\chunked_carver.exe"
if (-not (Test-Path $executable)) {
    $executable = ".\build\chunked_carver.exe"
    if (-not (Test-Path $executable)) {
        Write-Error "Executável não encontrado. Compile o projeto primeiro."
        exit 1
    }
}

Write-Host "Usando executável: $executable" -ForegroundColor Yellow

# Cria arquivo de teste se não existir
if (-not (Test-Path $TestImage)) {
    Write-Host "Criando arquivo de teste..." -ForegroundColor Yellow
    
    # Cria um arquivo de teste com alguns dados conhecidos
    $testData = @()
    
    # Adiciona header JPEG
    $testData += 0xFF, 0xD8, 0xFF
    $testData += 1..1000 | ForEach-Object { Get-Random -Maximum 256 }
    $testData += 0xFF, 0xD9  # Footer JPEG
    
    # Adiciona alguns bytes aleatórios
    $testData += 1..500 | ForEach-Object { Get-Random -Maximum 256 }
    
    # Adiciona header PNG
    $testData += 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
    $testData += 1..800 | ForEach-Object { Get-Random -Maximum 256 }
    $testData += 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82  # Footer PNG
    
    # Escreve arquivo
    [byte[]]$bytes = $testData
    [System.IO.File]::WriteAllBytes($TestImage, $bytes)
    
    Write-Host "Arquivo de teste criado: $TestImage ($($bytes.Length) bytes)" -ForegroundColor Green
}

# Remove diretório de saída anterior
if (Test-Path $OutputDir) {
    Remove-Item -Recurse -Force $OutputDir
}

Write-Host "Executando testes..." -ForegroundColor Yellow

# Teste 1: Recuperação básica
Write-Host "`nTeste 1: Recuperação básica" -ForegroundColor Cyan
& $executable -i $TestImage -o "$OutputDir/test1" --verbose

# Teste 2: Apenas detecção (sem extração)
Write-Host "`nTeste 2: Apenas detecção" -ForegroundColor Cyan
& $executable -i $TestImage -o "$OutputDir/test2" --no-extract --csv

# Teste 3: Tipos específicos
Write-Host "`nTeste 3: Apenas imagens" -ForegroundColor Cyan
& $executable -i $TestImage -o "$OutputDir/test3" -t JPEG,PNG

# Teste 4: Configuração customizada
Write-Host "`nTeste 4: Configuração customizada" -ForegroundColor Cyan
& $executable -i $TestImage -o "$OutputDir/test4" --min-size 100 --chunk-size 1K

Write-Host "`n=== Resultados dos Testes ===" -ForegroundColor Green

# Verifica resultados
foreach ($testDir in @("test1", "test2", "test3", "test4")) {
    $fullPath = "$OutputDir/$testDir"
    if (Test-Path $fullPath) {
        $files = Get-ChildItem $fullPath -File
        Write-Host "$testDir`: $($files.Count) arquivos encontrados" -ForegroundColor Yellow
        
        foreach ($file in $files) {
            Write-Host "  - $($file.Name) ($($file.Length) bytes)" -ForegroundColor Gray
        }
    } else {
        Write-Host "$testDir`: Diretório não criado" -ForegroundColor Red
    }
}

# Verifica relatórios
$reports = Get-ChildItem . -Name "*report*"
if ($reports) {
    Write-Host "`nRelatórios gerados:" -ForegroundColor Green
    foreach ($report in $reports) {
        Write-Host "  - $report" -ForegroundColor Gray
    }
}

Write-Host "`nTestes concluídos!" -ForegroundColor Green