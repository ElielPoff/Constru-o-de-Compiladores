import subprocess

# Função para processar o código-fonte no arquivo
def processar_codigo_fonte(caminho_arquivo):
    # Chama o código C compilado e captura a saída
    result = subprocess.run(["./analizador_lexico", caminho_arquivo], capture_output=True, text=True)
    print("Saída do Analisador Léxico:")
    print(result.stdout)  # Imprime a saída para depuração
    return result.stdout

# Função para processar os tokens gerados pelo analisador léxico
def processar_tokens(output):
    tokens = []
    for token_str in output.splitlines():
        # Ignora linhas em branco e verifica se a linha está no formato correto
        if ":" in token_str:
            tipo, valor = token_str.split(":", 1)  # Garantir que só dividimos uma vez
            tokens.append(Token(tipo, valor))
        else:
            print(f"Formato inválido: {token_str}")  # Para debugar e ver o que está dando errado
    return tokens

# Classe para representar os tokens
class Token:
    def __init__(self, tipo, valor):
        self.tipo = tipo
        self.valor = valor

# Tipos de tokens que o analisador sintático lidará
TIPOS_DE_TOKENS = {
    'NUM_INT': 'NUM_INT',
    'NUM_DEC': 'NUM_DEC',
    'IDENTIFICADOR': 'IDENTIFICADOR',
    'TEXTO': 'TEXTO',
    'PALAVRA_RESERVADA': 'PALAVRA_RESERVADA',
    'COMENTARIO': 'COMENTARIO',
    'OPERADOR': 'OPERADOR',
    'SIMBOLO_ESPECIAL': 'SIMBOLO_ESPECIAL',
    'DESCONHECIDO': 'DESCONHECIDO'
}

# Função para realizar a análise sintática
def analisar_sintatico(tokens):
    pos = 0

    def atual():
        return tokens[pos] if pos < len(tokens) else None

    def consumir():
        nonlocal pos
        pos += 1

    def verificar(tipo):
        return atual() and atual().tipo == tipo

    def erro(msg):
        print(f"Erro sintático: {msg}")
        exit(1)

    # Começo da análise sintática
    def programa():
        while atual():
            declaracao()

    def declaracao():
        if verificar(TIPOS_DE_TOKENS['PALAVRA_RESERVADA']) and atual().valor in ['int', 'float', 'char', 'boolean']:
            declaracao_variavel()
        elif verificar(TIPOS_DE_TOKENS['PALAVRA_RESERVADA']) and atual().valor == 'void':
            declaracao_funcao()
        else:
            erro("Declaração inválida")

    def declaracao_variavel():
        tipo = atual()
        consumir()
        if verificar(TIPOS_DE_TOKENS['IDENTIFICADOR']):
            id_token = atual()
            consumir()
            if verificar('SIMBOLO_ESPECIAL') and atual().valor == ';':
                consumir()
            elif verificar('SIMBOLO_ESPECIAL') and atual().valor == '=':
                consumir()
                expressao()
                if verificar('SIMBOLO_ESPECIAL') and atual().valor == ';':
                    consumir()
            else:
                erro(f"Esperado ';' ou '=' após {id_token.valor}")
        else:
            erro("Esperado identificador após tipo")

    def declaracao_funcao():
        tipo = atual()
        consumir()
        if verificar(TIPOS_DE_TOKENS['IDENTIFICADOR']):
            id_token = atual()
            consumir()
            if verificar('SIMBOLO_ESPECIAL') and atual().valor == '(':
                consumir()
                parametros()
                if verificar('SIMBOLO_ESPECIAL') and atual().valor == ')':
                    consumir()
                    bloco()
                else:
                    erro("Esperado ')' após parâmetros")
            else:
                erro("Esperado '(' após identificador de função")
        else:
            erro("Esperado identificador após tipo")

    def parametros():
        if verificar(TIPOS_DE_TOKENS['PALAVRA_RESERVADA']):
            tipo = atual()
            consumir()
            if verificar(TIPOS_DE_TOKENS['IDENTIFICADOR']):
                id_token = atual()
                consumir()
                if verificar('SIMBOLO_ESPECIAL') and atual().valor == ',':
                    consumir()
                    parametros()
                elif verificar('SIMBOLO_ESPECIAL') and atual().valor == ')':
                    return
                else:
                    erro("Esperado ',' ou ')' após identificador de parâmetro")
            else:
                erro("Esperado identificador de parâmetro")
        else:
            erro("Esperado tipo para parâmetro")

    def expressao():
        if verificar(TIPOS_DE_TOKENS['IDENTIFICADOR']):
            id_token = atual()
            consumir()
            if verificar('OPERADOR') and atual().valor == '=':
                consumir()
                expressao()
            else:
                erro("Esperado operador '=' após identificador")
        else:
            erro("Esperado identificador para expressão")

    def bloco():
        if verificar('SIMBOLO_ESPECIAL') and atual().valor == '{':
            consumir()
            while atual() and not (verificar('SIMBOLO_ESPECIAL') and atual().valor == '}'):
                declaracao()
            if verificar('SIMBOLO_ESPECIAL') and atual().valor == '}':
                consumir()
            else:
                erro("Esperado '}' para fechar o bloco")
        else:
            erro("Esperado '{' para início do bloco")

    # Começa a análise do programa
    programa()

# Função para imprimir os tokens
def imprimir_tokens(tokens):
    for token in tokens:
        print(f"{token.tipo}: {token.valor}")

# Função principal
def main():
    # Solicita o caminho do arquivo
    caminho_arquivo = input("Digite o caminho do arquivo com o código-fonte: ")

    # Processa o arquivo e captura os tokens
    output = processar_codigo_fonte(caminho_arquivo)

    # Converte a saída do analisador léxico em tokens
    tokens = processar_tokens(output)

    # Inicia a análise sintática
    analisar_sintatico(tokens)
    imprimir_tokens(tokens)

if __name__ == "__main__":
    main()
