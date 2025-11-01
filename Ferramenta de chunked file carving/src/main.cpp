#include "file_carver.h"
#include "command_line_parser.h"
#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Exibe barra de progresso
 * @param progress Progresso de 0.0 a 100.0
 */
void display_progress_bar(double progress) {
    const int bar_width = 50;
    std::cout << "\r[";
    
    int pos = static_cast<int>(bar_width * progress / 100.0);
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) {
            std::cout << "=";
        } else if (i == pos) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "] " << std::fixed << std::setprecision(1) << progress << "%";
    std::cout.flush();
}

/**
 * @brief Converte bytes para formato legível
 * @param bytes Número de bytes
 * @return String formatada
 */
std::string format_bytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    const size_t num_units = sizeof(units) / sizeof(units[0]);
    
    double size = static_cast<double>(bytes);
    size_t unit_index = 0;
    
    while (size >= 1024.0 && unit_index < num_units - 1) {
        size /= 1024.0;
        unit_index++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

/**
 * @brief Exibe estatísticas finais
 * @param carver Instância do file carver
 * @param elapsed_time Tempo decorrido em segundos
 */
void display_final_statistics(const FileCarver& carver, double elapsed_time) {
    size_t files_found, files_extracted, bytes_processed;
    carver.get_statistics(files_found, files_extracted, bytes_processed);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ESTATÍSTICAS FINAIS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Arquivos encontrados: " << files_found << std::endl;
    std::cout << "Arquivos extraídos: " << files_extracted << std::endl;
    std::cout << "Dados processados: " << format_bytes(bytes_processed) << std::endl;
    std::cout << "Tempo decorrido: " << std::fixed << std::setprecision(2) << elapsed_time << " segundos" << std::endl;
    
    if (elapsed_time > 0) {
        double speed = bytes_processed / elapsed_time;
        std::cout << "Velocidade média: " << format_bytes(static_cast<size_t>(speed)) << "/s" << std::endl;
    }
    
    if (files_found > 0) {
        double success_rate = (static_cast<double>(files_extracted) / files_found) * 100.0;
        std::cout << "Taxa de sucesso: " << std::fixed << std::setprecision(1) << success_rate << "%" << std::endl;
    }
    
    std::cout << std::string(60, '=') << std::endl;
}

/**
 * @brief Exibe resumo dos tipos de arquivo encontrados
 * @param carver Instância do file carver
 */
void display_file_type_summary(const FileCarver& carver) {
    const auto& carved_files = carver.get_carved_files();
    
    if (carved_files.empty()) {
        std::cout << "\nNenhum arquivo foi encontrado." << std::endl;
        return;
    }
    
    // Conta tipos de arquivo
    std::map<std::string, size_t> type_counts;
    std::map<std::string, size_t> type_sizes;
    
    for (const auto& file : carved_files) {
        type_counts[file.type]++;
        type_sizes[file.type] += file.file_size;
    }
    
    std::cout << "\nRESUMO POR TIPO DE ARQUIVO:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::left << std::setw(10) << "Tipo" 
              << std::setw(10) << "Qtd" 
              << std::setw(15) << "Tamanho Total" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& pair : type_counts) {
        std::cout << std::left << std::setw(10) << pair.first
                  << std::setw(10) << pair.second
                  << std::setw(15) << format_bytes(type_sizes[pair.first]) << std::endl;
    }
    std::cout << std::string(50, '-') << std::endl;
}

/**
 * @brief Configura o logger baseado na configuração
 * @param config Configuração da aplicação
 */
void setup_logger(const CarverConfig& config) {
    LogLevel level = config.verbose ? LogLevel::DEBUG : LogLevel::INFO;
    std::string log_file = config.verbose ? "carver_debug.log" : "";
    
    Logger& logger = Logger::getInstance();
    if (!logger.initialize(log_file, level, true)) {
        std::cerr << "Aviso: Não foi possível inicializar o logger" << std::endl;
    }
    
    if (config.verbose) {
        LOG_INFO("Logger inicializado em modo verbose");
        LOG_INFO("Arquivo de log: carver_debug.log");
    }
}

