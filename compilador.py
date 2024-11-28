import re

# Definições de tipos de tokens
class TipoToken:
    NUM_INT = "NUM_INT"
    NUM_DEC = "NUM_DEC"
    IDENTIFICADOR = "IDENTIFICADOR"
    TEXTO = "TEXTO"
    PALAVRA_RESERVADA = "PALAVRA_RESERVADA"
    COMENTARIO = "COMENTARIO"
    OPERADOR = "OPERADOR"
    SIMBOLO_ESPECIAL = "SIMBOLO_ESPECIAL"
    DESCONHECIDO = "DESCONHECIDO"

# Classe Token
class Token:
    def __init__(self, tipo, valor):
        self.tipo = tipo
        self.valor = valor

# Palavras reservadas
palavras_reservadas = [
    "int", "float", "char", "boolean", "void", "if", "else", "for", "while",
    "scanf", "println", "main", "return"
]

# Tabela de símbolos
tabela_simbolos = {}
indice_tabela_simbolos = 0

# Lista de tokens global
tokens = []

# Funções auxiliares
def imprimir_token(token):
    print(f"{token.tipo}: {token.valor}")

def eh_palavra_reservada(valor):
    return valor in palavras_reservadas

def eh_numero_inteiro(valor):
    return valor.isdigit()

def eh_numero_decimal(valor):
    return bool(re.fullmatch(r"\d+\.\d*|\.\d+", valor))

def eh_identificador(valor):
    return bool(re.fullmatch(r"[a-zA-Z_][a-zA-Z0-9_]*", valor))

def eh_comentario(valor):
    return valor.startswith("//")

def eh_operador(valor):
    operadores = {"=", "+", "-", "*", "/", "%", "&&", "||", "!", ">", "<", ">=", "<=", "!=", "=="}
    return valor in operadores

def eh_simbolo_especial(valor):
    return valor in "()[]{};," 

def analisar_lexico(entrada):
    global indice_tabela_simbolos
    tokens.clear()  
    token_strs = re.findall(r'//.*|".*?"|\d+\.\d*|\d+|[a-zA-Z_][a-zA-Z0-9_]*|[(){};=+\-*/%&|<>!,.]', entrada)
    for token_str in token_strs:
        if eh_comentario(token_str):
            token = Token(TipoToken.COMENTARIO, token_str)
        elif eh_numero_decimal(token_str):
            token = Token(TipoToken.NUM_DEC, token_str)
        elif eh_numero_inteiro(token_str):
            token = Token(TipoToken.NUM_INT, token_str)
        elif eh_palavra_reservada(token_str):
            token = Token(TipoToken.PALAVRA_RESERVADA, token_str)
        elif eh_identificador(token_str):
            token = Token(TipoToken.IDENTIFICADOR, token_str)
            if token_str not in tabela_simbolos:
                indice_tabela_simbolos += 1
                tabela_simbolos[token_str] = indice_tabela_simbolos
        elif token_str.startswith('"') and token_str.endswith('"'):
            token = Token(TipoToken.TEXTO, token_str.strip('"'))
        elif eh_operador(token_str):
            token = Token(TipoToken.OPERADOR, token_str)
        elif eh_simbolo_especial(token_str):
            token = Token(TipoToken.SIMBOLO_ESPECIAL, token_str)
        else:
            token = Token(TipoToken.DESCONHECIDO, token_str)
        
        # Armazena o token na lista
        tokens.append(token)
        imprimir_token(token)

def imprimir_tabela_simbolos():
    print("\nTabela de Símbolos:")
    for nome, id_ in tabela_simbolos.items():
        print(f"Identificador (ID {id_}): {nome}")

# Funções para manipular o fluxo de tokens
posicao_atual = 0

def verificar_token(tipo):
    return tokens[posicao_atual].tipo == tipo

def token_atual():
    return tokens[posicao_atual]

def consumir_token():
    global posicao_atual
    posicao_atual += 1


def analisar_expressao():
    analisar_expressao_logica()

def analisar_expressao_logica():
    analisar_expressao_relacional()
    while verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor in ["&&", "||"]:
        consumir_token()
        analisar_expressao_relacional()

def analisar_expressao_relacional():
    analisar_expressao_aritmetica()
    while verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor in [">", "<", ">=", "<=", "==", "!="]:
        consumir_token()
        analisar_expressao_aritmetica()

