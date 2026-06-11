#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CONTAS_H
#define CONTAS_H

#define ARQUIVO_BANCO  "contas.dat"
#define MAX_NOME       50
#define MAX_REGISTROS  100

typedef struct {
    int    numeroConta;
    char   nome[MAX_NOME];
    float  saldo;
    int    ativo;
} Cliente;

#endif

void   exibirMenu(void);
void   inicializarArquivo(void);
FILE  *abrirArquivo(const char *modo);
long   totalPosicoes(FILE *arq);
void   cadastrarCliente(void);
void   consultarCliente(void);
void   atualizarSaldo(void);
void   encerrarConta(void);
void   listarClientes(void);
void   rewindEListar(void);
int    buscarPorConta(int numeroConta, long *posEncontrada);
void   imprimirCabecalho(void);
void   imprimirCliente(long pos, Cliente c);
long   contarAtivosRecursivo(FILE *arq, long pos, long total);

void inicializarArquivo(void) {
    FILE *arq = fopen(ARQUIVO_BANCO, "rb");
    if (arq == NULL) {
        arq = fopen(ARQUIVO_BANCO, "wb");
        if (arq == NULL) {
            printf("\nErro ao criar o arquivo de dados!\n");
            exit(1);
        }
        printf("\nArquivo '%s' criado com sucesso.\n", ARQUIVO_BANCO);
    }
    fclose(arq);
}

FILE *abrirArquivo(const char *modo) {
    FILE *arq = fopen(ARQUIVO_BANCO, modo);
    if (arq == NULL) {
        printf("\nErro na abertura do arquivo '%s'!\n", ARQUIVO_BANCO);
        exit(1);
    }
    return arq;
}

long totalPosicoes(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    long bytes = ftell(arq);
    return bytes / (long)sizeof(Cliente);
}

int buscarPorConta(int numeroConta, long *posEncontrada) {
    FILE *arq = abrirArquivo("rb");
    Cliente c;
    long pos = 0;
    int achou = 0;

    fread(&c, sizeof(Cliente), 1, arq);
    if (ferror(arq)) { printf("\nErro na leitura.\n"); fclose(arq); return 0; }

    while (!feof(arq)) {
        if (c.ativo && c.numeroConta == numeroConta) {
            *posEncontrada = pos;
            achou = 1;
            break;
        }
        pos++;
        fread(&c, sizeof(Cliente), 1, arq);
        if (ferror(arq)) { printf("\nErro na leitura.\n"); break; }
    }

    fclose(arq);
    return achou;
}

void imprimirCabecalho(void) {
    printf("\n%-6s %-8s %-40s %12s\n", "Pos.", "Conta", "Nome", "Saldo (R$)");
    printf("---------------------------------------------------------------\n");
}

void imprimirCliente(long pos, Cliente c) {
    printf("%-6ld %-8d %-40s %12.2f\n", pos, c.numeroConta, c.nome, c.saldo);
}

long contarAtivosRecursivo(FILE *arq, long pos, long total) {
    if (pos >= total)
        return 0;

    Cliente c;
    fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
    fread(&c, sizeof(Cliente), 1, arq);
    if (ferror(arq)) return 0;

    long resto = contarAtivosRecursivo(arq, pos + 1, total);

    return (c.ativo ? 1 : 0) + resto;
}

void cadastrarCliente(void) {
    long pos;
    Cliente c;

    printf("\n--- CADASTRAR CLIENTE ---\n");
    printf("Posicao desejada (0 a %d): ", MAX_REGISTROS - 1);
    scanf("%ld%*c", &pos);

    if (pos < 0 || pos >= MAX_REGISTROS) {
        printf("Posicao invalida.\n");
        return;
    }

    FILE *arq = abrirArquivo("rb+");
    long total = totalPosicoes(arq);

    if (pos < total) {
        Cliente existente;
        fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
        fread(&existente, sizeof(Cliente), 1, arq);
        if (ferror(arq)) {
            printf("\nErro na leitura.\n");
            fclose(arq);
            return;
        }
        if (existente.ativo) {
            printf("Posicao %ld ja ocupada pela conta %d (%s).\n",
                   pos, existente.numeroConta, existente.nome);
            fclose(arq);
            return;
        }
    }

    printf("Numero da conta : "); scanf("%d%*c",    &c.numeroConta);
    printf("Nome do cliente : "); scanf("%49[^\n]",  c.nome);
    getchar();
    printf("Saldo inicial   : "); scanf("%f%*c",    &c.saldo);
    c.ativo = 1;

    fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, arq);

    if (ferror(arq))
        printf("Erro na gravacao.\n");
    else
        printf("Cliente cadastrado com sucesso na posicao %ld.\n", pos);

    fclose(arq);
}

void consultarCliente(void) {
    int numero;
    long pos;

    printf("\n--- CONSULTAR CLIENTE ---\n");
    printf("Numero da conta: "); scanf("%d%*c", &numero);

    if (buscarPorConta(numero, &pos)) {
        FILE *arq = abrirArquivo("rb");
        Cliente c;
        fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
        fread(&c, sizeof(Cliente), 1, arq);

        if (ferror(arq))
            printf("Erro na leitura.\n");
        else {
            printf("\nPosicao : %ld\n",   pos);
            printf("Conta   : %d\n",      c.numeroConta);
            printf("Nome    : %s\n",      c.nome);
            printf("Saldo   : R$ %.2f\n", c.saldo);
        }
        fclose(arq);
    } else {
        printf("Conta %d nao encontrada.\n", numero);
    }
}

