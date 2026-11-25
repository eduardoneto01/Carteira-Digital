# Carteira Digital em C

Simulação de uma carteira digital de banco feita em C para a disciplina Algoritmos e Pensamento Computacional, vinculada ao curso de tecnologia em Análise e Desenvolvimento de Sistemas do Centro Universitário de João Pessoa (UNIPÊ) e ministrada pela Me. Joanacelle Caldas de Melo.

O programa permite cadastrar clientes, consultar o histórico do banco, realizar sorteios e, ao sair, salva todos os dados em um arquivo de texto.

## Funcionalidades

- **Abrir banco:** cadastra um ou mais clientes com nome e tipo de conta.
- **Consultar histórico:** mostra o saldo total do banco e lista os clientes em diferentes categorias: aptos a fazerem empréstimo, com saldo negativo, aptos a comprar título de capitalização e ganhadores do sorteio. Só fica disponível depois que existir pelo menos um cliente.
- **Efetuar sorteio:** realiza sorteio de um prêmio equivalente a 10 vezes o valor do saldo da conta salário para os (ou até) 5 clientes com maior saldo na conta salário que possuírem título de capitalização.
- **Sair (`s`):** salva os dados em `banco.txt` e encerra o programa.
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

**Histórico de transações do cliente**

- Guarda as **últimas 5 transações** (depósito, saque, empréstimo, título e/ou prêmio). Quando chega a 5, a mais antiga é descartada.

**Empréstimo**

- Exige conta corrente e poupança, além de saldo positivo na conta poupança.
- Cada cliente pode fazer apenas **um** empréstimo.
- Limite: `25% de 10 × (saldo da poupança + saldo da conta corrente)`.
- O valor aprovado é creditado na conta corrente.

**Histórico do banco**

- Saldo total do banco.
- Clientes que podem fazer empréstimo e o limite de cada um (aptos a empréstimo).
- Clientes com saldo negativo na conta corrente.
- Clientes com saldo entre R$ 1.000,00 e R$ 1.500,00 na conta salário (aptos a compra de título).

**Sorteio**

- Realizado entre os cinco clientes com os maiores saldos na conta salário.
- Cada cliente pode comprar apenas **um** título de capitalização.
- Cada título de capitalização custa R$ 20,00 e o valor é debitado diretamente da conta salário.
- Apenas clientes com saldo entre R$ 1.000,00 e R$ 1.500,00 na conta salário podem realizar a compra de título.
- O título de capitalização do vencedor é resgatado após o sorteio para evitar acúmulo de prêmios.
- Para a realização do sorteio, o número de cada participante é sorteado utilizando a função `rand()`, da biblioteca `<stdlib.h>` e, conforme estabelecido previamente em sala de aula, é calculado a soma da sequência de Fibonacci deste valor, seguindo três possíveis critérios para a escolha do vencedor: **(i) maior número primo**; **(ii) maior número par**; **(iii) maior número acima da média**.

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

CONTA 1000
NOME Maria Silva
POSSUI 1 0 1
SALDO 650.00 0.00 200.00
EMPRESTIMO 1
TITULO 0
PREMIO 0.00
HISTORICO 4
300.00 D
200.00 D
150.00 S
500.00 E
```

Para começar com o banco vazio, basta apagar o arquivo `banco.txt`.

## Autores

Eduardo Francisco Firme da Silva e Florentino Cazé de Andrade Segundo Neto.