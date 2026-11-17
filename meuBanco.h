#ifndef MEUBANCO_H
#define MEUBANCO_H

#define TAMANHO_NOME 50
#define MAX_HISTORICO 5
#define ARQUIVO_BANCO "banco.txt"

// TIPOS DE TRANSAÇÃO

#define DEPOSITO 1
#define SAQUE 2
#define EMPRESTIMO 3

// TIPOS DE CONTA

#define CORRENTE 0
#define SALARIO 1
#define POUPANCA 2
#define QUANTIDADE_TIPOS 3

typedef struct
{
    float valor;
    int tipo; // Depósito, saque ou empréstimo.
} Transacao;

typedef struct
{
    char cliente[TAMANHO_NOME];
    int numero;
    int possui[QUANTIDADE_TIPOS];
    float saldo[QUANTIDADE_TIPOS];
    Transacao historico[MAX_HISTORICO];
    int quantidadeHistorico; // 0 a 5.
    int temEmprestimo;       // 0 ou 1 (máximo de 1 empréstimo).
} Conta;

typedef struct
{
    Conta *contas; // Vetor dinâmico que cresce com o realloc.
    int quantidadeClientes;
} Banco;

// LEITURA

void lerTexto(const char *mensagem, char *input, int tamanho);
int lerInteiro(const char *mensagem);
float lerFloat(const char *mensagem);

// CADASTRO

int criarConta(Banco *banco);

// CONSULTA

Conta *consultarCliente(Banco *banco, int numero);
void exibirDadosCliente(const Conta *c);
void exibirHistorico(const Conta *c);

// ARQUIVOS

int salvarBanco(const Banco *banco, const char *caminho);
int carregarBanco(Banco *banco, const char *caminho);

// ALOCAÇÃO E LIBERAÇÃO DE MEMÓRIA

int adicionarConta(Banco *banco, const Conta *nova);
void liberarBanco(Banco *banco);

// TRANSAÇÕES

void registrarTransacao(Conta *c, float valor, int tipo);
int depositar(Conta *c, int tipoConta, float valor);
int sacar(Conta *c, int tipoConta, float valor);

// EMPRÉSTIMO

int podeFazerEmprestimo(const Conta *c);
float informarLimiteEmprestimo(float saldoPoupanca, float saldoCorrente);
int solicitarEmprestimo(Conta *c, float valor);

// RELATÓRIOS

float consultarSaldoTotalBanco(const Banco *banco);
void listarClientesEmprestimo(const Banco *banco);
void verificarSaldoNegativo(const Banco *banco);
void listarClientesSalarioEntre(const Banco *banco, float min, float max);
void exibirRelatorio(const Banco *banco);

#endif