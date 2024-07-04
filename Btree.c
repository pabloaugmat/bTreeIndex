#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Btree.h"

#define MAX_CHAVES 4 

typedef struct idx
{
    int chave;   // chave primaria
    int posicao; // posição no arquivo
} idx;

typedef struct node
{
    idx **indices;        // array dos pares chave-deslocamento
    struct node **filhos; // array com ponteiros para os filhos
    int qnt;              // numero de chaves no nó
    bool folha;
    struct node *pai; // aponta pro pai
} node;

Btree *criaBtree(int ordem)
{
    Btree *novaBtree = (Btree *)malloc(sizeof(Btree));
    if (novaBtree == NULL)
    {
        return NULL;
    }
    novaBtree->ordem = ordem;
    novaBtree->raiz = NULL;
    return novaBtree;
}

node *criaNo() {
    node *novo_no = (node *)malloc(sizeof(node));
    novo_no->indices = (idx **)malloc(MAX_CHAVES * sizeof(idx *));
    novo_no->filhos = (node **)malloc((MAX_CHAVES + 1) * sizeof(node *));
    novo_no->qnt = 0;
    novo_no->folha = true;
    novo_no->pai = NULL;
    return novo_no;
}


void ordenaIndices(node *no) {
    int i, j;
    for (i = 0; i < no->qnt - 1; i++) {
        for (j = 0; j < no->qnt - i - 1; j++) {
            if (no->indices[j]->chave > no->indices[j + 1]->chave) {
                idx *temp = no->indices[j];
                no->indices[j] = no->indices[j + 1];
                no->indices[j + 1] = temp;
            }
        }
    }
}
void split(Btree *btree, node *no, int indice_filho) {
    node *filho = no->filhos[indice_filho];
    node *novo_no = criaNo();
    novo_no->folha = filho->folha;
    novo_no->qnt = MAX_CHAVES / 2;

    for (int i = 0; i < MAX_CHAVES / 2; i++) {
        novo_no->indices[i] = filho->indices[i + MAX_CHAVES / 2];
    }

    if (!filho->folha) {
        for (int i = 0; i <= MAX_CHAVES / 2; i++) {
            novo_no->filhos[i] = filho->filhos[i + MAX_CHAVES / 2];
            novo_no->filhos[i]->pai = novo_no;
        }
    }

    filho->qnt = MAX_CHAVES / 2;
    no->qnt++;
    for (int i = no->qnt - 1; i > indice_filho; i--) {
        no->filhos[i + 1] = no->filhos[i];
    }
    no->filhos[indice_filho + 1] = novo_no;
    novo_no->pai = no;

    for (int i = no->qnt - 1; i > indice_filho; i--) {
        no->indices[i] = no->indices[i - 1];
    }
    no->indices[indice_filho] = filho->indices[MAX_CHAVES / 2 - 1];
}

void insereNaPagina(Btree *btree, node *no, idx *indice) {
    int i = no->qnt - 1;
    if (no->folha) {
        while (i >= 0 && indice->chave < no->indices[i]->chave) {
            no->indices[i + 1] = no->indices[i];
            i--;
        }
        no->indices[i + 1] = indice;
        no->qnt++;
    } else {
        while (i >= 0 && indice->chave < no->indices[i]->chave) {
            i--;
        }
        i++;
        if (no->filhos[i]->qnt == MAX_CHAVES) {
            split(btree, no, i);
            if (indice->chave > no->indices[i]->chave) {
                i++;
            }
        }
        insereNaPagina(btree, no->filhos[i], indice);
    }
}

node *buscaFolha(Btree *btree, node *noAtual, int chave)
{
    while (noAtual != NULL && noAtual->folha == false)
    {
        int i = 0;
        while (i < noAtual->qnt && chave > noAtual->indices[i]->chave)
        {
            i++;
        }

        if (i < noAtual->qnt && chave == noAtual->indices[i]->chave)
        {
            return noAtual;
        }

        if (chave < noAtual->indices[0]->chave)
        {
            noAtual = noAtual->filhos[0];
        }
        else
        {
            for (int j = 0; j < noAtual->qnt + 1; j++)
            {
                if (chave > noAtual->indices[j]->chave)
                {
                    if (j == noAtual->qnt - 1 || chave < noAtual->indices[j + 1]->chave)
                    {
                        noAtual = noAtual->filhos[j + 1];
                        break;
                    }
                }
            }
        }
    }
    return noAtual; 
}

