#include "chunk_reader.h"
#include <iostream>
#include <algorithm>

ChunkReader::ChunkReader(size_t chunk_size, size_t overlap_size)
    : file_size_(0)
    , current_position_(0)
    , chunk_size_(chunk_size)
    , is_open_(false)
    , overlap_size_(overlap_size)
    , total_bytes_read_(0)
    , chunks_read_(0) {
    
    buffer_.reserve(chunk_size_);
    overlap_buffer_.reserve(overlap_size_);
}

ChunkReader::~ChunkReader() {
    close();
}

bool ChunkReader::open(const std::string& filename) {
    close(); // Fecha arquivo anterior se estiver aberto
    
    filename_ = filename;
    file_.open(filename_, std::ios::binary);
    
    if (!file_.is_open()) {
        std::cerr << "Erro ao abrir arquivo: " << filename_ << std::endl;
        return false;
    }
    
    is_open_ = true;
    current_position_ = 0;
    total_bytes_read_ = 0;
    chunks_read_ = 0;
    
    calculate_file_size();
    
    std::cout << "Arquivo aberto: " << filename_ 
              << " (" << file_size_ << " bytes)" << std::endl;
    
    return true;
}

void ChunkReader::close() {
    if (file_.is_open()) {
        file_.close();
    }
    
    is_open_ = false;
    filename_.clear();
    buffer_.clear();
    overlap_buffer_.clear();
    file_size_ = 0;
    current_position_ = 0;
}

bool ChunkReader::read_chunk(std::vector<uint8_t>& data, size_t& bytes_read) {
    if (!is_open_) {
        bytes_read = 0;
        return false;
    }
    
    if (is_eof()) {
        bytes_read = 0;
        return false;
    }
    
    // Calcula quantos bytes ler
    size_t bytes_to_read = std::min(chunk_size_, file_size_ - current_position_);
    
    // Inclui overlap do chunk anterior (exceto no primeiro chunk)
    size_t total_size = bytes_to_read;
    if (current_position_ > 0 && !overlap_buffer_.empty()) {
        total_size += overlap_buffer_.size();
    }
    
    // Redimensiona o buffer de saída
    data.resize(total_size);
    bytes_read = 0;
    
    // Copia overlap do chunk anterior
    if (current_position_ > 0 && !overlap_buffer_.empty()) {
        std::copy(overlap_buffer_.begin(), overlap_buffer_.end(), data.begin());
        bytes_read = overlap_buffer_.size();
    }
    
    // Lê novos dados
    file_.seekg(current_position_);
    file_.read(reinterpret_cast<char*>(data.data() + bytes_read), bytes_to_read);
    
    size_t new_bytes_read = file_.gcount();
    bytes_read += new_bytes_read;
    
    // Atualiza posição
    current_position_ += new_bytes_read;
    total_bytes_read_ += new_bytes_read;
    chunks_read_++;
    
    // Prepara overlap para o próximo chunk
    overlap_buffer_.clear();
    if (new_bytes_read >= overlap_size_ && current_position_ < file_size_) {
        size_t overlap_start = bytes_read - overlap_size_;
        overlap_buffer_.assign(data.begin() + overlap_start, data.end());
    }
    
    // Ajusta o tamanho do buffer de saída para o que foi efetivamente lido
    data.resize(bytes_read);
    
    return bytes_read > 0;
}

size_t ChunkReader::read_at_position(size_t position, size_t size, std::vector<uint8_t>& data) {
    if (!is_open_ || position >= file_size_) {
        return 0;
    }
    
    size_t bytes_to_read = std::min(size, file_size_ - position);
    data.resize(bytes_to_read);
    
    file_.seekg(position);
    file_.read(reinterpret_cast<char*>(data.data()), bytes_to_read);
    
    return file_.gcount();
}

bool ChunkReader::seek(size_t position) {
    if (!is_open_) {
        return false;
    }
    
    if (position > file_size_) {
        position = file_size_;
    }
    
    current_position_ = position;
    overlap_buffer_.clear(); // Limpa overlap ao fazer seek manual
    
    return true;
}

size_t ChunkReader::tell() const {
    return current_position_;
}

size_t ChunkReader::get_file_size() const {
    return file_size_;
}

bool ChunkReader::is_eof() const {
    return !is_open_ || current_position_ >= file_size_;
}

bool ChunkReader::is_open() const {
    return is_open_;
}

const std::string& ChunkReader::get_filename() const {
    return filename_;
}

double ChunkReader::get_progress() const {
    if (file_size_ == 0) {
        return 0.0;
    }
    
    return (static_cast<double>(current_position_) / file_size_) * 100.0;
}

void ChunkReader::set_chunk_size(size_t new_chunk_size) {
    chunk_size_ = new_chunk_size;
    buffer_.reserve(chunk_size_);
}

void ChunkReader::set_overlap_size(size_t new_overlap_size) {
    overlap_size_ = new_overlap_size;
    overlap_buffer_.reserve(overlap_size_);
}

void ChunkReader::get_stats(size_t& total_bytes_read, size_t& chunks_read) const {
    total_bytes_read = total_bytes_read_;
    chunks_read = chunks_read_;
}

void ChunkReader::reset_stats() {
    total_bytes_read_ = 0;
    chunks_read_ = 0;
}

void ChunkReader::calculate_file_size() {
    if (!file_.is_open()) {
        file_size_ = 0;
        return;
    }
    
    // Salva posição atual
    auto current_pos = file_.tellg();
    
    // Vai para o final do arquivo
    file_.seekg(0, std::ios::end);
    file_size_ = file_.tellg();
    
    // Restaura posição original
    file_.seekg(current_pos);
}