#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meuBanco.h"

#define CRITERIO_PRIMO 1
#define CRITERIO_PAR 2
#define CRITERIO_ACIMA_MEDIA 3

// Transforma o código na primeira letra do nome da transação. Ex.: 3 (EMPRESTIMO) → "E".
static const char *codigoTransacao(int tipo)
{
    switch (tipo)
    {
    case DEPOSITO:
        return "D";
    case SAQUE:
        return "S";
    case EMPRESTIMO:
        return "E";
    case TITULO:
        return "T";
    case PREMIO:
        return "P";
    default:
        return "DESCONHECIDA";
    }
}

// Transforma a primeira letra do nome da transação no código correspondente. Ex.: "S" → 2 (SAQUE). Retorna 0 se não reconhecer.
static int tipoDoCodigo(const char *codigo)
{
    if (strcmp(codigo, "D") == 0)
    {
        return DEPOSITO;
    }
    if (strcmp(codigo, "S") == 0)
    {
        return SAQUE;
    }
    if (strcmp(codigo, "E") == 0)
    {
        return EMPRESTIMO;
    }
    if (strcmp(codigo, "T") == 0)
    {
        return TITULO;
    }
    if (strcmp(codigo, "P") == 0)
    {
        return PREMIO;
    }
    return 0;
}

static int proximoNumeroConta(const Banco *banco)
{
    int maior = NUMERO_INICIAL - 1;
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        if (banco->contas[i].numero > maior)
            maior = banco->contas[i].numero;
    }
    return maior + 1;
}

// Lê a próxima linha útil do arquivo, pulando linhas vazias e comentários (#). Remove o \n do final. Retorna 0 no fim do arquivo.
static int lerLinhaArquivo(FILE *arquivo, char *linha, int tamanho)
{
    while (fgets(linha, tamanho, arquivo) != NULL)
    {
        linha[strcspn(linha, "\r\n")] = '\0'; // O "\r" aparece se o arquivo foi editado no Windows.

        if (linha[0] != '\0' && linha[0] != '#')
            return 1;
    }
    return 0;
}

// LEITURA

void lerTexto(const char *mensagem, char *input, int tamanho)
{
    printf("%s", mensagem);
    if (fgets(input, tamanho, stdin) == NULL)
    {
        input[0] = '\0';
        return;
    }
    size_t comprimento = strcspn(input, "\n");
    if (input[comprimento] == '\n')
    {
        input[comprimento] = '\0';
    }
    else
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
}

int lerInteiro(const char *mensagem)
{
    char input[32];
    int valor;

    while (1)
    {
        lerTexto(mensagem, input, sizeof input);

        if (sscanf(input, "%d", &valor) == 1)
            return valor;

        printf("Valor inválido, tente novamente.\n");
    }
}

float lerFloat(const char *mensagem)
{
    char input[32];
    float valor;

    while (1)
    {
        lerTexto(mensagem, input, sizeof input);

        if (sscanf(input, "%f", &valor) == 1)
            return valor;

        printf("Valor inválido, tente novamente.\n");
    }
}

// INTERFACE

void limparTela(void)
{
#ifdef _WIN32
    system("cls"); // Windows.
#else
    system("clear"); // macOS e Linux.
#endif
}

void pausar(void)
{
    char tecla[8];
    lerTexto("\nPressione Enter para continuar...", tecla, sizeof tecla);
}

// NUMERAÇÃO

// Adiciona ao número da conta o sufixo correspondente ao tipo de conta (Corrente, "-01"; Salário, "-02"; Poupança, "-03").
void formatarNumeroConta(char *destino, int tamanho, int numero, int tipoConta)
{
    snprintf(destino, tamanho, "%d-%02d", numero, tipoConta + 1);
}

// CADASTRO

const char *nomeTipoConta(int tipo)
{
    switch (tipo)
    {
    case CORRENTE:
        return "Corrente";
    case SALARIO:
        return "Salário";
    case POUPANCA:
        return "Poupança";
    default:
        return "Desconhecida";
    }
}

