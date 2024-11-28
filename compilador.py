import re

# Definições de tokens e classes
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

# Funções auxiliares para análise léxica
def imprimir_token(token):
    print(f"{token.tipo}: {token.valor}")

def eh_palavra_reservada(valor):
    return valor in palavras_reservadas

def eh_numero_inteiro(valor):
    return valor.isdigit()

def eh_numero_decimal(valor):
    return bool(re.fullmatch(r"\d+\.\d+", valor))

def eh_identificador(valor):
    return bool(re.fullmatch(r"[a-zA-Z_][a-zA-Z0-9_]*", valor))

def eh_comentario(valor):
    return valor.startswith("//")

def eh_operador(valor):
    operadores = {"=", "+", "-", "*", "/", "%", "&&", "||", "!", ">", "<", ">=", "<=", "!=", "=="}
    return valor in operadores

def eh_simbolo_especial(valor):
    return valor in "()[]{};,"

# Análise léxica
def analisar_lexico(entrada):
    global indice_tabela_simbolos
    tokens_gerados = []
    tokens = re.findall(r'//.*|".*?"|\w+|[^\s\w]', entrada)
    for token_str in tokens:
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
        tokens_gerados.append(token)
    return tokens_gerados

# Funções para análise sintática
posicao_atual = 0
tokens = []

def verificar_erro(mensagem):
    print(f"Erro de sintaxe: {mensagem}")
    exit(1)

def verificar_token(tipo):
    return posicao_atual < len(tokens) and tokens[posicao_atual].tipo == tipo

def token_atual():
    return tokens[posicao_atual]

def consumir_token():
    global posicao_atual
    if posicao_atual < len(tokens):
        posicao_atual += 1

def imprimir_tabela_simbolos():
    print("\nTabela de Símbolos:")
    for nome, id_ in tabela_simbolos.items():
        print(f"Identificador (ID {id_}): {nome}")


# Funções de análise gramatical
def analisar_programa():
    while posicao_atual < len(tokens):
        analisar_declaracao()

def analisar_declaracao():
    if verificar_token(TipoToken.PALAVRA_RESERVADA):
        if token_atual().valor in ["int", "float", "char", "boolean", "void"]:
            consumir_token()
            if verificar_token(TipoToken.IDENTIFICADOR):
                consumir_token()
                if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
                    analisar_declaracao_funcao()
                elif verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ";":
                    consumir_token()
                else:
                    verificar_erro("Erro na declaração.")
            else:
                verificar_erro("Identificador esperado.")
    else:
        verificar_erro("Declaração inválida.")

def analisar_declaracao_funcao():
    if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "(":
        consumir_token()
        if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == ")":
            consumir_token()
            if verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "{":
                consumir_token()
                while not (verificar_token(TipoToken.SIMBOLO_ESPECIAL) and token_atual().valor == "}"):
                    analisar_declaracao()
                consumir_token()
            else:
                verificar_erro("Abertura de bloco esperada.")
        else:
            verificar_erro("Fechamento ')' esperado.")
    else:
        verificar_erro("Abertura '(' esperada.")












# Função principal do compilador
def main():
    global tokens, posicao_atual
    caminho_arquivo = input("Digite o caminho do arquivo com o código-fonte: ").strip()
    try:
        with open(caminho_arquivo, "r", encoding="utf-8") as arquivo:
            codigo_fonte = arquivo.read()
            # Análise léxica
            tokens = analisar_lexico(codigo_fonte)
            for token in tokens:
                imprimir_token(token)
            # Análise sintática
            posicao_atual = 0
            analisar_programa()
    except FileNotFoundError:
        print(f"Erro: O arquivo '{caminho_arquivo}' não foi encontrado.")
        return
    except IOError as e:
        print(f"Erro ao abrir o arquivo: {e}")
        return
    
    imprimir_tabela_simbolos()
        
if __name__ == "__main__":
    main()
