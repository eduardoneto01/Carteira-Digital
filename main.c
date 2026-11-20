#include <ctype.h>
#include <stdio.h>
#include <string.h>
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

// Mostra só as contas que o cliente possui e devolve o valor do tipo escolhido (CORRENTE, SALARIO ou POUPANCA). Se ele tiver uma única conta, escolhe automaticamente.
static int escolherTipoConta(const Conta *c)
{
    const char *nomes[QUANTIDADE_TIPOS] = {"Corrente", "Salário", "Poupança"};
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

    printf("Em qual conta?\n");
    for (int i = 0; i < quantidade; i++)
    {
        printf("%d - %s\n", i + 1, nomes[tiposDisponiveis[i]]);
    }

    int opcao;
    do
    {
        opcao = lerInteiro("Opção: ");
        if (opcao < 1 || opcao > quantidade)
            printf("Opção inválida.\n");
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
        printf("4 - Voltar\n");
        opcao = lerOpcao("Opção: ", '4');

        switch (opcao)
        {
        case '1':
        {
            int tipo = escolherTipoConta(c);
            float valor = lerFloat("Valor do depósito: R$ ");
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
                printf("(Exige conta corrente + poupança, saldos positivos e nenhum empréstimo anterior.)\n");
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
            break;
        default:
            printf("Opção inválida.\n");
        }
    } while (opcao != '4');
}

// Opção "Consultar histórico" do menu principal.
static void consultarHistorico(Banco *banco)
{
    if (banco->quantidadeClientes == 0)
    {
        printf("\nNenhum cliente cadastrado. Use \"Abrir banco\" primeiro.\n");
        return;
    }

    printf("\n== CLIENTES ==\n");
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        printf("Conta %d - %s\n", banco->contas[i].numero, banco->contas[i].cliente);
    }

    int numero = lerInteiro("\nNúmero da conta: ");
    Conta *c = consultarCliente(banco, numero);

    if (c == NULL)
    {
        printf("Conta não encontrada.\n");
        return;
    }

    menuCliente(c);
}

// PROGRAMA PRINCIPAL

int main()
{
// Habilitar o UTF-8 no terminal.
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    Banco banco = {NULL, 0};

    if (!carregarBanco(&banco, ARQUIVO_BANCO))
    {
        printf("Não foi possivel ler os dados salvos. Iniciando com o banco vazio.\n");
    }
    else if (banco.quantidadeClientes > 0)
    {
        printf("%d cliente(s) carregado(s).\n", banco.quantidadeClientes);
    }

    char opcao;

    do
    {
        printf("\n== CARTEIRA DIGITAL ==\n");
        printf("1 - Abrir banco\n");
        printf("2 - Consultar histórico\n");
        printf("s - Sair\n");
        opcao = lerOpcao("Opção: ", 's'); // aceita 's' e 'S'

        switch (opcao)
        {
        case '1':
            criarConta(&banco);
            break;
        case '2':
            consultarHistorico(&banco);
            break;
        case 's':
            break;
        default:
            printf("Opção inválida.\n");
        }
    } while (opcao != 's');

    exibirRelatorio(&banco);

    if (salvarBanco(&banco, ARQUIVO_BANCO))
    {
        printf("\nDados salvos em \"%s\".\n", ARQUIVO_BANCO);
    }

    liberarBanco(&banco);
    printf("Até a próxima!\n");
    return 0;
}