def analisar_expressao_aritmetica():
    analisar_expressao_multiplicativa()
    while verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor in ["+", "-"]:
        consumir_token()
        analisar_expressao_multiplicativa()

def analisar_expressao_multiplicativa():
    analisar_expressao_unaria()
    while verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor in ["*", "/", "%"]:
        consumir_token()
        analisar_expressao_unaria()

def analisar_expressao_unaria():
    if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor in ["-", "++", "--", "!"]:
        consumir_token()
        analisar_expressao_unaria()
    else:
        analisar_expressao_primaria()

def analisar_expressao_primaria():
    if verificar_token(TipoToken.IDENTIFICADOR):
        consumir_token()
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "[":
            consumir_token()
            analisar_expressao()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "]":
                consumir_token()
            else:
                verificar_erro("Fechamento ']' esperado para índice do array")
    elif verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
        consumir_token()
        analisar_expressao()
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
            consumir_token()
        else:
            verificar_erro("Fechamento ')' esperado")
    else:
        verificar_erro("Expressão inválida")

# Função principal de análise sintática
def analisar_programa():
    while posicao_atual < len(tokens):
        print(f"Token atual: {token_atual().valor} ({token_atual().tipo})")
        
        if verificar_token(TipoToken.PALAVRA_RESERVADA):
            if token_atual().valor == "int":
                analisar_declaracao_variavel()
            elif token_atual().valor == "return":
                analisar_comando()
            elif token_atual().valor == "main":
                consumir_token()  # Consome 'main'
                
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                    consumir_token()  # Consome '('
                    if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                        consumir_token()  # Consome ')'
                        
                        # Analisando o corpo de 'main' com suporte a um corpo vazio ou com comandos
                        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "{":
                            consumir_token()  # Consome '{'
                            
                            # Verificar se há comandos ou um bloco vazio
                            while not verificar_token(TipoToken.SIMBOLO_ESPECIAL) or token_atual().valor != "}":
                                analisar_comando()  # Aqui você pode analisar os comandos dentro do bloco
                            
                            # Consome o fechamento '}'
                            consumir_token()  # Consome '}'
                        else:
                            verificar_erro("Abertura '{' esperada após '(' no corpo do 'main'.")
                    else:
                        verificar_erro("Fechamento ')' esperado após '(' no 'main'.")
                else:
                    verificar_erro("Abertura '(' esperada após 'main'.")
        else:
            verificar_erro("Comando ou declaração esperado.")
    
    print("Análise feita com sucesso!")

def verificar_erro(mensagem):
    print(f"Erro de sintaxe: {mensagem}")
    exit(1)

def analisar_declaracao():
    if verificar_token(TipoToken.PALAVRA_RESERVADA):
        tipo = token_atual().valor
        if tipo in ["int", "float", "char", "boolean"]:
            consumir_token()  # Consome o tipo (ex: 'int')
            if verificar_token(TipoToken.IDENTIFICADOR):
                nome = token_atual().valor
                print(f"Declaração de {tipo} {nome} encontrada.")
                consumir_token()  # Consome o identificador (ex: 'x')
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                    consumir_token()  # Consome o ponto e vírgula
                else:
                    verificar_erro("Esperado ';' após identificador.")
            else:
                verificar_erro("Esperado identificador após tipo.")
        else:
            verificar_erro("Tipo desconhecido na declaração.")
    else:
        verificar_erro("Esperado palavra reservada na declaração.")

def analisar_declaracao_variavel():
    if verificar_token(TipoToken.PALAVRA_RESERVADA):
        tipo = token_atual().valor
        consumir_token()  # Consome o tipo (int, float, etc.)
        
        if verificar_token(TipoToken.IDENTIFICADOR):
            nome = token_atual().valor
            consumir_token()  # Consome o identificador
            print(f"Declaração de {tipo} {nome} encontrada.")
  
            # Caso com inicialização
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "=":
                consumir_token()  # Consome o igual
                if verificar_token(TipoToken.NUM_INT):  # Espera um número inteiro
                    print(f"Atribuição de valor: {token_atual().valor}")
                    consumir_token()  # Consome o valor da atribuição
                else:
                    print("Erro: Atribuição com valor não numérico.")
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                    consumir_token()  # Consome o ponto e vírgula
                else:
                    print("Erro: Ponto e vírgula esperado após atribuição.")
            
            # Caso sem inicialização
            elif verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()  # Consome o ponto e vírgula
            else:
                print("Erro: Símbolo de atribuição ou ponto e vírgula esperado.")
        else:
            print("Erro: Identificador esperado após tipo.")
    else:
        print("Erro: Tipo esperado para declaração de variável.")