int criarConta(Banco *banco)
{
    Conta nova;
    memset(&nova, 0, sizeof nova); // Zera saldos e histórico.

    printf("\n== ABRIR CONTA ==\n");

    do
    {
        lerTexto("Nome do cliente: ", nova.cliente, sizeof nova.cliente);
        if (strlen(nova.cliente) == 0)
        {
            printf("O nome não pode ficar vazio.\n");
        }
    } while (strlen(nova.cliente) == 0);

    printf("\nTipo de conta:\n");
    printf("1 - Corrente\n");
    printf("2 - Salário\n");
    printf("3 - Poupança\n");
    printf("4 - Corrente + Poupança\n");
    printf("5 - Salário + Poupança\n");

    int opcao;
    do
    {
        opcao = lerInteiro("Opção: ");
        if (opcao < 1 || opcao > 5)
        {
            printf("Opção inválida. Escolha de 1 a 5.\n");
        }
    } while (opcao < 1 || opcao > 5);

    switch (opcao)
    {
    case 1:
        nova.possui[CORRENTE] = 1;
        break;
    case 2:
        nova.possui[SALARIO] = 1;
        break;
    case 3:
        nova.possui[POUPANCA] = 1;
        break;
    case 4:
        nova.possui[CORRENTE] = 1;
        nova.possui[POUPANCA] = 1;
        break;
    case 5:
        nova.possui[SALARIO] = 1;
        nova.possui[POUPANCA] = 1;
        break;
    }

    nova.numero = proximoNumeroConta(banco);

    if (!adicionarConta(banco, &nova))
    {
        printf("Não foi possivel cadastrar o cliente.\n");
        return 0;
    }

    printf("\nConta(s) criada(s) com sucesso!\n");

    for (int tipo = 0; tipo < QUANTIDADE_TIPOS; tipo++)
    {
        if (nova.possui[tipo])
        {
            char numeroConta[16];
            formatarNumeroConta(numeroConta, sizeof numeroConta, nova.numero, tipo);

            printf("\n[Conta %s]\n", nomeTipoConta(tipo));
            printf("Número: %s\n", numeroConta);
        }
    }
    return 1;
}

int abrirBanco(Banco *banco)
{
    int numeroClientes;

    do
    {
        numeroClientes = lerInteiro("Quantos clientes você deseja cadastrar? ");

        if (numeroClientes <= 0)
        {
            printf("O número de clientes precisa ser positivo, tente novamente.\n");
        }
    } while (numeroClientes <= 0);

    for (int cliente = 0; cliente < numeroClientes; cliente++)
    {
        printf("\n-- Cliente %d --\n", cliente + 1);

        if (!criarConta(banco))
        {
            printf("Erro ao cadastrar cliente.\n");
            return 0;
        }
    }

    return 1;
}

// CONSULTA

// Lista todos os clientes cadastrados no banco.
void consultarClientes(Banco *banco)
{
    if (banco->quantidadeClientes == 0)
    {
        printf("\nNenhum cliente cadastrado. Use \"Abrir banco\" primeiro.\n");
        return;
    }

    printf("== CLIENTES ==\n\n");
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        printf("Conta %d - %s\n", banco->contas[i].numero, banco->contas[i].cliente);
    }
}

// Retorna um ponteiro para a conta com o número escolhido, ou NULL. O ponteiro deixa de valer depois que um novo cliente for cadastrado, já que o realloc pode mudar o vetor de lugar na memória.
Conta *consultarCliente(Banco *banco, int numero)
{
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        if (banco->contas[i].numero == numero)
        {
            return &banco->contas[i];
        }
    }
    return NULL;
}

void exibirDadosCliente(const Conta *c)
{
    printf("\n== DADOS DO CLIENTE ==\n\n");
    printf("Cliente: %s\n\n", c->cliente);

    float total = 0;
    for (int tipo = 0; tipo < QUANTIDADE_TIPOS; tipo++)
    {
        if (c->possui[tipo])
        {
            char numeroConta[16];
            formatarNumeroConta(numeroConta, sizeof numeroConta, c->numero, tipo);

            printf("[Conta %s]\n", nomeTipoConta(tipo));
            printf("Número: %s\n", numeroConta);
            printf("Saldo: R$ %.2f\n\n", c->saldo[tipo]);
            alertarSaldoNegativo(c);
            total += c->saldo[tipo];
        }
    }
    printf("Saldo total: R$ %.2f\n", total);

    if (c->temEmprestimo)
    {
        printf("Empréstimo: já utilizado\n");
    }
    else if (podeFazerEmprestimo(c))
    {
        printf("Empréstimo: disponível (limite de R$ %.2f)\n",
               informarLimiteEmprestimo(c->saldo[POUPANCA], c->saldo[CORRENTE]));
    }
    else
    {
        printf("Empréstimo: não disponível\n");
    }
}

