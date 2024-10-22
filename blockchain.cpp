#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

using namespace std;

class Block {
public:
    int index;
    string data;
    string previousHash;
    string hash;
    time_t timestamp;

    Block(int idx, string d, string prevHash)
        : index(idx), data(d), previousHash(prevHash), timestamp(time(0)) {
        hash = calculateHash();
    }

    string calculateHash() {
        // Use uma função hash como SHA-256 ou uma simplificada
        return to_string(index) + data + previousHash + to_string(timestamp);
    }
};

class Blockchain {
public:
    vector<Block> chain;

    Blockchain() {
        chain.push_back(createGenesisBlock());
    }

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

    Block getLatestBlock() {
        return chain.back();
    }

    void addBlock(string data) {
        Block newBlock(chain.size(), data, getLatestBlock().hash);
        chain.push_back(newBlock);
    }
};

// PC QUE VAI RODAR O SERVIDOR:
// g++ server.cpp -o server -lboost_system
// ./server

// PC QUE VAI RODAR OS CLIENTES:
// g++ client.cpp -o client -lboost_system
// ./client