def analisar_declaracao_funcao():
    if verificar_token(TipoToken.PALAVRA_RESERVADA):
        consumir_token()  # Consome o tipo de retorno
        if verificar_token(TipoToken.IDENTIFICADOR):
            consumir_token()  # Consome o nome da função
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_parametros()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
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
    if verificar_token(TipoToken.PALAVRA_RESERVADA) and token_atual().valor == "struct":
        consumir_token()  # Consome "struct"
        if verificar_token(TipoToken.IDENTIFICADOR):
            consumir_token()  # Consome o nome da estrutura
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "{":
                consumir_token()
                while verificar_token(TipoToken.PALAVRA_RESERVADA):
                    analisar_declaracao_variavel()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "}":
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
    if verificar_token(TipoToken.PALAVRA_RESERVADA) and token_atual().valor == "if":
        consumir_token()
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
            consumir_token()
            analisar_expressao()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                consumir_token()
                analisar_bloco()
                if verificar_token(TipoToken.PALAVRA_RESERVADA) and token_atual().valor == "else":
                    consumir_token()
                    analisar_bloco()
            else:
                verificar_erro("Fechamento ')' esperado após condição do if")
        else:
            verificar_erro("Abertura '(' esperada após 'if'")
    # Adicionar lógica para outras estruturas de controle


def analisar_expressao():
    if verificar_token(TipoToken.IDENTIFICADOR):
        consumir_token()
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor in ["=", "+=", "-=", "*=", "/="]:
            consumir_token()
            analisar_expressao()
        else:
            verificar_erro("Operador esperado em expressão")
    else:
        verificar_erro("Expressão inválida")



def analisar_parametros():
    if verificar_token(TipoToken.PALAVRA_RESERVADA):
        consumir_token()
        if verificar_token(TipoToken.IDENTIFICADOR):
            consumir_token()
            while verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ",":
                consumir_token()
                analisar_parametros()
        else:
            verificar_erro("Identificador de parâmetro esperado")


def analisar_bloco():
    if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "{":
        consumir_token()  # Consome a chave de abertura
        while posicao_atual < len(tokens) and not (verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "}"):
            analisar_programa()  # Análise recursiva do programa dentro do bloco
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "}":
            consumir_token()  # Consome a chave de fechamento
        else:
            verificar_erro("Fechamento '}' esperado para o bloco de código.")
    else:
        verificar_erro("Abertura '{' esperada para bloco de código.")


def analisar_comentario():
    if verificar_token(TipoToken.COMENTARIO):
        consumir_token()  # Ignora o comentário
    else:
        verificar_erro("Comentário inválido")


def analisar_estrutura_controle():
    if verificar_token(TipoToken.PALAVRA_RESERVADA):
        palavra = token_atual().valor
        if palavra == "if":
            consumir_token()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco()
                    if verificar_token(TipoToken.PALAVRA_RESERVADA) and token_atual().valor == "else":
                        consumir_token()
                        analisar_bloco()
                else:
                    verificar_erro("Fechamento ')' esperado após condição do if")
            else:
                verificar_erro("Abertura '(' esperada após 'if'")
        elif palavra == "while":
            consumir_token()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco()
                else:
                    verificar_erro("Fechamento ')' esperado após condição do while")
            else:
                verificar_erro("Abertura '(' esperada após 'while'")
        elif palavra == "for":
            consumir_token()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                    consumir_token()
                    analisar_expressao()
                    if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                        consumir_token()
                        analisar_expressao()
                        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
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
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                consumir_token()
                analisar_expressao()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
                    consumir_token()
                    analisar_bloco_switch()
                else:
                    verificar_erro("Fechamento ')' esperado após expressão do switch")
            else:
                verificar_erro("Abertura '(' esperada após 'switch'")
        elif palavra in ["break", "continue"]:
            consumir_token()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()
            else:
                verificar_erro("Ponto e vírgula esperado após 'break' ou 'continue'")
        elif palavra == "return":
            consumir_token()
            analisar_expressao()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()
            else:
                verificar_erro("Ponto e vírgula esperado após 'return'")
    else:
        verificar_erro("Estrutura de controle inválida")

