import pandas as pd
import scipy as sp 
import scipy.signal as sig
import numpy as np
import matplotlib.pyplot as plt 
from sympy import *
from sympy import symbols, simplify, lambdify, I 

k       = 1000
Blue    = (       0, 101/255, 189/255 )
Green   = (  52/255, 129/255,  65/255 )
Orange  = ( 241/255,  89/255,  41/255 )

def PrintTransferFn(num,den,precision = 2):

    s    = symbols("s")
    zn   = len(num) - 1
    pn   = len(den) - 1
    naux = sum([ num[j]*s**(zn-j) for j in range(zn+1)]).evalf(precision)
    daux = sum([ den[j]*s**(pn-j) for j in range(pn+1)]).evalf(precision)

    print(latex(naux/daux))

def calculate_margins(w, h):
    # Convert the magnitude and phase response to dB and degrees
    mag = 20 * np.log10(np.abs(h))
    phase = np.angle(h, deg=True)

    # Find gain crossover frequency (magnitude = 0 dB)
    gain_crossover_idx = np.where(np.isclose(mag, 0, atol=0.05))[0]
    if gain_crossover_idx.size > 0:
        gain_crossover_freq = w[gain_crossover_idx[0]]
        phase_at_gain_crossover = phase[gain_crossover_idx[0]]
        phase_margin = 180 + phase_at_gain_crossover
    else:
        gain_crossover_freq = None
        phase_margin = None

    # Find phase crossover frequency (phase = -180 degrees)
    phase_crossover_idx = np.where(np.isclose(phase, -180, atol=1))[0]
    if phase_crossover_idx.size > 0:
        phase_crossover_freq = w[phase_crossover_idx[0]]
        gain_at_phase_crossover = mag[phase_crossover_idx[0]]
        gain_margin = -gain_at_phase_crossover
    else:
        phase_crossover_freq = None
        gain_margin = None

    return gain_margin, phase_margin

def plotScoppyBode(path):
    
    df   = pd.read_csv(
        path,
        delimiter=',',
        #on_bad_lines='skip',
         low_memory=False
        )

    #print(df.head())

    plt.figure(figsize=(10, 8))
    plt.subplot(2, 1, 1)
    plt.semilogx(df['Frequency(Hz)'], df['Magnitude(dB)'], color=Blue)
    plt.title('Magnitude vs Frequency')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude (dB)')
    plt.grid(True)

    plt.subplot(2, 1, 2)
    plt.semilogx(df['Frequency(Hz)'], df['Phase(°)'], color=Blue)
    plt.title('Phase vs Frequency')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Phase (°)')
    plt.grid(True)

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

def prewrap(wp, Ts):
    return (2/Ts)*np.tan(wp*Ts/2)

def GenFIRCppHeader(coefs, path=''):
    out = ""
    out += "#ifndef FIR_H\n"
    out += "#define FIR_H\n"
    out += "\n"
    out += "#include <vector>\n"
    out += "using namespace std;\n\n"
    out += "const std::vector<double> coefs{\n    "
    for c in coefs:
        out += str(c) + ", "
    out = out[:-2]
    out += "\n};\n\n#endif"

    if len(path) > 1:
        with open(path, "w") as file:
            file.write(out)
    else:
        print(out)

#TODO ver se funciona
def PlotBode(num,den,labels=[],fmin = -1,fmax = 4,ppd = 1000,title = "Bode Magnitude Plot"):

    if( len(num) != len(den) ):
        print("len(num) != len(den)")
        return -1

    if( len(labels) != len(num) and len(labels) != 0):
        print("Different number of labels and bodes")
        return -1

    if( len(num) > 4 ):
        print("Too many bodes max = 4")
        return -1
    
    if( len(labels) ):
        labels = [0 for _ in range(4) ]
    
    frequencies = np.logspace(fmin, fmax, num=ppd)
    colours = [ Blue,Orange,Green,'black' ]
    # Compute frequency response for each filter
    for i in range(len(num)):
        
        plt.subplot(2, 1, 1)
        waux, haux = sig.freqs( num[i], den[i], worN=frequencies )
        plt.semilogx(waux, 20 * np.log10(abs(haux)), color=colours[i] , label=label[i])
        plt.title(title)
        plt.ylabel('Magnitude (dB)')
        plt.grid(True)
        plt.legend()

        # Bode Phase response
        plt.subplot(2, 1, 2)
        plt.semilogx(waux, np.angle(haux, deg=True), color=colours[i]   , label=labels[i])
        plt.xlabel('Frequency (rad/s)')
        plt.ylabel('Phase (degrees)')
        plt.grid(True)
        plt.legend()

    plt.tight_layout()
    plt.show()

plotScoppyBode("csv/emsfinalanalogbode.csv")