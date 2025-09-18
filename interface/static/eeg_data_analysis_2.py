import numpy as np
import matplotlib.pyplot as plt
import os
from scipy.signal import butter, filtfilt, spectrogram
import plotly.graph_objs as go
import plotly.io as pio
import re

# Path to the EEG data file
print("File name: ", end='')
data_name = input()
data_file = os.path.join(os.path.dirname(__file__), f'../eeg_data/{data_name}')

# Read EEG data - AGORA COM TIMESTAMP EM SEGUNDOS
def read_eeg_data(filepath):
    with open(filepath, 'r') as f:
        # Try to auto-detect delimiter and check header
        first_line = f.readline().strip()
        delimiter = '\t' if '\t' in first_line else ','
        
        # Check if first column is timestamp in seconds
        f.seek(0)
        if 'timestamp' in first_line.lower():
            # Skip header
            data = np.loadtxt(f, delimiter=delimiter, skiprows=1)
        else:
            # No header
            data = np.loadtxt(f, delimiter=delimiter)
    
    return data

def plot_time_domain(data, timestamps=None, fs=4000, save_dir=None, dt_str=""):
    """
    Plota dados no domínio do tempo
    data: array numpy com canais EEG
    timestamps: array com timestamps em segundos (opcional)
    fs: taxa de amostragem (4000 Hz)
    """
    n_channels = data.shape[1]
    
    for ch in range(n_channels):
        if timestamps is not None:
            t = timestamps  # Já está em segundos
        else:
            t = np.arange(data.shape[0]) / fs
        
        plt.figure(figsize=(12, 5))
        plt.plot(t, data[:, ch], label=f'Channel {ch+1}', linewidth=0.5)
        plt.title(f'EEG Signal - Channel {ch+1} ({fs} Hz)')
        plt.xlabel('Time (s)')
        plt.ylabel('Amplitude (ADC units)')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_dir:
            png_path = os.path.join(save_dir, f'time_domain_{dt_str}_ch{ch+1}.png')
            plt.savefig(png_path, dpi=300, bbox_inches='tight')
        plt.close()

        # Plotly HTML
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=t, y=data[:, ch], mode='lines', 
                               name=f'Channel {ch+1}', line=dict(width=1)))
        fig.update_layout(title=f'EEG Signal - Channel {ch+1} ({fs} Hz)',
                        xaxis_title='Time (s)', 
                        yaxis_title='Amplitude (ADC units)',
                        template='plotly_white')
        if save_dir:
            html_path = os.path.join(save_dir, f'time_domain_{dt_str}_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def plot_frequency_domain(data, fs=4000, save_dir=None, dt_str="", max_freq=100):
    """
    Plota espectro de frequência
    max_freq: frequência máxima a ser mostrada (Hz)
    """
    n_channels = data.shape[1]
    
    for ch in range(n_channels):
        n = data.shape[0]
        freqs = np.fft.rfftfreq(n, d=1/fs)
        fft_vals = np.abs(np.fft.rfft(data[:, ch]))
        
        # Aplicar janela de Hann para melhor resolução espectral
        window = np.hanning(n)
        fft_vals_windowed = np.abs(np.fft.rfft(data[:, ch] * window))
        
        # Limitar ao range de frequências desejado
        freq_mask = freqs <= max_freq
        
        plt.figure(figsize=(12, 5))
        plt.plot(freqs[freq_mask], fft_vals_windowed[freq_mask], 
                label=f'Channel {ch+1}', linewidth=1)
        plt.title(f'Frequency Spectrum - Channel {ch+1}')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Amplitude')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        if save_dir:
            png_path = os.path.join(save_dir, f'freq_domain_{dt_str}_ch{ch+1}.png')
            plt.savefig(png_path, dpi=300, bbox_inches='tight')
        plt.close()

        # Plotly HTML
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=freqs[freq_mask], y=fft_vals_windowed[freq_mask], 
                               mode='lines', name=f'Channel {ch+1}'))
        fig.update_layout(title=f'Frequency Spectrum - Channel {ch+1}',
                        xaxis_title='Frequency (Hz)', 
                        yaxis_title='Amplitude',
                        template='plotly_white')
        if save_dir:
            html_path = os.path.join(save_dir, f'freq_domain_{dt_str}_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def plot_spectrogram(data, fs=4000, save_dir=None, dt_str="", max_freq=100):
    """
    Plota espectrograma com parâmetros otimizados para 4kHz
    """
    n_channels = data.shape[1]
    
    for ch in range(n_channels):
        # Parâmetros otimizados para 4kHz
        nperseg = 1024  # Janela maior para melhor resolução frequencial
        noverlap = 512   # 50% de overlap
        
        f, t, Sxx = spectrogram(data[:, ch], fs=fs, 
                               nperseg=nperseg, noverlap=noverlap,
                               window='hann')
        
        # Limitar frequências
        freq_mask = f <= max_freq
        
        plt.figure(figsize=(12, 6))
        plt.pcolormesh(t, f[freq_mask], 10 * np.log10(Sxx[freq_mask]), 
                      shading='gouraud', cmap='viridis')
        plt.title(f'Spectrogram - Channel {ch+1}')
        plt.xlabel('Time (s)')
        plt.ylabel('Frequency (Hz)')
        plt.colorbar(label='Power/Frequency (dB/Hz)')
        plt.ylim(0, max_freq)
        plt.tight_layout()
        
        if save_dir:
            png_path = os.path.join(save_dir, f'spectrogram_{dt_str}_ch{ch+1}.png')
            plt.savefig(png_path, dpi=300, bbox_inches='tight')
        plt.close()

        # Plotly HTML
        fig = go.Figure(data=go.Heatmap(
            z=10 * np.log10(Sxx[freq_mask] + 1e-12), 
            x=t, y=f[freq_mask], 
            colorscale='Viridis',
            hovertemplate='Time: %{x:.2f}s<br>Freq: %{y:.1f}Hz<br>Power: %{z:.1f}dB<extra></extra>'
        ))
        fig.update_layout(title=f'Spectrogram - Channel {ch+1}',
                        xaxis_title='Time (s)', 
                        yaxis_title='Frequency (Hz)',
                        template='plotly_white')
        if save_dir:
            html_path = os.path.join(save_dir, f'spectrogram_{dt_str}_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def bandpass_filter(data, fs, lowcut=1.0, highcut=45.0, order=4):
    """
    Filtro passa-banda Butterworth otimizado para EEG
    """
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return filtfilt(b, a, data, axis=0)

def notch_filter(data, fs, notch_freq=60.0, quality_factor=30.0):
    """
    Filtro notch para remover ruído de 60Hz (ou 50Hz)
    """
    from scipy.signal import iirnotch
    nyq = 0.5 * fs
    freq = notch_freq / nyq
    b, a = iirnotch(freq, quality_factor)
    return filtfilt(b, a, data, axis=0)

def print_top_frequencies(data, fs=4000, top_n=10, max_freq=100):
    """
    Imprime as frequências dominantes
    """
    for ch in range(data.shape[1]):
        n = data.shape[0]
        freqs = np.fft.rfftfreq(n, d=1/fs)
        fft_vals = np.abs(np.fft.rfft(data[:, ch]))
        
        # Aplicar janela
        window = np.hanning(n)
        fft_vals = np.abs(np.fft.rfft(data[:, ch] * window))
        
        # Ignorar DC e limitar frequência
        freq_mask = (freqs > 0.5) & (freqs <= max_freq)
        freqs = freqs[freq_mask]
        fft_vals = fft_vals[freq_mask]
        
        # Encontrar picos
        from scipy.signal import find_peaks
        peaks, _ = find_peaks(fft_vals, height=np.mean(fft_vals)*2)
        
        print(f"\nChannel {ch+1} - Top {top_n} frequencies:")
        if len(peaks) > 0:
            # Ordenar picos por amplitude
            sorted_peaks = sorted(peaks, key=lambda x: fft_vals[x], reverse=True)[:top_n]
            for i, peak in enumerate(sorted_peaks):
                print(f"  {i+1}: {freqs[peak]:6.2f} Hz - Amplitude: {fft_vals[peak]:8.2f}")
        else:
            print("  No significant peaks found")

def main():
    # Carregar dados
    data = read_eeg_data(data_file)
    
    # Seus dados têm: timestamp, ch1, ch2, ch3, ch4
    timestamps = data[:, 0]  # Já em segundos!
    eeg_channels = data[:, 1:5]  # Canais 1-4
    
    print(f"Data shape: {eeg_channels.shape}")
    print(f"Duration: {timestamps[-1]:.2f} seconds")
    print(f"Sampling rate: {len(timestamps)/timestamps[-1]:.0f} Hz")
    
    # Extrair timestamp do filename
    match = re.search(r'eeg_data_(\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2})', data_file)
    dt_str = match.group(1) if match else "unknown"
    
    # Criar diretório de saída
    save_dir = os.path.join(os.path.dirname(__file__), 'images_4khz')
    os.makedirs(save_dir, exist_ok=True)
    
    # === PRÉ-PROCESSAMENTO ===
    print("\n=== Pre-processing ===")
    
    # 1. Remover média (DC offset)
    eeg_detrended = eeg_channels - np.mean(eeg_channels, axis=0)
    
    # 2. Filtro notch 60Hz (se necessário)
    eeg_notch = notch_filter(eeg_detrended, fs=4000, notch_freq=60.0)
    
    # 3. Filtro passa-banda 1-45Hz para EEG
    eeg_filtered = bandpass_filter(eeg_notch, fs=4000, lowcut=1.0, highcut=45.0)
    
    # === ANÁLISE ===
    print("\n=== Time Domain Analysis ===")
    plot_time_domain(eeg_filtered, timestamps=timestamps, fs=4000, 
                    save_dir=save_dir, dt_str=dt_str)
    
    print("\n=== Frequency Domain Analysis ===")
    plot_frequency_domain(eeg_filtered, fs=4000, save_dir=save_dir, 
                         dt_str=dt_str, max_freq=100)
    
    print("\n=== Spectrogram Analysis ===")
    plot_spectrogram(eeg_filtered, fs=4000, save_dir=save_dir, 
                    dt_str=dt_str, max_freq=100)
    
    print("\n=== Top Frequencies ===")
    print_top_frequencies(eeg_filtered, fs=4000, top_n=10, max_freq=100)
    
    # Salvar dados processados
    processed_data = np.column_stack((timestamps, eeg_filtered))
    output_file = os.path.join(save_dir, f'processed_{dt_str}.csv')
    np.savetxt(output_file, processed_data, delimiter=',',
              header='timestamp,ch1,ch2,ch3,ch4', comments='')
    
    print(f"\nProcessing complete! Results saved in: {save_dir}")

if __name__ == '__main__':
    main()
