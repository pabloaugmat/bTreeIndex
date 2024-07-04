/*
    CTCO02 ALGORITMOS E ESTRUTURA DE DADOS 2 - ARVORE B
    PROF. VANESSA SOUZA
    
        ANA CLARA BRAZ - 2022001760
        CAIO TEODORO - 2020004501
        EMAANUEL MARTINS - 2022004073
        PABLO AUGUSTO - 2022015139

 */

typedef struct idx idx;

typedef struct node node;

typedef struct Btree
{
    int ordem;
    node *raiz;
} Btree;

Btree *criaBtree(int ordem);

node *criaNo();

void ordenaIndices(node *no);

void split(Btree *btree, node *no, int indice_filho);

void insereNaPagina(Btree *btree, node *no, idx *indice);

node *buscaFolha(Btree *btree, node *noAtual, int chave);

void insere(Btree *btree, int chave, int posicao);

int buscaPosicao(Btree *btree, node *noAtual, int chave);

void removeIndice(Btree *btree, int chave);

void imprimeBTree(node *raiz, int nivel);

