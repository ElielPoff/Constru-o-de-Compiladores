#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexico.c"
#include "global.h"

Token tokens[100];
int indiceTokens = 0;
int posicaoAtual = 0;

// Funções auxiliares
void verificarErro(const char *mensagem) {
    printf("Erro de sintaxe: %s\n", mensagem);
    exit(1);
}

int verificar_token(TipoToken tipo) {
    return tokens[posicaoAtual].tipo == tipo;
}

Token token_atual() {
    return tokens[posicaoAtual];
}

void consumir_token() {
    posicaoAtual++;
}

// Declaração de funções
void analisarPrograma();
void analisarDeclaracao();
void analisarDeclaracaoVariavel();
void analisarDeclaracaoFuncao();
void analisarDeclaracaoEstrutura();
void analisarComentario();
void analisarBloco();
void analisarParametros();
void analisarExpressao();
void analisarEstruturaControle();
void analisarEstruturaControleSwitch();
void analisarExpressaoLogica();
void analisarExpressaoRelacional();
void analisarExpressaoAritmetica();
void analisarExpressaoMultiplicativa();
void analisarExpressaoUnaria();
void analisarExpressaoPostfix();
void analisarPrimaria();
void analisarArgumentos();

// Função principal de análise do programa
void analisarPrograma() {
    while (posicaoAtual < indiceTokens) {
        analisarDeclaracao();
    }
}

// Analisador de Declaração
void analisarDeclaracao() {
    if (verificar_token(PALAVRA_RESERVADA)) {
        if (strcmp(token_atual().valor, "int") == 0 || strcmp(token_atual().valor, "float") == 0 || 
            strcmp(token_atual().valor, "double") == 0 || strcmp(token_atual().valor, "char") == 0 || 
            strcmp(token_atual().valor, "boolean") == 0) {
            analisarDeclaracaoVariavel();
        } else if (strcmp(token_atual().valor, "struct") == 0) {
            analisarDeclaracaoEstrutura();
        } else if (strcmp(token_atual().valor, "return") == 0 || strcmp(token_atual().valor, "if") == 0 ||
                   strcmp(token_atual().valor, "while") == 0 || strcmp(token_atual().valor, "for") == 0) {
            analisarEstruturaControle();
        } else if (strcmp(token_atual().valor, "void") == 0) {
            analisarDeclaracaoFuncao();
        } else {
            verificarErro("Declaração desconhecida");
        }
    } else {
        verificarErro("Declaração inválida");
    }
}

// Analisador de Declaração de Variável
void analisarDeclaracaoVariavel() {
    if (verificar_token(PALAVRA_RESERVADA)) {
        consumir_token(); // Consome o tipo (int, float, etc.)
        if (verificar_token(IDENTIFICADOR)) {
            consumir_token(); // Consome o identificador
            if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ";") == 0) {
                consumir_token(); // Consome o ponto e vírgula
            } else if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "=") == 0) {
                consumir_token();
                analisarExpressao();  // Analisando a expressão de inicialização
                if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ";") == 0) {
                    consumir_token(); // Consome o ponto e vírgula
                } else {
                    verificarErro("Ponto e vírgula esperado após a declaração de variável");
                }
            } else {
                verificarErro("Símbolo de atribuição ou ponto e vírgula esperado após o identificador");
            }
        } else {
            verificarErro("Identificador esperado após o tipo");
        }
    } else {
        verificarErro("Tipo esperado para declaração de variável");
    }
}

// Analisador de Declaração de Função
void analisarDeclaracaoFuncao() {
    if (verificar_token(PALAVRA_RESERVADA)) {
        consumir_token(); // Consome o tipo de retorno
        if (verificar_token(IDENTIFICADOR)) {
            consumir_token(); // Consome o nome da função
            if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "(") == 0) {
                consumir_token();
                analisarParametros();  // Analisando os parâmetros da função
                if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ")") == 0) {
                    consumir_token();
                    analisarBloco();  // Analisando o bloco da função
                } else {
                    verificarErro("Fechamento ')' esperado após parâmetros da função");
                }
            } else {
                verificarErro("Abertura '(' esperada após o nome da função");
            }
        } else {
            verificarErro("Nome da função esperado");
        }
    } else {
        verificarErro("Tipo de retorno esperado para declaração de função");
    }
}

// Analisador de Declaração de Estrutura
void analisarDeclaracaoEstrutura() {
    if (verificar_token(PALAVRA_RESERVADA) && strcmp(token_atual().valor, "struct") == 0) {
        consumir_token(); // Consome "struct"
        if (verificar_token(IDENTIFICADOR)) {
            consumir_token(); // Consome o nome da estrutura
            if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "{") == 0) {
                consumir_token();
                while (verificar_token(PALAVRA_RESERVADA)) {
                    analisarDeclaracaoVariavel(); // Declarando variáveis dentro da estrutura
                }
                if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "}") == 0) {
                    consumir_token();
                } else {
                    verificarErro("Fechamento '}' esperado para a estrutura");
                }
            } else {
                verificarErro("Abertura '{' esperada após o nome da estrutura");
            }
        } else {
            verificarErro("Nome da estrutura esperado");
        }
    } else {
        verificarErro("Palavra-chave 'struct' esperada");
    }
}

// Analisador de Comentário
void analisarComentario() {
    if (verificar_token(COMENTARIO)) {
        consumir_token(); // Consome o comentário
    } else {
        verificarErro("Comentário inválido");
    }
}

// Analisador de Bloco de Código
void analisarBloco() {
    if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "{") == 0) {
        consumir_token();
        while (!verificar_token(SIMBOLO_ESPECIAL) || strcmp(token_atual().valor, "}") != 0) {
            analisarDeclaracao();
        }
        consumir_token(); // Consome o fechamento '}'
    } else {
        verificarErro("Abertura '{' esperada para o bloco de código");
    }
}

// Analisador de Parâmetros de Função
void analisarParametros() {
    if (verificar_token(PALAVRA_RESERVADA)) {
        consumir_token(); // Consome o tipo do parâmetro
        if (verificar_token(IDENTIFICADOR)) {
            consumir_token(); // Consome o identificador
            while (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ",") == 0) {
                consumir_token();
                analisarParametros();  // Recurssão para múltiplos parâmetros
            }
        } else {
            verificarErro("Identificador de parâmetro esperado");
        }
    }
}

// Analisador de Expressões
void analisarExpressao() {
    if (verificar_token(IDENTIFICADOR)) {
        consumir_token();  // Consome o identificador
        if (verificar_token(SIMBOLO_ESPECIAL)) {
            if (strcmp(token_atual().valor, "=") == 0 || strcmp(token_atual().valor, "+=") == 0 ||
                strcmp(token_atual().valor, "-=") == 0 || strcmp(token_atual().valor, "*=") == 0 ||
                strcmp(token_atual().valor, "/=") == 0) {
                consumir_token();
                analisarExpressao();  // Atribuição ou operação
            } else {
                verificarErro("Operador inválido em expressão");
            }
        }
    } else {
        verificarErro("Identificador ou valor esperado na expressão");
    }
}

// Analisador de Estruturas de Controle
void analisarEstruturaControle() {
    if (verificar_token(PALAVRA_RESERVADA)) {
        if (strcmp(token_atual().valor, "if") == 0) {
            consumir_token();
            if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, "(") == 0) {
                consumir_token();
                analisarExpressao(); // Condição
                if (verificar_token(SIMBOLO_ESPECIAL) && strcmp(token_atual().valor, ")") == 0) {
                    consumir_token();
                    analisarBloco(); // Bloco do if
                    if (verificar_token(PALAVRA_RESERVADA) && strcmp(token_atual().valor, "else") == 0) {
                        consumir_token();
                        analisarBloco(); // Bloco do else
                    }
                } else {
                    verificarErro("Fechamento ')' esperado para a condição if");
                }
            } else {
                verificarErro("Abertura '(' esperada após 'if'");
            }
        }
        // Adicionar mais verificações para os outros tipos de estruturas de controle (while, for, etc.)
    }
}


int main() {
    const char *codigoFonte = "int main() { int x = 10; if (x > 5) { x = x + 1; } }";
    
    // Primeiro, é necessário rodar o analisador léxico (lexico.c) para preencher o array `tokens`
    analisarLexico(codigoFonte);  // Assume que a função 'analisarLexico' já foi implementada e preenche 'tokens'

    // Agora, iniciamos a análise sintática
    analisarPrograma();
    
    return 0;
}