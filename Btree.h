

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