/**
 * @brief Configura o file carver baseado na configuração
 * @param carver Instância do file carver
 * @param config Configuração da aplicação
 */
void configure_carver(FileCarver& carver, const CarverConfig& config) {
    carver.set_min_file_size(config.min_file_size);
    carver.set_max_file_size(config.max_file_size);
    carver.set_extract_files(config.extract_files);
    carver.set_use_footers(config.use_footers);
    carver.set_search_window(config.search_window);
    
    // Configura callback de progresso se não estiver em modo verbose
    if (!config.verbose) {
        carver.set_progress_callback(display_progress_bar);
    }
    
    LOG_INFO("File carver configurado com sucesso");
    LOG_DEBUG("Tamanho mínimo: " + std::to_string(config.min_file_size) + " bytes");
    LOG_DEBUG("Tamanho máximo: " + (config.max_file_size == 0 ? "Sem limite" : std::to_string(config.max_file_size) + " bytes"));
    LOG_DEBUG("Janela de busca: " + std::to_string(config.search_window) + " bytes");
}

/**
 * @brief Gera relatórios conforme configuração
 * @param carver Instância do file carver
 * @param config Configuração da aplicação
 */
void generate_reports(const FileCarver& carver, const CarverConfig& config) {
    if (config.generate_report) {
        if (carver.generate_report(config.report_file)) {
            LOG_INFO("Relatório gerado: " + config.report_file);
        } else {
            LOG_ERROR("Erro ao gerar relatório: " + config.report_file);
        }
    }
    
    if (config.generate_csv) {
        if (carver.generate_csv_report(config.csv_file)) {
            LOG_INFO("Relatório CSV gerado: " + config.csv_file);
        } else {
            LOG_ERROR("Erro ao gerar relatório CSV: " + config.csv_file);
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Chunked File Carver v1.0.0" << std::endl;
    std::cout << "Ferramenta para recuperação de arquivos fragmentados" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Parse dos argumentos da linha de comando
    CommandLineParser parser;
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    const CarverConfig& config = parser.get_config();
    
    // Configura o logger
    setup_logger(config);
    
    // Exibe configuração se em modo verbose
    if (config.verbose) {
        parser.print_config();
    }
    
    try {
        // Cria diretório de saída
        if (!fs::exists(config.output_directory)) {
            fs::create_directories(config.output_directory);
            LOG_INFO("Diretório de saída criado: " + config.output_directory);
        }
        
        // Inicializa file carver
        FileCarver carver(config.output_directory);
        configure_carver(carver, config);
        
        // Marca tempo de início
        auto start_time = std::chrono::high_resolution_clock::now();
        
        LOG_INFO("Iniciando file carving: " + config.input_file);
        
        // Executa file carving
        bool success;
        if (config.file_types.empty()) {
            success = carver.carve_file(config.input_file);
        } else {
            success = carver.carve_file_types(config.input_file, config.file_types);
        }
        
        // Calcula tempo decorrido
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        double elapsed_seconds = duration.count() / 1000.0;
        
        if (!success) {
            LOG_ERROR("Erro durante o processo de file carving");
            return 1;
        }
        
        LOG_INFO("File carving concluído com sucesso");
        
        // Exibe estatísticas
        display_final_statistics(carver, elapsed_seconds);
        display_file_type_summary(carver);
        
        // Gera relatórios
        generate_reports(carver, config);
        
        std::cout << "\nProcesso concluído com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Exceção capturada: " + std::string(e.what()));
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        LOG_ERROR("Exceção desconhecida capturada");
        std::cerr << "Erro desconhecido ocorreu" << std::endl;
        return 1;
    }
    
    // Finaliza logger
    Logger::getInstance().close();
    
    return 0;
}