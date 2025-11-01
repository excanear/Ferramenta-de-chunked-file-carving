#include "logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

std::unique_ptr<Logger> Logger::instance_ = nullptr;
std::mutex Logger::mutex_;

Logger::~Logger() {
    close();
}

Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = std::unique_ptr<Logger>(new Logger());
        instance_->min_level_ = LogLevel::INFO;
        instance_->log_to_console_ = true;
        instance_->log_to_file_ = false;
    }
    return *instance_;
}

bool Logger::initialize(const std::string& log_file, LogLevel min_level, bool log_to_console) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    min_level_ = min_level;
    log_to_console_ = log_to_console;
    
    if (!log_file.empty()) {
        if (log_file_.is_open()) {
            log_file_.close();
        }
        
        log_file_.open(log_file, std::ios::app);
        if (log_file_.is_open()) {
            log_to_file_ = true;
            log_filename_ = log_file;
            
            // Escreve cabeçalho no log
            log_file_ << "\n" << std::string(50, '=') << "\n";
            log_file_ << "Logger inicializado em: " << get_timestamp() << "\n";
            log_file_ << std::string(50, '=') << "\n";
            log_file_.flush();
        } else {
            log_to_file_ = false;
            if (log_to_console_) {
                std::cerr << "ERRO: Não foi possível abrir arquivo de log: " << log_file << std::endl;
            }
            return false;
        }
    } else {
        log_to_file_ = false;
    }
    
    return true;
}

void Logger::set_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    min_level_ = level;
}

void Logger::set_console_logging(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_to_console_ = enable;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_level_) {
        return;
    }
    
    write_log(level, message);
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_to_file_ && log_file_.is_open()) {
        log_file_.flush();
    }
    if (log_to_console_) {
        std::cout.flush();
        std::cerr.flush();
    }
}

void Logger::close() {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_file_.is_open()) {
        log_file_ << "Logger fechado em: " << get_timestamp() << "\n";
        log_file_.close();
    }
    log_to_file_ = false;
}

std::string Logger::get_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::write_log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    std::string timestamp = get_timestamp();
    std::string level_str = level_to_string(level);
    std::string formatted_message = "[" + timestamp + "] [" + level_str + "] " + message;
    
    // Log para console
    if (log_to_console_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted_message << std::endl;
        } else {
            std::cout << formatted_message << std::endl;
        }
    }
    
    // Log para arquivo
    if (log_to_file_ && log_file_.is_open()) {
        log_file_ << formatted_message << std::endl;
    }
}