void atualizarSaldo(void) {
    int numero;
    long pos;

    printf("\n--- ATUALIZAR SALDO ---\n");
    printf("Numero da conta: "); scanf("%d%*c", &numero);

    if (!buscarPorConta(numero, &pos)) {
        printf("Conta %d nao encontrada.\n", numero);
        return;
    }

    FILE *arq = abrirArquivo("rb+");
    Cliente c;

    fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
    fread(&c, sizeof(Cliente), 1, arq);
    if (ferror(arq)) { printf("Erro na leitura.\n"); fclose(arq); return; }

    printf("Saldo atual : R$ %.2f\n", c.saldo);
    printf("Novo saldo  : R$ "); scanf("%f%*c", &c.saldo);

    fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, arq);

    if (ferror(arq))
        printf("Erro na gravacao.\n");
    else
        printf("Saldo atualizado com sucesso.\n");

    fclose(arq);
}

void encerrarConta(void) {
    int numero;
    long pos;

    printf("\n--- ENCERRAR CONTA ---\n");
    printf("Numero da conta a encerrar: "); scanf("%d%*c", &numero);

    if (!buscarPorConta(numero, &pos)) {
        printf("Conta %d nao encontrada.\n", numero);
        return;
    }

    FILE *arq = abrirArquivo("rb+");
    Cliente c;

    fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
    fread(&c, sizeof(Cliente), 1, arq);
    if (ferror(arq)) { printf("Erro na leitura.\n"); fclose(arq); return; }

    c.ativo = 0;

    fseek(arq, pos * (long)sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, arq);

    if (ferror(arq))
        printf("Erro ao encerrar conta.\n");
    else
        printf("Conta %d encerrada. Posicao %ld liberada.\n", numero, pos);

    fclose(arq);
}

void listarClientes(void) {
    printf("\n--- LISTA DE CLIENTES ---\n");

    FILE *arq = abrirArquivo("rb");
    long total = totalPosicoes(arq);

    if (total == 0) {
        printf("Nenhum registro no arquivo.\n");
        fclose(arq);
        return;
    }

    imprimirCabecalho();

    Cliente c;
    long pos = 0;

    fread(&c, sizeof(Cliente), 1, arq);
    if (ferror(arq)) { printf("Erro na leitura.\n"); fclose(arq); return; }

    while (!feof(arq)) {
        if (c.ativo)
            imprimirCliente(pos, c);
        pos++;
        fread(&c, sizeof(Cliente), 1, arq);
        if (ferror(arq)) { printf("Erro na leitura.\n"); break; }
    }

    long ativos = contarAtivosRecursivo(arq, 0, total);
    printf("\nTotal de clientes ativos (contado recursivamente): %ld\n", ativos);

    fclose(arq);
}

void rewindEListar(void) {
    printf("\n--- REWIND: releitura do arquivo a partir do inicio ---\n");

    FILE *arq = abrirArquivo("rb");
    long total = totalPosicoes(arq);

    if (total == 0) {
        printf("Nenhum registro no arquivo.\n");
        fclose(arq);
        return;
    }

    fseek(arq, total * (long)sizeof(Cliente), SEEK_SET);
    printf("(Cursor estava no fim do arquivo)\n");

    rewind(arq);
    printf("rewind() aplicado - cursor voltou ao inicio.\n\n");

    imprimirCabecalho();

    Cliente c;
    long pos = 0;

    fread(&c, sizeof(Cliente), 1, arq);
    if (ferror(arq)) { printf("Erro na leitura.\n"); fclose(arq); return; }

    while (!feof(arq)) {
        if (c.ativo)
            imprimirCliente(pos, c);
        pos++;
        fread(&c, sizeof(Cliente), 1, arq);
        if (ferror(arq)) { printf("Erro na leitura.\n"); break; }
    }

    printf("\nLeitura sequencial concluida via rewind().\n");
    fclose(arq);
}

void exibirMenu(void) {
    printf("\n========================================\n");
    printf("   SISTEMA DE MANUTENCAO DE CONTAS\n");
    printf("========================================\n");
    printf(" 1. Cadastrar novo cliente\n");
    printf(" 2. Consultar cliente (por conta)\n");
    printf(" 3. Atualizar saldo\n");
    printf(" 4. Encerrar conta\n");
    printf(" 5. Listar todos os clientes\n");
    printf(" 6. Rewind + relistar do inicio\n");
    printf(" 7. Encerrar programa\n");
    printf("========================================\n");
    printf("Opcao: ");
}

int main(void) {
    int opcao;

    inicializarArquivo();

    do {
        exibirMenu();
        scanf("%d%*c", &opcao);

        switch (opcao) {
            case 1: cadastrarCliente(); break;
            case 2: consultarCliente(); break;
            case 3: atualizarSaldo();   break;
            case 4: encerrarConta();    break;
            case 5: listarClientes();   break;
            case 6: rewindEListar();    break;
            case 7: printf("\nSistema encerrado.\n"); break;
            default: printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 7);

    return 0;
}