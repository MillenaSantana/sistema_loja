
#include <stdio.h>
#include "loja.h"

#define LIMITE_ID 1000000

// Compara nomes sem diferenciar letras maiusculas e minusculas.
int compararNomeCadastro(char a[], char b[]) {
    int i = 0;
    char x, y;

    while (a[i] != '\0' && b[i] != '\0') {
        x = a[i];
        y = b[i];

        if (x >= 'A' && x <= 'Z') {
            x = x + 32;
        }

        if (y >= 'A' && y <= 'Z') {
            y = y + 32;
        }

        if (x != y) {
            return 0;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

// Le textos com espacos e verifica se a entrada e valida.
int lerTextoCadastro(const char mensagem[], char texto[], int tamanho) {
    int i = 0;
    int c;

    printf("%s", mensagem);

    if (fgets(texto, tamanho, stdin) == NULL) {
        return 0;
    }

    while (texto[i] != '\0' &&
           texto[i] != '\n' &&
           texto[i] != '\r') {
        i++;
    }

    if (texto[i] == '\0' && i == tamanho - 1) {
        while ((c = getchar()) != '\n' && c != EOF) {
        }

        printf("Texto muito longo.\n");
        return 0;
    }

    texto[i] = '\0';

    if (i == 0) {
        printf("Campo vazio.\n");
        return 0;
    }

    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == '|') {
            printf("Nao use o caractere |.\n");
            return 0;
        }
    }

    return 1;
}

// Le e valida a quantidade do produto.
int lerQuantidadeCadastro(int *quantidade) {
    char linha[100];
    char extra;

    printf("Quantidade: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        return 0;
    }

    if (sscanf(linha, " %d %c", quantidade, &extra) != 1 ||
        *quantidade < 0) {
        printf("Quantidade invalida.\n");
        return 0;
    }

    return 1;
}

// Le e valida o preco do produto.
int lerPrecoCadastro(float *preco) {
    char linha[100];
    char extra;
    int i;

    printf("Preco: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        return 0;
    }

    // Permite digitar preco com virgula ou ponto.
    for (i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == ',') {
            linha[i] = '.';
        }
    }

    if (sscanf(linha, " %f %c", preco, &extra) != 1 ||
        !(*preco > 0) ||
        *preco > 100000000.0f) {
        printf("Preco invalido.\n");
        return 0;
    }

    return 1;
}

// Grava os dados de um produto no arquivo.
int gravarProdutoCadastro(FILE *arquivo, Produto *produto) {
    return fprintf(
        arquivo,
        "%d|%s|%s|%.2f|%d|%s\n",
        produto->id,
        produto->nome,
        produto->categoria,
        produto->preco,
        produto->quantidade,
        produto->fornecedor
    ) > 0;
}

// Le um produto salvo no arquivo.
// Retorna 1 para produto valido, 0 para fim e -1 para erro.
int lerProdutoCadastro(FILE *arquivo, Produto *produto) {
    char linha[512];
    int campos;

    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        if (feof(arquivo)) {
            return 0;
        }

        return -1;
    }

    campos = sscanf(
        linha,
        "%d|%49[^|]|%49[^|]|%f|%d|%49[^\r\n]",
        &produto->id,
        produto->nome,
        produto->categoria,
        &produto->preco,
        &produto->quantidade,
        produto->fornecedor
    );

    if (campos != 6 ||
        produto->id <= 0 ||
        !(produto->preco > 0) ||
        produto->quantidade < 0) {
        return -1;
    }

    return 1;
}

// Recupera o ultimo ID utilizado.
int ultimoIdCadastro(void) {
    FILE *arquivo = fopen(ARQUIVO_IDS, "r");
    int id = 0;

    if (arquivo == NULL) {
        return 0;
    }

    if (fscanf(arquivo, "%d", &id) != 1 ||
        id < 0 ||
        id > LIMITE_ID) {
        id = 0;
    }

    fclose(arquivo);
    return id;
}

// Salva o ultimo ID em um arquivo auxiliar.
int salvarIdCadastro(int id) {
    FILE *arquivo = fopen(ARQUIVO_IDS, "w");
    int ok;

    if (arquivo == NULL) {
        return 0;
    }

    ok = fprintf(arquivo, "%d\n", id) > 0;

    if (fclose(arquivo) != 0) {
        ok = 0;
    }

    return ok;
}

