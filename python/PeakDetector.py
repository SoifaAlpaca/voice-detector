import scipy as sp 
import scipy.signal as sig
import numpy as np
from sympy import *
import matplotlib.pyplot as plt 
from Funcs import *

VoltPerdB = lambda x : (10**((x-42)/20) * 20e-6/sqrt(2) )* 1000   #miliVolt

print(VoltPerdB(60).evalf())
print(VoltPerdB(70).evalf())

val_arr   = np.arange(10,100,0.5)

plt.figure(figsize=FigDim)
plt.plot(val_arr, VoltPerdB(val_arr), color=Blue)
plt.title("Volts per Sound Volume")
plt.ylabel( "(mV)" )
plt.xlabel("Sound Volume (dB)")
plt.grid(True)
plt.tight_layout()
plt.show()
