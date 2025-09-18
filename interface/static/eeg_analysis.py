import numpy as np
import matplotlib.pyplot as plt
import os
from scipy.signal import butter, filtfilt
import plotly.graph_objs as go
import plotly.io as pio


# Path to the EEG data file
print("File name: ",end='')
data_name = input()
data_file = os.path.join(os.path.dirname(__file__), f'../eeg_data/{data_name}')

# Read EEG data (assuming tab or comma separated, channels in columns)
def read_eeg_data(filepath):
    with open(filepath, 'r') as f:
        # Try to auto-detect delimiter
        first_line = f.readline()
        delimiter = '\t' if '\t' in first_line else ','
        f.seek(0)
        data = np.loadtxt(f, delimiter=delimiter, skiprows=1)
    return data

def plot_time_domain(data, timestamps=None, fs=4000, save_dir=None, dt_str=""):
    for ch in range(data.shape[1]):
        if timestamps is not None:
            t = timestamps
        else:
            t = np.arange(data.shape[0]) / fs
        
        plt.figure(figsize=(10, 4))
        plt.plot(t, data[:, ch], label=f'Channel {ch+1}')
        plt.title(f'EEG Signal Amplitude (Time Domain) - Channel {ch+1}')
        plt.xlabel('Time (s)')
        plt.ylabel('Amplitude (ADC units)')
        plt.ylim(0, 65535)  # ← FIXO DE 0 A 65535
        plt.legend()
        plt.tight_layout()
        if save_dir:
            png_path = os.path.join(save_dir, f'time_domain_{dt_str}_ch{ch+1}.png')
            plt.savefig(png_path)
        plt.close()

        # Plotly HTML também com eixo Y fixo
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=t, y=data[:, ch], mode='lines', name=f'Channel {ch+1}'))
        fig.update_layout(title=f'EEG Signal Amplitude (Time Domain) - Channel {ch+1}',
                          xaxis_title='Time (s)', yaxis_title='Amplitude (ADC units)',
                          yaxis_range=[0, 65535])  # ← FIXO DE 0 A 65535
        if save_dir:
            html_path = os.path.join(save_dir, f'time_domain_{dt_str}_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def plot_frequency_domain(data, fs=4000, save_dir=None, dt_str="", max_freq=50):
    for ch in range(data.shape[1]):
        n = data.shape[0]
        freqs = np.fft.rfftfreq(n, d=1/fs)
        fft_vals = np.abs(np.fft.rfft(data[:, ch]))
        
        # Limitar ao range 0-50Hz
        mask = freqs <= max_freq
        freqs = freqs[mask]
        fft_vals = fft_vals[mask]
        
        plt.figure(figsize=(10, 4))
        plt.plot(freqs, fft_vals, label=f'Channel {ch+1}')
        plt.title(f'EEG Signal Amplitude (Frequency Domain) - Channel {ch+1}')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Amplitude')
        plt.xlim(0, max_freq)  # Garantir limite de 50Hz
        plt.legend()
        plt.tight_layout()
        if save_dir:
            png_path = os.path.join(save_dir, f'freq_domain_{dt_str}_ch{ch+1}.png')
            plt.savefig(png_path)
        plt.close()

        # Plotly HTML também limitado
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=freqs, y=fft_vals, mode='lines', name=f'Channel {ch+1}'))
        fig.update_layout(title=f'EEG Signal Amplitude (Frequency Domain) - Channel {ch+1}',
                          xaxis_title='Frequency (Hz)', yaxis_title='Amplitude',
                          xaxis_range=[0, max_freq])  # Limitar eixo X
        if save_dir:
            html_path = os.path.join(save_dir, f'freq_domain_{dt_str}_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def plot_spectrogram(data, fs=256, save_dir=None, dt_str=""):
    from scipy.signal import spectrogram
    for ch in range(data.shape[1]):
        f, t, Sxx = spectrogram(data[:, ch], fs=fs, nperseg=256, noverlap=128)
        plt.figure(figsize=(10, 4))
        plt.pcolormesh(t, f, 10 * np.log10(Sxx), shading='gouraud', cmap='viridis')
        plt.title(f'Channel {ch+1} Spectrogram')
        plt.xlabel('Time (s)')
        plt.ylabel('Frequency (Hz)')
        plt.colorbar(label='Power/Frequency (dB/Hz)')
        plt.tight_layout()
        if save_dir:
            png_path = os.path.join(save_dir, f'spectrogram_{dt_str}_ch{ch+1}.png')
            plt.savefig(png_path)
        plt.close()

        # Plotly HTML
        fig = go.Figure(data=go.Heatmap(
            z=10 * np.log10(Sxx + 1e-12), x=t, y=f, colorscale='Viridis'))
        fig.update_layout(title=f'Channel {ch+1} Spectrogram',
                          xaxis_title='Time (s)', yaxis_title='Frequency (Hz)')
        if save_dir:
            html_path = os.path.join(save_dir, f'spectrogram_{dt_str}_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def highpass_filter(data, fs, cutoff=1.0, order=4):
    nyq = 1 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='high', analog=False)
    return filtfilt(b, a, data, axis=0)

def print_top_frequencies(data, fs=256, top_n=5):
    for ch in range(data.shape[1]):
        n = data.shape[0]
        freqs = np.fft.rfftfreq(n, d=1/fs)
        fft_vals = np.abs(np.fft.rfft(data[:, ch]))
        # Ignore the DC component (0 Hz) if desired
        fft_vals[0] = 0
        # Get indices of top N peaks
        top_indices = np.argsort(fft_vals)[-top_n:][::-1]
        print(f"Channel {ch+1} top {top_n} frequencies (Hz):")
        for idx in top_indices:
            print(f"  {freqs[idx]:.2f} Hz (amplitude: {fft_vals[idx]:.2f})")
        print()


def lowpass_filter(data, fs, cutoff=45.0, order=4):
    """
    Filtro passa-baixa Butterworth.
    
    data: numpy array (amostras x canais)
    fs: taxa de amostragem (Hz)
    cutoff: frequência de corte (Hz)
    order: ordem do filtro
    """
    nyq = 0.5 * fs  # frequência de Nyquist
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return filtfilt(b, a, data, axis=0)

def main():
    data = read_eeg_data(data_file)
    timestamps = data[:, 0]
    eeg_channels = data[:, 1:]
    print(f"Data shape: {eeg_channels.shape} (samples, channels)")

    import re
    match = re.search(r'eeg_data_(\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2})', os.path.basename(data_file))
    dt_str = match.group(1) if match else "unknown"

    # === Aplica o filtro passa-baixa em 45 Hz ===
    fs = 4000  # taxa de amostragem
    eeg_filtered = lowpass_filter(eeg_channels, fs, cutoff=100, order=4)

    save_dir = os.path.join(os.path.dirname(__file__), 'images')
    os.makedirs(save_dir, exist_ok=True)

    # Plots com os novos parâmetros
    plot_time_domain(eeg_filtered, timestamps=timestamps, fs=fs, 
                    save_dir=save_dir, dt_str=dt_str)
    
    eeg_filtered_zero_mean = eeg_filtered - np.mean(eeg_filtered, axis=0)
    
    # FFT limitada a 50Hz
    plot_frequency_domain(eeg_filtered_zero_mean, fs, 
                         save_dir=save_dir, dt_str=dt_str, max_freq=80)
    
    plot_spectrogram(eeg_filtered_zero_mean, fs, save_dir=save_dir, dt_str=dt_str)
    print_top_frequencies(eeg_filtered_zero_mean, fs)


if __name__ == '__main__':
    main()
