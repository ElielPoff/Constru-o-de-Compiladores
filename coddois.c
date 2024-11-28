#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // Para a função exit

#define MAX_TOKENS 1000 // Define o máximo de tokens

// Declaração de tipos
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

// Estrutura de Token
typedef struct {
    TipoToken tipo;
    char valor[100];
} Token;

// Declaração global de tokens
Token tokens[MAX_TOKENS]; // Array para armazenar tokens
int indiceTokens = 0;     // Índice do próximo token disponível
int posicaoAtual = 0;     // Índice usado na análise sintática

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
                // Verifica se o identificador já está na tabela de símbolos
                int idExistente = -1;
                for (int i = 0; i < indiceTabelaSimbolos; i++) {
                    if (strcmp(tabelaSimbolos[i].nome, token.valor) == 0) {
                        idExistente = tabelaSimbolos[i].id;
                        break;
                    }
                }

                if (idExistente == -1) {
                    // Se o identificador não existe, insira-o na tabela de símbolos
                    idExistente = indiceTabelaSimbolos++;
                    strcpy(tabelaSimbolos[idExistente].nome, token.valor);
                    tabelaSimbolos[idExistente].id = idExistente; // Atribui um ID único para o identificador
                }

                // Atribui o ID do identificador à variável token para exibição
                sprintf(token.valor, "%d", idExistente);  // Guarda o ID no valor para exibição
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

        tokens[indiceTokens++] = token; // Adiciona o token ao vetor de tokens
    }
}

void imprimirTokens() {
    printf("\nLista de Tokens:\n");
    for (int i = 0; i < indiceTokens; i++) {
        printf("[%d] ", i);
        imprimirToken(tokens[i]); // Usa a função já existente
    }
}

// Função para imprimir a tabela de símbolos
void imprimirTabelaSimbolos() {
    printf("\nTabela de Símbolos:\n");
    for (int i = 0; i < indiceTabelaSimbolos; i++) {
        printf("Identificador (ID %d) : %s\n", tabelaSimbolos[i].id, tabelaSimbolos[i].nome);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
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
        printf("Analisando tipo: %s\n", token_atual().valor); // Exibe o tipo para depuração
        consumir_token(); // Consome o tipo (int, float, etc.)
        
        // Verifica se o próximo token é um identificador
        if (verificar_token(IDENTIFICADOR)) {
            printf("Analisando identificador: %s\n", token_atual().valor);
            consumir_token(); // Consome o identificador

            // Verifica se o próximo token é um símbolo de ponto e vírgula ou um operador de atribuição
            if (verificar_token(SIMBOLO_ESPECIAL) && (strcmp(token_atual().valor, ";") == 0 || strcmp(token_atual().valor, "=") == 0)) {
                consumir_token(); // Consome o ponto e vírgula ou o sinal de atribuição
                if (strcmp(token_atual().valor, "=") == 0) {
                    analisarExpressao(); // Analisando a expressão de inicialização
                }
            } else {
                verificarErro("Ponto e vírgula ou símbolo de atribuição esperado após o identificador");
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

// Função principal
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
        analisarLexico(linha); // Preenche o vetor de tokens
    }

    fclose(arquivo);

    imprimirTokens(); 

    imprimirTabelaSimbolos(); // Imprime a tabela de símbolos ao final

    // Chamar a análise sintática
    posicaoAtual = 0; // Reinicia a posição atual
    analisarPrograma(); // Análise sintática do código fonte

    return 0;
}