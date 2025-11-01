#pragma once

#include <string>
#include <vector>
#include <map>

/**
 * @brief Estrutura para configurações da aplicação
 */
struct CarverConfig {
    std::string input_file;
    std::string output_directory = "output";
    std::vector<std::string> file_types;
    size_t min_file_size = 512;
    size_t max_file_size = 0; // 0 = sem limite
    size_t chunk_size = 65536; // 64KB
    size_t overlap_size = 4096; // 4KB
    size_t search_window = 1048576; // 1MB
    bool extract_files = true;
    bool use_footers = true;
    bool verbose = false;
    bool generate_report = true;
    bool generate_csv = false;
    std::string report_file = "carving_report.txt";
    std::string csv_file = "carving_report.csv";
};

/**
 * @brief Classe para parsing de argumentos de linha de comando
 */
class CommandLineParser {
private:
    CarverConfig config_;
    std::map<std::string, std::string> help_messages_;
    
    void initialize_help_messages();
    void print_usage(const std::string& program_name) const;
    void print_help() const;
    void print_version() const;
    bool parse_size_argument(const std::string& arg, size_t& value) const;
    std::vector<std::string> split_string(const std::string& str, char delimiter) const;
    
public:
    /**
     * @brief Construtor
     */
    CommandLineParser();
    
    /**
     * @brief Faz o parsing dos argumentos da linha de comando
     * @param argc Número de argumentos
     * @param argv Array de argumentos
     * @return true se o parsing foi bem-sucedido
     */
    bool parse(int argc, char* argv[]);
    
    /**
     * @brief Retorna a configuração parseada
     * @return Estrutura com a configuração
     */
    const CarverConfig& get_config() const;
    
    /**
     * @brief Valida se a configuração é válida
     * @return true se válida
     */
    bool validate_config() const;
    
    /**
     * @brief Exibe a configuração atual
     */
    void print_config() const;
};