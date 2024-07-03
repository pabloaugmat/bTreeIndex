#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Btree.h"

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

node *criaNo(Btree *btree)
{
    node *novoNo = (node *)malloc(sizeof(node));
    if (novoNo == NULL)
    {
        return NULL;
    }
    novoNo->indices = (idx **)malloc((btree->ordem - 1) * sizeof(idx *));
    novoNo->filhos = (node **)malloc(btree->ordem * sizeof(node *));
    novoNo->qnt = 0;
    novoNo->folha = true;
    novoNo->pai = NULL;
    return novoNo;
}

void ordenaIndices(node *no)
{
    idx *IndiceAux;
    bool troca;
    for (int i = 0; i < no->qnt - 1; i++)
    {
        troca = false;
        for (int j = 0; j < no->qnt - i - 1; j++)
        {
            if (no->indices[j]->chave > no->indices[j + 1]->chave)
            {
                IndiceAux = no->indices[j];
                no->indices[j] = no->indices[j + 1];
                no->indices[j + 1] = IndiceAux;
                troca = true;
            }
        }
        if (troca == false)
        {
            break;
        }
    }
}

void split(Btree *btree, node *no, idx *indice)
{
    node *novoNo = criaNo(btree);
    int meio = (btree->ordem - 1) / 2;

    // Se o novo índice deve ficar na segunda metade
    if (indice->chave > no->indices[meio]->chave)
    {
        insereNaPagina(btree, novoNo, indice);
        for (int i = 0; i < meio; i++)
        {
            insereNaPagina(btree, novoNo, no->indices[meio + i + 1]);
            no->indices[meio + i + 1] = NULL;
            no->qnt--;
            novoNo->filhos[i] = no->filhos[meio + i + 1];
            no->filhos[meio + i + 1] = NULL;
        }
        if (no->pai == NULL)
        {
            node *novoPai = criaNo(btree);
            novoPai->folha = false;
            novoPai->filhos[0] = no;
            novoPai->filhos[1] = novoNo;
            insereNaPagina(btree, novoPai, no->indices[meio]);
            no->indices[meio] = NULL;
            no->qnt--;
            btree->raiz = novoPai; // Atualiza a raiz da árvore
        }
        else
        {
            insereNaPagina(btree, no->pai, no->indices[meio]);
            no->qnt--;
        }
    }
    // Se o novo índice deve ficar na primeira metade
    else if (indice->chave < no->indices[meio]->chave)
    {
        insereNaPagina(btree, novoNo, no->indices[meio]);
        no->indices[meio] = NULL;
        no->qnt--;
        for (int i = 0; i < meio; i++)
        {
            insereNaPagina(btree, novoNo, no->indices[meio + i + 1]);
            no->indices[meio + i + 1] = NULL;
            no->qnt--;
            novoNo->filhos[i] = no->filhos[meio + i + 1];
            no->filhos[meio + i + 1] = NULL;
        }
        insereNaPagina(btree, no, indice);
        if (no->pai == NULL)
        {
            node *novoPai = criaNo(btree);
            novoPai->folha = false;
            novoPai->filhos[0] = no;
            novoPai->filhos[1] = novoNo;
            insereNaPagina(btree, novoPai, no->indices[meio]);
            no->indices[meio] = NULL;
            no->qnt--;
            btree->raiz = novoPai; // Atualiza a raiz da árvore
        }
        else
        {
            insereNaPagina(btree, no->pai, no->indices[meio]);
            no->qnt--;
        }
    }
}

void insereNaPagina(Btree *btree, node *no, idx *indice)
{
    if (no->qnt < btree->ordem - 1)
    {
        no->indices[no->qnt] = indice;
        no->qnt++;
        ordenaIndices(no);
    }
    else
    {
        split(btree, no, indice);
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
    return noAtual; // Retorna NULL se não encontrar a folha
}

void insere(Btree *btree, int chave, int posicao)
{
    // cria par de indice
    idx *novoIndice = (idx *)malloc(sizeof(idx));
    novoIndice->chave = chave;
    novoIndice->posicao = posicao;

    // logica caso a arvore esteja vazia
    if (btree->raiz == NULL)
    {
        node *novoNo = criaNo(btree);
        insereNaPagina(btree, novoNo, novoIndice);
        btree->raiz = novoNo; // Define o novo nó como raiz
    }
    // caso não esteja vazia
    else
    {
        node *folha = buscaFolha(btree, btree->raiz, chave);
        insereNaPagina(btree, folha, novoIndice);
    }
}

// se retornar -1 a chave não esta na arvore
int buscaPosicao(Btree *btree, node *noAtual, int chave)
{
    // Procura apenas no nó atual se for uma folha
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
        // Busca nos filhos com base nos índices do nó atual
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

        // Continua a busca no filho apropriado
        return buscaPosicao(btree, noAtual->filhos[i], chave);
    }

    // Se a chave não foi encontrada em nenhum lugar
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

                    // Move os filhos, se existirem
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

