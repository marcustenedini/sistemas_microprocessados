import numpy as np
import matplotlib.pyplot as plt

# Parâmetros do sinal
f_max = 10  # Frequência máxima do sinal (10 Hz)
f_sample = 30  # Taxa de amostragem (30 Hz)
t = np.linspace(0, 1, int(f_sample), endpoint=False)  # Vetor de tempo

# Sinal triangular (exemplo simplificado)
x = np.abs(np.mod(t, 1/f_max) - 0.5/f_max) * 2 - 1

# Aplicar a FFT
X = np.fft.fft(x)

# Frequências correspondentes
frequencies = np.fft.fftfreq(len(x), 1/f_sample)

# Plotar o espectro de frequência
plt.figure(figsize=(10, 6))
plt.plot(frequencies, np.abs(X))
plt.title('Espectro de Frequência do Sinal Triangular Amostrado')
plt.xlabel('Frequência (Hz)')
plt.ylabel('Magnitude')
plt.grid()
plt.show()