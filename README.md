# Carteira Digital em C

Simulação de uma carteira digital de banco feita em C para a disciplina Algoritmos e Pensamento Computacional, vinculada ao curso de tecnologia em Análise e Desenvolvimento de Sistemas do Centro Universitário de João Pessoa (UNIPÊ) e ministrada pela Me. Joanacelle Caldas de Melo.

O programa permite cadastrar clientes, movimentar contas (depósito, saque e empréstimo), consultar o histórico das últimas transações e, ao sair, exibe um relatório geral e salva todos os dados em um arquivo de texto.

## Funcionalidades

- **Abrir banco:** cadastra um cliente com nome e tipo de conta.
- **Consultar histórico:** mostra os dados e as últimas 5 transações de um cliente e permite depositar, sacar e solicitar empréstimo. Só fica disponível depois que existir pelo menos um cliente.
- **Sair (`s`):** exibe o relatório do banco, salva os dados em `banco.txt` e encerra o programa.
- **Persistência:** ao iniciar, os dados salvos anteriormente são carregados automaticamente.

## Regras de negócio

**Tipos de conta permitidos**

| Opção | Contas |
|---|---|
| 1 | Corrente |
| 2 | Salário |
| 3 | Poupança |
| 4 | Corrente + Poupança |
| 5 | Salário + Poupança |

Não é permitido ter conta corrente e conta salário ao mesmo tempo.

**Empréstimo**

- Exige conta corrente e poupança, com saldo positivo nas duas.
- Cada cliente pode fazer apenas **um** empréstimo.
- Limite: `25% de 10 × (saldo da poupança + saldo da conta corrente)`.
- O valor aprovado é creditado na conta corrente.

**Histórico**

- Guarda as **últimas 5 transações** (depósito, saque ou empréstimo). Quando chega a 5, a mais antiga é descartada.

**Relatório (exibido ao sair)**

- Saldo total do banco.
- Clientes que podem fazer empréstimo e o limite de cada um.
- Clientes com saldo negativo na conta corrente.
- Clientes com saldo entre R$ 1.000,00 e R$ 1.500,00 na conta salário.

## Estrutura do projeto

```
├── main.c        # Menu principal e submenu de operações
├── banco.c       # Implementação das funções (cadastro, transações, regras, relatórios e arquivos)
├── meuBanco.h    # Structs, constantes e protótipos das funções
└── banco.txt     # Gerado pelo programa com os dados salvos
```

## Como compilar e executar

É necessário ter o `gcc` instalado.

**macOS / Linux / WSL**

```bash
gcc main.c banco.c -o banco -Wall -Wextra
./banco
```

No macOS, se o `gcc` não estiver instalado, rode `xcode-select --install`.

**Windows (MinGW / MSYS2)**

```
gcc main.c banco.c -o banco.exe -Wall -Wextra
.\banco.exe
```

No Windows 11, o *Controle Inteligente de Aplicações* (Smart App Control) pode bloquear o executável. Nesse caso, é necessário desativar temporariamente essa proteção na Segurança do Windows, ou compilar e rodar pelo WSL.

## Formato do arquivo de dados

Os dados são salvos no arquivo `banco.txt`, seguindo o formato:

```
# Carteira digital – Dados dos clientes
# POSSUI e SALDO seguem a ordem: Corrente – Salario – Poupanca
NUMERO DE CLIENTES 1

CONTA 1
NOME Maria Silva
POSSUI 1 0 1
SALDO 650.00 0.00 200.00
EMPRESTIMO 1
HISTORICO 4
300.00 D
200.00 D
150.00 S
500.00 E
```

Para começar com o banco vazio, basta apagar o arquivo `banco.txt`.

## Autores

Eduardo Francisco Firme da Silva e Florentino Cazé de Andrade Segundo Neto.