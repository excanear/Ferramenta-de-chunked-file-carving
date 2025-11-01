#pragma once

#include "file_signature.h"
#include "chunk_reader.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

/**
 * @brief Estrutura que representa um arquivo encontrado
 */
struct CarvedFile {
    std::string filename;           // Nome do arquivo de saída
    std::string type;              // Tipo detectado
    size_t start_offset;           // Posição inicial no arquivo fonte
    size_t end_offset;             // Posição final no arquivo fonte
    size_t file_size;              // Tamanho do arquivo carved
    bool has_valid_footer;         // Se foi encontrado footer válido
    bool extracted;                // Se foi extraído com sucesso
};

/**
 * @brief Classe principal para file carving
 */
class FileCarver {
private:
    std::unique_ptr<FileSignatureDetector> signature_detector_;
    std::unique_ptr<ChunkReader> chunk_reader_;
    std::string output_directory_;
    std::vector<CarvedFile> carved_files_;
    
    // Configurações
    size_t min_file_size_;
    size_t max_file_size_;
    bool extract_files_;
    bool use_footers_;
    size_t search_window_;
    
    // Estatísticas
    size_t files_found_;
    size_t files_extracted_;
    size_t bytes_processed_;
    
    // Callback para progresso
    std::function<void(double)> progress_callback_;
    
public:
    /**
     * @brief Construtor
     * @param output_dir Diretório para salvar arquivos carved
     */
    explicit FileCarver(const std::string& output_dir = "output");
    
    /**
     * @brief Destrutor
     */
    ~FileCarver();
    
    /**
     * @brief Configura tamanho mínimo de arquivo
     * @param min_size Tamanho mínimo em bytes
     */
    void set_min_file_size(size_t min_size);
    
    /**
     * @brief Configura tamanho máximo de arquivo
     * @param max_size Tamanho máximo em bytes (0 = sem limite)
     */
    void set_max_file_size(size_t max_size);
    
    /**
     * @brief Define se deve extrair arquivos ou apenas detectar
     * @param extract true para extrair arquivos
     */
    void set_extract_files(bool extract);
    
    /**
     * @brief Define se deve usar footers para delimitar arquivos
     * @param use_footers true para usar footers
     */
    void set_use_footers(bool use_footers);
    
    /**
     * @brief Define janela de busca para footers
     * @param window_size Tamanho da janela em bytes
     */
    void set_search_window(size_t window_size);
    
    /**
     * @brief Define callback para atualizações de progresso
     * @param callback Função que recebe percentual (0.0-100.0)
     */
    void set_progress_callback(std::function<void(double)> callback);
    
    /**
     * @brief Adiciona uma assinatura de arquivo customizada
     * @param signature Assinatura a ser adicionada
     */
    void add_custom_signature(const FileSignature& signature);
    
    /**
     * @brief Executa o processo de carving em um arquivo
     * @param input_file Caminho para o arquivo de entrada
     * @return true se o processo foi executado com sucesso
     */
    bool carve_file(const std::string& input_file);
    
    /**
     * @brief Executa o processo de carving em uma lista de tipos específicos
     * @param input_file Caminho para o arquivo de entrada
     * @param file_types Lista de tipos de arquivo para procurar
     * @return true se o processo foi executado com sucesso
     */
    bool carve_file_types(const std::string& input_file, const std::vector<std::string>& file_types);
    
    /**
     * @brief Retorna lista de arquivos encontrados
     * @return Vetor com arquivos carved
     */
    const std::vector<CarvedFile>& get_carved_files() const;
    
    /**
     * @brief Retorna estatísticas do processo
     * @param files_found Número de arquivos encontrados
     * @param files_extracted Número de arquivos extraídos
     * @param bytes_processed Bytes processados
     */
    void get_statistics(size_t& files_found, size_t& files_extracted, size_t& bytes_processed) const;
    
    /**
     * @brief Gera relatório em formato texto
     * @param output_file Arquivo de saída para o relatório
     * @return true se o relatório foi gerado com sucesso
     */
    bool generate_report(const std::string& output_file) const;
    
    /**
     * @brief Gera relatório em formato CSV
     * @param output_file Arquivo de saída para o relatório CSV
     * @return true se o relatório foi gerado com sucesso
     */
    bool generate_csv_report(const std::string& output_file) const;
    
    /**
     * @brief Limpa resultados anteriores
     */
    void clear_results();

private:
    /**
     * @brief Processa um chunk de dados
     * @param data Buffer com os dados
     * @param chunk_size Tamanho do chunk
     * @param file_offset Offset do chunk no arquivo original
     * @param file_types Tipos de arquivo para procurar (vazio = todos)
     */
    void process_chunk(const std::vector<uint8_t>& data, size_t chunk_size, 
                      size_t file_offset, const std::vector<std::string>& file_types);
    
    /**
     * @brief Procura por footers para delimitar arquivo
     * @param start_offset Posição inicial do arquivo
     * @param signature Assinatura do arquivo
     * @param max_search_size Tamanho máximo para procurar
     * @return Posição do footer ou SIZE_MAX se não encontrado
     */
    size_t find_footer(size_t start_offset, const FileSignature* signature, size_t max_search_size);
    
    /**
     * @brief Extrai arquivo para disco
     * @param carved_file Informações do arquivo a ser extraído
     * @return true se extraído com sucesso
     */
    bool extract_file(CarvedFile& carved_file);
    
    /**
     * @brief Gera nome único para arquivo
     * @param base_name Nome base
     * @param extension Extensão do arquivo
     * @return Nome único gerado
     */
    std::string generate_unique_filename(const std::string& base_name, const std::string& extension);
    
    /**
     * @brief Valida se um arquivo carved é válido
     * @param carved_file Arquivo a ser validado
     * @return true se válido
     */
    bool validate_carved_file(const CarvedFile& carved_file) const;
    
    /**
     * @brief Atualiza progresso e chama callback se definido
     * @param progress Progresso atual (0.0-100.0)
     */
    void update_progress(double progress);
};