#include "command_line_parser.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

CommandLineParser::CommandLineParser() {
    initialize_help_messages();
}

void CommandLineParser::initialize_help_messages() {
    help_messages_["input"] = "Arquivo de entrada para file carving (obrigatório)";
    help_messages_["output"] = "Diretório de saída para arquivos extraídos (padrão: output)";
    help_messages_["types"] = "Tipos de arquivo para procurar, separados por vírgula (ex: JPEG,PNG,PDF)";
    help_messages_["min-size"] = "Tamanho mínimo de arquivo em bytes (padrão: 512)";
    help_messages_["max-size"] = "Tamanho máximo de arquivo em bytes (0 = sem limite, padrão: 0)";
    help_messages_["chunk-size"] = "Tamanho do chunk de leitura em bytes (padrão: 65536)";
    help_messages_["overlap-size"] = "Tamanho do overlap entre chunks em bytes (padrão: 4096)";
    help_messages_["search-window"] = "Janela de busca para footers em bytes (padrão: 1048576)";
    help_messages_["no-extract"] = "Apenas detecta arquivos, não os extrai";
    help_messages_["no-footers"] = "Não usa footers para delimitar arquivos";
    help_messages_["verbose"] = "Modo verboso - exibe informações detalhadas";
    help_messages_["no-report"] = "Não gera relatório de texto";
    help_messages_["csv"] = "Gera relatório em formato CSV";
    help_messages_["report-file"] = "Nome do arquivo de relatório (padrão: carving_report.txt)";
    help_messages_["csv-file"] = "Nome do arquivo CSV (padrão: carving_report.csv)";
    help_messages_["help"] = "Exibe esta mensagem de ajuda";
    help_messages_["version"] = "Exibe informações de versão";
}

bool CommandLineParser::parse(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_help();
            return false;
        }
        else if (arg == "-v" || arg == "--version") {
            print_version();
            return false;
        }
        else if (arg == "-i" || arg == "--input") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --input requer um argumento" << std::endl;
                return false;
            }
            config_.input_file = argv[++i];
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --output requer um argumento" << std::endl;
                return false;
            }
            config_.output_directory = argv[++i];
        }
        else if (arg == "-t" || arg == "--types") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --types requer um argumento" << std::endl;
                return false;
            }
            config_.file_types = split_string(argv[++i], ',');
        }
        else if (arg == "--min-size") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --min-size requer um argumento" << std::endl;
                return false;
            }
            if (!parse_size_argument(argv[++i], config_.min_file_size)) {
                std::cerr << "Erro: valor inválido para --min-size" << std::endl;
                return false;
            }
        }
        else if (arg == "--max-size") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --max-size requer um argumento" << std::endl;
                return false;
            }
            if (!parse_size_argument(argv[++i], config_.max_file_size)) {
                std::cerr << "Erro: valor inválido para --max-size" << std::endl;
                return false;
            }
        }
        else if (arg == "--chunk-size") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --chunk-size requer um argumento" << std::endl;
                return false;
            }
            if (!parse_size_argument(argv[++i], config_.chunk_size)) {
                std::cerr << "Erro: valor inválido para --chunk-size" << std::endl;
                return false;
            }
        }
        else if (arg == "--overlap-size") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --overlap-size requer um argumento" << std::endl;
                return false;
            }
            if (!parse_size_argument(argv[++i], config_.overlap_size)) {
                std::cerr << "Erro: valor inválido para --overlap-size" << std::endl;
                return false;
            }
        }
        else if (arg == "--search-window") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --search-window requer um argumento" << std::endl;
                return false;
            }
            if (!parse_size_argument(argv[++i], config_.search_window)) {
                std::cerr << "Erro: valor inválido para --search-window" << std::endl;
                return false;
            }
        }
        else if (arg == "--no-extract") {
            config_.extract_files = false;
        }
        else if (arg == "--no-footers") {
            config_.use_footers = false;
        }
        else if (arg == "--verbose") {
            config_.verbose = true;
        }
        else if (arg == "--no-report") {
            config_.generate_report = false;
        }
        else if (arg == "--csv") {
            config_.generate_csv = true;
        }
        else if (arg == "--report-file") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --report-file requer um argumento" << std::endl;
                return false;
            }
            config_.report_file = argv[++i];
        }
        else if (arg == "--csv-file") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: --csv-file requer um argumento" << std::endl;
                return false;
            }
            config_.csv_file = argv[++i];
        }
        else if (arg[0] == '-') {
            std::cerr << "Erro: argumento desconhecido: " << arg << std::endl;
            return false;
        }
        else {
            // Se não começa com -, assume que é o arquivo de entrada
            if (config_.input_file.empty()) {
                config_.input_file = arg;
            } else {
                std::cerr << "Erro: múltiplos arquivos de entrada especificados" << std::endl;
                return false;
            }
        }
    }
    
    return validate_config();
}

