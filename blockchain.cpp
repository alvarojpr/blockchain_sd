// #include <string>
#include <iostream>
#include <fstream>

using namespace std; // essa linha é necessária para não precisar escrever std::

#define pi = 3.1415926535
#define pular_2_linhas cout<<"\n\n"

void tipos_de_variaveis(){
    const int valor_constante_inteiro=500; // esse valor nao muda
    int inteiro = 1, inteiro2 = 2;
    float numero_com_decimal=1.5;
    double float_com_alta_precisao=500.50501;
    char letra = 'A'; // ' ';
    string nome = "lanche cussao!"; // ""
    bool palmeiras_nao_tem_mundial = true;
}

void enumeracoes(){
    enum Cor { VERMELHO, VERDE, AZUL, BRANCO };
    Cor minhaCor1 = VERMELHO;
    Cor minhaCor2 = AZUL;
}

void ponteiros(){
    int valor = 10;
    int* ponteiro = &valor;  // ponteiro armazena o endereço de valor
    int valor2 = *ponteiro;  // valor2 é igual a 10
}

void leitura_e_escrita_de_arquivos(){
    //deve-se incluir a lib <fstream> (combinação de ifstream + ofstream)
    fstream arquivo("exemplo.txt", ios::in | ios::out | ios::app);
    string linha;

    if (arquivo.is_open()) {
        // Escrever no arquivo
        arquivo << "Adicionando mais uma linha.\n";

        // Resetar para o início do arquivo para leitura
        arquivo.seekg(0);

        // Ler do arquivo
        while (getline(arquivo, linha)) {
            cout << linha << endl;
        }

        // limpar o arquivo
        // arquivo.clear();

        arquivo.close();
    } else {
        cerr << "Não foi possível abrir o arquivo." << endl;
    }
}
//STRUCTS
struct Pessoa {
    string nome;
    int idade;
    float altura;
};
//Pessoa p1 = {"Alice", 30, 1.65};
//cout << "Nome: " << p1.nome << endl;

//UNIONS
//Unions são usadas para economizar espaço quando é necessário armazenar diferentes tipos de dados, mas nunca mais de um ao mesmo tempo.
union Numero {
    int i;
    float f;
};

int main() {
    system("cls"); //limpa o que havia antes no terminal

    int idade;
    string nome;
    string endereco;

    cout<<"Idade: ";
    cin>>idade;

    pular_2_linhas;
    
    cout<<"Nome: ";
    cin>>nome;
    fflush(stdin); //depois de ler string, tem que meter essa.

    pular_2_linhas;

    cout<<"Qual o seu endereco? ";
    getline(cin, endereco);

    pular_2_linhas;

    cout<<"Seu nome é "<<nome<< " e você tem "<<idade<< " anos e mora em "<<endereco<<".\n";
    return idade;
}