void exibirHistorico(const Conta *c)
{
    printf("\n== HISTÓRICO (últimas %d transações) ==\n", MAX_HISTORICO);

    if (c->quantidadeHistorico == 0)
    {
        printf("Nenhuma transação registrada.\n");
        return;
    }

    for (int i = 0; i < c->quantidadeHistorico; i++)
    {
        float valor = c->historico[i].valor;
        float absoluto = (valor < 0) ? -valor : valor;

        printf("%d. R$ %.2f (%s)\n", i + 1, absoluto, codigoTransacao(c->historico[i].tipo));
    }
}

void alertarSaldoNegativo(const Conta *c)
{
    if (c->possui[CORRENTE] && c->saldo[CORRENTE] < 0)
    {
        char numeroConta[16];
        formatarNumeroConta(numeroConta, sizeof numeroConta, c->numero, CORRENTE);

        printf("Alerta! Sua conta %s está com saldo negativo de -R$ %.2f. Evite encargos fazendo uma transação.\n", numeroConta, (c->saldo[CORRENTE] * -1));
    }
}

// ARQUIVOS

int salvarBanco(const Banco *banco, const char *caminho)
{
    FILE *arquivo = fopen(caminho, "w");
    if (arquivo == NULL)
    {
        perror("Erro ao abrir o arquivo para salvar");
        return 0;
    }

    fprintf(arquivo, "# Carteira digital – Dados dos clientes\n");
    fprintf(arquivo, "# POSSUI e SALDO seguem a ordem: Corrente - Salario - Poupanca\n");
    fprintf(arquivo, "\nNUMERO DE CLIENTES %d\n", banco->quantidadeClientes);

    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        const Conta *c = &banco->contas[i];

        fprintf(arquivo, "\nCONTA %d\n", c->numero);
        fprintf(arquivo, "NOME %s\n", c->cliente);
        fprintf(arquivo, "POSSUI %d %d %d\n", c->possui[CORRENTE], c->possui[SALARIO], c->possui[POUPANCA]);
        fprintf(arquivo, "SALDO %.2f %.2f %.2f\n", c->saldo[CORRENTE], c->saldo[SALARIO], c->saldo[POUPANCA]);
        fprintf(arquivo, "EMPRESTIMO %d\n", c->temEmprestimo);
        fprintf(arquivo, "TITULO %d\n", c->temTitulo);
        fprintf(arquivo, "PREMIO %.2f\n", c->premioRecebido);
        fprintf(arquivo, "HISTORICO %d\n", c->quantidadeHistorico);

        for (int j = 0; j < c->quantidadeHistorico; j++)
        {
            fprintf(arquivo, "%.2f %s\n", c->historico[j].valor, codigoTransacao(c->historico[j].tipo));
        }
    }

    // O "ferror" avisa se algum "fprintf" falhou. O "fclose" também pode falhar (é nele que o buffer vai para o disco).
    int ok = !ferror(arquivo);
    if (fclose(arquivo) != 0)
    {
        ok = 0;
    }

    if (!ok)
    {
        printf("Erro ao gravar os dados no arquivo.\n");
    }

    return ok;
}

