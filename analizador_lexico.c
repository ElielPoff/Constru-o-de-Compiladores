#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    NUM_INT,
    NUM_DEC,
    IDENTIFICADOR,
    TEXTO,
    PALAVRA_RESERVADA,
    COMENTARIO,
    OPERADOR,
    SIMBOLO_ESPECIAL,
    DESCONHECIDO
} TipoToken;

typedef struct {
    TipoToken tipo;
    char valor[100];
} Token;

// Palavras reservadas
const char *palavrasReservadas[] = {
    "int", "float", "char", "boolean", "void", "if", "else", "for", "while", "scanf", "println", "main", "return"
};

// Tabela de símbolos
typedef struct {
    char nome[100];
    int id;
} TabelaSimbolos;

TabelaSimbolos tabelaSimbolos[100];
int indiceTabelaSimbolos = 0;

// Função para imprimir o token
void imprimirToken(Token token) {
    switch (token.tipo) {
        case NUM_INT:
            printf("NUM_INT:%s\n", token.valor);
            break;
        case NUM_DEC:
            printf("NUM_DEC:%s\n", token.valor);
            break;
        case IDENTIFICADOR:
            printf("IDENTIFICADOR:%s\n", token.valor);
            break;
        case TEXTO:
            printf("TEXTO:%s\n", token.valor);
            break;
        case PALAVRA_RESERVADA:
            printf("PALAVRA_RESERVADA:%s\n", token.valor);
            break;
        case COMENTARIO:
            printf("COMENTARIO:%s\n", token.valor);
            break;
        case OPERADOR:
            printf("OPERADOR:%s\n", token.valor);
            break;
        case SIMBOLO_ESPECIAL:
            printf("SIMBOLO_ESPECIAL:%s\n", token.valor);
            break;
        default:
            printf("DESCONHECIDO:%s\n", token.valor);
            break;
    }
}

// Função para analisar o código-fonte
void analisarLexico(const char *entrada, Token tokens[], int *tokenCount) {
    size_t i = 0;
    size_t len = strlen(entrada);

    while (i < len) {
        if (isspace(entrada[i])) {
            i++;
            continue;
        }

        Token token;
        int j = 0;

        // Verifica comentário
        if (entrada[i] == '/' && entrada[i + 1] == '/') {
            token.tipo = COMENTARIO;
            while (entrada[i] != '\n' && i < len) {
                token.valor[j++] = entrada[i++];
            }
            token.valor[j] = '\0';
        }
        // Verifica números inteiros
        else if (isdigit(entrada[i])) {
            token.tipo = NUM_INT;
            while (isdigit(entrada[i])) {
                token.valor[j++] = entrada[i++];
            }
            token.valor[j] = '\0';
            if (entrada[i] == '.') { // Verifica se é um número decimal
                token.tipo = NUM_DEC;
                token.valor[j++] = entrada[i++];
                while (isdigit(entrada[i])) {
                    token.valor[j++] = entrada[i++];
                }
                token.valor[j] = '\0';
            }
        }
        // Verifica identificadores
        else if (isalpha(entrada[i]) || entrada[i] == '_') {
            while (isalpha(entrada[i]) || isdigit(entrada[i]) || entrada[i] == '_') {
                token.valor[j++] = entrada[i++];
            }
            token.valor[j] = '\0';
            token.tipo = IDENTIFICADOR;
        }
        // Verifica texto (strings)
        else if (entrada[i] == '"') {
            token.tipo = TEXTO;
            i++;
            while (entrada[i] != '"' && i < len) {
                token.valor[j++] = entrada[i++];
            }
            token.valor[j] = '\0';
            i++; // Avança para o caractere após o "
        }
        // Verifica operadores
        else if (entrada[i] == '=' || entrada[i] == '+' || entrada[i] == '-' || entrada[i] == '*' ||
                 entrada[i] == '/' || entrada[i] == '%' || entrada[i] == '!' || entrada[i] == '>' || entrada[i] == '<') {
            token.tipo = OPERADOR;
            token.valor[j++] = entrada[i++];
            if (entrada[i] == '=' || entrada[i] == '>' || entrada[i] == '<') {
                token.valor[j++] = entrada[i++];
            }
            token.valor[j] = '\0';
        }
        // Verifica símbolos especiais
        else if (entrada[i] == '(' || entrada[i] == ')' || entrada[i] == '[' || entrada[i] == ']' ||
                 entrada[i] == '{' || entrada[i] == '}' || entrada[i] == ',' || entrada[i] == ';') {
            token.tipo = SIMBOLO_ESPECIAL;
            token.valor[j++] = entrada[i++];
            token.valor[j] = '\0';
        }
        // Caso desconhecido
        else {
            token.tipo = DESCONHECIDO;
            token.valor[j++] = entrada[i++];
            token.valor[j] = '\0';
        }

        tokens[*tokenCount] = token;  // Armazenando o token na lista
        (*tokenCount)++;
    }
}

// Função para imprimir a tabela de símbolos
void imprimirTabelaSimbolos() {
    printf("\nTabela de Símbolos:\n");
    for (int i = 0; i < indiceTabelaSimbolos; i++) {
        printf("Identificador (ID %d) : %s\n", tabelaSimbolos[i].id, tabelaSimbolos[i].nome);
    }
}

// Função que processa o arquivo
void processarArquivo(const char *caminhoArquivo) {
    FILE *arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char linha[256];
    Token tokens[1000];
    int tokenCount = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        analisarLexico(linha, tokens, &tokenCount);
    }

    fclose(arquivo);
    imprimirTabelaSimbolos(); // Imprime a tabela de símbolos ao final
}
