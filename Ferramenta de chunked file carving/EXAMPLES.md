# Exemplos de Uso - Chunked File Carver

Este arquivo contém exemplos práticos de uso da ferramenta de chunked file carving para diferentes cenários.

## 📋 Cenários Básicos

### 1. Recuperação Simples de Imagem de Disco

```bash
# Recupera todos os tipos de arquivo de uma imagem DD
chunked_carver disk_image.dd

# Especifica diretório de saída
chunked_carver -i forensic_image.dd -o evidence_recovered
```

### 2. Análise Sem Extração

```bash
# Apenas identifica arquivos sem extraí-los
chunked_carver -i large_disk.dd --no-extract

# Gera relatório CSV para análise posterior
chunked_carver -i disk.dd --no-extract --csv --csv-file analysis.csv
```

## 🎯 Cenários Específicos por Tipo

### Recuperação de Imagens

```bash
# Apenas imagens JPEG e PNG
chunked_carver -i sd_card.dd -t JPEG,PNG -o photos_recovered

# Imagens com tamanho mínimo (evita thumbnails)
chunked_carver -i camera_memory.dd -t JPEG,PNG --min-size 50K

# Todas as imagens suportadas
chunked_carver -i disk.dd -t JPEG,PNG,GIF87a,GIF89a,BMP,TIFF_LE,TIFF_BE
```

### Recuperação de Documentos

```bash
# Documentos PDF apenas
chunked_carver -i laptop_disk.dd -t PDF --min-size 1K --max-size 100M

# Documentos do Office
chunked_carver -i work_computer.dd -t DOC,DOCX --min-size 5K

# Todos os documentos
chunked_carver -i disk.dd -t PDF,DOC,DOCX -o documents_recovered
```

### Recuperação de Mídia

```bash
# Arquivos de áudio
chunked_carver -i phone_memory.dd -t MP3 --min-size 1M

# Arquivos de vídeo
chunked_carver -i tablet.dd -t MP4,AVI --min-size 10M --max-size 2G

# Mídia completa
chunked_carver -i multimedia_disk.dd -t MP3,MP4,AVI --min-size 1M
```

### Recuperação de Arquivos Compactados

```bash
# Arquivos ZIP e RAR
chunked_carver -i backup_disk.dd -t ZIP,RAR,7ZIP

# Apenas arquivos grandes compactados
chunked_carver -i storage.dd -t ZIP,RAR --min-size 100K --max-size 1G
```

## ⚙️ Otimização de Performance

### Para Discos Grandes (>100GB)

```bash
# Aumenta chunk size para melhor performance
chunked_carver -i large_disk.dd --chunk-size 1M --overlap-size 8K

# Janela de busca maior para arquivos grandes
chunked_carver -i raid_disk.dd --chunk-size 2M --search-window 5M
```

### Para Dispositivos Lentos (USB, CD/DVD)

```bash
# Chunks menores para dispositivos lentos
chunked_carver -i usb_drive.dd --chunk-size 256K --overlap-size 2K

# Reduz janela de busca para economizar tempo
chunked_carver -i cd_image.iso --search-window 512K
```

### Para SSDs (Performance Máxima)

```bash
# Chunks grandes para SSDs rápidos
chunked_carver -i ssd_image.dd --chunk-size 4M --overlap-size 16K
```

## 🔍 Debugging e Análise Detalhada

### Modo Verbose

```bash
# Logs detalhados na tela e arquivo
chunked_carver -i suspicious_disk.dd --verbose

# Verbose sem extração (análise apenas)
chunked_carver -i evidence.dd --verbose --no-extract --csv
```

### Configuração de Logs

```bash
# O modo verbose automaticamente cria carver_debug.log
chunked_carver -i disk.dd --verbose

# Gera relatórios detalhados
chunked_carver -i disk.dd --verbose --csv --report-file detailed_report.txt
```

## 📊 Análise Forense

### Busca Específica em Evidência

```bash
# Procura apenas executáveis suspeitos
chunked_carver -i suspect_computer.dd -t EXE --min-size 1K --max-size 50M

# Busca bancos de dados
chunked_carver -i server_disk.dd -t SQLITE --min-size 10K

# Evidências de documentos apagados
chunked_carver -i deleted_files.dd -t PDF,DOC,DOCX --no-footers
```

### Recuperação Conservadora (Menos Falsos Positivos)

