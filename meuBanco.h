#ifndef MEUBANCO_H
#define MEUBANCO_H

#define TAMANHO_NOME 50
#define MAX_HISTORICO 5
#define ARQUIVO_BANCO "banco.txt"

#define NUMERO_INICIAL 1000
#define TITULO_MIN 1000.0f
#define TITULO_MAX 1500.0f
#define PRECO_TITULO 20.0f
#define MAX_PARTICIPANTES_SORTEIO 5
#define MULTIPLICADOR_PREMIO 10
#define LIMITE_NUMERO_SORTEIO 30

// TIPOS DE TRANSAÇÃO
#define DEPOSITO 1
#define SAQUE 2
#define EMPRESTIMO 3
#define TITULO 4
#define PREMIO 5

// TIPOS DE CONTA
#define CORRENTE 0
#define SALARIO 1
#define POUPANCA 2
#define QUANTIDADE_TIPOS 3

typedef struct
{
    float valor;
    int tipo;
} Transacao;

typedef struct
{
    char cliente[TAMANHO_NOME];
    int numero;
    int possui[QUANTIDADE_TIPOS];
    float saldo[QUANTIDADE_TIPOS];
    Transacao historico[MAX_HISTORICO];
    int quantidadeHistorico;
    int temEmprestimo;
    int temTitulo;        // 0 ou 1.
    float premioRecebido; // Soma dos prêmios de sorteios.
} Conta;

typedef struct
{
    Conta *contas;
    int quantidadeClientes;
} Banco;

// LEITURA
void lerTexto(const char *mensagem, char *input, int tamanho);
int lerInteiro(const char *mensagem);
float lerFloat(const char *mensagem);

// INTERFACE
void limparTela(void);
void pausar(void);

// NUMERAÇÃO
void formatarNumeroConta(char *destino, int tamanho, int numero, int tipoConta);

// CADASTRO
int abrirBanco(Banco *banco);
int criarConta(Banco *banco);

// CONSULTA
Conta *consultarCliente(Banco *banco, int numero);
void exibirDadosCliente(const Conta *c);
void exibirHistorico(const Conta *c);
void alertarSaldoNegativo(const Conta *c);

// ARQUIVOS
int salvarBanco(const Banco *banco, const char *caminho);
int carregarBanco(Banco *banco, const char *caminho);

// MEMÓRIA
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

// TÍTULOS
int podeComprarTitulo(const Conta *c);
int comprarTitulo(Conta *c);

// SORTEIO
int fibonacci(int valor);
int somaFibonacci(int valor);
int ehPrimo(int valor);
int efetuarSorteio(Banco *banco);

// RELATÓRIOS
void consultarSaldoTotalBanco(const Banco *banco);
void listarClientesEmprestimo(const Banco *banco);
void verificarSaldoNegativo(const Banco *banco);
void listarClientesAptosTitulo(const Banco *banco);
void listarClientesSorteados(const Banco *banco);
void exibirRelatorio(const Banco *banco);

#endif