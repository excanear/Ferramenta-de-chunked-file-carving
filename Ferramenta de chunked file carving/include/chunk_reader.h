#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
#include <memory>

/**
 * @brief Classe para leitura eficiente de dados em chunks
 */
class ChunkReader {
private:
    std::ifstream file_;
    std::string filename_;
    size_t file_size_;
    size_t current_position_;
    size_t chunk_size_;
    std::vector<uint8_t> buffer_;
    bool is_open_;
    
    // Buffer para overlap entre chunks
    std::vector<uint8_t> overlap_buffer_;
    size_t overlap_size_;
    
public:
    /**
     * @brief Construtor
     * @param chunk_size Tamanho do chunk em bytes (padrão: 64KB)
     * @param overlap_size Tamanho do overlap entre chunks (padrão: 4KB)
     */
    explicit ChunkReader(size_t chunk_size = 65536, size_t overlap_size = 4096);
    
    /**
     * @brief Destrutor
     */
    ~ChunkReader();
    
    /**
     * @brief Abre um arquivo para leitura
     * @param filename Caminho para o arquivo
     * @return true se aberto com sucesso
     */
    bool open(const std::string& filename);
    
    /**
     * @brief Fecha o arquivo
     */
    void close();
    
    /**
     * @brief Lê o próximo chunk de dados
     * @param data Buffer para armazenar os dados lidos
     * @param bytes_read Número de bytes efetivamente lidos
     * @return true se a leitura foi bem-sucedida
     */
    bool read_chunk(std::vector<uint8_t>& data, size_t& bytes_read);
    
    /**
     * @brief Lê dados de uma posição específica
     * @param position Posição no arquivo
     * @param size Número de bytes a ler
     * @param data Buffer para armazenar os dados
     * @return Número de bytes efetivamente lidos
     */
    size_t read_at_position(size_t position, size_t size, std::vector<uint8_t>& data);
    
    /**
     * @brief Move para uma posição específica no arquivo
     * @param position Nova posição
     * @return true se a operação foi bem-sucedida
     */
    bool seek(size_t position);
    
    /**
     * @brief Retorna a posição atual no arquivo
     * @return Posição atual
     */
    size_t tell() const;
    
    /**
     * @brief Retorna o tamanho total do arquivo
     * @return Tamanho do arquivo em bytes
     */
    size_t get_file_size() const;
    
    /**
     * @brief Verifica se chegou ao final do arquivo
     * @return true se chegou ao final
     */
    bool is_eof() const;
    
    /**
     * @brief Verifica se o arquivo está aberto
     * @return true se o arquivo está aberto
     */
    bool is_open() const;
    
    /**
     * @brief Retorna o nome do arquivo atual
     * @return Nome do arquivo
     */
    const std::string& get_filename() const;
    
    /**
     * @brief Calcula o progresso da leitura em percentual
     * @return Progresso de 0.0 a 100.0
     */
    double get_progress() const;
    
    /**
     * @brief Redefine o tamanho do chunk
     * @param new_chunk_size Novo tamanho do chunk
     */
    void set_chunk_size(size_t new_chunk_size);
    
    /**
     * @brief Redefine o tamanho do overlap
     * @param new_overlap_size Novo tamanho do overlap
     */
    void set_overlap_size(size_t new_overlap_size);
    
    /**
     * @brief Retorna estatísticas de leitura
     * @param total_bytes_read Total de bytes lidos
     * @param chunks_read Número de chunks lidos
     */
    void get_stats(size_t& total_bytes_read, size_t& chunks_read) const;
    
    /**
     * @brief Redefine as estatísticas
     */
    void reset_stats();

private:
    size_t total_bytes_read_;
    size_t chunks_read_;
    
    /**
     * @brief Calcula o tamanho do arquivo
     */
    void calculate_file_size();
};