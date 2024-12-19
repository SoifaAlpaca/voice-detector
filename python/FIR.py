import numpy as np
from Funcs import *
import scipy.signal as sg
import matplotlib.pyplot as plt
from sympy import *
import math
# -3dB cutoff at 250Hz
# $40 dB attenuation at 300Hz
# linear phase
# sampling rate = 10kHz

# Function to plot the frequency response of a filter
k = 1000
Blue = (0, 101/255, 189/255)
Green = (52/255, 129/255,  65/255)
Orange = (241/255,  89/255,  41/255)


def myfreqz(filters, w1, w2):
    plt.figure(figsize=(10, 8))

    for label, b in filters.items():
        w, h = sg.freqz(b)
        h_dB = 20 * np.log10(abs(h))

        # Magnitude plot
        plt.subplot(311)
        plt.plot(w / max(w), h_dB, label=label)
        plt.ylim(-100, 5)
        plt.xlim(0, 0.5)
        plt.ylabel('Magnitude (dB)')
        plt.xlabel('Normalized Frequency ($\\pi$ rad/sample)')
        plt.title('Frequency response')
        plt.legend()
        #plt.axvline(w1, color=Blue, linestyle='--')
        #plt.axvline(w2, color=Green, linestyle='--')
        plt.axhline(Amin, color=Orange, linestyle='--')

        # Phase plot
        plt.subplot(312)
        h_Phase = np.unwrap(np.arctan2(np.imag(h), np.real(h)))
        plt.plot(w / max(w), h_Phase, label=label)
        plt.xlim(0, 0.5)
        plt.ylabel('Phase (radians)')
        plt.xlabel('Normalized Frequency ($\\pi$ rad/sample)')
        plt.title('Phase response')
        plt.legend()
        #plt.axvline(w1, color=Blue, linestyle='--')
        #plt.axvline(w2, color=Green, linestyle='--')

        # Group delay plot
        plt.subplot(313)
        w_gd, gd = sg.group_delay((b, [1.0]))
        plt.plot(w_gd / max(w_gd), gd, label=label)
        plt.xlim(0, 0.5)
        plt.ylabel('Group delay (samples)')
        plt.xlabel('Normalized Frequency ($\\pi$ rad/sample)')
        plt.title('Group delay')
        plt.legend()
        #plt.axvline(w1, color=Blue, linestyle='--')
        #plt.axvline(w2, color=Green, linestyle='--')

    plt.subplots_adjust(hspace=0.5)
    plt.show()


def FreqResponse(FilterWindows):

    FilterWindows = np.pad(
        FilterWindows, (len(FilterWindows), 1024), 'constant', constant_values=0)

    Fr = np.fft.fft(FilterWindows)  # Signal Frequency
    frequencies = np.fft.fftfreq(len(FilterWindows), d=Ts)

    FpInd = np.where(frequencies >= 0)
    frequencies = frequencies[FpInd]  # Discard Negative Freqs
    Fr = Fr[FpInd]

    magnitude = 20 * np.log10(np.abs(Fr))
    phase = np.angle(Fr, deg=True)

    # Magnitude response
    plt.subplot(2, 1, 1)
    plt.semilogx(frequencies, magnitude)
    plt.title('Bode Magnitude Plot ')
    plt.ylabel('Magnitude (dB)')
    plt.grid(True)
    # plt.legend()

    # Bode Phase response
    plt.subplot(2, 1, 2)
    plt.semilogx(frequencies, phase)
    plt.xlabel('Frequency (rad/s)')
    plt.ylabel('Phase (degrees)')
    plt.grid(True)
    # plt.legend()
    plt.tight_layout()
    plt.show()


def PlotWindow(FilterWindow):

    TimeArray = [n*Ts for n in range(len(FilterWindow))]
    plt.plot(TimeArray, FilterWindow, 'o', markersize=2, color=Blue)
    plt.vlines(TimeArray, ymin=0, ymax=FilterWindow,
               color=Blue, linestyle='-', linewidth=0.5)
    plt.title("FIR Window")
    # plt.ylabel( "DUNNO" )
    plt.xlabel("Time(s)")
    plt.grid(True)
    plt.tight_layout()
    plt.show()


# Filter parameters                   # Filter order
cutoff_freq = [400 * 2 * np.pi, 4*k * 2 * np.pi]     # [wp1, wp2]Cutoff frequency (in rad/s)
sampling_rate = 10*k
stopband_frequency = [0.1 * 2 * np.pi, 5*k * 2 * np.pi]     #[ws1, ws2]
Amin = -40  # Minimum attenuation in dB the filter windows with this requirement are hamming, hann and blackman

Bt = min((cutoff_freq[0] - stopband_frequency[0]), (stopband_frequency[1] - cutoff_freq[1] ))

print(Bt)
# Pre-warp the cutoff frequency
wc = cutoff_freq
ws = stopband_frequency
Fs = sampling_rate
Ts = 1/Fs
# normalized frequencies
w1 = np.array(cutoff_freq)/(Fs*2*np.pi)
w2 = np.array(stopband_frequency)/(Fs*2*np.pi)
#w1 = wc/(Fs*2*np.pi)  # is the normalized cutoff frequency
#w2 = ws/(Fs*2*np.pi)  # is the normalized stopband frequency

# Filter order
M = (8 * np.pi) / (Bt/(Fs*2*np.pi))
print(M)
# M is equal to the first odd integer greater than 8*pi/(w2-w1) odd symmetry -> M is odd -> linear phase
M = math.ceil(M)
print(M)
if M % 2 != 0:
    M += 1

# Design filters with different windows
windows = ['hann', 'hamming', 'blackman']
filters = {window: sg.firwin(
    M, cutoff=w1, window=window, pass_zero='bandpass', scale=True) for window in windows}
print("M  = "+str(M))
# Plot frequency responses for each filter
myfreqz(filters, w1, w2)

print('w1 =', w1)
print('w2 =', w2)

# print filters
"""
for window in windows:
    coefficients = ", ".join(map(str, filters[window]))
    print(f"{window}\n{coefficients}")
"""
PlotWindow(filters["hamming"])
FreqResponse(filters["hamming"])

GenFIRCppHeader(filters['blackman'],path='FIR.h')

# GenHeader(filters["hamming"], path="src/FIR.h")
