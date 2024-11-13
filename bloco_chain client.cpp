#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>
#include <Windows.h>

using namespace std;

struct SharedMemoryBlock {
    int num_messages;
    char messages[256];
    char hash_anterior[256];
    char hash_proximo[256];
    bool resolvido;
};
#define SHARED_MEMORY_NAME L"BlockchainShared"
#define BUFFER_SIZE sizeof(SharedMemoryBlock)
HANDLE hMapFile = NULL;
HANDLE hMutex;
SharedMemoryBlock* pSharedMem = NULL;

void createSharedMemory() {
 
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        cout << "Erro ao criar memória compartilhada." << endl;
        exit(1);
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"BlockchainMutex");
    if (hMutex == NULL) {
        cout << "Erro ao abrir o mutex." << endl;
        exit(1);
    }

    pSharedMem = (SharedMemoryBlock*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pSharedMem == NULL) {
        cout << "Erro ao mapear a memória compartilhada." << endl;
        CloseHandle(hMapFile);
    }
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
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    std::vector<int> T(256, -1);

    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

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


void readFromSharedMemory() {
    WaitForSingleObject(hMutex, INFINITE);

    cout << "Mensagem: " << pSharedMem->messages << endl;

    std::string mensagem_base64 = pSharedMem->messages;
    std::string mensagem_decodificada = base64_decode(mensagem_base64);

    bool mensagem_resolvida = false;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2);

    while (!mensagem_resolvida) {
        std::string chave = std::to_string(distrib(gen));
        std::string mensagem_original = xorEncryptDecrypt(mensagem_decodificada, chave);

        cout << "Tentando chave: " << chave << endl;
        cout << "Tentativa de decript da MSG com chave: " << chave << ": " << mensagem_original << endl;

        if (mensagem_original.find("Mensagem_") != std::string::npos) {
            cout << "Mensagem resolvida: " << mensagem_original << endl;

            pSharedMem->resolvido = true;
            mensagem_resolvida = true;
        }
    }

    cout << "Hash armazenado bloco_anterior: " << pSharedMem->hash_anterior << endl;
    cout << "Resolvido: " << pSharedMem->resolvido << endl;

    ReleaseMutex(hMutex);
}


void writeToSharedMemory(const string& msg, const string& hash_anterior, const string& hash_proximo, bool resolvido) {
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

int main() {
    createSharedMemory();

 
    readFromSharedMemory();

    std::cin.get();  

    closeSharedMemory();

    return 0;
}