void insere(Btree *btree, int chave, int posicao) {
    node *raiz = btree->raiz;
    idx *indice = (idx *)malloc(sizeof(idx));
    indice->chave = chave;
    indice->posicao = posicao;

    if (raiz == NULL) {
        
        raiz = criaNo();
        raiz->indices[0] = indice;
        raiz->qnt = 1;
        btree->raiz = raiz;
    } else {
        if (raiz->qnt == MAX_CHAVES) {
            
            node *novo_no = criaNo();
            btree->raiz = novo_no;
            novo_no->folha = false;
            novo_no->filhos[0] = raiz;
            split(btree, novo_no, 0);
            insereNaPagina(btree, novo_no, indice);
        } else {
            insereNaPagina(btree, raiz, indice);
        }
    }
}


int buscaPosicao(Btree *btree, node *noAtual, int chave)
{
    
    if (noAtual->folha == true)
    {
        for (int i = 0; i < noAtual->qnt; i++)
        {
            if (chave == noAtual->indices[i]->chave)
            {
                return noAtual->indices[i]->posicao;
            }
        }
    }
    else
    {
        int i;
        for (i = 0; i < noAtual->qnt; i++)
        {
            if (chave == noAtual->indices[i]->chave)
            {
                return noAtual->indices[i]->posicao;
            }
            if (chave < noAtual->indices[i]->chave)
            {
                break; // Encontrou o local de inserção
            }
        }
        return buscaPosicao(btree, noAtual->filhos[i], chave);
    }

    return -1;
}


