/*


===============================================================
Cabeçalho de Implementação - Versão Simplificada de Blockchain
===============================================================
Objetivos:
Desenvolver uma versão simplificada de um sistema de Blockchain em um ambiente simulado, onde múltiplos nós validam e adicionam blocos de transações de forma segura e imutável.

Status de Implementação em relação aos requisitos até agora!!!:

1. Criação e Estrutura de Blocos:
   - [x] Cada bloco contém um número de identificação, lista de transações, hash do bloco anterior e hash próprio.
   - [ ] Implementação de uma função de hashing robusta (ex.: SHA-256) ainda não foi realizada; atualmente, utiliza uma função XOR e Base64 para criptografia simplificada.

2. Validação de Transações:
   - [x] Validação de transações duplicadas antes de adicionar ao bloco (implementada na função `is_duplicate_transaction`).
   - [ ] Validação completa e abrangente das transações ainda não foi implementada.

3. Adição de Blocos e Encadeamento:
   - [x] Encadeamento dos blocos com hash do bloco anterior, garantindo integridade básica da cadeia.
   - [ ] Compartilhamento de blocos entre múltiplos nós e verificação da integridade pela rede não foi implementado.

4. Implementação do Mecanismo de Consenso:
   - [ ] Mecanismo de consenso (Prova de Trabalho - PoW) não implementado; não há verificação de desafio criptográfico para adicionar um bloco.

5. Simulação de Falhas e Recuperação:
   - [ ] Falha de nós e manejo de divergência de cadeias não implementados.

6. Execução de Transações e Sincronização de Cadeias:
   - [ ] Compartilhamento de transações e sincronização entre nós não implementados.
   - [ ] Atualização de saldo de contas e envio de transações ainda não realizado.

===============================================================


*/










#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>  // std::put_time

using namespace std;

// Contadores globais
int cont = 0;
int n_mensagens = 0;
int cont_block = 0;


// Estrutura de um cliente (processo)
struct client {
    int id;
};



// Estrutura de uma mensagem
struct message_structure {
    client origin;
    client destiny;
    string texto;
    string hash_mensagem;
};



// Estrutura de um bloco
struct block {
    std::string hash_id;
    std::string hash_anterior;
    std::string hash_proximo;
    std::list<message_structure> messages;
    int numero;
};


/*
===============================================================
Função de geração de data atual para uso em hashes e registro de blocos
===============================================================
Objetivo: Obter a data atual em formato "YYYY-MM-DD" para ser usada no processo de hashing dos blocos.
Função: `get_current_date`

===============================================================
*/


// criptografia GAMA=====================================


/*
std::string get_current_date() {
    using namespace std::chrono;
    auto now = system_clock::to_time_t(system_clock::now());
    std::tm localTime;
    localtime_s(&now, &localTime);  // Alterei para `localtime_r` (mais universal)

    char date[11]; // yyyy-mm-dd\0
    std::strftime(date, sizeof(date), "%Y-%m-%d", &localTime);

    return std::string(date);
}
*/


//mais flexível em termos de uso e compatibilidade

std::string get_current_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    
    std::tm localTime = *std::localtime(&time); // Converte para `tm`
    
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d"); //  yyyy-mm-dd

    return oss.str();
}


/*
===============================================================
Função de Criptografia Simplificada usando XOR
===============================================================
Objetivo: Encriptar e desencriptar dados com uma chave de criptografia.
Função: `xorEncryptDecrypt`

===============================================================
*/


std::string xorEncryptDecrypt(const std::string& data, const std::string& key) {
    std::string output = data;
    size_t keyLength = key.length();

    for (size_t i = 0; i < data.length(); ++i) {
        output[i] = data[i] ^ key[i % keyLength];
    }
    return output;
}




std::string base64_decode(const std::string& in) {
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++)
        T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}



std::string base64_encode(const std::string& in) {
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
        out.push_back('=');
    }
    return out;
}


