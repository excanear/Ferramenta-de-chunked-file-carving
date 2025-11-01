#include "file_carver.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

FileCarver::FileCarver(const std::string& output_dir)
    : output_directory_(output_dir)
    , min_file_size_(512)
    , max_file_size_(0)
    , extract_files_(true)
    , use_footers_(true)
    , search_window_(1024 * 1024) // 1MB
    , files_found_(0)
    , files_extracted_(0)
    , bytes_processed_(0) {
    
    signature_detector_ = std::make_unique<FileSignatureDetector>();
    chunk_reader_ = std::make_unique<ChunkReader>();
    
    // Cria diretório de saída se não existir
    try {
        fs::create_directories(output_directory_);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao criar diretório de saída: " << e.what() << std::endl;
    }
}

FileCarver::~FileCarver() = default;

void FileCarver::set_min_file_size(size_t min_size) {
    min_file_size_ = min_size;
}

void FileCarver::set_max_file_size(size_t max_size) {
    max_file_size_ = max_size;
}

void FileCarver::set_extract_files(bool extract) {
    extract_files_ = extract;
}

void FileCarver::set_use_footers(bool use_footers) {
    use_footers_ = use_footers;
}

void FileCarver::set_search_window(size_t window_size) {
    search_window_ = window_size;
}

void FileCarver::set_progress_callback(std::function<void(double)> callback) {
    progress_callback_ = callback;
}

void FileCarver::add_custom_signature(const FileSignature& signature) {
    signature_detector_->add_signature(signature);
}

bool FileCarver::carve_file(const std::string& input_file) {
    return carve_file_types(input_file, {});
}

bool FileCarver::carve_file_types(const std::string& input_file, const std::vector<std::string>& file_types) {
    clear_results();
    
    if (!chunk_reader_->open(input_file)) {
        std::cerr << "Erro ao abrir arquivo: " << input_file << std::endl;
        return false;
    }
    
    std::cout << "Iniciando file carving em: " << input_file << std::endl;
    std::cout << "Tamanho do arquivo: " << chunk_reader_->get_file_size() << " bytes" << std::endl;
    
    if (!file_types.empty()) {
        std::cout << "Procurando por tipos: ";
        for (const auto& type : file_types) {
            std::cout << type << " ";
        }
        std::cout << std::endl;
    }
    
    std::vector<uint8_t> chunk_data;
    size_t bytes_read;
    size_t file_offset = 0;
    
    while (chunk_reader_->read_chunk(chunk_data, bytes_read)) {
        process_chunk(chunk_data, bytes_read, file_offset, file_types);
        
        file_offset = chunk_reader_->tell();
        bytes_processed_ += bytes_read;
        
        // Atualiza progresso
        double progress = chunk_reader_->get_progress();
        update_progress(progress);
        
        // Exibe progresso a cada 5%
        static int last_progress = -1;
        int current_progress = static_cast<int>(progress / 5) * 5;
        if (current_progress != last_progress && current_progress % 5 == 0) {
            std::cout << "Progresso: " << current_progress << "%" << std::endl;
            last_progress = current_progress;
        }
    }
    
    chunk_reader_->close();
    
    std::cout << "\nFile carving concluído!" << std::endl;
    std::cout << "Arquivos encontrados: " << files_found_ << std::endl;
    std::cout << "Arquivos extraídos: " << files_extracted_ << std::endl;
    std::cout << "Bytes processados: " << bytes_processed_ << std::endl;
    
    return true;
}

const std::vector<CarvedFile>& FileCarver::get_carved_files() const {
    return carved_files_;
}

void FileCarver::get_statistics(size_t& files_found, size_t& files_extracted, size_t& bytes_processed) const {
    files_found = files_found_;
    files_extracted = files_extracted_;
    bytes_processed = bytes_processed_;
}

bool FileCarver::generate_report(const std::string& output_file) const {
    std::ofstream report(output_file);
    if (!report.is_open()) {
        std::cerr << "Erro ao criar relatório: " << output_file << std::endl;
        return false;
    }
    
    report << "=== RELATÓRIO DE FILE CARVING ===" << std::endl;
    report << "Data: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    report << "Arquivos encontrados: " << files_found_ << std::endl;
    report << "Arquivos extraídos: " << files_extracted_ << std::endl;
    report << "Bytes processados: " << bytes_processed_ << std::endl;
    report << std::endl;
    
    report << "DETALHES DOS ARQUIVOS:" << std::endl;
    report << std::string(80, '-') << std::endl;
    
    for (const auto& file : carved_files_) {
        report << "Arquivo: " << file.filename << std::endl;
        report << "  Tipo: " << file.type << std::endl;
        report << "  Offset inicial: 0x" << std::hex << file.start_offset << std::dec 
               << " (" << file.start_offset << ")" << std::endl;
        report << "  Offset final: 0x" << std::hex << file.end_offset << std::dec 
               << " (" << file.end_offset << ")" << std::endl;
        report << "  Tamanho: " << file.file_size << " bytes" << std::endl;
        report << "  Footer válido: " << (file.has_valid_footer ? "Sim" : "Não") << std::endl;
        report << "  Extraído: " << (file.extracted ? "Sim" : "Não") << std::endl;
        report << std::endl;
    }
    
    report.close();
    std::cout << "Relatório salvo em: " << output_file << std::endl;
    return true;
}