// Lê os dados de um cliente por completo, desde "CONTA" até a última transação. Retorna 0 se alguma linha não estiver no formato esperado.
static int lerContaArquivo(FILE *arquivo, Conta *c)
{
    char linha[128];
    memset(c, 0, sizeof *c);

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "CONTA %d", &c->numero) != 1)
    {
        return 0;
    }

    // O nome pode ter espaços, então copia tudo depois de "NOME ".
    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || strncmp(linha, "NOME ", 5) != 0)
    {
        return 0;
    }
    strncpy(c->cliente, linha + 5, TAMANHO_NOME - 1); // O memset garante o '\0' no final.

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "POSSUI %d %d %d", &c->possui[CORRENTE], &c->possui[SALARIO], &c->possui[POUPANCA]) != 3)
    {
        return 0;
    }

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "SALDO %f %f %f", &c->saldo[CORRENTE], &c->saldo[SALARIO], &c->saldo[POUPANCA]) != 3)
    {
        return 0;
    }

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "EMPRESTIMO %d", &c->temEmprestimo) != 1)
    {
        return 0;
    }

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "TITULO %d", &c->temTitulo) != 1)
    {
        return 0;
    }

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "PREMIO %f", &c->premioRecebido) != 1)
    {
        return 0;
    }

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "HISTORICO %d", &c->quantidadeHistorico) != 1 || c->quantidadeHistorico < 0 || c->quantidadeHistorico > MAX_HISTORICO)
    {
        return 0;
    }

    for (int j = 0; j < c->quantidadeHistorico; j++)
    {
        char codigo[16];
        float valor;

        if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "%f %15s", &valor, codigo) != 2)
        {
            return 0;
        }

        int tipo = tipoDoCodigo(codigo);
        if (tipo == 0)
        {
            return 0;
        }

        c->historico[j].tipo = tipo;
        c->historico[j].valor = valor;
    }

    return 1;
}

// Retorna 1 se carregou (ou se o arquivo ainda não existe, que é o caso da primeira execução) e 0 se o arquivo existe, mas está com problema. O banco deve estar vazio ao chamar esta função.
int carregarBanco(Banco *banco, const char *caminho)
{
    banco->contas = NULL;
    banco->quantidadeClientes = 0;

    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL)
    {
        return 1;
    }

    char linha[128];
    int quantidade;

    if (!lerLinhaArquivo(arquivo, linha, sizeof linha) || sscanf(linha, "NUMERO DE CLIENTES %d", &quantidade) != 1 || quantidade < 0)
    {
        printf("Arquivo de dados inválido.\n");
        fclose(arquivo);
        return 0;
    }

    for (int i = 0; i < quantidade; i++)
    {
        Conta c;

        if (!lerContaArquivo(arquivo, &c))
        {
            printf("Arquivo de dados inválido (problema no cliente %d).\n", i + 1);
            fclose(arquivo);
            liberarBanco(banco); // Descarta o que já tinha sido carregado.
            return 0;
        }

        // Reaproveita a mesma função do cadastro (realloc).
        if (!adicionarConta(banco, &c))
        {
            fclose(arquivo);
            liberarBanco(banco);
            return 0;
        }
    }

    fclose(arquivo);
    return 1;
}

// MEMÓRIA

int adicionarConta(Banco *banco, const Conta *nova)
{
    Conta *temporaria = realloc((*banco).contas, ((*banco).quantidadeClientes + 1) * sizeof(Conta));
    if (temporaria == NULL)
    {
        printf("Erro ao alocar memória.\n");
        return 0;
    }
    (*banco).contas = temporaria;
    (*banco).contas[(*banco).quantidadeClientes] = *nova;
    (*banco).quantidadeClientes++;
    return 1;
}

void liberarBanco(Banco *banco)
{
    if ((*banco).contas != NULL)
    {
        free((*banco).contas);
        (*banco).contas = NULL;
        (*banco).quantidadeClientes = 0;
    }
}

// TRANSAÇÕES

void registrarTransacao(Conta *c, float valor, int tipo)
{
    if ((*c).quantidadeHistorico >= MAX_HISTORICO)
    {
        for (int i = 0; i < MAX_HISTORICO - 1; i++)
        {
            (*c).historico[i] = (*c).historico[i + 1];
        }
        (*c).quantidadeHistorico = MAX_HISTORICO - 1;
    }
    (*c).historico[(*c).quantidadeHistorico].valor = valor;
    (*c).historico[(*c).quantidadeHistorico].tipo = tipo;
    (*c).quantidadeHistorico++;
}

