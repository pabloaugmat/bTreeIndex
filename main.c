#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "Btree.h"

#define SEED 2616

void criarArquivoAlunos() {
    const char *nomeArquivo = "alunos.txt";

    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo %s\n", nomeArquivo);
        return;
    }

    int matricula;
    char nome[6] = "aluno";
    int idade = 28;
    char email[16] = "aluno@email.com";

    for (int i = 0; i < 20000; i++) {
        matricula = rand() % 90000 + 10000;
        fprintf(arquivo, "%d %s %d %s\n", matricula, nome, idade, email);
    }

    fclose(arquivo);
    printf("Arquivo %s criado e escrito com sucesso!\n", nomeArquivo);
}

void processarArquivo(Btree *btree) {
    const char *nomeArquivo = "alunos.txt";
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char linha[256];
    int chave;
    long posicao;

    while (fgets(linha, sizeof(linha), arquivo)) {
        posicao = ftell(arquivo) - strlen(linha);

        if (sscanf(linha, "%d", &chave) != 1) {
            printf("Erro ao ler matrícula na linha: %s\n", linha);
            continue;
        }

        insere(btree, chave, posicao);
    }

    fclose(arquivo);
}

void acessarPosicaoArquivo(const char *nomeArquivo, long posicao) {
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    if (fseek(arquivo, posicao, SEEK_SET) != 0) {
        printf("Erro ao mover o ponteiro do arquivo para a posição %ld\n", posicao);
        fclose(arquivo);
        return;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) != NULL) {
        printf("Conteúdo a partir da posição %ld: %s\n", posicao, linha);
    } else {
        printf("Erro ao ler a partir da posição %ld\n", posicao);
    }

    fclose(arquivo);
}

void buscaSequencial(const char *nomeArquivo, int chaveProcurada) {
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char linha[256];
    long posicao;
    int chave;

    while (fgets(linha, sizeof(linha), arquivo)) {
        posicao = ftell(arquivo) - strlen(linha);

        if (sscanf(linha, "%d", &chave) != 1) {
            printf("Erro ao ler matrícula na linha: %s\n", linha);
            continue;
        }

        if (chave == chaveProcurada) {
            printf("Registro encontrado na posição: %ld\n", posicao);
            fclose(arquivo);
            return;
        }
    }

    printf("Registro não encontrado no arquivo\n");
    fclose(arquivo);
}

int main() {
    int ordem = 4;

    Btree *btree = criaBtree(ordem);

    srand(SEED);
    criarArquivoAlunos();

    int opcao;
    long posicao;
    int chave;
    clock_t inicio, fim;
    double tempo_gasto;

    while (true) {
        printf("\nMenu:\n");
        printf("1. Criar índice\n");
        printf("2. Procurar elementos\n");
        printf("3. Remover registro\n");
        printf("4. Imprimir\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                inicio = clock();
                processarArquivo(btree);
                fim = clock();
                tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
                printf("Índice criado em %.6f segundos.\n", tempo_gasto);
                break;

            case 2:
                printf("Digite a chave que deseja procurar: ");
                scanf("%d", &chave);

                // Busca Binária
                inicio = clock();
                posicao = buscaPosicao(btree, btree->raiz, chave);
                fim = clock();
                tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
                printf("Tempo gasto na busca binária: %.6f segundos.\n", tempo_gasto);

                if (posicao != -1) {
                    printf("Registro encontrado na posição: %ld\n", posicao);
                    inicio = clock();
                    acessarPosicaoArquivo("alunos.txt", posicao);
                    fim = clock();
                    tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
                    printf("Tempo gasto na leitura do arquivo: %.6f segundos.\n", tempo_gasto);
                } else {
                    printf("Registro não encontrado na árvore.\n");
                }

                // Busca Sequencial
                inicio = clock();
                buscaSequencial("alunos.txt", chave);
                fim = clock();
                tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
                printf("Tempo gasto na busca sequencial: %.6f segundos.\n", tempo_gasto);
                break;

            case 3:
                printf("Digite a chave do registro que deseja remover: ");
                scanf("%d", &chave);

                posicao = buscaPosicao(btree, btree->raiz, chave);
                if (posicao != -1) {
                    printf("Registro encontrado na posição: %ld\n", posicao);
                    acessarPosicaoArquivo("alunos.txt", posicao);
                    removeIndice(btree, chave);
                    printf("Registro removido da árvore.\n");
                } else {
                    printf("Registro não encontrado na árvore.\n");
                }
                break;

            case 4:
                imprimeBTree(btree->raiz,0);
                break;

            default:
                printf("Opção inválida. Escolha novamente.\n");
                break;
        }
    }

    return 0;
}
