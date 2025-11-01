#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

/**
 * @brief Estrutura que representa uma assinatura de arquivo
 */
struct FileSignature {
    std::string name;           // Nome do tipo de arquivo (ex: "JPEG", "PNG")
    std::string extension;      // Extensão do arquivo (ex: ".jpg", ".png")
    std::vector<uint8_t> header; // Bytes do cabeçalho
    std::vector<uint8_t> footer; // Bytes do rodapé (opcional)
    size_t max_size;            // Tamanho máximo esperado do arquivo (0 = sem limite)
    bool has_footer;            // Se o arquivo tem rodapé conhecido
};

/**
 * @brief Classe para detectar e gerenciar assinaturas de arquivos
 */
class FileSignatureDetector {
private:
    std::vector<FileSignature> signatures_;
    std::unordered_map<std::string, size_t> name_to_index_;
    
    void initialize_common_signatures();
    
public:
    /**
     * @brief Construtor - inicializa com assinaturas comuns
     */
    FileSignatureDetector();
    
    /**
     * @brief Adiciona uma nova assinatura de arquivo
     * @param signature Assinatura a ser adicionada
     */
    void add_signature(const FileSignature& signature);
    
    /**
     * @brief Detecta o tipo de arquivo baseado nos primeiros bytes
     * @param data Buffer com os dados para análise
     * @param size Tamanho do buffer
     * @return Ponteiro para a assinatura detectada ou nullptr se não encontrada
     */
    const FileSignature* detect_header(const uint8_t* data, size_t size) const;
    
    /**
     * @brief Verifica se os dados contêm um rodapé válido
     * @param data Buffer com os dados para análise
     * @param size Tamanho do buffer
     * @param signature Assinatura para verificar o rodapé
     * @return true se o rodapé for encontrado
     */
    bool detect_footer(const uint8_t* data, size_t size, const FileSignature* signature) const;
    
    /**
     * @brief Busca por uma assinatura específica nos dados
     * @param data Buffer com os dados
     * @param size Tamanho do buffer
     * @param pattern Padrão a ser procurado
     * @return Posição onde o padrão foi encontrado ou SIZE_MAX se não encontrado
     */
    size_t find_pattern(const uint8_t* data, size_t size, const std::vector<uint8_t>& pattern) const;
    
    /**
     * @brief Retorna todas as assinaturas disponíveis
     * @return Vetor com todas as assinaturas
     */
    const std::vector<FileSignature>& get_signatures() const;
    
    /**
     * @brief Busca uma assinatura pelo nome
     * @param name Nome da assinatura
     * @return Ponteiro para a assinatura ou nullptr se não encontrada
     */
    const FileSignature* get_signature(const std::string& name) const;
    
    /**
     * @brief Carrega assinaturas de um arquivo de configuração
     * @param filename Caminho para o arquivo de configuração
     * @return true se carregado com sucesso
     */
    bool load_signatures_from_file(const std::string& filename);
};