int depositar(Conta *c, int tipoConta, float valor)
{
    if (valor <= 0 || tipoConta < 0 || tipoConta >= QUANTIDADE_TIPOS)
    {
        return 0;
    }
    if (!c->possui[tipoConta])
    {
        return 0;
    }

    c->saldo[tipoConta] += valor;
    registrarTransacao(c, valor, DEPOSITO);
    return 1;
}

int sacar(Conta *c, int tipoConta, float valor)
{
    if (valor <= 0 || tipoConta < 0 || tipoConta >= QUANTIDADE_TIPOS)
    {
        return 0;
    }
    if (!c->possui[tipoConta])
    {
        return 0;
    }
    if (tipoConta != CORRENTE && c->saldo[tipoConta] < valor)
    {
        return 0; // Salário e poupança não podem ficar negativas.
    }

    c->saldo[tipoConta] -= valor;
    registrarTransacao(c, valor, SAQUE);
    return 1;
}

// EMPRÉSTIMO

// Pode pedir empréstimo quem tem conta corrente, saldo na poupança e ainda não fez empréstimo. Como a corrente pode estar negativa, o limite também precisa ser positivo.
int podeFazerEmprestimo(const Conta *c)
{
    if (c->temEmprestimo || !c->possui[CORRENTE] || !c->possui[POUPANCA])
    {
        return 0;
    }
    if (c->saldo[POUPANCA] <= 0)
    {
        return 0;
    }
    return informarLimiteEmprestimo(c->saldo[POUPANCA], c->saldo[CORRENTE]) > 0;
}

float informarLimiteEmprestimo(float saldoPoupanca, float saldoCorrente)
{
    return 10.0 * (saldoPoupanca + saldoCorrente) * 0.25;
}

int solicitarEmprestimo(Conta *c, float valor)
{
    if (valor <= 0)
    {
        return 0;
    }

    if (podeFazerEmprestimo(c))
    {
        float limite = informarLimiteEmprestimo((*c).saldo[POUPANCA], (*c).saldo[CORRENTE]);
        if (valor <= limite)
        {
            (*c).saldo[CORRENTE] += valor;
            (*c).temEmprestimo = 1;
            registrarTransacao(c, valor, EMPRESTIMO);
            return 1;
        }
    }
    return 0;
}

// TÍTULOS

// Pode comprar título quem tem conta salário, ainda não tem título e possui entre R$ 1.000,00 e R$ 1.500,00 na salário.
int podeComprarTitulo(const Conta *c)
{
    return c->possui[SALARIO] && !c->temTitulo && c->saldo[SALARIO] >= TITULO_MIN && c->saldo[SALARIO] <= TITULO_MAX;
}

// Debita o preço do título da conta salário. Retorna 1 se comprou.
int comprarTitulo(Conta *c)
{
    if (!podeComprarTitulo(c))
    {
        return 0;
    }

    c->saldo[SALARIO] -= PRECO_TITULO;
    c->temTitulo = 1;
    registrarTransacao(c, PRECO_TITULO, TITULO);
    return 1;
}

// SORTEIO

int fibonacci(int valor)
{
    if (valor == 0)
    {
        return 0;
    }

    if (valor == 1)
    {
        return 1;
    }

    return fibonacci(valor - 1) + fibonacci(valor - 2);
}

int somaFibonacci(int valor)
{
    int soma = 0;

    for (int i = 1; i <= valor; i++)
    {
        soma += fibonacci(i);
    }

    return soma;
}

int ehPrimo(int valor)
{
    if (valor <= 1)
    {
        return 0;
    }

    for (int i = 2; i <= valor / i; i++)
    {
        if (valor % i == 0)
        {
            return 0;
        }
    }

    return 1;
}

// Posição (no vetor de participantes) do maior resultado que atende ao critério, ou -1 se ninguém atender. Em caso de empate, fica quem aparece primeiro (maior saldo).
static int maiorResultadoPorCriterio(const int resultados[], int quantidade, int criterio, double media)
{
    int vencedor = -1;

    for (int i = 0; i < quantidade; i++)
    {
        int atende = 0;

        if (criterio == CRITERIO_PRIMO)
        {
            atende = ehPrimo(resultados[i]);
        }
        else if (criterio == CRITERIO_PAR)
        {
            atende = resultados[i] % 2 == 0;
        }
        else if (criterio == CRITERIO_ACIMA_MEDIA)
        {
            atende = resultados[i] > media;
        }

        if (atende && (vencedor == -1 || resultados[i] > resultados[vencedor]))
        {
            vencedor = i;
        }
    }

    return vencedor;
}