/*
===============================================================
Função para Gerar um Hash de Bloco (Simples)
===============================================================
Objetivo: Criar um hash para o bloco combinando a data atual e o endereço de memória do bloco.
Função: `encript_hash_bloco`
Implementação: Funciona como uma implementação de hash básico, mas ainda não cumpre o requisito de segurança como SHA-256.
===============================================================
*/



std::string encript_hash_bloco(const block* bloco) {
    std::string current_date = get_current_date();

    std::ostringstream oss;
    oss << std::hex << reinterpret_cast<uintptr_t>(bloco);
    std::string endereco_memoria = oss.str();

    std::string hash_inicial = current_date + "|" + endereco_memoria;

    std::string chave = "BLOCKCHAIN_CRIPT_PATRAO";
    std::string hash_encriptado = xorEncryptDecrypt(hash_inicial, chave);

    return base64_encode(hash_encriptado);
}



std::string decript_hash_bloco(const std::string& hash_encriptado_base64) {
    std::string hash_encriptado = base64_decode(hash_encriptado_base64);

    std::string chave = "BLOCKCHAIN_CRIPT_PATRAO";

    std::string hash_original = xorEncryptDecrypt(hash_encriptado, chave);

    size_t separator_pos = hash_original.find('|');
    if (separator_pos == std::string::npos) {
        return "Erro: Hash invalido";
    }

    std::string data_descriptografada = hash_original.substr(0, separator_pos);
    std::string endereco_memoria = hash_original.substr(separator_pos + 1);

    if (data_descriptografada != get_current_date()) {
        return "Erro: Data de hash invalida";
    }
    return endereco_memoria;
}
// criptografia GAMA=====================================





// Funções auxiliares
string hash_mensagem();
void create_block(const std::list<message_structure>& lista, block*& block_head);
string hash_block(const list<message_structure>& lista);
void bloco_genesis_cabecao();
void exibir_mensagens_block(int n, block* block_head);




// Cria um hash para cada mensagem
string hash_mensagem(int cont) {
    static const char caracteres[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, sizeof(caracteres) - 2);

    string hash;
    for (int i = 0; i < 6; ++i) {
        hash += caracteres[distrib(gen)];
    }
    // Adiciona o contador para garantir unicidade
    hash += "_" + to_string(cont);
    return hash;
}


/*
===============================================================
Função de Validação de Transação Duplicada
===============================================================
Objetivo: Garantir que uma transação não seja duplicada antes de adicioná-la ao bloco.
Função: `is_duplicate_transaction`
 Esta função atende parcialmente ao requisito de validação de transações, evitando duplicidade.
===============================================================
*/


// Função para validar se uma transação já existe no bloco atual
bool is_duplicate_transaction(const std::list<message_structure>& lista, const message_structure& nova_mensagem) {
    for (const auto& mensagem : lista) {
        if (mensagem.hash_mensagem == nova_mensagem.hash_mensagem) {
            return true;  // Transação duplicada encontrada
        }
    }
    return false;
}


/*
===============================================================
Função de Envio de Mensagens entre Clientes
===============================================================
Objetivo: Simular o envio de mensagens (transações) entre clientes, com verificação de duplicatas.
Função: `send_message`
Cumpre parcialmente a validação de transações, pois verifica duplicatas antes de adicionar a mensagem ao bloco.
===============================================================
*/


// Envia a mensagem entre clientes, agora com verificação de transações duplicadas
void send_message(client* c1, client* c2, std::list<message_structure>& lista, block*& block_head) {
    string texto = "mensagem numero " + to_string(cont);
    cont++;

    message_structure mensagem;
    mensagem.origin = *c1;
    mensagem.destiny = *c2;
    mensagem.texto = texto;
    mensagem.hash_mensagem = hash_mensagem(cont);

    // Verifica se a mensagem é duplicada antes de adicioná-la
    if (is_duplicate_transaction(lista, mensagem)) {
        cout << "Transação duplicada detectada. Mensagem ignorada.\n";
        return;
    }

    n_mensagens += 1;
    cout << c1->id << " enviou para " << c2->id << " a mensagem: " << mensagem.texto << endl;

    lista.push_back(mensagem);

    if (n_mensagens == 5) {
        create_block(lista, block_head);
        n_mensagens = 0;
        lista.clear();
    }
}


