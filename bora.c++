#include <iostream>
#include <list>
#include <string>
#include <random>

using namespace std;

// conta o numero de mensagens (totais), conta quantas nesse bloco (6 forma 1 bloco), conta quantos blocos tem
int cont = 0;
int n_mensagens = 0;
int cont_block = 0;

// estrutura de uma mensagem
struct message_structure {
    client origin;
    client destiny;
    string texto;
    string hash_mensagem;
};

// estrutura de um cliente (processo)
struct client {
    int id;
};

// estrutura de um bloco
struct block {
    string hash_id;
    list<message_structure> messages;
    int numero;
    block *anterior;
    block *proximo;
};

// cria um hash pra cada mensagem
string hash_mensagem() {
    static const char caracteres[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    random_device rd;  // fonte de aleatoriedade
    mt19937 gen(rd()); // gerador baseado em Mersenne Twister
    uniform_int_distribution<> distrib(0, sizeof(caracteres) - 2);

    string hash;
    for (int i = 0; i < 6; ++i) {
        hash += caracteres[distrib(gen)];
    }
    return hash;
}

// envia a mensagem entre clientes
void send_message(client *c1, client *c2, list<message_structure>& lista, block *block_head) {
    string texto = "mensagem numero " + to_string(cont);
    cont++;

    message_structure mensagem;
    mensagem.origin = *c1;
    mensagem.destiny = *c2; 
    mensagem.texto = texto;
    mensagem.hash_mensagem = hash_mensagem();
    n_mensagens += 1;
    
    cout << c1->id << " enviou para " << c2->id << " a mensagem: " << mensagem.texto << endl;

    lista.push_back(mensagem);  // Adiciona a mensagem à lista

    if (n_mensagens == 5) {
        create_block(lista, block_head);
        n_mensagens = 0;
        lista.clear();  // Limpa a lista de mensagens para o próximo bloco
    }
}

// cria bloco quando atinge 6 mensagens
void create_block(const list<message_structure>& lista, block *block_head) {
    block novo_bloco;
    
    if (block_head == NULL) {
        bloco_genesis_cabecao();
        return;
    }

    if(block_head->proximo == NULL){
        block_head->proximo = &novo_bloco;
        novo_bloco.anterior = block_head;
        novo_bloco.proximo = NULL;
    } else {
        block *bloco_atual = block_head;
        while (bloco_atual->proximo != NULL) {
            bloco_atual = bloco_atual->proximo;
        }
        bloco_atual->proximo = &novo_bloco;
        novo_bloco.anterior = bloco_atual;
        novo_bloco.proximo = NULL;
    }

    novo_bloco.hash_id = hash_block(lista);
    novo_bloco.messages = lista;
    novo_bloco.numero = cont_block;
    cont_block++;

    cout << "Bloco criado com hash_id: " << novo_bloco.hash_id << endl;
}

// primeiro bloco
void bloco_genesis_cabecao(){
    // message_structure msg1 = {NULL,NULL, "bloco genesis 1,msg 1", "000000"};
    // message_structure msg2 = {NULL,NULL, "bloco genesis 1,msg 2", "000001"};
    // message_structure msg3 = {NULL,NULL, "bloco genesis 1,msg 3", "000002"};
    // message_structure msg6 = {NULL,NULL, "bloco genesis 1,msg 4", "000003"};
    // message_structure msg5 = {NULL,NULL, "bloco genesis 1,msg 5", "000004"};
    // message_structure msg4 = {NULL,NULL, "bloco genesis 1,msg 6", "000005"};
    // list<message_structure> messages = {msg1, msg2, msg3, msg4, msg5, msg6};

    block bloco_genesis;
    bloco_genesis.hash_id = "000000";
    bloco_genesis.numero = 0;
    bloco_genesis.anterior = NULL;
    bloco_genesis.proximo = NULL;
    // bloco_genesis.messages = messages;
    cout << "Bloco Genesis criado com hash_id: " << bloco_genesis.hash_id << endl;
}

// exibe as mensagens de um bloco
void exibir_mensagens_block(int n, block block_head){
    bool eh_ou_nao_eh = false;
    block bloco_atual = block_head;
    
    while (eh_ou_nao_eh == false){
        if(bloco_atual.numero == n){
            // Exibir as mensagens do bloco
            for (const auto& msg : bloco_atual.messages) {
                cout << "Mensagem: " << msg.texto << " (de " << msg.origin.id << " para " << msg.destiny.id << ")" << endl;
            }
            eh_ou_nao_eh = true;
        }
    }
}
// cria o hash do bloco
string hash_block(const list<message_structure>& lista){
    string hash_criptografado;

    for (const message_structure& msg : lista) {
        // Pegar os 3 primeiros caracteres de hash_mensagem
        string hash_parcial = msg.hash_mensagem.substr(0, 3);
        hash_criptografado += hash_parcial;
    }
    
    return hash_criptografado;
}

// cada processo (cliente) tem um id. Clientes enviam mensagens entre si e quando atinge 6 mensagens, cria um bloco.
int main() {
    random_device rd;  // Gerador de números aleatórios baseado em hardware
    mt19937 gen(rd()); // Inicializa o gerador com a semente
    uniform_int_distribution<> distrib(0, 9); // Define a distribuição entre 0 e 9

    client cliente;
    cliente.id = distrib(gen);



    return 0;
}