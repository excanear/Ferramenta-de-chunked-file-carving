#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

/**
 * @brief Níveis de log
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

/**
 * @brief Classe para logging com suporte a thread-safety
 */
class Logger {
private:
    static std::unique_ptr<Logger> instance_;
    static std::mutex mutex_;
    
    std::ofstream log_file_;
    LogLevel min_level_;
    bool log_to_console_;
    bool log_to_file_;
    std::string log_filename_;
    mutable std::mutex log_mutex_;
    
    Logger() = default;
    
    std::string get_timestamp() const;
    std::string level_to_string(LogLevel level) const;
    void write_log(LogLevel level, const std::string& message);
    
public:
    /**
     * @brief Destructor
     */
    ~Logger();
    
    // Não permite cópia
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Retorna a instância singleton
     * @return Referência para a instância do logger
     */
    static Logger& getInstance();
    
    /**
     * @brief Inicializa o logger
     * @param log_file Arquivo de log (vazio = não salva em arquivo)
     * @param min_level Nível mínimo de log
     * @param log_to_console Se deve exibir logs no console
     * @return true se inicializado com sucesso
     */
    bool initialize(const std::string& log_file = "", 
                   LogLevel min_level = LogLevel::INFO,
                   bool log_to_console = true);
    
    /**
     * @brief Define o nível mínimo de log
     * @param level Novo nível mínimo
     */
    void set_level(LogLevel level);
    
    /**
     * @brief Define se deve fazer log no console
     * @param enable true para habilitar
     */
    void set_console_logging(bool enable);
    
    /**
     * @brief Log de debug
     * @param message Mensagem de debug
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log de informação
     * @param message Mensagem informativa
     */
    void info(const std::string& message);
    
    /**
     * @brief Log de warning
     * @param message Mensagem de warning
     */
    void warning(const std::string& message);
    
    /**
     * @brief Log de erro
     * @param message Mensagem de erro
     */
    void error(const std::string& message);
    
    /**
     * @brief Log genérico
     * @param level Nível do log
     * @param message Mensagem
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Força flush dos buffers
     */
    void flush();
    
    /**
     * @brief Fecha o arquivo de log
     */
    void close();
};

// Macros para facilitar uso
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)