// Sorteia entre os (ou até) 5 clientes com título de maior saldo na conta salário. Retorna 1 se houve vencedor.
int efetuarSorteio(Banco *banco)
{
    // Índices dos clientes que têm título.
    int *indices = malloc(banco->quantidadeClientes * sizeof(int));
    if (indices == NULL)
    {
        printf("Erro ao alocar memória.\n");
        return 0;
    }

    int clientesComTitulo = 0;
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        if (banco->contas[i].temTitulo)
        {
            indices[clientesComTitulo++] = i;
        }
    }

    if (clientesComTitulo == 0)
    {
        printf("Nenhum cliente possui título de capitalização. Não há participantes para o sorteio.\n");
        free(indices);
        return 0;
    }

    // Ordena os índices pelo saldo da conta salário, do maior para o menor (selection sort).
    for (int i = 0; i < clientesComTitulo - 1; i++)
    {
        int maior = i;
        for (int j = i + 1; j < clientesComTitulo; j++)
        {
            if (banco->contas[indices[j]].saldo[SALARIO] > banco->contas[indices[maior]].saldo[SALARIO])
            {
                maior = j;
            }
        }
        int temporario = indices[i];
        indices[i] = indices[maior];
        indices[maior] = temporario;
    }

    int participantes = (clientesComTitulo < MAX_PARTICIPANTES_SORTEIO) ? clientesComTitulo : MAX_PARTICIPANTES_SORTEIO;

    // Sorteia um número para cada participante e calcula a soma de Fibonacci.
    int numeros[MAX_PARTICIPANTES_SORTEIO];
    int resultados[MAX_PARTICIPANTES_SORTEIO];
    double media = 0;

    for (int i = 0; i < participantes; i++)
    {
        numeros[i] = rand() % LIMITE_NUMERO_SORTEIO + 1;
        resultados[i] = somaFibonacci(numeros[i]);
        media += resultados[i];
    }
    media /= participantes;

    // Mostra os participantes.
    printf("== Participantes ==\n");
    for (int i = 0; i < participantes; i++)
    {
        const Conta *c = &banco->contas[indices[i]];
        char numeroConta[16];
        formatarNumeroConta(numeroConta, sizeof numeroConta, c->numero, SALARIO);

        printf("%s | %s | Saldo: R$ %.2f | Número sorteado: %d | Soma Fibonacci: %d%s%s\n", c->cliente, numeroConta, c->saldo[SALARIO], numeros[i], resultados[i], ehPrimo(resultados[i]) ? " (primo)" : "", resultados[i] % 2 == 0 ? " (par)" : "");
    }
    printf("Média dos resultados: %.2f\n", media);

    // Escolhe o vencedor: maior primo, depois maior par, depois maior acima da média.
    const char *criterio = "maior resultado primo";
    int vencedor = maiorResultadoPorCriterio(resultados, participantes, CRITERIO_PRIMO, media);

    if (vencedor == -1)
    {
        criterio = "maior resultado par";
        vencedor = maiorResultadoPorCriterio(resultados, participantes, CRITERIO_PAR, media);
    }
    if (vencedor == -1)
    {
        criterio = "maior resultado acima da média";
        vencedor = maiorResultadoPorCriterio(resultados, participantes, CRITERIO_ACIMA_MEDIA, media);
    }

    if (vencedor == -1)
    {
        printf("\nNenhum participante atendeu aos critérios. O sorteio ficou sem vencedor.\n");
        free(indices);
        return 0;
    }

    // Credita o prêmio na conta salário do vencedor.
    Conta *ganhador = &banco->contas[indices[vencedor]];
    float premio = MULTIPLICADOR_PREMIO * ganhador->saldo[SALARIO];

    ganhador->saldo[SALARIO] += premio;
    ganhador->premioRecebido += premio;
    ganhador->temTitulo = 0;
    registrarTransacao(ganhador, premio, PREMIO);

    char numeroConta[16];
    formatarNumeroConta(numeroConta, sizeof numeroConta, ganhador->numero, SALARIO);

    printf("\n== Cliente Contemplado no Sorteio ==\n");
    printf("%s | %s | R$ %.2f\n", ganhador->cliente, numeroConta, premio);
    printf("Critério: %s.\n", criterio);

    free(indices);
    return 1;
}

