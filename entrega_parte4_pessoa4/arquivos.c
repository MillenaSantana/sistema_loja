#include <stdio.h>   // printf, scanf, getchar, FILE, fopen, fgets, fprintf, fclose, remove, rename
#include "loja.h"    // Produto, ARQUIVO_PRODUTOS e as declarações das funções

#define ARQUIVO_TEMPORARIO "produtos.tmp" // nome do arquivo temporário

// Lê um texto com espaços (fgets) e tira o \n do final, usando ponteiro.
static void lerTexto(char *destino, int tamanho) {
    char *p = destino;                          // ponteiro que vai percorrer o texto
    fgets(destino, tamanho, stdin);             // lê a linha inteira, com espaços
    while (*p != '\0' && *p != '\n') p++;       // anda até achar o fim do texto ou o \n
    *p = '\0';                                  // troca o \n por fim de string
}

// Compara dois textos ignorando maiúsculas e minúsculas. Retorna 1 se forem iguais.
static int iguais(char *a, char *b) {
    while (*a != '\0' && *b != '\0') {          // percorre os dois textos juntos
        char x = *a;                            // letra do primeiro texto
        char y = *b;                            // letra do segundo texto
        if (x >= 'A' && x <= 'Z') x = x + 32;   // converte para minúscula
        if (y >= 'A' && y <= 'Z') y = y + 32;   // converte para minúscula
        if (x != y) return 0;                   // letras diferentes: textos diferentes
        a++;                                    // avança no primeiro texto
        b++;                                    // avança no segundo texto
    }
    return *a == *b;                            // iguais só se os dois terminaram juntos
}

// Copia para "destino" o campo número "numero" de uma linha (0 = ID, 1 = nome, 2 = categoria,
// 3 = preço, 4 = quantidade, 5 = fornecedor). Os campos são separados por |.
static void pegarCampo(char *linha, int numero, char *destino, int tamanho) {
    char *p = linha;                            // ponteiro que percorre a linha
    int campo = 0;                              // número do campo em que o ponteiro está
    int i = 0;                                  // posição de escrita no destino
    while (campo < numero && *p != '\0') {      // pula os campos que vêm antes
        if (*p == '|') campo++;                 // cada | começa um novo campo
        p++;                                    // avança um caractere
    }
    while (*p != '|' && *p != '\n' && *p != '\r' && *p != '\0' && i < tamanho - 1) {
        destino[i] = *p;                        // copia o caractere para o destino
        i++;                                    // avança no destino
        p++;                                    // avança na linha
    }
    destino[i] = '\0';                          // termina o texto copiado
}

// Mostra todos os produtos em formato de tabela, sem alterar o arquivo.
void listarProdutos(void) {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "r"); // abre só para leitura
    char linha[300];                            // guarda uma linha do arquivo
    char id[50], nome[50], categoria[50];       // campos de texto da linha
    char preco[50], quantidade[50], fornecedor[50];
    int total = 0;                              // conta os produtos listados

    if (arquivo == NULL) {                      // testa se o arquivo abriu
        printf("Erro ao abrir o arquivo de produtos.\n");
        return;
    }
    printf("\n%-5s | %-25s | %-18s | %10s | %5s | %-20s\n",  // cabeçalho da tabela
           "ID", "Nome", "Categoria", "Preço", "Qtd", "Fornecedor");
    printf("------+---------------------------+--------------------+------------+-------+---------------------\n");
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {  // lê linha por linha
        if (linha[0] == '\n' || linha[0] == '\r') continue; // ignora linha em branco
        pegarCampo(linha, 0, id, sizeof(id));               // separa cada campo da linha
        pegarCampo(linha, 1, nome, sizeof(nome));
        pegarCampo(linha, 2, categoria, sizeof(categoria));
        pegarCampo(linha, 3, preco, sizeof(preco));
        pegarCampo(linha, 4, quantidade, sizeof(quantidade));
        pegarCampo(linha, 5, fornecedor, sizeof(fornecedor));
        printf("%-5s | %-25.25s | %-18.18s | %10s | %5s | %-20.20s\n",  // uma linha da tabela
               id, nome, categoria, preco, quantidade, fornecedor);
        total++;                                            // conta o produto
    }
    if (total == 0) printf("Nenhum produto cadastrado.\n"); // arquivo sem produtos
    fclose(arquivo);                                        // fecha o arquivo
}

