# Simulação de Blockchain com Memória Compartilhada e Sincronização

Este projeto implementa uma simulação de blockchain básica utilizando memória compartilhada e mutexes para sincronização de processos. A comunicação entre o `servidor` (que simula a criação de blocos) e o `cliente` (que verifica e decripta as mensagens) é feita por meio de um bloco de memória compartilhada. Esta simulação foi desenvolvida em C++ e utiliza a API do Windows para manipulação de memória compartilhada e sincronização com mutex.

## Estrutura de Arquivos

- **server.cpp**: Implementa o processo do servidor, que inicializa a memória compartilhada, gera blocos com hashes e os armazena na memória para verificação.
- **client.cpp**: Implementa o processo do cliente, que lê as mensagens dos blocos, tenta decriptá-las e valida o hash. Ao decriptar com sucesso, o cliente marca o bloco como "resolvido" na memória compartilhada.

## Pré-Requisitos

- **Sistema Operacional**: Windows
- **Compilador C++**: Requer um compilador compatível com a API do Windows (ex.: Visual Studio, MinGW)
- **Ambiente de Desenvolvimento**: Visual Studio ou qualquer IDE compatível com C++ e suporte a projetos para Windows

## Configuração e Compilação

### Observação: Nenhuma Configuração Adicional é Necessária!

1. **Compilação**:
   Compile os arquivos `server.cpp` e `client.cpp` em executáveis diferentes de 64 bits. Se estiver usando o Visual Studio, basta criar um projeto de Console para cada arquivo. Caso utilize MinGW, compile com os comandos:

