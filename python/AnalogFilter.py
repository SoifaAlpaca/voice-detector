import scipy as sp
import scipy.signal as sig
import numpy as np
from sympy import *
import matplotlib.pyplot as plt


num_eli , den_eli = sig.ellip(6, 1, 40, (200, 4000), 'bandpass', True, 'ba')
num_che, den_che = sig.cheby1(6, 1, (200, 4000), 'bandpass', analog = True, output='ba')
num_but, den_but = sig.butter(6, (200, 4000), 'bandpass', analog = True, output='ba')
num_bes, den_bes = sig.bessel(6, (200, 4000), 'bandpass', analog = True, output='ba')

print(num_eli)
print(den_eli)
print(num_che)
print(den_che)

fmin = 0 # 10^-1 = 0.1
fmax = 6
ppd  = 1000 #Points Per Decade
frequencies = np.logspace(fmin, fmax, num=ppd)

w_ellip , h_ellip   = sig.freqs(num_eli , den_eli , worN=frequencies)
w_cheb , h_cheb = sig.freqs(num_che, den_che, worN = frequencies)
w_but, h_but = sig.freqs(num_but, den_but, worN=frequencies)
w_bes, h_bes = sig.freqs(num_bes, den_bes, worN=frequencies)
plt.figure()
plt.grid(True)
plt.semilogx(w_cheb , 20 * np.log10(abs(h_cheb)) , color='red', label='Chebyshev')
plt.semilogx(w_ellip , 20 * np.log10(abs(h_ellip)) , color='black', label='Elliptical')
plt.semilogx(w_but , 20 * np.log10(abs(h_but)) , color='blue', label='Butterworth')
plt.semilogx(w_bes , 20 * np.log10(abs(h_bes)) , color='green', label='Bessel')
plt.title('Bode Magnitude Plot ( 6th Order)')
plt.ylabel('Magnitude (dB)')
plt.xlabel('Frequency (Hz)')
plt.legend()
plt.show()


num, den = sig.cheby1(6, 1, (350, 3600), 'bandpass', analog = True, output='ba')
w, h = sig.freqs(num , den , worN=frequencies)

plt.figure()
plt.subplot(2, 1, 1)
plt.grid(True)
plt.semilogx(w , 20 * np.log10(abs(h)), label='Chebyshev')
plt.title('Bode Diagram Plot')
plt.ylabel('Magnitude (dB)')
plt.grid(True)
plt.legend()

plt.subplot(2, 1, 2)
plt.semilogx(w  , np.angle(h  , deg=True),  label='Chebyshev')
plt.xlabel('Frequency (rad/s)')
plt.ylabel('Phase (degrees)')
plt.grid(True)
plt.legend()

plt.tight_layout()
plt.show()

s    = symbols("s")
zn   = len(num) - 1
pn   = len(den) - 1
precision = 2
naux = sum([ num[j]*s**(zn-j) for j in range(zn+1)]).evalf(precision)
daux = sum([ den[j]*s**(pn-j) for j in range(pn+1)]).evalf(precision)

print(latex(naux/daux))