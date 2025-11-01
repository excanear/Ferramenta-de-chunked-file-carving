#include "file_signature.h"
#include <algorithm>
#include <fstream>
#include <iostream>

FileSignatureDetector::FileSignatureDetector() {
    initialize_common_signatures();
}

void FileSignatureDetector::initialize_common_signatures() {
    // JPEG
    add_signature({
        "JPEG",
        ".jpg",
        {0xFF, 0xD8, 0xFF},
        {0xFF, 0xD9},
        0, // sem limite de tamanho
        true
    });
    
    // PNG
    add_signature({
        "PNG",
        ".png",
        {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
        {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82},
        0,
        true
    });
    
    // GIF87a
    add_signature({
        "GIF87a",
        ".gif",
        {0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
        {0x00, 0x3B},
        0,
        true
    });
    
    // GIF89a
    add_signature({
        "GIF89a",
        ".gif",
        {0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
        {0x00, 0x3B},
        0,
        true
    });
    
    // PDF
    add_signature({
        "PDF",
        ".pdf",
        {0x25, 0x50, 0x44, 0x46, 0x2D}, // %PDF-
        {0x25, 0x25, 0x45, 0x4F, 0x46}, // %%EOF
        0,
        true
    });
    
    // ZIP
    add_signature({
        "ZIP",
        ".zip",
        {0x50, 0x4B, 0x03, 0x04},
        {0x50, 0x4B, 0x05, 0x06},
        0,
        true
    });
    
    // RAR
    add_signature({
        "RAR",
        ".rar",
        {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00},
        {},
        0,
        false
    });
    
    // 7-Zip
    add_signature({
        "7ZIP",
        ".7z",
        {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C},
        {},
        0,
        false
    });
    
    // Microsoft Office (DOCX, XLSX, PPTX)
    add_signature({
        "DOCX",
        ".docx",
        {0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x06, 0x00},
        {},
        0,
        false
    });
    
    // MP3
    add_signature({
        "MP3",
        ".mp3",
        {0x49, 0x44, 0x33}, // ID3
        {},
        0,
        false
    });
    
    // MP3 (alternativo)
    add_signature({
        "MP3_ALT",
        ".mp3",
        {0xFF, 0xFB},
        {},
        0,
        false
    });
    
    // MP4
    add_signature({
        "MP4",
        ".mp4",
        {0x00, 0x00, 0x00, 0x20, 0x66, 0x74, 0x79, 0x70},
        {},
        0,
        false
    });
    
    // AVI
    add_signature({
        "AVI",
        ".avi",
        {0x52, 0x49, 0x46, 0x46}, // RIFF
        {},
        0,
        false
    });
    
    // BMP
    add_signature({
        "BMP",
        ".bmp",
        {0x42, 0x4D},
        {},
        0,
        false
    });
    
    // TIFF (Little Endian)
    add_signature({
        "TIFF_LE",
        ".tiff",
        {0x49, 0x49, 0x2A, 0x00},
        {},
        0,
        false
    });
    
    // TIFF (Big Endian)
    add_signature({
        "TIFF_BE",
        ".tiff",
        {0x4D, 0x4D, 0x00, 0x2A},
        {},
        0,
        false
    });
    
    // MS Word DOC
    add_signature({
        "DOC",
        ".doc",
        {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1},
        {},
        0,
        false
    });
    
    // Executable (PE)
    add_signature({
        "EXE",
        ".exe",
        {0x4D, 0x5A}, // MZ
        {},
        0,
        false
    });
    
    // SQLite Database
    add_signature({
        "SQLITE",
        ".db",
        {0x53, 0x51, 0x4C, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74, 0x20, 0x33, 0x00},
        {},
        0,
        false
    });
}

void FileSignatureDetector::add_signature(const FileSignature& signature) {
    signatures_.push_back(signature);
    name_to_index_[signature.name] = signatures_.size() - 1;
}

const FileSignature* FileSignatureDetector::detect_header(const uint8_t* data, size_t size) const {
    for (const auto& sig : signatures_) {
        if (size >= sig.header.size()) {
            if (std::equal(sig.header.begin(), sig.header.end(), data)) {
                return &sig;
            }
        }
    }
    return nullptr;
}

bool FileSignatureDetector::detect_footer(const uint8_t* data, size_t size, const FileSignature* signature) const {
    if (!signature || !signature->has_footer || signature->footer.empty()) {
        return false;
    }
    
    if (size < signature->footer.size()) {
        return false;
    }
    
    // Procura o footer nos últimos bytes
    const uint8_t* footer_start = data + size - signature->footer.size();
    return std::equal(signature->footer.begin(), signature->footer.end(), footer_start);
}

size_t FileSignatureDetector::find_pattern(const uint8_t* data, size_t size, const std::vector<uint8_t>& pattern) const {
    if (pattern.empty() || size < pattern.size()) {
        return SIZE_MAX;
    }
    
    for (size_t i = 0; i <= size - pattern.size(); ++i) {
        if (std::equal(pattern.begin(), pattern.end(), data + i)) {
            return i;
        }
    }
    
    return SIZE_MAX;
}

const std::vector<FileSignature>& FileSignatureDetector::get_signatures() const {
    return signatures_;
}

const FileSignature* FileSignatureDetector::get_signature(const std::string& name) const {
    auto it = name_to_index_.find(name);
    if (it != name_to_index_.end()) {
        return &signatures_[it->second];
    }
    return nullptr;
}

bool FileSignatureDetector::load_signatures_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo de assinaturas: " << filename << std::endl;
        return false;
    }
    
    // Implementação básica - pode ser expandida para ler formato JSON ou XML
    // Por enquanto, apenas retorna true como placeholder
    file.close();
    return true;
}