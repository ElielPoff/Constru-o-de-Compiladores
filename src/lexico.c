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

#define MAX_TOKENS 1000

Token tokens[MAX_TOKENS];  // Armazena os tokens extraídos pelo analisador léxico
int currentToken = 0;  // Índice do token atual

// Função para pegar o próximo token da fila
Token obterToken() {
    return tokens[currentToken++];
}

// Função para verificar se há mais tokens
int haMaisTokens() {
    return currentToken < MAX_TOKENS && tokens[currentToken].tipo != DESCONHECIDO;
}

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
            printf("NUM_INT: %s\n", token.valor);
            break;
        case NUM_DEC:
            printf("NUM_DEC: %s\n", token.valor);
            break;
        case IDENTIFICADOR:
            printf("Identificador: ID %d\n", indiceTabelaSimbolos);
            break;
        case TEXTO:
            printf("TEXTO: %s\n", token.valor);
            break;
        case PALAVRA_RESERVADA:
            printf("PALAVRA_RESERVADA: %s\n", token.valor);
            break;
        case COMENTARIO:
            printf("COMENTARIO: %s\n", token.valor);
            break;
        case OPERADOR:
            printf("OPERADOR: %s\n", token.valor);
            break;
        case SIMBOLO_ESPECIAL:
            printf("SIMBOLO_ESPECIAL: %s\n", token.valor);
            break;
        default:
            printf("DESCONHECIDO: %s\n", token.valor);
            break;
    }
}

// Verifica se o token é uma palavra reservada
int ehPalavraReservada(const char *str) {
    for (size_t i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]); i++) {
        if (strcmp(str, palavrasReservadas[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Verifica se o token é um número inteiro
int ehNumeroInteiro(const char *str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

// Verifica se o token é um número decimal
int ehNumeroDecimal(const char *str) {
    int ponto = 0;
    while (*str) {
        if (*str == '.') {
            if (ponto) return 0; // Se já houver um ponto, não é um número decimal válido
            ponto = 1;
        } else if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return ponto; // Deve conter pelo menos um ponto para ser um número decimal
}

// Verifica se o token é um identificador válido
int ehIdentificador(const char *str) {
    if (!isalpha(str[0]) && str[0] != '_') return 0;
    for (int i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i]) && str[i] != '_') return 0;
    }
    return 1;
}

// Verifica se o token é um comentário
int ehComentario(const char *str) {
    return str[0] == '/' && str[1] == '/';
}

// Verifica se o token é um operador
int ehOperador(const char *str) {
    return (strcmp(str, "=") == 0 || strcmp(str, "+") == 0 || strcmp(str, "-") == 0 || strcmp(str, "*") == 0 ||
            strcmp(str, "/") == 0 || strcmp(str, "%") == 0 || strcmp(str, "&&") == 0 || strcmp(str, "||") == 0 ||
            strcmp(str, "!") == 0 || strcmp(str, ">") == 0 || strcmp(str, "<") == 0 || strcmp(str, ">=") == 0 ||
            strcmp(str, "<=") == 0 || strcmp(str, "!=") == 0 || strcmp(str, "==") == 0);
}

// Verifica se o token é um símbolo especial
int ehSimboloEspecial(char c) {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == ',' || c == ';';
}

// Função para analisar o código-fonte
void analisarLexico(const char *entrada) {
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
            if (ehPalavraReservada(token.valor)) {
                token.tipo = PALAVRA_RESERVADA;
            } else {
                token.tipo = IDENTIFICADOR;
            }
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
        else if (ehSimboloEspecial(entrada[i])) {
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

        // Armazena o token na fila
        tokens[currentToken++] = token;
    }
}

// Função para imprimir a tabela de símbolos
void imprimirTabelaSimbolos() {
    printf("\nTabela de Símbolos:\n");
    for (int i = 0; i < indiceTabelaSimbolos; i++) {
        printf("Identificador (ID %d) : %s\n", tabelaSimbolos[i].id, tabelaSimbolos[i].nome);
    }
}
