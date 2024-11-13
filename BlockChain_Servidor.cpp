
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>  // std::put_time

#include "Windows.h"
#include <mutex>
using namespace std;

int cont = 0;
int n_mensagens = 0;
int cont_block = 0;

struct client {
    int id;
};

struct message_structure {
    client origin;
    client destiny;
    string texto;
    string hash_mensagem;
};




struct block {
    std::string hash_id;
    std::string hash_anterior;
    std::string hash_proximo;
    std::list<message_structure> messages;
    int numero;
};



std::string get_current_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime = *std::localtime(&time); 

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d"); 

    return oss.str();
}



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

void create_block(const std::list<message_structure>& lista, block*& block_head);
string hash_block(const list<message_structure>& lista);
void bloco_genesis_cabecao();
void createSharedMemory();
void closeSharedMemory();

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

    hash += "_" + to_string(cont);
    return hash;
}


bool is_duplicate_transaction(const std::list<message_structure>& lista, const message_structure& nova_mensagem) {
    for (const auto& mensagem : lista) {
        if (mensagem.hash_mensagem == nova_mensagem.hash_mensagem) {
            return true;  
        }
    }
    return false;
}



void send_message(client* c1, client* c2, std::list<message_structure>& lista, block*& block_head) {
    string texto = "Mensagem numero " + to_string(cont);
    cont++;

    message_structure mensagem;
    mensagem.origin = *c1;
    mensagem.destiny = *c2;
    mensagem.texto = texto;
    mensagem.hash_mensagem = hash_mensagem(cont);

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

void writeToSharedMemory(const string& msg, const string& hash_anterior, bool resolvido);

void create_block(const std::list<message_structure>& lista, block*& block_head) {
    block* novo_bloco = new block;
    novo_bloco->messages = lista;
    novo_bloco->numero = cont_block;
    novo_bloco->hash_id = hash_block(lista);
    cont_block++;

  
    if (block_head == nullptr) {
        novo_bloco->hash_anterior = "000000";
        novo_bloco->hash_proximo = "";
        block_head = novo_bloco;
        cout << "Bloco Genesis criado com hash_id: " << novo_bloco->hash_id << endl;
    }
    else {
        novo_bloco->hash_anterior = block_head->hash_id;
        block_head->hash_proximo = novo_bloco->hash_id;
        block_head = novo_bloco;
        cout << "Bloco criado com hash_id: " << novo_bloco->hash_id << endl;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2);
    int chave_aleatoria = distrib(gen);
    std::cout<<std::endl;
    std::string chave = std::to_string(2);
    std::string hash_bloco = "Mensagem_" + novo_bloco->hash_anterior + hash_mensagem(4);
    std::string mensagem_criptografada = xorEncryptDecrypt(hash_bloco, chave);
    std::string mensagem_base64 = base64_encode(mensagem_criptografada);
    std::cout << "Mensagem original que deve ser decriptada: " << hash_bloco <<std::endl;
    writeToSharedMemory(mensagem_base64, novo_bloco->hash_anterior, false);
    std::cout << std::endl;
    
}

void bloco_genesis_cabecao() {
    block* bloco_genesis = new block;
    bloco_genesis->hash_id = "000000";
    bloco_genesis->numero = 0;
    bloco_genesis->hash_anterior = "";
    bloco_genesis->hash_proximo = "";
    bloco_genesis->messages.clear(); 
    cout << "Bloco Genesis criado com hash_id: " << bloco_genesis->hash_id << endl;
}


string hash_block(const list<message_structure>& lista) {
    stringstream ss;
    for (const auto& msg : lista) {
        ss << msg.hash_mensagem;
    }
    return base64_encode(ss.str());
}

#define SHARED_MEMORY_NAME L"BlockchainShared"
#define BUFFER_SIZE sizeof(SharedMemoryBlock)


struct SharedMemoryBlock {
    int num_messages;  
    char messages[256];
    char hash_anterior[256]; 
    char hash_proximo[256];
    bool resolvido;
};

HANDLE hMapFile = NULL;
HANDLE hMutex;
SharedMemoryBlock* pSharedMem = NULL;
void createSharedMemory() {

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        cout << "Erro ao criar memória compartilhada." << endl;
        exit(1);
    }

 
    hMutex = CreateMutex(NULL, FALSE, L"BlockchainMutex");
    if (hMutex == NULL) {
        cout << "Erro ao criar o mutex." << endl;
        exit(1);
    }

    pSharedMem = (SharedMemoryBlock*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pSharedMem == NULL) {
        cout << "Erro ao mapear a memória compartilhada." << endl;
        CloseHandle(hMapFile);
    }
}


void writeToSharedMemory(const string& msg, const string& hash_anterior, bool resolvido) {
 
    WaitForSingleObject(hMutex, INFINITE);


    if (msg.length() < sizeof(pSharedMem->messages)) {
        strcpy_s(pSharedMem->messages, sizeof(pSharedMem->messages), msg.c_str());
    }
    if (hash_anterior.length() < sizeof(pSharedMem->hash_anterior)) {
        strcpy_s(pSharedMem->hash_anterior, hash_anterior.c_str());
    }

    pSharedMem->resolvido = resolvido;
    pSharedMem->num_messages++;


    ReleaseMutex(hMutex);
}

void closeSharedMemory() {
   
    CloseHandle(hMutex);

    if (pSharedMem != NULL) {
        UnmapViewOfFile(pSharedMem);
    }
    if (hMapFile != NULL) {
        CloseHandle(hMapFile);
    }
}

std::mutex mtx;

int main() {
    list<message_structure> lista;
    block* block_head = nullptr;

    createSharedMemory();

    for (int i = 0; i < 5; i++) {
        client c1, c2;
        c1.id = i;
        c2.id = i + 1;
        send_message(&c1, &c2, lista, block_head);    
    }
    while (true)
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (pSharedMem->resolvido)
        {
            std::cout << "Bloco resolvido, criando um novo bloco e enviando mensagens novamente..." << std::endl;

            for (int i = 0; i < 5; i++) {
                client c1, c2;
                c1.id = i;
                c2.id = i + 1;
                send_message(&c1, &c2, lista, block_head);
              
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cin.get();
    closeSharedMemory();
    std::cin.get();
    return 0;
}