// Altera um produto pelo ID. Copia tudo para um temporário e depois substitui o original.
int alterarProduto(void) {
    char idDigitado[50];                        // ID digitado pelo usuário (como texto)
    char linha[300];                            // linha lida do arquivo
    char idLinha[50];                           // ID da linha lida
    char nomeLinha[50];                         // nome da linha lida
    int achou = 0;                              // 1 se o ID foi encontrado
    int duplicado = 0;                          // 1 se o novo nome já existe em outro produto
    Produto novo;                               // novos dados digitados
    FILE *origem;                               // arquivo original
    FILE *temp;                                 // arquivo temporário

    printf("ID do produto que deseja alterar: ");
    lerTexto(idDigitado, sizeof(idDigitado));   // lê o ID
    printf("Novo nome: ");
    lerTexto(novo.nome, sizeof(novo.nome));     // lê o nome (com espaços)
    printf("Nova categoria: ");
    lerTexto(novo.categoria, sizeof(novo.categoria)); // lê a categoria
    printf("Novo preço (use ponto, ex.: 49.90): ");
    scanf("%f", &novo.preco);                   // lê o preço
    printf("Nova quantidade: ");
    scanf("%d", &novo.quantidade);              // lê a quantidade
    getchar();                                  // descarta o \n que sobrou do scanf
    printf("Novo fornecedor: ");
    lerTexto(novo.fornecedor, sizeof(novo.fornecedor)); // lê o fornecedor

    origem = fopen(ARQUIVO_PRODUTOS, "r");      // abre o original para leitura
    if (origem == NULL) {                       // testa se abriu
        printf("Erro ao abrir o arquivo de produtos.\n");
        return 0;
    }
    temp = fopen(ARQUIVO_TEMPORARIO, "w");      // cria o temporário para escrita
    if (temp == NULL) {                         // testa se criou
        printf("Erro ao criar o arquivo temporário.\n");
        fclose(origem);                         // fecha o original
        return 0;
    }

    while (fgets(linha, sizeof(linha), origem) != NULL) {   // lê linha por linha
        pegarCampo(linha, 0, idLinha, sizeof(idLinha));     // pega o ID da linha
        if (iguais(idLinha, idDigitado)) {                  // achou o produto a alterar
            fprintf(temp, "%s|%s|%s|%.2f|%d|%s\n",          // grava a linha com os novos dados
                    idLinha, novo.nome, novo.categoria, novo.preco, novo.quantidade, novo.fornecedor);
            achou = 1;                                      // marca que encontrou
        } else {
            pegarCampo(linha, 1, nomeLinha, sizeof(nomeLinha)); // pega o nome da linha
            if (iguais(nomeLinha, novo.nome)) {             // outro produto já tem esse nome
                printf("Erro: já existe outro produto com esse nome.\n");
                duplicado = 1;                              // marca o erro
                break;                                      // para de copiar
            }
            fprintf(temp, "%s", linha);                     // copia a linha sem mudar
        }
    }
    fclose(origem);                             // fecha o original
    fclose(temp);                               // fecha o temporário (salva os dados)

    if (duplicado || !achou) {                  // se deu erro ou não achou o ID
        remove(ARQUIVO_TEMPORARIO);             // apaga o temporário
        if (!duplicado) printf("Produto com ID %s não encontrado.\n", idDigitado);
        return 0;                               // o original continua intacto
    }
    remove(ARQUIVO_PRODUTOS);                   // apaga o original
    rename(ARQUIVO_TEMPORARIO, ARQUIVO_PRODUTOS); // o temporário vira o original
    printf("Produto alterado com sucesso!\n");
    return 1;
}

// Exclui um produto pelo ID. Copia os outros para um temporário e depois substitui o original.
int excluirProduto(void) {
    char idDigitado[50];                        // ID digitado pelo usuário (como texto)
    char linha[300];                            // linha lida do arquivo
    char idLinha[50];                           // ID da linha lida
    int achou = 0;                              // 1 se o ID foi encontrado
    FILE *origem;                               // arquivo original
    FILE *temp;                                 // arquivo temporário

    printf("ID do produto que deseja excluir: ");
    lerTexto(idDigitado, sizeof(idDigitado));   // lê o ID

    origem = fopen(ARQUIVO_PRODUTOS, "r");      // abre o original para leitura
    if (origem == NULL) {                       // testa se abriu
        printf("Erro ao abrir o arquivo de produtos.\n");
        return 0;
    }
    temp = fopen(ARQUIVO_TEMPORARIO, "w");      // cria o temporário para escrita
    if (temp == NULL) {                         // testa se criou
        printf("Erro ao criar o arquivo temporário.\n");
        fclose(origem);                         // fecha o original
        return 0;
    }

    while (fgets(linha, sizeof(linha), origem) != NULL) {   // lê linha por linha
        pegarCampo(linha, 0, idLinha, sizeof(idLinha));     // pega o ID da linha
        if (iguais(idLinha, idDigitado)) {                  // achou o produto a excluir
            achou = 1;                                      // marca que encontrou (não copia)
        } else {
            fprintf(temp, "%s", linha);                     // copia os outros para o temporário
        }
    }
    fclose(origem);                             // fecha o original
    fclose(temp);                               // fecha o temporário (salva os dados)

    if (!achou) {                               // se o ID não existe
        remove(ARQUIVO_TEMPORARIO);             // apaga o temporário
        printf("Produto com ID %s não encontrado.\n", idDigitado);
        return 0;                               // o original continua intacto
    }
    remove(ARQUIVO_PRODUTOS);                   // apaga o original
    rename(ARQUIVO_TEMPORARIO, ARQUIVO_PRODUTOS); // o temporário vira o original
    printf("Produto excluído com sucesso!\n");
    return 1;
}
