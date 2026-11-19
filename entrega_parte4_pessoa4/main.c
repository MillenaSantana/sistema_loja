/* SISTEMA DE CADASTRO DE PRODUTOS - ARQUIVO UNICO PARA ONLINEGDB
   Integracao: Pessoa 1 (cadastro), Pessoa 2 (arquivos),
   Pessoa 3 (Bubble Sort) e Pessoa 4 (menu e CSV).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define ARQUIVO_PRODUTOS "produtos.txt"
#define ARQUIVO_IDS "ultimo_id.txt"
#define ARQUIVO_TEMPORARIO "produtos.tmp"
#define LIMITE_ID 1000000

typedef struct {
    int id;
    char nome[50];
    char categoria[50];
    float preco;
    int quantidade;
    char fornecedor[50];
} Produto;

/* Declaracoes das funcoes usadas entre as partes */
int criarArquivoInicial(void);
int cadastrarProduto(void);
void listarProdutos(void);
int alterarProduto(void);
int excluirProduto(void);
void ordenarProdutosPorNomeAZ(void);
void ordenarProdutosPorPrecoCrescente(void);

/* cadastro(1).c */




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

/* arquivos(2).c */



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

/* bubble.c */

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

/* main(1).c */

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
    if (*fim != '\0' || valor < 0 || valor > 7) return -1;
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
        printf("6 - Ordenar produtos por nome (A-Z)\n");
        printf("7 - Ordenar produtos por preco (crescente)\n");
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
            case 6:
                ordenarProdutosPorNomeAZ();
                break;
            case 7:
                ordenarProdutosPorPrecoCrescente();
                break;
            case 0:
                printf("Encerrando o sistema. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida. Digite um numero de 0 a 7.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
