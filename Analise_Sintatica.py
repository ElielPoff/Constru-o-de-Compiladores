class Token:
    def __init__(self, tipo, valor):
        self.tipo = tipo
        self.valor = valor


# Tipos de tokens (definir conforme necessário)
PALAVRA_RESERVADA = "PALAVRA_RESERVADA"
IDENTIFICADOR = "IDENTIFICADOR"
SIMBOLO_ESPECIAL = "SIMBOLO_ESPECIAL"
COMENTARIO = "COMENTARIO"


# Funções auxiliares
def verificar_erro(mensagem):
    print(f"Erro de sintaxe: {mensagem}")
    exit(1)


def verificar_token(tipo):
    return tokens[posicao_atual].tipo == tipo


def token_atual():
    return tokens[posicao_atual]


def consumir_token():
    global posicao_atual
    posicao_atual += 1


# Declaração de funções
def analisar_programa():
    while posicao_atual < len(tokens):
        analisar_declaracao()


def analisar_declaracao():
    if verificar_token(PALAVRA_RESERVADA):
        if token_atual().valor in ["int", "float", "double", "char", "boolean"]:
            analisar_declaracao_variavel()
        elif token_atual().valor == "struct":
            analisar_declaracao_estrutura()
        elif token_atual().valor in ["return", "if", "while", "for"]:
            analisar_estrutura_controle()
        elif token_atual().valor == "void":
            analisar_declaracao_funcao()
        else:
            verificar_erro("Declaração desconhecida")
    else:
        verificar_erro("Declaração inválida")


def analisar_declaracao_variavel():
    if verificar_token(PALAVRA_RESERVADA):
        consumir_token()  # Consome o tipo
        if verificar_token(IDENTIFICADOR):
            consumir_token()  # Consome o identificador
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()  # Consome o ponto e vírgula
            elif verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "=":
                consumir_token()
                analisar_expressao()  # Analisando a inicialização
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                    consumir_token()  # Consome o ponto e vírgula
                else:
                    verificar_erro("Ponto e vírgula esperado após declaração de variável")
            else:
                verificar_erro("Símbolo de atribuição ou ponto e vírgula esperado")
        else:
            verificar_erro("Identificador esperado após o tipo")
    else:
        verificar_erro("Tipo esperado para declaração de variável")


def analisar_declaracao_funcao():
    if verificar_token(PALAVRA_RESERVADA):
        consumir_token()  # Consome o tipo de retorno
        if verificar_token(IDENTIFICADOR):
            consumir_token()  # Consome o nome da função
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_parametros()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco()
                else:
                    verificar_erro("Fechamento ')' esperado após parâmetros")
            else:
                verificar_erro("Abertura '(' esperada após o nome da função")
        else:
            verificar_erro("Nome da função esperado")
    else:
        verificar_erro("Tipo de retorno esperado para declaração de função")


def analisar_declaracao_estrutura():
    if verificar_token(PALAVRA_RESERVADA) and token_atual().valor == "struct":
        consumir_token()  # Consome "struct"
        if verificar_token(IDENTIFICADOR):
            consumir_token()  # Consome o nome da estrutura
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "{":
                consumir_token()
                while verificar_token(PALAVRA_RESERVADA):
                    analisar_declaracao_variavel()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "}":
                    consumir_token()
                else:
                    verificar_erro("Fechamento '}' esperado para a estrutura")
            else:
                verificar_erro("Abertura '{' esperada após o nome da estrutura")
        else:
            verificar_erro("Nome da estrutura esperado")
    else:
        verificar_erro("Palavra-chave 'struct' esperada")


def analisar_estrutura_controle():
    if verificar_token(PALAVRA_RESERVADA) and token_atual().valor == "if":
        consumir_token()
        if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
            consumir_token()
            analisar_expressao()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                consumir_token()
                analisar_bloco()
                if verificar_token(PALAVRA_RESERVADA) and token_atual().valor == "else":
                    consumir_token()
                    analisar_bloco()
            else:
                verificar_erro("Fechamento ')' esperado após condição do if")
        else:
            verificar_erro("Abertura '(' esperada após 'if'")
    # Adicionar lógica para outras estruturas de controle


def analisar_expressao():
    if verificar_token(IDENTIFICADOR):
        consumir_token()
        if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor in ["=", "+=", "-=", "*=", "/="]:
            consumir_token()
            analisar_expressao()
        else:
            verificar_erro("Operador esperado em expressão")
    else:
        verificar_erro("Expressão inválida")


