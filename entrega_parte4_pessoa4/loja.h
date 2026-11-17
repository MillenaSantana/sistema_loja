#ifndef LOJA_H
#define LOJA_H

#define ARQUIVO_PRODUTOS "produtos.txt"
#define ARQUIVO_IDS "ultimo_id.txt"

typedef struct {
    int id;
    char nome[50];
    char categoria[50];
    float preco;
    int quantidade;
    char fornecedor[50];
} Produto;

/* Funcoes da Pessoa 1 */
int criarArquivoInicial(void);
int cadastrarProduto(void);

/* Funcoes da Pessoa 2 */
void listarProdutos(void);
int alterarProduto(void);
int excluirProduto(void);

#endif
