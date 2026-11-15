#include <stdio.h> 
#include <string.h> 
#include "meuBanco.h"

// struct
typedef struct
{
    float valor;
    int tipo; // Depósito, saque ou empréstimo
} Transacao;

typedef struct
{
    char cliente[TAMANHO_NOME];
    int numero;
    int possui[QUANTIDADE_TIPOS];
    float saldo[QUANTIDADE_TIPOS];
    Transacao historico[MAX_HISTORICO];
    int quantidadeHistorico; // 0 a 5
    int temEmprestimo;       // 0 ou 1 (máximo de 1 empréstimo)
} Conta;

typedef struct
{
    Conta *contas; // Vetor dinâmico, cresce com realloc
    int quantidadeClientes;
} Banco;





// Alocação e Liberação de memória
int adicionarConta(Banco *banco, const Conta *nova) {
    Conta *tempo = realloc((*banco).contas, ((*banco).total + 1) * sizeof(Conta));
    if (tempo == NULL) {
        printf("Erro ao alocar memoria!\n");
        return 0; 
    }
    (*banco).contas = tempo;
    (*banco).contas[(*banco).total] = *nova; 
    (*banco).total++;
    return 1; 
}

void liberarBanco(Banco *banco) {
    if ((*banco).contas != NULL) {
        free((*banco).contas);
        (*banco).contas = NULL;
        (*banco).total = 0;
    }
}

// Transações 
void registrarTransacao(Conta *c, float valor, int tipo) {
    if ((*c).qtdHistorico >= MAX_HISTORICO) {
        for (int i = 0; i < MAX_HISTORICO - 1; i++) {
            (*c).historico[i] = (*c).historico[i + 1];
        }
        (*c).qtdHistorico = MAX_HISTORICO - 1;
    }
    (*c).historico[(*c).qtdHistorico].valor = valor;
    (*c).historico[(*c).qtdHistorico].tipo = tipo;
    (*c).qtdHistorico++;
}

int depositar(Conta *c, int tipoConta, float valor) {
    if (valor <= 0) return 0;

    if (tipoConta == CORRENTE || tipoConta == SALARIO) {
        (*c).saldo[0] += valor;
        registrarTransacao(c, valor, DEPOSITO);
        return 1;
    } else if (tipoConta == POUPANCA) {
        (*c).saldo[1] += valor;
        registrarTransacao(c, valor, DEPOSITO);
        return 1;
    }
    return 0;
}

int sacar(Conta *c, int tipoConta, float valor) {
    if (valor <= 0) return 0;

    if (tipoConta == CORRENTE || tipoConta == SALARIO) {
        if ((*c).saldo[0] >= valor) {
            (*c).saldo[0] -= valor;
            registrarTransacao(c, -valor, SAQUE);
            return 1;
        }
    } else if (tipoConta == POUPANCA) {
        if ((*c).saldo[1] >= valor) {
            (*c).saldo[1] -= valor;
            registrarTransacao(c, -valor, SAQUE);
            return 1;
        }
    }
    return 0; 
}

// Empréstimo
int podeFazerEmprestimo(const Conta *c) {
    if (!(*c).temEmprestimo && (*c).saldo[1] > 0 && (*c).saldo[0] > 0) {
        return 1; 
    }
    return 0; 
}

float informarLimiteEmprestimo(float saldoPoupanca, float saldoCorrente) {
    return 10.0 * (saldoPoupanca + saldoCorrente) * 0.25;
}


int solicitarEmprestimo(Conta *c, float valor) {
    if (podeFazerEmprestimo(c)) {
        float limite = informarLimiteEmprestimo((*c).saldo[1], (*c).saldo[0]);
        if (valor <= limite) {
            (*c).saldo[0] += valor; 
            (*c).temEmprestimo = 1; 
            registrarTransacao(c, valor, EMPRESTIMO);
            return 1;
        }
    }
    return 0;
}

// Relatórios

float consultarSaldoTotalBanco(const Banco *banco) {
	float totalBanco = 0;
	for (int i = 0; i < (*banco).total; i++) {
		totalBanco += (*banco).contas[i].saldo[0] + (*banco).contas[i].saldo[1];
    }
    return totalBanco;
}

void listarClientesEmprestimo(const Banco *banco) {
    printf("\n=== CLIENTES PERMITIDOS PARA FAZER EMPRESTIMO ===\n");
    int count = 0;
    for (int i = 0; i < (*banco).total; i++) {
        if (podeFazerEmprestimo(&(*banco).contas[i])) {
            float limite = informarLimiteEmprestimo((*banco).contas[i].saldo[1], (*banco).contas[i].saldo[0]);
            printf("Cliente: %s. Conta: %d. Limite: R$ %.2f.\n", (*banco).contas[i].cliente, (*banco).contas[i].numero, limite);
            count++;
        }
    }
    if (count == 0) printf("Nenhum cliente esta autorizado a fazer emprestimo no momento.\n");
}

void verificarSaldoNegativo(const Banco *banco) {
    printf("\n=== CLIENTES COM SALDO NEGATIVO NA (CONTA CORRENTE.) ===\n");
    int count = 0;
    for (int i = 0; i < (*banco).total; i++) {
        if ((*banco).contas[i].saldo[0] < 0) {
            printf("Cliente: %s. Conta: %d. Saldo: R$ %.2f.\n", (*banco).contas[i].cliente, (*banco).contas[i].numero, (*banco).contas[i].saldo[0]);
            count++;
        }
    }
    if (count == 0) printf("Nenhum cliente esta com saldo negativo na Conta Corrente.\n");
}

void listarClientesSalarioEntre(const Banco *banco, float min, float max) {
    printf("\n=== CLIENTES QUE CONTEM O SALARIO ENTRE R$ %.2f E R$ %.2f ===\n", min, max);
    int count = 0;
    for (int i = 0; i < (*banco).total; i++) {
        if ((*banco).contas[i].possui[SALARIO] == 1 && (*banco).contas[i].saldo[SALARIO] >= min && (*banco).contas[i].saldo[SALARIO] <= max) {
            printf("Cliente: %s. Conta: %d. Salario: R$ %.2f.\n", (*banco).contas[i].cliente, (*banco).contas[i].numero, (*banco).contas[i].saldo[SALARIO]);
            count++;
        }
    }
    if (count == 0) printf("NAO EXISTE NENHUM CLIENTE NESSA FAIXA DE SALARIO.\n");
}

void exibirRelatorio(const Banco *banco) {
	printf("/n=== RELATORIO GERAL DO BANCO ===\n");
    
    printf("1. Saldo Total do Banco: R$ %.2f\n", consultarSaldoTotalBanco(banco));
    listarClientesEmprestimo(banco);
    verificarSaldoNegativo(banco);
    listarClientesSalarioEntre(banco, 1000.0, 1500.0);
}