const CarverConfig& CommandLineParser::get_config() const {
    return config_;
}

bool CommandLineParser::validate_config() const {
    if (config_.input_file.empty()) {
        std::cerr << "Erro: arquivo de entrada não especificado" << std::endl;
        return false;
    }
    
    if (!fs::exists(config_.input_file)) {
        std::cerr << "Erro: arquivo de entrada não existe: " << config_.input_file << std::endl;
        return false;
    }
    
    if (config_.chunk_size < 1024) {
        std::cerr << "Erro: chunk-size deve ser pelo menos 1024 bytes" << std::endl;
        return false;
    }
    
    if (config_.overlap_size >= config_.chunk_size) {
        std::cerr << "Erro: overlap-size deve ser menor que chunk-size" << std::endl;
        return false;
    }
    
    if (config_.max_file_size > 0 && config_.min_file_size > config_.max_file_size) {
        std::cerr << "Erro: min-size não pode ser maior que max-size" << std::endl;
        return false;
    }
    
    return true;
}

void CommandLineParser::print_config() const {
    std::cout << "=== CONFIGURAÇÃO ===" << std::endl;
    std::cout << "Arquivo de entrada: " << config_.input_file << std::endl;
    std::cout << "Diretório de saída: " << config_.output_directory << std::endl;
    
    if (!config_.file_types.empty()) {
        std::cout << "Tipos de arquivo: ";
        for (size_t i = 0; i < config_.file_types.size(); ++i) {
            std::cout << config_.file_types[i];
            if (i < config_.file_types.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Tipos de arquivo: Todos" << std::endl;
    }
    
    std::cout << "Tamanho mínimo: " << config_.min_file_size << " bytes" << std::endl;
    std::cout << "Tamanho máximo: " << (config_.max_file_size == 0 ? "Sem limite" : std::to_string(config_.max_file_size) + " bytes") << std::endl;
    std::cout << "Tamanho do chunk: " << config_.chunk_size << " bytes" << std::endl;
    std::cout << "Tamanho do overlap: " << config_.overlap_size << " bytes" << std::endl;
    std::cout << "Janela de busca: " << config_.search_window << " bytes" << std::endl;
    std::cout << "Extrair arquivos: " << (config_.extract_files ? "Sim" : "Não") << std::endl;
    std::cout << "Usar footers: " << (config_.use_footers ? "Sim" : "Não") << std::endl;
    std::cout << "Modo verboso: " << (config_.verbose ? "Sim" : "Não") << std::endl;
    std::cout << "Gerar relatório: " << (config_.generate_report ? "Sim" : "Não") << std::endl;
    std::cout << "Gerar CSV: " << (config_.generate_csv ? "Sim" : "Não") << std::endl;
    std::cout << std::endl;
}

void CommandLineParser::print_usage(const std::string& program_name) const {
    std::cout << "Uso: " << program_name << " [opções] <arquivo_entrada>" << std::endl;
    std::cout << "     " << program_name << " -i <arquivo_entrada> [opções]" << std::endl;
    std::cout << std::endl;
    std::cout << "Use --help para ver todas as opções disponíveis." << std::endl;
}

void CommandLineParser::print_help() const {
    std::cout << "Ferramenta de Chunked File Carving" << std::endl;
    std::cout << "Recupera arquivos fragmentados de dispositivos de armazenamento" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OPÇÕES:" << std::endl;
    std::cout << "  -i, --input <arquivo>      " << help_messages_.at("input") << std::endl;
    std::cout << "  -o, --output <diretório>   " << help_messages_.at("output") << std::endl;
    std::cout << "  -t, --types <tipos>        " << help_messages_.at("types") << std::endl;
    std::cout << "      --min-size <bytes>     " << help_messages_.at("min-size") << std::endl;
    std::cout << "      --max-size <bytes>     " << help_messages_.at("max-size") << std::endl;
    std::cout << "      --chunk-size <bytes>   " << help_messages_.at("chunk-size") << std::endl;
    std::cout << "      --overlap-size <bytes> " << help_messages_.at("overlap-size") << std::endl;
    std::cout << "      --search-window <bytes>" << help_messages_.at("search-window") << std::endl;
    std::cout << "      --no-extract           " << help_messages_.at("no-extract") << std::endl;
    std::cout << "      --no-footers           " << help_messages_.at("no-footers") << std::endl;
    std::cout << "      --verbose              " << help_messages_.at("verbose") << std::endl;
    std::cout << "      --no-report            " << help_messages_.at("no-report") << std::endl;
    std::cout << "      --csv                  " << help_messages_.at("csv") << std::endl;
    std::cout << "      --report-file <arquivo>" << help_messages_.at("report-file") << std::endl;
    std::cout << "      --csv-file <arquivo>   " << help_messages_.at("csv-file") << std::endl;
    std::cout << "  -h, --help                 " << help_messages_.at("help") << std::endl;
    std::cout << "  -v, --version              " << help_messages_.at("version") << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXEMPLOS:" << std::endl;
    std::cout << "  chunked_carver disk_image.dd" << std::endl;
    std::cout << "  chunked_carver -i disk_image.dd -o recovered_files" << std::endl;
    std::cout << "  chunked_carver -i disk_image.dd -t JPEG,PNG,PDF --min-size 1024" << std::endl;
    std::cout << "  chunked_carver -i disk_image.dd --no-extract --csv" << std::endl;
    std::cout << std::endl;
    
    std::cout << "TIPOS DE ARQUIVO SUPORTADOS:" << std::endl;
    std::cout << "  JPEG, PNG, GIF87a, GIF89a, PDF, ZIP, RAR, 7ZIP, DOCX," << std::endl;
    std::cout << "  MP3, MP4, AVI, BMP, TIFF_LE, TIFF_BE, DOC, EXE, SQLITE" << std::endl;
}

void CommandLineParser::print_version() const {
    std::cout << "Chunked File Carver v1.0.0" << std::endl;
    std::cout << "Desenvolvido para recuperação de arquivos fragmentados" << std::endl;
    std::cout << "Compilado com C++17" << std::endl;
}

bool CommandLineParser::parse_size_argument(const std::string& arg, size_t& value) const {
    try {
        std::string str = arg;
        size_t multiplier = 1;
        
        // Verifica sufixos de tamanho
        if (!str.empty()) {
            char last_char = std::tolower(str.back());
            if (last_char == 'k') {
                multiplier = 1024;
                str.pop_back();
            } else if (last_char == 'm') {
                multiplier = 1024 * 1024;
                str.pop_back();
            } else if (last_char == 'g') {
                multiplier = 1024 * 1024 * 1024;
                str.pop_back();
            }
        }
        
        value = std::stoull(str) * multiplier;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> CommandLineParser::split_string(const std::string& str, char delimiter) const {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        // Remove espaços em branco
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        if (!item.empty()) {
            // Converte para maiúscula
            std::transform(item.begin(), item.end(), item.begin(), ::toupper);
            result.push_back(item);
        }
    }
    
    return result;
}