void removeIndice(Btree *btree, int chave)
{
    node *folha = buscaFolha(btree, btree->raiz, chave);

    // Verifica se a chave está presente na árvore
    int posicao = buscaPosicao(btree, folha, chave);
    if (posicao == -1)
    {
        printf("Chave %d nao encontrada na arvore.\n", chave);
        return;
    }

    // Remove a chave da folha
    int i = 0;
    while (i < folha->qnt && folha->indices[i]->chave != chave)
    {
        i++;
    }

    // Desloca os índices subsequentes na folha
    for (int j = i; j < folha->qnt - 1; j++)
    {
        folha->indices[j] = folha->indices[j + 1];
    }

    // Atualiza a quantidade de índices na folha
    folha->qnt--;

    // Verifica se a folha está abaixo do limite mínimo de chaves
    if (folha->qnt < (btree->ordem - 1) / 2)
    {
        // Caso a raiz seja a única folha e tenha menos de 2 chaves
        if (btree->raiz == folha && folha->qnt < 1)
        {
            free(folha);
            btree->raiz = NULL;
        }
        else
        {
            // Tentativa de emprestar de um irmão
            bool emprestado = false;
            if (folha->pai != NULL)
            {
                node *pai = folha->pai;
                int indiceFolha = 0;
                while (pai->filhos[indiceFolha] != folha)
                {
                    indiceFolha++;
                }

                // Tentar emprestar do irmão esquerdo
                if (indiceFolha > 0 && pai->filhos[indiceFolha - 1]->qnt > (btree->ordem - 1) / 2)
                {
                    // Transfere o último índice do filho esquerdo para a folha
                    node *esquerdo = pai->filhos[indiceFolha - 1];
                    idx *indiceTransferido = esquerdo->indices[esquerdo->qnt - 1];
                    esquerdo->indices[esquerdo->qnt - 1] = NULL;
                    esquerdo->qnt--;

                    
                    if (!folha->folha)
                    {
                        for (int j = folha->qnt; j > 0; j--)
                        {
                            folha->filhos[j] = folha->filhos[j - 1];
                        }
                        folha->filhos[0] = esquerdo->filhos[esquerdo->qnt];
                        esquerdo->filhos[esquerdo->qnt] = NULL;
                    }

                    // Insere o índice transferido na folha
                    for (int j = folha->qnt; j > 0; j--)
                    {
                        folha->indices[j] = folha->indices[j - 1];
                    }
                    folha->indices[0] = indiceTransferido;
                    folha->qnt++;

                    // Atualiza o índice do pai
                    pai->indices[indiceFolha - 1] = folha->indices[0];

                    emprestado = true;
                }
                // Tentar emprestar do irmão direito
                else if (indiceFolha < pai->qnt && pai->filhos[indiceFolha + 1]->qnt > (btree->ordem - 1) / 2)
                {
                    // Transfere o primeiro índice do filho direito para a folha
                    node *direito = pai->filhos[indiceFolha + 1];
                    idx *indiceTransferido = direito->indices[0];

                    // Move os filhos, se existirem
                    if (!folha->folha)
                    {
                        folha->filhos[folha->qnt + 1] = direito->filhos[0];
                        direito->filhos[0] = NULL;
                    }

                    // Remove o índice do filho direito
                    for (int j = 0; j < direito->qnt - 1; j++)
                    {
                        direito->indices[j] = direito->indices[j + 1];
                    }
                    direito->indices[direito->qnt - 1] = NULL;
                    direito->qnt--;

                    // Insere o índice transferido na folha
                    folha->indices[folha->qnt] = indiceTransferido;
                    folha->qnt++;

                    // Atualiza o índice do pai
                    pai->indices[indiceFolha] = direito->indices[0];

                    emprestado = true;
                }

                // Rebalanceia se nenhum empréstimo foi possível
                if (!emprestado)
                {
                    // Fusão com o irmão esquerdo
                    if (indiceFolha > 0)
                    {
                        node *esquerdo = pai->filhos[indiceFolha - 1];

                        // Transfere todos os índices da folha para o irmão esquerdo
                        for (int j = 0; j < folha->qnt; j++)
                        {
                            esquerdo->indices[esquerdo->qnt] = folha->indices[j];
                            esquerdo->qnt++;
                        }

                        // Transfere os filhos, se existirem
                        if (!folha->folha)
                        {
                            for (int j = 0; j <= folha->qnt; j++)
                            {
                                esquerdo->filhos[esquerdo->qnt] = folha->filhos[j];
                                folha->filhos[j] = NULL;
                                esquerdo->qnt++;
                            }
                        }

                        // Remove a folha do pai
                        free(folha);
                        for (int j = indiceFolha; j < pai->qnt - 1; j++)
                        {
                            pai->indices[j] = pai->indices[j + 1];
                            pai->filhos[j + 1] = pai->filhos[j + 2];
                        }
                        pai->indices[pai->qnt - 1] = NULL;
                        pai->filhos[pai->qnt] = NULL;
                        pai->qnt--;

                        // Rebalanceia o pai se necessário
                        if (pai->qnt < (btree->ordem - 1) / 2 && pai != btree->raiz)
                        {
                            removeIndice(btree, pai->indices[0]->chave);
                        }
                    }
                    // Fusão com o irmão direito
                    else
                    {
                        node *direito = pai->filhos[indiceFolha + 1];

                        // Transfere todos os índices do irmão direito para a folha
                        for (int j = 0; j < direito->qnt; j++)
                        {
                            folha->indices[folha->qnt] = direito->indices[j];
                            folha->qnt++;
                        }

                        // Transfere os filhos, se existirem
                        if (!folha->folha)
                        {
                            for (int j = 0; j <= direito->qnt; j++)
                            {
                                folha->filhos[folha->qnt] = direito->filhos[j];
                                direito->filhos[j] = NULL;
                                folha->qnt++;
                            }
                        }

                        // Remove o irmão direito do pai
                        free(direito);
                        for (int j = indiceFolha; j < pai->qnt - 1; j++)
                        {
                            pai->indices[j] = pai->indices[j + 1];
                            pai->filhos[j + 1] = pai->filhos[j + 2];
                        }
                        pai->indices[pai->qnt - 1] = NULL;
                        pai->filhos[pai->qnt] = NULL;
                        pai->qnt--;

                        // Rebalanceia o pai se necessário
                        if (pai->qnt < (btree->ordem - 1) / 2 && pai != btree->raiz)
                        {
                            removeIndice(btree, pai->indices[0]->chave);
                        }
                    }
                }
            }
        }
    }
}


void imprimeBTree(node *raiz, int nivel)
{
    if (raiz != NULL)
    {
        int i;
        for (i = 0; i < raiz->qnt; i++)
        {
            imprimeBTree(raiz->filhos[i], nivel + 1);
            printf("\n");
            for (int j = 0; j < nivel; j++)
            {
                printf("| ");
            }
            printf("(%d, %d)", raiz->indices[i]->chave, raiz->indices[i]->posicao);
        }
        imprimeBTree(raiz->filhos[i], nivel + 1);
    }
}
