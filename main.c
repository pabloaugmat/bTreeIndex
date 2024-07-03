#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Btree.h"
#include <time.h>

#define SEED 2616

FILE *criarArquivoAlunos()
{

    const char *nomeArquivo = "alunos.txt";

    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL)
    {
        printf("Erro ao criar o arquivo %s\n", nomeArquivo);
        return NULL;
    }

    int matricula;
    char nome[6] = "aluno";
    int idade = 28;
    char email[16] = "aluno@email.com";
    for (int i = 0; i < 11000; i++)
    {
        matricula = rand() % 90000 + 10000;
        fprintf(arquivo, "%d %s %d %s\n", matricula, nome, idade, email);
    }

    fclose(arquivo);

    printf("Arquivo %s criado e escrito com sucesso!\n", nomeArquivo);
    return arquivo;
}

void processarArquivo(Btree *btree)
{
    const char *nomeArquivo = "alunos.txt";
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char linha[256];
    int posicao;
    int chave;

    while (fgets(linha, sizeof(linha), arquivo))
    {
        posicao = ftell(arquivo) - strlen(linha);

        if (sscanf(linha, "%d", &chave) != 1)
        {
            printf("Erro ao ler matrícula na linha: %s\n", linha);
            continue;
        }

        printf("Posição: %d, Matrícula: %d\n", posicao, chave);
        insere(btree, chave, posicao);
    }

    fclose(arquivo);
}

void acessarPosicaoArquivo(const char *nomeArquivo, long posicao)
{
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    if (fseek(arquivo, posicao, SEEK_SET) != 0)
    {
        printf("Erro ao mover o ponteiro do arquivo para a posição %ld\n", posicao);
        fclose(arquivo);
        return;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        printf("Conteúdo a partir da posição %ld: %s\n", posicao, linha);
    }
    else
    {
        printf("Erro ao ler a partir da posição %ld\n", posicao);
    }

    fclose(arquivo);
}

void buscaSequencial(const char *nomeArquivo, int chaveProcurada)
{
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char linha[256];
    int posicao;
    int chave;

    while (fgets(linha, sizeof(linha), arquivo))
    {
        posicao = ftell(arquivo) - strlen(linha);

        if (sscanf(linha, "%d", &chave) != 1)
        {
            printf("Erro ao ler matrícula na linha: %s\n", linha);
            continue;
        }

        if (chave == chaveProcurada)
        {
            printf("Registro encontrado na posição: %d\n", posicao);
            fclose(arquivo);
            return;
        }
    }

    printf("Registro não encontrado no arquivo\n");
    fclose(arquivo);
}

int main()
{
    bool looping = true;
    int opcao, ordem = 3, chave, posicao;
    double time_spent = 0.0;
    clock_t inicio, fim;

    while ((ordem & 1) != 0)
    {
        printf("Digite a ordem (PAR) desejada para a arvore: ");
        scanf("%d", &ordem);
    }

    Btree *btree = criaBtree(ordem);

    srand(SEED);
    criarArquivoAlunos();

    while (looping)
    {
        printf("Menu:\n");
        printf("1. Criar indice\n");
        printf("2. Procurar elementos\n");
        printf("3. Remover registro\n");
        printf("4. Sair\n");
        scanf("%d", &opcao);
        switch (opcao)
        {
        case 1:
            processarArquivo(btree);
            break;
        case 2:
            chave = rand() % 90000 + 10000; // gerando aleatorio pra comparação
            printf("Matricula procurada: %d\n ", chave);
            time_spent = 0.0;
            inicio = clock();
            posicao = buscaPosicao(btree, btree->raiz, chave);
            fim = clock();
            time_spent = (double)(fim - inicio) / CLOCKS_PER_SEC;
            printf("TEMPO GASTO NA BUSCA BINARIA: %f\n", time_spent);
            printf("encontrado na posição: %d\n", posicao);
            time_spent = 0.0;
            inicio = clock();
            buscaSequencial("alunos.txt", posicao);
            fim = clock();
            time_spent = (double)(fim - inicio) / CLOCKS_PER_SEC;
            printf("TEMPO GASTO NA BUSCA SEQUENCIAL: %f\n", time_spent);
            if (posicao > -1)
            {
                acessarPosicaoArquivo("alunos.txt", posicao);
            }
            break;
        case 3:
            printf("digite a chave do indice que deseja remover:");
            scanf("%d", &chave);
            printf("o indice do seguinte registro sera removido:\n");
            posicao = buscaPosicao(btree, btree->raiz, chave);
            printf("encontrado na posição: %d\n", posicao);
            if (posicao > -1)
            {
                acessarPosicaoArquivo("alunos.txt", posicao);
            }
            removeIndice(btree, chave);
            break;
        case 4:
            looping = false;
            break;

        default:
            break;
        }
    }

    free(btree);
    return 0;
}
