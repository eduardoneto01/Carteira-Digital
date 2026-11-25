#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "meuBanco.h"

// Habilitar o UTF-8 no terminal.
#ifdef _WIN32
#include <windows.h>
#endif

// Lê uma opção de menu de um único caractere, já em minúsculo. Mais de um caractere (ex.: "10" ou "sim") conta como inválido e devolve '?'. Se a entrada acabar (Ctrl+D / Ctrl+Z), devolve "seAcabar" para o programa conseguir sair e salvar em vez de ficar em loop.
static char lerOpcao(const char *mensagem, char seAcabar)
{
    char texto[8];
    lerTexto(mensagem, texto, sizeof texto);

    if (feof(stdin))
    {
        return seAcabar;
    }
    if (strlen(texto) != 1)
    {
        return '?';
    }
    return (char)tolower((unsigned char)texto[0]);
}

// Exibe o cabeçalho formatado e estilizado para cada função do programa.
static void exibirCabecalho(const char *cabecalho)
{
    printf("==================================\n");
    printf("%s\n", cabecalho);
    printf("==================================\n\n");
}

// Consulta as contas que o cliente possui e devolve o valor do tipo escolhido (CORRENTE, SALARIO ou POUPANCA). Se ele tiver uma única conta, escolhe automaticamente.
static int escolherTipoConta(const Conta *c)
{
    int tiposDisponiveis[QUANTIDADE_TIPOS];
    int quantidade = 0;

    for (int tipo = 0; tipo < QUANTIDADE_TIPOS; tipo++)
    {
        if (c->possui[tipo])
        {
            tiposDisponiveis[quantidade++] = tipo;
        }
    }

    if (quantidade == 1)
    {
        return tiposDisponiveis[0];
    }

    printf("\nEm qual conta?\n");
    for (int i = 0; i < quantidade; i++)
    {
        char numeroConta[16];
        formatarNumeroConta(numeroConta, sizeof numeroConta, c->numero, tiposDisponiveis[i]);
        printf("%d - %s [%s]\n", i + 1, nomeTipoConta(tiposDisponiveis[i]), numeroConta);
    }

    int opcao;
    do
    {
        opcao = lerInteiro("Opção: ");
        if (opcao < 1 || opcao > quantidade)
        {
            printf("Opção inválida.\n");
        }
    } while (opcao < 1 || opcao > quantidade);

    return tiposDisponiveis[opcao - 1];
}

// Submenu de movimentações na conta de um cliente.
static void menuCliente(Conta *c)
{
    char opcao;

    do
    {
        exibirDadosCliente(c);
        exibirHistorico(c);

        printf("\n-- Operações --\n");
        printf("1 - Depositar\n");
        printf("2 - Sacar\n");
        printf("3 - Solicitar empréstimo\n");
        printf("4 - Comprar título de capitalização\n");
        printf("5 - Voltar\n");
        opcao = lerOpcao("Opção: ", '5');

        switch (opcao)
        {
        case '1':
        {
            int tipo = escolherTipoConta(c);
            float valor = lerFloat("\nValor do depósito: R$ ");
            if (depositar(c, tipo, valor))
            {
                printf("Depósito realizado.\n");
            }
            else
            {
                printf("Depósito não realizado: o valor deve ser maior que zero.\n");
            }
            break;
        }
        case '2':
        {
            int tipo = escolherTipoConta(c);
            float valor = lerFloat("Valor do saque: R$ ");
            if (sacar(c, tipo, valor))
            {
                printf("Saque realizado.\n");
            }
            else
            {
                printf("Saque não realizado: valor inválido ou saldo insuficiente.\n");
            }
            break;
        }
        case '3':
        {
            if (!podeFazerEmprestimo(c))
            {
                printf("Empréstimo não disponível para este cliente.\n");
                printf("(Exige conta corrente, saldo na poupança e nenhum empréstimo anterior.)\n");
                break;
            }
            float limite = informarLimiteEmprestimo(c->saldo[POUPANCA], c->saldo[CORRENTE]);
            printf("Limite disponivel: R$ %.2f\n", limite);
            float valor = lerFloat("Valor do Empréstimo: R$ ");
            if (solicitarEmprestimo(c, valor))
            {
                printf("Empréstimo aprovado e creditado na conta corrente.\n");
            }
            else
            {
                printf("Empréstimo negado: o valor deve ser maior que zero e dentro do limite.\n");
            }
            break;
        }
        case '4':
        {
            if (!podeComprarTitulo(c))
            {
                printf("Compra de título não disponível para este cliente.\n");
                printf("(Exige conta salário com saldo entre R$ 1.000,00 e 1.500,00 e não possuir título de capitalização ativo no momento.)\n");
                break;
            }
            if (comprarTitulo(c))
            {
                printf("Título adquirido. R$ %.2f debitados da conta salário.\n", PRECO_TITULO);
            }
            else
            {
                printf("Compra de título negada.\n");
            }
            break;
        }
        case '5':
            break;
        default:
            printf("Opção inválida.\n");
        }
    } while (opcao != '5');
}

// PROGRAMA PRINCIPAL

int main()
{
// Habilitar o UTF-8 no terminal.
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    srand((unsigned)time(NULL)); // Inicializa o gerador de números pseudoaleatórios "rand()" usando o horário atual para que a sequência de números gerados mude a cada execução do programa
    Banco banco = {NULL, 0};

    if (!carregarBanco(&banco, ARQUIVO_BANCO))
    {
        printf("Não foi possivel ler os dados salvos. Iniciando com o banco vazio.\n");
        pausar();
    }
    else if (banco.quantidadeClientes > 0)
    {
        limparTela();
        printf("%d cliente(s) carregado(s).\n", banco.quantidadeClientes);
        pausar();
    }

    char opcao;

    do
    {
        limparTela();
        exibirCabecalho("CARTEIRA DIGITAL");
        printf("1 - Abrir banco\n");
        printf("2 - Consultar histórico\n");
        printf("3 - Efetuar sorteio\n");
        printf("s - Sair\n");
        opcao = lerOpcao("Opção: ", 's'); // Aceita 's' e 'S'

        switch (opcao)
        {
        case '1':
            limparTela();
            exibirCabecalho("ABRIR BANCO");
            abrirBanco(&banco);
            pausar();
            break;
        case '2':
            limparTela();
            exibirCabecalho("HISTÓRICO DO BANCO");
            if (banco.quantidadeClientes == 0)
            {
                printf("Abra o banco antes de consultar o histórico.\n");
            }
            else
            {
                exibirRelatorio(&banco);
            }
            pausar();
            break;
        case '3':
            limparTela();
            exibirCabecalho("SORTEIO");
            if (banco.quantidadeClientes == 0)
            {
                printf("Abra o banco antes de efetuar o sorteio.\n");
            }
            else
            {
                efetuarSorteio(&banco);
            }
            pausar();
            break;
        case 's':
            break;
        default:
            limparTela();
            printf("Opção inválida.\n");
            pausar();
        }
    } while (opcao != 's');

    limparTela();
    exibirCabecalho("ENCERRANDO...");

    if (salvarBanco(&banco, ARQUIVO_BANCO))
    {
        printf("Dados salvos em \"%s\".\n", ARQUIVO_BANCO);
    }

    liberarBanco(&banco);
    printf("Até a próxima!\n");
    pausar();
    limparTela();
    return 0;
}