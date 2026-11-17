#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "loja.h"

/* Le uma opcao inteira e evita que letras ou entradas invalidas quebrem o menu. */
static int lerOpcao(void) {
    char entrada[100];
    char *fim;
    long valor;

    if (fgets(entrada, sizeof(entrada), stdin) == NULL) return -1;
    errno = 0;
    valor = strtol(entrada, &fim, 10);

    if (fim == entrada || errno != 0) return -1;
    while (*fim == ' ' || *fim == '\t' || *fim == '\n' || *fim == '\r') fim++;
    if (*fim != '\0' || valor < 0 || valor > 5) return -1;
    return (int)valor;
}

/* Escreve texto no CSV entre aspas e duplica aspas que ja existirem no texto. */
static void escreverCampoCSV(FILE *arquivo, const char *texto) {
    fputc('"', arquivo);
    while (*texto != '\0') {
        if (*texto == '"') fputc('"', arquivo);
        fputc(*texto, arquivo);
        texto++;
    }
    fputc('"', arquivo);
}

/* Converte o arquivo de produtos separado por | para um arquivo CSV. */
static int exportarCSV(void) {
    FILE *origem = fopen(ARQUIVO_PRODUTOS, "r");
    FILE *destino;
    char linha[400];
    int total = 0;

    if (origem == NULL) {
        printf("Nao foi possivel abrir o arquivo de produtos.\n");
        return 0;
    }

    destino = fopen("produtos.csv", "w");
    if (destino == NULL) {
        printf("Nao foi possivel criar o arquivo CSV.\n");
        fclose(origem);
        return 0;
    }

    fprintf(destino, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n");

    while (fgets(linha, sizeof(linha), origem) != NULL) {
        char *campos[6];
        char *parte;
        int quantidadeCampos = 0;
        char *quebra;

        quebra = strpbrk(linha, "\r\n");
        if (quebra != NULL) *quebra = '\0';
        if (linha[0] == '\0') continue;

        parte = strtok(linha, "|");
        while (parte != NULL && quantidadeCampos < 6) {
            campos[quantidadeCampos++] = parte;
            parte = strtok(NULL, "|");
        }

        if (quantidadeCampos != 6) continue;

        fprintf(destino, "%s,", campos[0]);
        escreverCampoCSV(destino, campos[1]); fprintf(destino, ",");
        escreverCampoCSV(destino, campos[2]); fprintf(destino, ",%s,%s,", campos[3], campos[4]);
        escreverCampoCSV(destino, campos[5]);
        fputc('\n', destino);
        total++;
    }

    fclose(origem);
    fclose(destino);
    printf("Exportacao concluida! %d produto(s) exportado(s) para produtos.csv.\n", total);
    return 1;
}

int main(void) {
    int opcao;

    /* Garante que o arquivo inicial exista antes de mostrar o menu. */
    if (!criarArquivoInicial()) {
        printf("Nao foi possivel preparar o arquivo inicial.\n");
        return 1;
    }

    do {
        printf("\n========== SISTEMA DE LOJA ==========\n");
        printf("1 - Cadastrar produto\n");
        printf("2 - Listar produtos\n");
        printf("3 - Alterar produto\n");
        printf("4 - Excluir produto\n");
        printf("5 - Exportar produtos para CSV\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");

        opcao = lerOpcao();

        switch (opcao) {
            case 1:
                cadastrarProduto();
                break;
            case 2:
                listarProdutos();
                break;
            case 3:
                alterarProduto();
                break;
            case 4:
                excluirProduto();
                break;
            case 5:
                exportarCSV();
                break;
            case 0:
                printf("Encerrando o sistema. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida. Digite um numero de 0 a 5.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