/*
===============================================================
Função de Criação de Bloco e Encadeamento com o Bloco Anterior
===============================================================
Objetivo: Criar um novo bloco e encadeá-lo ao bloco anterior, incluindo o hash para manter a integridade da cadeia.
Função: `create_block`
Esta função encadeia blocos usando o hash do bloco anterior, atendendo ao requisito básico de integridade da cadeia.
===============================================================
*/


// Cria bloco quando atinge 6 mensagens
void create_block(const std::list<message_structure>& lista, block*& block_head) {
   
   
    block* novo_bloco = new block;
    novo_bloco->messages = lista;
    novo_bloco->numero = cont_block;
    novo_bloco->hash_id = hash_block(lista);
    cont_block++;

    if (block_head == nullptr) {
        bloco_genesis_cabecao();
        block_head = novo_bloco;
        return;
    }

    std::string hash_anterior = encript_hash_bloco(block_head);
    novo_bloco->hash_anterior = hash_anterior;
    block_head->hash_proximo = encript_hash_bloco(novo_bloco);
    block_head = novo_bloco;

    std::cout << "Bloco criado com hash_id: " << novo_bloco->hash_id << std::endl;
}



block* get_block_by_hash(const std::string& hash_encriptado) {
    // A função blockchain não existe mais, agora buscamos o bloco diretamente usando o hash_encriptado
   
    return nullptr; 
}


/*
===============================================================
Bloco Genesis (Inicial)
===============================================================
Objetivo: Criar o primeiro bloco da cadeia com hash inicial para iniciar o encadeamento.
Função: `bloco_genesis_cabecao`
Funciona como o bloco inicial (Gênesis) da cadeia, cumprindo parcialmente a estrutura básica da blockchain.
===============================================================
*/


// Bloco Genesis
void bloco_genesis_cabecao() {
    block* bloco_genesis = new block;
    bloco_genesis->hash_id = "000000";
    bloco_genesis->numero = 0;
    bloco_genesis->hash_anterior = "";
    bloco_genesis->hash_proximo = "";
    bloco_genesis->messages.clear();  // Garantir que não há mensagens no bloco Genesis
    cout << "Bloco Genesis criado com hash_id: " << bloco_genesis->hash_id << endl;
}



/*
===============================================================
Função para Exibir Mensagens de um Bloco
===============================================================
Objetivo: Exibir as transações (mensagens) armazenadas em um bloco específico.
Função: `exibir_mensagens_block`
Exibe as mensagens de um bloco da cadeia, facilitando a verificação do conteúdo dos blocos.
===============================================================
*/

// Exibe as mensagens de um bloco
void exibir_mensagens_block(int n, block* block_head) {
    block* bloco_atual = block_head;
    while (bloco_atual != nullptr) {
        if (bloco_atual->numero == n) {
            for (const auto& msg : bloco_atual->messages) {
                cout << "Mensagem: " << msg.texto << " (de " << msg.origin.id << " para " << msg.destiny.id << ")" << endl;
            }
            break;
        }
        bloco_atual = get_block_by_hash(bloco_atual->hash_proximo);  // Itera para o próximo bloco
    }
}



// Cria o hash do bloco
string hash_block(const list<message_structure>& lista) {
    stringstream ss;
    for (const auto& msg : lista) {
        ss << msg.hash_mensagem;
    }
    return base64_encode(ss.str());
}




int main() {
    list<message_structure> lista;
    block* block_head = nullptr;

    for (int i = 0; i < 10; i++) {
        client c1, c2;
        c1.id = i;
        c2.id = i + 1;
        send_message(&c1, &c2, lista, block_head);
    }

    return 0;
}