// Cria o arquivo inicial com quatro produtos.
int criarArquivoInicial(void) {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "r");

    Produto iniciais[4] = {
        {1, "Camiseta Básica", "Vestuário", 49.90f, 30, "Moda Brasil"},
        {2, "Mouse Óptico USB", "Informática", 59.99f, 15, "TecnoShop"},
        {3, "Shampoo 300ml", "Higiene Pessoal", 22.50f, 40, "Beleza Pura"},
        {4, "Café 500g", "Alimentos", 18.00f, 25, "Grãos do Sul"}
    };

    int i;
    int ok = 1;

    // Se o arquivo ja existe, nao apaga os produtos.
    if (arquivo != NULL) {
        fclose(arquivo);
        return 1;
    }

    arquivo = fopen(ARQUIVO_PRODUTOS, "w");

    if (arquivo == NULL) {
        printf("Nao foi possivel criar produtos.txt.\n");
        return 0;
    }

    // Grava os quatro produtos iniciais.
    for (i = 0; i < 4; i++) {
        if (!gravarProdutoCadastro(arquivo, &iniciais[i])) {
            ok = 0;
        }
    }

    if (fclose(arquivo) != 0) {
        ok = 0;
    }

    if (!ok) {
        printf("Erro ao criar arquivo inicial.\n");
        return 0;
    }

    // Registra que os IDs de 1 a 4 ja foram utilizados.
    if (ultimoIdCadastro() < 4) {
        if (!salvarIdCadastro(4)) {
            printf("Aviso: controle de IDs nao salvo.\n");
        }
    }

    printf("Arquivo inicial criado com quatro produtos.\n");

    return 1;
}

// Cadastra um produto com ID automatico.
int cadastrarProduto(void) {
    Produto novo;
    Produto atual;
    FILE *arquivo;

    int maior = 0;
    int ultimo;
    int leitura;
    int ok;

    // Solicita os dados do novo produto.
    if (!lerTextoCadastro("Nome: ", novo.nome,
                          sizeof(novo.nome))) {
        return 0;
    }

    if (!lerTextoCadastro("Categoria: ", novo.categoria,
                          sizeof(novo.categoria))) {
        return 0;
    }

    if (!lerPrecoCadastro(&novo.preco)) {
        return 0;
    }

    if (!lerQuantidadeCadastro(&novo.quantidade)) {
        return 0;
    }

    if (!lerTextoCadastro("Fornecedor: ", novo.fornecedor,
                          sizeof(novo.fornecedor))) {
        return 0;
    }

    // Abre o arquivo para verificar nomes repetidos.
    arquivo = fopen(ARQUIVO_PRODUTOS, "r");

    if (arquivo == NULL) {
        printf("Arquivo nao encontrado.\n");
        return 0;
    }

    while ((leitura = lerProdutoCadastro(arquivo, &atual)) == 1) {

        // Impede cadastrar produtos com nomes repetidos.
        if (compararNomeCadastro(novo.nome, atual.nome)) {
            printf("Produto ja cadastrado!\n");
            fclose(arquivo);
            return 0;
        }

        // Procura o maior ID existente.
        if (atual.id > maior) {
            maior = atual.id;
        }
    }

    fclose(arquivo);

    if (leitura == -1) {
        printf("Registro invalido. Cadastro cancelado.\n");
        return 0;
    }

    // Recupera o ultimo ID, mesmo apos exclusoes.
    ultimo = ultimoIdCadastro();

    if (ultimo > maior) {
        maior = ultimo;
    }

    if (maior >= LIMITE_ID) {
        printf("Limite de IDs atingido.\n");
        return 0;
    }

    // Gera automaticamente o proximo ID.
    novo.id = maior + 1;

    // Abre o arquivo para adicionar o produto.
    arquivo = fopen(ARQUIVO_PRODUTOS, "a");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo.\n");
        return 0;
    }

    ok = gravarProdutoCadastro(arquivo, &novo);

    if (fclose(arquivo) != 0) {
        ok = 0;
    }

    if (!ok) {
        printf("Erro ao salvar produto.\n");
        return 0;
    }

    // Atualiza o controle de IDs.
    if (!salvarIdCadastro(novo.id)) {
        printf("Aviso: controle de IDs nao atualizado.\n");
    }

    printf("Produto cadastrado com sucesso! ID: %d\n", novo.id);

    return 1;
}