def analisar_estrutura_controle_for(self):
    if verificar_token("for"):
        consumir_token()  # Consome 'for'
        if verificar_token("("):
            consumir_token()  # Consome '('
            analisar_expressao()  # Expressão inicial
            if verificar_token(";"):
                consumir_token()  # Consome ';'
                analisar_expressao()  # Condição
                if verificar_token(";"):
                    consumir_token()  # Consome ';'
                    analisar_expressao()  # Incremento
                    if verificar_token(")"):
                        consumir_token()  # Fecha parêntese
                        analisar_bloco()  # Bloco de código
                    else:
                        verificar_erro("Fechamento ')' esperado após condição do for")
                else:
                    verificar_erro("Esperado ';' após condição do for")
            else:
                verificar_erro("Esperado ';' após a inicialização do for")
        else:
            verificar_erro("Esperado '(' após 'for'")
    else:
        verificar_erro("Estrutura 'for' inválida")

def declaracao_variavel(self):
    tipo = self.tipo()  # Verifica se é um tipo válido
    if tipo:
        identificador = self.identificador()  # Obtém o identificador da variável
        if self.token == "=":  # Caso haja inicialização
            self.avançar()  # Avança para o próximo token
            expressao = self.expressao()  # Verifica o valor da expressão
        if self.token == ";":  # O ponto e vírgula deve estar presente
            self.aceitar_token(";")
        else:
            raise SyntaxError("Erro de sintaxe: Esperado ';' após declaração de variável")
    else:
        raise SyntaxError("Erro de sintaxe: Tipo inválido")

def analisar_comando():
    if verificar_token(TipoToken.IDENTIFICADOR):
        identificador = token_atual().valor
        consumir_token()  # Consome o identificador
        if verificar_token(TipoToken.OPERADOR) and token_atual().valor == "=":
            consumir_token()  # Consome o sinal de igual
            analisar_expressao()  # Analisa a expressão à direita
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                consumir_token()  # Consome o ponto e vírgula
            else:
                verificar_erro("Esperado ';' após expressão.")
        else:
            verificar_erro("Esperado '=' após identificador.")
    else:
        verificar_erro("Esperado identificador para atribuição.")

def analisar_expressao():
    if verificar_token(TipoToken.NUM_INT) or verificar_token(TipoToken.NUM_DEC) or verificar_token(TipoToken.IDENTIFICADOR):
        consumir_token()  # Consome a expressão (número ou identificador)
        
        while verificar_token(TipoToken.OPERADOR):
            consumir_token()  # Consome o operador (como +, -, *, etc.)
            if verificar_token(TipoToken.NUM_INT) or verificar_token(TipoToken.NUM_DEC) or verificar_token(TipoToken.IDENTIFICADOR):
                consumir_token()  # Consome o operando após o operador
            else:
                verificar_erro("Operando esperado após operador.")
    else:
        verificar_erro("Valor esperado para a expressão.")

def analisar_bloco_switch():
    if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "{":
        consumir_token()
        while verificar_token(TipoToken.PALAVRA_RESERVADA) and token_atual().valor in ["case", "default"]:
            if token_atual().valor == "case":
                consumir_token()
                analisar_expressao()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ":":
                    consumir_token()
                    analisar_bloco()
            elif token_atual().valor == "default":
                consumir_token()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ":":
                    consumir_token()
                    analisar_bloco()
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "}":
            consumir_token()
        else:
            verificar_erro("Fechamento '}' esperado para switch")
    else:
        verificar_erro("Abertura '{' esperada para switch")

def main():
    caminho_arquivo = input("Digite o caminho do arquivo com o código-fonte: ").strip()
    try:
        with open(caminho_arquivo, "r", encoding="utf-8") as arquivo:
            for linha in arquivo:
                analisar_lexico(linha.strip())  # Faz a análise léxica
    except FileNotFoundError:
        print(f"Erro: O arquivo '{caminho_arquivo}' não foi encontrado.")
        return
    except IOError as e:
        print(f"Erro ao abrir o arquivo: {e}")
        return

    imprimir_tabela_simbolos()  
    print("\n")
    analisar_programa()  

if __name__ == "__main__":
    main()