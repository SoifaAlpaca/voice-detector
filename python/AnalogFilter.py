import scipy as sp 
import scipy.signal as sig
import numpy as np
from sympy import *
import matplotlib.pyplot as plt 




num , den = sig.ellip(4, 1, 40, (200, 3400), 'bandpass', True, 'ba')
num_che, den_che = sig.cheby1(6, 1, (200, 3400), 'bandpass', analog = True, output='ba')

print(num)
print(den)
print(num_che)
print(den_che)

fmin = 0 # 10^-1 = 0.1
fmax = 6
ppd  = 1000 #Points Per Decade
frequencies = np.logspace(fmin, fmax, num=ppd)

w_ellip , h_ellip   = sig.freqs(num , den , worN=frequencies)
w_cheb , h_cheb = sig.freqs(num_che, den_che, worN = frequencies)
plt.figure()
plt.grid(True)
plt.semilogx(w_cheb , 20 * np.log10(abs(h_cheb)) , color='red', label='Cheb')
plt.semilogx(w_ellip , 20 * np.log10(abs(h_ellip)) , color='black', label='Elliptical')
plt.show()

