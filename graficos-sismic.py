import matplotlib.pyplot as plt
from collections import deque
import time
import random

# Configuração do gráfico
max_points = 100  # Número máximo de pontos no gráfico
dados = deque(maxlen=max_points)  # Armazena os últimos 100 pontos de velocidade
tempo = deque(maxlen=max_points)  # Armazena os tempos correspondentes

plt.ion()  # Habilita o modo interativo do matplotlib
fig, ax = plt.subplots()
linha, = ax.plot(tempo, dados)
ax.set_ylim(0, 10)  # Ajuste os limites do eixo Y conforme necessário
ax.set_xlabel('Tempo (s)')
ax.set_ylabel('Velocidade (km/h)')
ax.set_title('Tempo vs Velocidade')

start_time = time.time()

def gerar_dado_simulado():
    """
    Gera um dado simulado de velocidade.
    Retorna um valor de velocidade aleatório entre 0 e 10 km/h.
    """
    return random.uniform(0, 10)

try:
    while True:
        # Verifica se a janela do gráfico ainda está aberta
        if not plt.get_fignums():
            print("Janela do gráfico fechada. Encerrando o script.")
            break

        # Gera um dado simulado de velocidade
        velocidade = gerar_dado_simulado()
        tempo_atual = time.time() - start_time

        # Adiciona a velocidade e o tempo aos deques
        dados.append(velocidade)
        tempo.append(tempo_atual)

        # Atualiza o gráfico
        linha.set_xdata(tempo)
        linha.set_ydata(dados)
        ax.relim()
        ax.autoscale_view()
        plt.draw()
        plt.pause(0.1)  # Pausa curta para atualizar o gráfico

except KeyboardInterrupt:
    print("Script interrompido pelo usuário.")

finally:
    plt.close()  # Fecha a janela do gráfico
    print("Gráfico fechado.")