def analisar_parametros():
    if verificar_token(PALAVRA_RESERVADA):
        consumir_token()
        if verificar_token(IDENTIFICADOR):
            consumir_token()
            while verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ",":
                consumir_token()
                analisar_parametros()
        else:
            verificar_erro("Identificador de parâmetro esperado")


def analisar_bloco():
    if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "{":
        consumir_token()
        while not verificar_token(SIMBOLO_ESPECIAL) or token_atual().valor != "}":
            analisar_declaracao()
        consumir_token()
    else:
        verificar_erro("Abertura '{' esperada para bloco de código")


def analisar_comentario():
    if verificar_token(COMENTARIO):
        consumir_token()  # Ignora o comentário
    else:
        verificar_erro("Comentário inválido")


def analisar_estrutura_controle():
    if verificar_token(PALAVRA_RESERVADA):
        palavra = token_atual().valor
        if palavra == "if":
            consumir_token()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco()
                    if verificar_token(PALAVRA_RESERVADA) and token_atual().valor == "else":
                        consumir_token()
                        analisar_bloco()
                else:
                    verificar_erro("Fechamento ')' esperado após condição do if")
            else:
                verificar_erro("Abertura '(' esperada após 'if'")
        elif palavra == "while":
            consumir_token()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco()
                else:
                    verificar_erro("Fechamento ')' esperado após condição do while")
            else:
                verificar_erro("Abertura '(' esperada após 'while'")
        elif palavra == "for":
            consumir_token()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                    consumir_token()
                    analisar_expressao()
                    if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                        consumir_token()
                        analisar_expressao()
                        if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                            consumir_token()
                            analisar_bloco()
                        else:
                            verificar_erro("Fechamento ')' esperado após for")
                    else:
                        verificar_erro("';' esperado após condição do for")
                else:
                    verificar_erro("';' esperado após inicialização do for")
            else:
                verificar_erro("Abertura '(' esperada após 'for'")
        elif palavra == "switch":
            consumir_token()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco_switch()
                else:
                    verificar_erro("Fechamento ')' esperado após expressão do switch")
            else:
                verificar_erro("Abertura '(' esperada após 'switch'")
        elif palavra in ["break", "continue"]:
            consumir_token()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()
            else:
                verificar_erro("Ponto e vírgula esperado após 'break' ou 'continue'")
        elif palavra == "return":
            consumir_token()
            analisar_expressao()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()
            else:
                verificar_erro("Ponto e vírgula esperado após 'return'")
    else:
        verificar_erro("Estrutura de controle inválida")


def analisar_expressao():
    analisar_expressao_logica()


def analisar_expressao_logica():
    analisar_expressao_relacional()
    while verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor in ["&&", "||"]:
        consumir_token()
        analisar_expressao_relacional()


def analisar_expressao_relacional():
    analisar_expressao_aritmetica()
    while verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor in [">", "<", ">=", "<=", "==", "!="]:
        consumir_token()
        analisar_expressao_aritmetica()


def analisar_expressao_aritmetica():
    analisar_expressao_multiplicativa()
    while verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor in ["+", "-"]:
        consumir_token()
        analisar_expressao_multiplicativa()


def analisar_expressao_multiplicativa():
    analisar_expressao_unaria()
    while verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor in ["*", "/", "%"]:
        consumir_token()
        analisar_expressao_unaria()


def analisar_expressao_unaria():
    if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor in ["-", "++", "--", "!"]:
        consumir_token()
        analisar_expressao_unaria()
    else:
        analisar_expressao_primaria()


def analisar_expressao_primaria():
    if verificar_token(IDENTIFICADOR):
        consumir_token()
        if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "[":
            consumir_token()
            analisar_expressao()
            if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "]":
                consumir_token()
            else:
                verificar_erro("Fechamento ']' esperado para índice do array")
    elif verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "(":
        consumir_token()
        analisar_expressao()
        if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ")":
            consumir_token()
        else:
            verificar_erro("Fechamento ')' esperado")
    else:
        verificar_erro("Expressão inválida")


def analisar_bloco_switch():
    if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "{":
        consumir_token()
        while verificar_token(PALAVRA_RESERVADA) and token_atual().valor in ["case", "default"]:
            if token_atual().valor == "case":
                consumir_token()
                analisar_expressao()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ":":
                    consumir_token()
                    analisar_bloco()
            elif token_atual().valor == "default":
                consumir_token()
                if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == ":":
                    consumir_token()
                    analisar_bloco()
        if verificar_token(SIMBOLO_ESPECIAL) and token_atual().valor == "}":
            consumir_token()
        else:
            verificar_erro("Fechamento '}' esperado para switch")
    else:
        verificar_erro("Abertura '{' esperada para switch")