// RELATÓRIOS

void consultarSaldoTotalBanco(const Banco *banco)
{
    float totalBanco = 0;
    for (int i = 0; i < (*banco).quantidadeClientes; i++)
    {
        for (int t = 0; t < QUANTIDADE_TIPOS; t++)
            totalBanco += banco->contas[i].saldo[t];
    }

    printf("== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", banco->quantidadeClientes, totalBanco);
}

void listarClientesEmprestimo(const Banco *banco)
{
    printf("\n== Clientes Aptos a Empréstimos ==\n");
    int contagem = 0;
    for (int i = 0; i < (*banco).quantidadeClientes; i++)
    {
        if (podeFazerEmprestimo(&(*banco).contas[i]))
        {
            float limite = informarLimiteEmprestimo((*banco).contas[i].saldo[POUPANCA], (*banco).contas[i].saldo[CORRENTE]);
            char numeroConta[16];

            formatarNumeroConta(numeroConta, sizeof numeroConta, (*banco).contas[i].numero, CORRENTE);

            printf("%s | %s | R$ %.2f\n", (*banco).contas[i].cliente, numeroConta, limite);
            contagem++;
        }
    }
    if (contagem == 0)
    {
        printf("Nenhum cliente está autorizado a fazer Empréstimo no momento.\n");
    }
}

void verificarSaldoNegativo(const Banco *banco)
{
    printf("\n== Clientes com Saldo Negativo na Conta Corrente ==\n");
    int contagem = 0;
    for (int i = 0; i < (*banco).quantidadeClientes; i++)
    {
        if ((*banco).contas[i].saldo[CORRENTE] < 0)
        {
            char numeroConta[16];
            formatarNumeroConta(numeroConta, sizeof numeroConta, (*banco).contas[i].numero, CORRENTE);

            printf("%s | %s | -R$ %.2f\n", (*banco).contas[i].cliente, numeroConta, ((*banco).contas[i].saldo[CORRENTE] * -1));
            contagem++;
        }
    }
    if (contagem == 0)
    {
        printf("Nenhum cliente está com Saldo Negativo na Conta Corrente.\n");
    }
}

void listarClientesAptosTitulo(const Banco *banco)
{
    printf("\n== Clientes Aptos a Compra de Títulos ==\n");
    int contagem = 0;
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        if (podeComprarTitulo(&banco->contas[i]))
        {
            char numeroConta[16];
            formatarNumeroConta(numeroConta, sizeof numeroConta, banco->contas[i].numero, SALARIO);

            printf("%s | %s | R$ %.2f\n", banco->contas[i].cliente, numeroConta, banco->contas[i].saldo[SALARIO]);
            contagem++;
        }
    }
    if (contagem == 0)
        printf("Nenhum cliente está apto a comprar títulos no momento.\n");
}

void listarClientesSorteados(const Banco *banco)
{
    printf("\n== Clientes Contemplados no Sorteio ==\n");
    int contagem = 0;
    for (int i = 0; i < banco->quantidadeClientes; i++)
    {
        if (banco->contas[i].premioRecebido > 0)
        {
            char numeroConta[16];
            formatarNumeroConta(numeroConta, sizeof numeroConta, banco->contas[i].numero, SALARIO);

            printf("%s | %s | R$ %.2f\n", banco->contas[i].cliente, numeroConta, banco->contas[i].premioRecebido);
            contagem++;
        }
    }
    if (contagem == 0)
        printf("Nenhum cliente foi contemplado em sorteios até o momento.\n");
}

void exibirRelatorio(const Banco *banco)
{
    consultarSaldoTotalBanco(banco);
    listarClientesEmprestimo(banco);
    verificarSaldoNegativo(banco);
    listarClientesAptosTitulo(banco);
    listarClientesSorteados(banco);
}