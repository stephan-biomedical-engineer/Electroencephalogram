import pandas as pd
import numpy as np
from scipy.signal import butter, filtfilt
import matplotlib.pyplot as plt

# Caminho para o arquivo
file_path = "/home/stephan/Documents/IB1/interface/eeg_data/eeg_data_2025-09-08_21-25-47.txt"

# Carregar o arquivo
df = pd.read_csv(file_path, sep="\t")

# Extrair colunas
timestamps = df["timestamp"].to_numpy()
channels = [df["ch1"].to_numpy(),
            df["ch2"].to_numpy(),
            df["ch3"].to_numpy(),
            df["ch4"].to_numpy()]

# Estimar frequência de amostragem (Hz) a partir dos timestamps
# Assumindo que timestamp está em ms
dt = np.median(np.diff(timestamps)) / 1000.0  # ms -> s
fs = 1.0 / dt
print(f"Frequência de amostragem estimada: {fs:.2f} Hz")

# Plot no domínio do tempo
plt.figure(figsize=(12, 8))
for i, ch in enumerate(channels, start=1):
    plt.subplot(4, 2, 2*i - 1)
    plt.plot(timestamps / 1000.0, ch, label=f"Canal {i}")
    plt.xlabel("Tempo (s)")
    plt.ylabel("Amplitude (ADC)")
    plt.title(f"Canal {i} - Domínio do Tempo")

    # Band-pass filter (0.5Hz - 45Hz) using Butterworth

    lowcut = 0.5
    nyq = 0.5 * fs
    # Ensure highcut does not exceed Nyquist frequency
    highcut = min(45.0, nyq * 0.99)
    order = 4
    if highcut <= lowcut or highcut >= nyq:
        raise ValueError(f"Invalid filter frequencies: lowcut={lowcut}, highcut={highcut}, nyq={nyq}")
    b, a = butter(order, [lowcut / nyq, highcut / nyq], btype='band')
    ch_filt = filtfilt(b, a, ch)

    # FFT on filtered signal
    n = len(ch_filt)
    freqs = np.fft.rfftfreq(n, d=dt)
    fft_vals = np.abs(np.fft.rfft(ch_filt - np.mean(ch_filt)))

    plt.subplot(4, 2, 2*i)
    plt.plot(freqs, fft_vals, label=f"Canal {i} FFT (Filtrado)")
    plt.xlabel("Frequência (Hz)")
    plt.ylabel("Magnitude")
    plt.title(f"Canal {i} - Domínio da Frequência")
    plt.grid()
    plt.legend()

    # Spectrogram
    plt.figure(figsize=(6, 3))
    plt.specgram(ch_filt, Fs=fs, NFFT=256, noverlap=128, cmap='viridis')
    plt.title(f"Canal {i} - Espectrograma")
    plt.xlabel("Tempo (s)")
    plt.ylabel("Frequência (Hz)")
    plt.colorbar(label='Intensidade (dB)')
    plt.tight_layout()
    plt.grid()
    plt.legend()

    # FFT
    n = len(ch)
    freqs = np.fft.rfftfreq(n, d=dt)
    fft_vals = np.abs(np.fft.rfft(ch - np.mean(ch)))  # tira DC

    plt.subplot(4, 2, 2*i)
    plt.plot(freqs, fft_vals, label=f"Canal {i} FFT")
    plt.xlabel("Frequência (Hz)")
    plt.ylabel("Magnitude")
    plt.title(f"Canal {i} - Domínio da Frequência")
    plt.grid()
    plt.legend()

plt.tight_layout()
plt.show()
