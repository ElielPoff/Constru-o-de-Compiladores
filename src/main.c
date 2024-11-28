#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexico.h"
#include "sintatico.h"

int main() {
    char caminhoArquivo[100];
    printf("Digite o caminho do arquivo com o código-fonte: ");
    scanf("%s", caminhoArquivo);

    FILE *arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        analisarLexico(linha);  // Realiza a análise léxica linha por linha
    }

    fclose(arquivo);

    // Depois que todos os tokens foram extraídos, chamamos o analisador sintático
    analisarPrograma();  // Análise sintática do código fonte

    return 0;
}
