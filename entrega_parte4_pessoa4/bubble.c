#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loja.h"

// Função auxiliar para comparar letras sem diferenciar maiúsculas/minúsculas
static int compararAlfabetica(const char *a, const char *b) {
    while (*a && *b) {
        unsigned char c1 = (unsigned char)tolower((unsigned char)*a);
        unsigned char c2 = (unsigned char)tolower((unsigned char)*b);
        if (c1 != c2) return (int)c1 - (int)c2;
        a++;
        b++;
    }
    return (int)(unsigned char)tolower((unsigned char)*a) -
           (int)(unsigned char)tolower((unsigned char)*b);
}

// ---------------------------------------------------------
// ORDENAÇÃO POR NOME (A-Z) USANDO BUBBLE SORT
// ---------------------------------------------------------
void ordenarProdutosPorNomeAZ(void) {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "r");
    Produto *lista = NULL;
    Produto temp;
    int total = 0, capacidade = 0;

    if (!arquivo) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    // Carrega do arquivo para a memória dinamicamente
    while (fscanf(arquivo, "%d|%49[^|]|%49[^|]|%f|%d|%49[^\n]", 
                  &temp.id, temp.nome, temp.categoria, 
                  &temp.preco, &temp.quantidade, temp.fornecedor) == 6) {
        if (total >= capacidade) {
            capacidade = (capacidade == 0) ? 10 : capacidade * 2;
            Produto *aux = realloc(lista, (size_t)capacidade * sizeof(Produto));
            if (!aux) {
                free(lista);
                fclose(arquivo);
                return;
            }
            lista = aux;
        }
        lista[total++] = temp;
    }
    fclose(arquivo);

    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        free(lista);
        return;
    }

    // --- ALGORITMO BUBBLE SORT (NOME A-Z) ---
    for (int i = 0; i < total - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < total - i - 1; j++) {
            if (compararAlfabetica(lista[j].nome, lista[j + 1].nome) > 0) {
                Produto aux = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = aux;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }

    // Exibe os dados ordenados
    printf("\n=== PRODUTOS ORDENADOS POR NOME (A - Z) ===\n");
    for (int i = 0; i < total; i++) {
        printf("ID: %d | Nome: %s | Preco: R$ %.2f | Qtd: %d\n", 
               lista[i].id, lista[i].nome, lista[i].preco, lista[i].quantidade);
    }
    free(lista);
}

// ---------------------------------------------------------
// ORDENAÇÃO POR PREÇO (CRESCENTE) USANDO BUBBLE SORT
// ---------------------------------------------------------
void ordenarProdutosPorPrecoCrescente(void) {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "r");
    Produto *lista = NULL;
    Produto temp;
    int total = 0, capacidade = 0;

    if (!arquivo) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    // Carrega do arquivo para a memória dinamicamente
    while (fscanf(arquivo, "%d|%49[^|]|%49[^|]|%f|%d|%49[^\n]", 
                  &temp.id, temp.nome, temp.categoria, 
                  &temp.preco, &temp.quantidade, temp.fornecedor) == 6) {
        if (total >= capacidade) {
            capacidade = (capacidade == 0) ? 10 : capacidade * 2;
            Produto *aux = realloc(lista, (size_t)capacidade * sizeof(Produto));
            if (!aux) {
                free(lista);
                fclose(arquivo);
                return;
            }
            lista = aux;
        }
        lista[total++] = temp;
    }
    fclose(arquivo);

    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        free(lista);
        return;
    }

    // --- ALGORITMO BUBBLE SORT (PREÇO CRESCENTE) ---
    for (int i = 0; i < total - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < total - i - 1; j++) {
            if (lista[j].preco > lista[j + 1].preco) {
                Produto aux = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = aux;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }

    // Exibe os dados ordenados
    printf("\n=== PRODUTOS ORDENADOS POR PRECO (CRESCENTE) ===\n");
    for (int i = 0; i < total; i++) {
        printf("ID: %d | Nome: %s | Preco: R$ %.2f | Qtd: %d\n", 
               lista[i].id, lista[i].nome, lista[i].preco, lista[i].quantidade);
    }
    free(lista);
}