bool FileCarver::generate_csv_report(const std::string& output_file) const {
    std::ofstream csv(output_file);
    if (!csv.is_open()) {
        std::cerr << "Erro ao criar relatório CSV: " << output_file << std::endl;
        return false;
    }
    
    // Cabeçalho CSV
    csv << "Arquivo,Tipo,Offset_Inicial,Offset_Final,Tamanho,Footer_Valido,Extraido" << std::endl;
    
    for (const auto& file : carved_files_) {
        csv << file.filename << ","
            << file.type << ","
            << file.start_offset << ","
            << file.end_offset << ","
            << file.file_size << ","
            << (file.has_valid_footer ? "Sim" : "Nao") << ","
            << (file.extracted ? "Sim" : "Nao") << std::endl;
    }
    
    csv.close();
    std::cout << "Relatório CSV salvo em: " << output_file << std::endl;
    return true;
}

void FileCarver::clear_results() {
    carved_files_.clear();
    files_found_ = 0;
    files_extracted_ = 0;
    bytes_processed_ = 0;
}

void FileCarver::process_chunk(const std::vector<uint8_t>& data, size_t chunk_size, 
                              size_t file_offset, const std::vector<std::string>& file_types) {
    
    for (size_t i = 0; i <= chunk_size - 4; ++i) { // Assume mínimo de 4 bytes para header
        const FileSignature* signature = signature_detector_->detect_header(data.data() + i, chunk_size - i);
        
        if (!signature) continue;
        
        // Verifica se deve processar este tipo de arquivo
        if (!file_types.empty()) {
            bool found = std::find(file_types.begin(), file_types.end(), signature->name) != file_types.end();
            if (!found) continue;
        }
        
        size_t start_offset = file_offset + i;
        size_t end_offset = start_offset;
        bool has_valid_footer = false;
        
        // Busca por footer se disponível e habilitado
        if (use_footers_ && signature->has_footer) {
            size_t footer_pos = find_footer(start_offset, signature, search_window_);
            if (footer_pos != SIZE_MAX) {
                end_offset = footer_pos + signature->footer.size();
                has_valid_footer = true;
            } else {
                // Se não encontrou footer, estima tamanho baseado na janela de busca
                end_offset = start_offset + search_window_;
            }
        } else {
            // Se não usa footers, estima tamanho baseado na janela de busca
            end_offset = start_offset + search_window_;
        }
        
        // Garante que não ultrapasse o tamanho do arquivo
        if (end_offset > chunk_reader_->get_file_size()) {
            end_offset = chunk_reader_->get_file_size();
        }
        
        size_t file_size = end_offset - start_offset;
        
        // Valida tamanho mínimo
        if (file_size < min_file_size_) continue;
        
        // Valida tamanho máximo
        if (max_file_size_ > 0 && file_size > max_file_size_) continue;
        
        // Cria entrada do arquivo carved
        CarvedFile carved_file;
        carved_file.type = signature->name;
        carved_file.start_offset = start_offset;
        carved_file.end_offset = end_offset;
        carved_file.file_size = file_size;
        carved_file.has_valid_footer = has_valid_footer;
        carved_file.extracted = false;
        
        // Gera nome único para o arquivo
        carved_file.filename = generate_unique_filename(signature->name, signature->extension);
        
        // Valida e extrai arquivo se necessário
        if (validate_carved_file(carved_file)) {
            if (extract_files_) {
                carved_file.extracted = extract_file(carved_file);
                if (carved_file.extracted) {
                    files_extracted_++;
                }
            }
            
            carved_files_.push_back(carved_file);
            files_found_++;
            
            std::cout << "Encontrado: " << signature->name 
                      << " em offset 0x" << std::hex << start_offset << std::dec
                      << " (" << file_size << " bytes)" << std::endl;
        }
    }
}

size_t FileCarver::find_footer(size_t start_offset, const FileSignature* signature, size_t max_search_size) {
    if (!signature || !signature->has_footer) {
        return SIZE_MAX;
    }
    
    std::vector<uint8_t> search_data;
    size_t actual_size = chunk_reader_->read_at_position(start_offset, max_search_size, search_data);
    
    if (actual_size == 0) {
        return SIZE_MAX;
    }
    
    size_t footer_pos = signature_detector_->find_pattern(search_data.data(), actual_size, signature->footer);
    
    if (footer_pos != SIZE_MAX) {
        return start_offset + footer_pos;
    }
    
    return SIZE_MAX;
}

bool FileCarver::extract_file(CarvedFile& carved_file) {
    std::string output_path = output_directory_ + "/" + carved_file.filename;
    
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Erro ao criar arquivo: " << output_path << std::endl;
        return false;
    }
    
    std::vector<uint8_t> file_data;
    size_t bytes_read = chunk_reader_->read_at_position(carved_file.start_offset, 
                                                       carved_file.file_size, file_data);
    
    if (bytes_read == 0) {
        output.close();
        fs::remove(output_path);
        return false;
    }
    
    output.write(reinterpret_cast<const char*>(file_data.data()), bytes_read);
    output.close();
    
    return true;
}

std::string FileCarver::generate_unique_filename(const std::string& base_name, const std::string& extension) {
    static std::map<std::string, int> counters;
    
    int& counter = counters[base_name];
    std::ostringstream oss;
    oss << base_name << "_" << std::setfill('0') << std::setw(6) << counter << extension;
    counter++;
    
    return oss.str();
}

bool FileCarver::validate_carved_file(const CarvedFile& carved_file) const {
    // Validações básicas já foram feitas (tamanho mínimo/máximo)
    // Aqui podem ser adicionadas validações mais específicas
    
    return true;
}

void FileCarver::update_progress(double progress) {
    if (progress_callback_) {
        progress_callback_(progress);
    }
}