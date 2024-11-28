#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definições de Token e Tipos
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

Token tokens[100];        // Armazena os tokens extraídos
int indiceTokens = 0;     // Número total de tokens
int posicaoAtual = 0;     // Posição atual para análise sintática

// Palavras reservadas
const char *palavrasReservadas[] = {
    "int", "float", "char", "boolean", "void", "if", "else", "for", "while",
    "scanf", "println", "main", "return"
};

// Função para verificar se é palavra reservada
int ehPalavraReservada(const char *str) {
    for (size_t i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]); i++) {
        if (strcmp(str, palavrasReservadas[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Função para imprimir o token
void imprimirToken(Token token) {
    switch (token.tipo) {
        case NUM_INT: printf("NUM_INT: %s\n", token.valor); break;
        case NUM_DEC: printf("NUM_DEC: %s\n", token.valor); break;
        case IDENTIFICADOR: printf("IDENTIFICADOR: %s\n", token.valor); break;
        case TEXTO: printf("TEXTO: %s\n", token.valor); break;
        case PALAVRA_RESERVADA: printf("PALAVRA_RESERVADA: %s\n", token.valor); break;
        case COMENTARIO: printf("COMENTARIO: %s\n", token.valor); break;
        case OPERADOR: printf("OPERADOR: %s\n", token.valor); break;
        case SIMBOLO_ESPECIAL: printf("SIMBOLO_ESPECIAL: %s\n", token.valor); break;
        default: printf("DESCONHECIDO: %s\n", token.valor); break;
    }
}

// Função para analisar o código-fonte e gerar tokens
void analisarLexico(const char *entrada) {
    size_t i = 0;
    size_t len = strlen(entrada);

    while (i < len) {
        if (isspace(entrada[i])) {
            i++;
            continue;
        }

        Token token = { .tipo = DESCONHECIDO };
        int j = 0;

        // Verifica números
        if (isdigit(entrada[i])) {
            token.tipo = NUM_INT;
            while (isdigit(entrada[i])) {
                token.valor[j++] = entrada[i++];
            }
            if (entrada[i] == '.') { // Número decimal
                token.tipo = NUM_DEC;
                token.valor[j++] = entrada[i++];
                while (isdigit(entrada[i])) {
                    token.valor[j++] = entrada[i++];
                }
            }
        }
        // Verifica identificadores e palavras reservadas
        else if (isalpha(entrada[i]) || entrada[i] == '_') {
            while (isalnum(entrada[i]) || entrada[i] == '_') {
                token.valor[j++] = entrada[i++];
            }
            token.valor[j] = '\0';
            if (ehPalavraReservada(token.valor)) {
                token.tipo = PALAVRA_RESERVADA;
            } else {
                token.tipo = IDENTIFICADOR;
            }
        }
        // Verifica operadores
        else if (strchr("+-*/=!<>", entrada[i])) {
            token.tipo = OPERADOR;
            token.valor[j++] = entrada[i++];
            if (entrada[i] == '=' || entrada[i] == entrada[i - 1]) { // Operadores compostos
                token.valor[j++] = entrada[i++];
            }
        }
        // Verifica símbolos especiais
        else if (strchr("(){}[],;:", entrada[i])) {
            token.tipo = SIMBOLO_ESPECIAL;
            token.valor[j++] = entrada[i++];
        }
        // Caso desconhecido
        else {
            token.tipo = DESCONHECIDO;
            token.valor[j++] = entrada[i++];
        }
        token.valor[j] = '\0';

        // Armazena o token no array
        tokens[indiceTokens++] = token;
        imprimirToken(token);
    }
}

// Funções auxiliares para análise sintática
void verificarErro(const char *mensagem) {
    printf("Erro de sintaxe: %s\n", mensagem);
    exit(1);
}

int verificar_token(TipoToken tipo) {
    return posicaoAtual < indiceTokens && tokens[posicaoAtual].tipo == tipo;
}

Token token_atual() {
    return tokens[posicaoAtual];
}

void consumir_token() {
    posicaoAtual++;
}

// Funções de análise sintática
void analisarPrograma();
void analisarDeclaracao();
void analisarBloco();
void analisarExpressao();

// Implementação das funções
void analisarPrograma() {
    while (posicaoAtual < indiceTokens) {
        analisarDeclaracao();
    }
}

void analisarDeclaracao() {
    if (verificar_token(PALAVRA_RESERVADA)) {
        consumir_token();
        if (verificar_token(IDENTIFICADOR)) {
            consumir_token();
            if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "(") == 0) {
                // Declaração de função
                consumir_token();
                if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ")") == 0) {
                    consumir_token();
                    analisarBloco();
                } else {
                    verificarErro("Fechamento ')' esperado na declaração de função");
                }
            } else {
                // Declaração de variável
                if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ";") == 0) {
                    consumir_token();
                } else {
                    verificarErro("Ponto e vírgula esperado na declaração de variável");
                }
            }
        } else {
            verificarErro("Identificador esperado após palavra reservada");
        }
    } else {
        verificarErro("Palavra reservada esperada");
    }
}

void analisarBloco() {
    if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "{") == 0) {
        consumir_token();
        while (!verificar_token(SIMBOLO_ESPECIAL) || strcmp(token_atual().valor, "}") != 0) {
            analisarDeclaracao();
        }
        consumir_token();
    } else {
        verificarErro("Abertura '{' esperada para bloco");
    }
}

void analisarExpressao() {
    if (verificar_token(IDENTIFICADOR)) {
        consumir_token();
        if (verificar_token(OPERADOR)) {
            consumir_token();
            analisarExpressao();
        }
    } else {
        verificarErro("Expressão inválida");
    }
}

int main() {
    const char *codigoFonte = "int main() { int x = 10; if (x > 5) { x = x + 1; } }";

    // Análise léxica
    analisarLexico(codigoFonte);

    // Análise sintática
    analisarPrograma();

    return 0;
}