```bash
# Usa footers para validação rigorosa
chunked_carver -i evidence.dd -t JPEG,PNG,PDF --search-window 2M

# Tamanhos mínimos maiores
chunked_carver -i disk.dd --min-size 10K --max-size 500M
```

### Recuperação Agressiva (Máxima Cobertura)

```bash
# Sem uso de footers (pode gerar mais falsos positivos)
chunked_carver -i damaged_disk.dd --no-footers --min-size 512

# Janela de busca muito grande
chunked_carver -i corrupted.dd --search-window 10M --no-footers
```

## 🚀 Casos de Uso Avançados

### Recuperação de Dispositivo Móvel

```bash
# Imagens típicas de smartphones
chunked_carver -i phone_dump.dd -t JPEG,PNG,MP4,MP3 --min-size 10K

# Documentos e apps de celular
chunked_carver -i android_image.dd -t SQLITE,ZIP --min-size 1K --max-size 100M
```

### Recuperação de Servidor Web

```bash
# Arquivos web comuns
chunked_carver -i web_server.dd -t JPEG,PNG,GIF87a,PDF,ZIP

# Bancos de dados do servidor
chunked_carver -i database_partition.dd -t SQLITE --min-size 1M
```

### Recuperação de Workstation Gráfica

```bash
# Arquivos de design grandes
chunked_carver -i designer_pc.dd -t TIFF_LE,TIFF_BE,PNG --min-size 1M --max-size 500M

# Projetos e documentos
chunked_carver -i creative_disk.dd -t PDF,ZIP,7ZIP --min-size 100K
```

## 📈 Comparação de Estratégias

### Estratégia Rápida (Tipos Específicos)
```bash
# Foco em tipos conhecidos, chunks grandes
chunked_carver -i disk.dd -t JPEG,PNG,PDF --chunk-size 2M --min-size 50K
```

### Estratégia Completa (Tudo)
```bash
# Todos os tipos, configuração padrão
chunked_carver -i disk.dd --verbose
```

### Estratégia Conservadora (Precisão)
```bash
# Tamanhos restritivos, validação rigorosa
chunked_carver -i disk.dd --min-size 10K --max-size 100M --search-window 1M
```

## 🔧 Automação com Scripts

### Script Batch para Windows
```batch
@echo off
set INPUT=%1
set OUTPUT=recovered_%date:~-4,4%%date:~-7,2%%date:~-10,2%

echo Iniciando recuperacao de %INPUT%
chunked_carver.exe -i "%INPUT%" -o "%OUTPUT%" --verbose --csv

echo Processo concluido. Arquivos em: %OUTPUT%
pause
```

### Script Shell para Linux
```bash
#!/bin/bash
INPUT="$1"
OUTPUT="recovered_$(date +%Y%m%d_%H%M%S)"

echo "Iniciando recuperação de $INPUT"
./chunked_carver -i "$INPUT" -o "$OUTPUT" --verbose --csv

echo "Processo concluído. Arquivos em: $OUTPUT"
```

## 📋 Checklist de Melhores Práticas

### Antes de Iniciar
- [ ] Verificar espaço em disco suficiente para recuperação
- [ ] Definir tipos de arquivo de interesse
- [ ] Escolher tamanhos mínimo e máximo apropriados
- [ ] Considerar performance vs. completude

### Durante o Processo
- [ ] Monitorar progresso e logs
- [ ] Verificar se não há erros de E/S
- [ ] Considerar pausar se disco de origem apresentar problemas

### Após a Recuperação
- [ ] Verificar relatórios gerados
- [ ] Validar arquivos recuperados
- [ ] Documentar processo para relatório forense
- [ ] Backup dos resultados

## ⚠️ Limitações e Considerações

### Quando Usar Esta Ferramenta
- ✅ Recuperação de arquivos fragmentados
- ✅ Análise forense de dispositivos
- ✅ Recuperação de dados apagados
- ✅ Extração de evidências digitais

### Quando NÃO Usar
- ❌ Arquivos não fragmentados (use ferramentas de recuperação simples)
- ❌ Sistemas de arquivo funcionais (use ferramentas de backup)
- ❌ Dados altamente criptografados
- ❌ Dispositivos fisicamente danificados (use laboratório especializado)

---

Para mais informações, consulte o README.md principal ou execute:
```bash
chunked_carver --help
```