import numpy as np
import matplotlib.pyplot as plt
import os
from scipy.signal import butter, filtfilt
import plotly.graph_objs as go
import plotly.io as pio


# Path to the EEG data file
data_file = os.path.join(os.path.dirname(__file__), '../eeg_data/eeg_data_2025-09-08_21-25-47.txt')

# Read EEG data (assuming tab or comma separated, channels in columns)
def read_eeg_data(filepath):
    with open(filepath, 'r') as f:
        # Try to auto-detect delimiter
        first_line = f.readline()
        delimiter = '\t' if '\t' in first_line else ','
        f.seek(0)
        data = np.loadtxt(f, delimiter=delimiter, skiprows=1)
    return data

def plot_time_domain(data, fs=256, save_dir=None):
    for ch in range(data.shape[1]):
        t = np.arange(data.shape[0]) / fs
        plt.figure(figsize=(10, 4))
        plt.plot(t, data[:, ch], label=f'Channel {ch+1}')
        plt.title(f'EEG Signal Amplitude (Time Domain) - Channel {ch+1}')
        plt.xlabel('Time (s)')
        plt.ylabel('Amplitude')
        plt.legend()
        plt.tight_layout()
        if save_dir:
            png_path = os.path.join(save_dir, f'time_domain_ch{ch+1}.png')
            plt.savefig(png_path)
        plt.close()

        # Plotly HTML
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=t, y=data[:, ch], mode='lines', name=f'Channel {ch+1}'))
        fig.update_layout(title=f'EEG Signal Amplitude (Time Domain) - Channel {ch+1}',
                          xaxis_title='Time (s)', yaxis_title='Amplitude')
        if save_dir:
            html_path = os.path.join(save_dir, f'time_domain_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def plot_frequency_domain(data, fs=256, save_dir=None):
    for ch in range(data.shape[1]):
        n = data.shape[0]
        freqs = np.fft.rfftfreq(n, d=1/fs)
        fft_vals = np.abs(np.fft.rfft(data[:, ch]))
        plt.figure(figsize=(10, 4))
        plt.plot(freqs, fft_vals, label=f'Channel {ch+1}')
        plt.title(f'EEG Signal Amplitude (Frequency Domain) - Channel {ch+1}')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Amplitude')
        plt.legend()
        plt.tight_layout()
        if save_dir:
            png_path = os.path.join(save_dir, f'freq_domain_ch{ch+1}.png')
            plt.savefig(png_path)
        plt.close()

        # Plotly HTML
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=freqs, y=fft_vals, mode='lines', name=f'Channel {ch+1}'))
        fig.update_layout(title=f'EEG Signal Amplitude (Frequency Domain) - Channel {ch+1}',
                          xaxis_title='Frequency (Hz)', yaxis_title='Amplitude')
        if save_dir:
            html_path = os.path.join(save_dir, f'freq_domain_ch{ch+1}.html')
            pio.write_html(fig, html_path)

def plot_spectrogram(data, fs=256, save_dir=None):
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
            png_path = os.path.join(save_dir, f'spectrogram_ch{ch+1}.png')
            plt.savefig(png_path)
        plt.close()

        # Plotly HTML
        fig = go.Figure(data=go.Heatmap(
            z=10 * np.log10(Sxx + 1e-12), x=t, y=f, colorscale='Viridis'))
        fig.update_layout(title=f'Channel {ch+1} Spectrogram',
                          xaxis_title='Time (s)', yaxis_title='Frequency (Hz)')
        if save_dir:
            html_path = os.path.join(save_dir, f'spectrogram_ch{ch+1}.html')
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

def main():
    data = read_eeg_data(data_file)
    fs = 256  # Change this if your sampling rate is different

    # Exclude the first column (timestamp), keep only EEG channels
    eeg_channels = data[:, 1:]

    # Ignore early acquisitions: skip the first N samples (e.g., first 10 seconds)
    skip_seconds = 10
    skip_samples = int(skip_seconds * fs)
    if eeg_channels.shape[0] > skip_samples:
        eeg_channels = eeg_channels[skip_samples:]
        print(f"Skipped first {skip_samples} samples ({skip_seconds} seconds).")
    else:
        print("Warning: Not enough samples to skip early acquisitions.")
    print(f"Data shape: {eeg_channels.shape} (samples, channels)")

    save_dir = os.path.join(os.path.dirname(__file__), 'images')
    os.makedirs(save_dir, exist_ok=True)

    plot_time_domain(eeg_channels, fs, save_dir=save_dir)
    eeg_channels_zero_mean = eeg_channels - np.mean(eeg_channels, axis=0)
    plot_frequency_domain(eeg_channels_zero_mean, fs, save_dir=save_dir)
    print_top_frequencies(eeg_channels_zero_mean, fs)

if __name__ == '__main__':
    main()