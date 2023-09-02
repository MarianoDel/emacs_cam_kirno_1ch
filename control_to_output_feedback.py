# -*- coding: utf-8 -*-
#usar python3
import numpy as np
from sympy import *
import matplotlib.pyplot as plt
from scipy.signal import lti, bode, lsim, dbode, zpk2tf, tf2zpk, step2, cont2discrete, dstep, freqz, freqs, dlti, TransferFunction
from tc_udemm import sympy_to_lti, lti_to_sympy, plot_argand
from pid_tf import PID_int
from recursive_tf import RecursiveTF

"""
        Magnet power output on channels from 1 to 4
        Control to Output Plant Analysis 

        Using Reduce and Complete Model

"""

############################################
# Which graph to show... for speed reasons #
############################################
Bode_Plant_Analog = False
Bode_Plant_Digital = False
Poles_Zeros_RM_Digital = False
Poles_Zeros_CM_Digital = False

Step_Plant_Digital = False
Step_Plant_Recursive_Digital = False

## for recursive system
Show_System_Inputs = False
PID_Single_Stage = True
Respuesta_CloseLoop_All_Inputs_Digital = True

def Adc12Bits (sample):
    adc = np.int16(0)
    sample = sample / 3.3
    sample = sample * 4095
    if sample < 0.0:
        sample = 0

    if sample > 4095:
        sample = 4095

    adc = sample
    return np.int16(adc)


def CurrentToVoltageSense (current):
    Rsense = 0.165
    Amp_gain = 3.2
    sense = current * Rsense * Amp_gain
    return sense


def VoltageSenseToCurrent (vsense):
    Rsense = 0.165
    Amp_gain = 3.2
    current = vsense / (Amp_gain * Rsense)
    return current


#TF without constant
s = Symbol('s')

# Typical Magnet Antenna
Rantenna = 30.3
Lantenna = 230e-3

# Sense filters
Rsense = 0.165
R1 = 180
C1 = 1e-9
R2 = 1800
C2 = 56e-9

# OPAMP gain
Amp_gain = 3.2

# VCC for low-signal graphics
# Vinput = 1
Vinput = 192

# reduced model
#    ---L---+
#           |
#           R
#           |
#           +---+
#           |   +--
#         Rsense   x13
#           |   +--
#    -------+---+

Zant = s * Lantenna + Rantenna
# Thevenin on Rsense
Zth1 = Rsense * Zant / (Rsense + Zant)
Vth1 = Vinput * Rsense / (Rsense + Zant)
TF_rm = Vth1 * Amp_gain

# complete model
#    ---L---+
#           |
#           R
#           |
#           +---R1---+---R2---+---+
#           |        |        |   +--
#           Rsense   C1       C2     x13
#           |        |        |   +--
#    -------+--------+--------+---+

# Thevenin on C1
ZC1 = 1 / (s * C1)
Zth2 = (Zth1 + R1) * ZC1 / (Zth1 + R1 + ZC1)
Vth2 = Vth1 * ZC1 / (ZC1 + Zth1 + R1)

# Thevenin on C2
ZC2 = 1 / (s * C2)
Zth3 = (Zth2 + R2) * ZC2 / (Zth2 + R2 + ZC2)
Vth3 = Vth2 * ZC2 / (ZC2 + Zth2 + R2)

TF_cm = Vth3 * Amp_gain


print (f'Voltage Sense with Vinput = {Vinput}V:')
print (' Reduce Model:')
Filter_out_rm_sim = TF_rm.simplify()
print (Filter_out_rm_sim)
print (' Complete Model:')
Filter_out_cm_sim = TF_cm.simplify()
print (Filter_out_cm_sim)



#######################################
# Confirm some things on analog parts #
#######################################
filter_rm_TF = sympy_to_lti(Filter_out_rm_sim)
filter_cm_TF = sympy_to_lti(Filter_out_cm_sim)
if Bode_Plant_Analog == True:
    wfreq = np.arange(2*np.pi, 2*np.pi*100000, 1)
    w, mag_r, phase_r = bode(filter_rm_TF, wfreq)
    w, mag_c, phase_c = bode(filter_cm_TF, wfreq)    

    fig, (ax1, ax2) = plt.subplots(2,1)
    ax1.semilogx (w/6.28, mag_r, 'b-', linewidth="1")
    ax1.semilogx (w/6.28, mag_c, 'y-', linewidth="1")
    ax1.set_title(f'Input to Output Analog Vinput = {Vinput}V')

    ax2.semilogx (w/6.28, phase_r, 'b-', linewidth="1")
    ax2.semilogx (w/6.28, phase_c, 'y-', linewidth="1")
    ax2.set_title('Phase')

    plt.tight_layout()
    plt.show()



# if Bode_Plant_Analog == True:
#     wfreq = np.arange(2*np.pi, 2*np.pi*100000, 1)
#     w, mag_p, phase_p = bode(filter_cm_TF, wfreq)    

#     fig, (ax1, ax2) = plt.subplots(2,1)
#     ax1.semilogx (w/6.28, mag_p, 'y-', linewidth="1")
#     ax1.set_title(f'Input to Output Complete Model Vinput = {Vinput}V')

#     ax2.semilogx (w/6.28, phase_p, 'y-', linewidth="1")
#     ax2.set_title('Phase')

#     plt.tight_layout()
#     plt.show()
    


##############################################
# Convert Plant at sampling frequency by zoh #
# to keep poles and zeroes                   #
##############################################
Fsampling = 7000
Tsampling = 1 / Fsampling

filter_rm_dig_zoh_n, filter_rm_dig_zoh_d, td = cont2discrete((filter_rm_TF.num, filter_rm_TF.den), Tsampling, method='zoh')
filter_cm_dig_zoh_n, filter_cm_dig_zoh_d, td = cont2discrete((filter_cm_TF.num, filter_cm_TF.den), Tsampling, method='zoh')

# normalized by TransferFunction
print ()
print ("Digital Filter Reduce Model Zoh:")
filter_rm_dig_zoh = TransferFunction(filter_rm_dig_zoh_n, filter_rm_dig_zoh_d, dt=td)
print (filter_rm_dig_zoh)

print ("Digital Filter Complete Model Zoh:")
filter_cm_dig_zoh = TransferFunction(filter_cm_dig_zoh_n, filter_cm_dig_zoh_d, dt=td)
print (filter_cm_dig_zoh)


########################################
# Confirm some things on digital parts #
########################################
if Bode_Plant_Digital == True:
    w, mag_r_zoh, phase_r_zoh = dbode(filter_rm_dig_zoh, n = 10000)
    w, mag_c_zoh, phase_c_zoh = dbode(filter_cm_dig_zoh, n = 10000)
    
    fig, (ax1, ax2) = plt.subplots(2,1)

    ax1.semilogx(w/(2*np.pi), mag_r_zoh, 'b')
    ax1.semilogx(w/(2*np.pi), mag_c_zoh, 'y')    
    ax1.set_title(f'Input to Output Digital ZOH Vinput = {Vinput}V')
    ax1.set_xlabel('Frequency [Hz]')

    ax2.semilogx(w/(2*np.pi), phase_r_zoh, 'b')
    ax2.semilogx(w/(2*np.pi), phase_c_zoh, 'y')    
    ax2.set_xlabel('Frequency [Hz]')

    plt.tight_layout()
    plt.show()

# if Bode_Plant_Digital == True:
#     w, mag_zoh, phase_zoh = dbode(filter_opamp_dig_zoh, n = 10000)

#     fig, (ax1, ax2) = plt.subplots(2,1)

#     ax1.semilogx(w/(2*np.pi), mag_zoh, 'y')
#     ax1.set_title(f'Filter and Opamp Digital Bode ZOH Vinput = {Vinput}V')
#     ax1.set_xlabel('Frequency [Hz]')

#     ax2.semilogx(w/(2*np.pi), phase_zoh, 'y')
#     ax2.set_xlabel('Frequency [Hz]')

#     plt.tight_layout()
#     plt.show()
    

    

######################################
# Polos y Ceros de la planta Digital #
######################################
if Poles_Zeros_RM_Digital == True:
    plot_argand(filter_rm_dig_zoh)

if Poles_Zeros_CM_Digital == True:
    plot_argand(filter_cm_dig_zoh)
    
    
##################################
# Step Response on Digital Plant #
##################################
simulation_time = 0.1
t = np.linspace(0, simulation_time, num=(simulation_time*Fsampling))

if Step_Plant_Digital == True:
    tout, yout_rm_zoh = dstep([filter_rm_dig_zoh.num, filter_rm_dig_zoh.den, td], t=t)
    tout, yout_cm_zoh = dstep([filter_cm_dig_zoh.num, filter_cm_dig_zoh.den, td], t=t)    
    yout1 = np.transpose(yout_rm_zoh)
    yout0 = yout1[0]
    yout_rm_zoh = yout0[:tout.size]

    yout1 = np.transpose(yout_cm_zoh)
    yout0 = yout1[0]
    yout_cm_zoh = yout0[:tout.size]
    
    fig, ax = plt.subplots()
    ax.set_title('Step Response RM [blue] CM [yellow] Digital ZOH')
    ax.set_ylabel('Voltage Output')
    ax.set_xlabel('Tiempo [s]')
    ax.grid()

    ax.plot(tout, yout_rm_zoh, 'b')
    ax.plot(tout, yout_cm_zoh, 'y')    
    # ax.set_ylim(ymin=-20, ymax=100)

    plt.tight_layout()
    plt.show()
    
    

######################################################
#  Convert to Recursive and check again the Response #
######################################################
if Step_Plant_Recursive_Digital == True:
    # ZOH
    b_plant = np.transpose(filter_rm_dig_zoh.num)
    a_plant = np.transpose(filter_rm_dig_zoh.den)

    vin_plant = np.ones(t.size)
    vout_plant_rm_method2 = np.zeros (t.size)
    vout_plant_adc = np.zeros (t.size)
    recur_plant = RecursiveTF(b_plant, a_plant)
    for i in range(t.size):
        vout_plant_rm_method2[i] = recur_plant.newOutput(vin_plant[i])
        vout_plant_adc[i] = Adc12Bits(vout_plant_rm_method2[i])

    b_plant = np.transpose(filter_cm_dig_zoh.num)
    a_plant = np.transpose(filter_cm_dig_zoh.den)

    vin_plant = np.ones(t.size)
    vout_plant_cm_method2 = np.zeros (t.size)    
    recur_plant = RecursiveTF(b_plant, a_plant)
    for i in range(t.size):
        vout_plant_cm_method2[i] = recur_plant.newOutput(vin_plant[i])
        

    fig, ax = plt.subplots()
    ax.set_title('Step Digital Plant Recursive ZOH RM blue CM yellow')
    ax.set_ylabel('Voltage Output')
    ax.set_xlabel('Tiempo [s]')
    ax.grid()
    ax.plot(t, vout_plant_rm_method2, 'b')
    ax.plot(t, vout_plant_cm_method2, 'y')
    # ax.plot(t, vout_plant_adc, 'm')
    plt.tight_layout()
    plt.show()
    

########################################
# Simulacion del sistema punto a punto #
########################################
tiempo_de_simulacion = 0.2
t = np.linspace(0, tiempo_de_simulacion, num=(tiempo_de_simulacion*Fsampling))

vsupply = np.ones(t.size) * 192    # not used, filter_rm_dig_zoh already has vsupply on it

## SQUARE SIGNAL
# fsignal = 60
# ref_peak = 915
# vref = np.zeros(t.size)
# for i in range(np.size(vref)):
#     vref[i] = int(ref_peak * np.sin(2 * np.pi * fsignal * tiempo_de_simulacion * (i/t.size)))

# for i in range(np.size(vref)):
#     if vref[i] > 0.0:
#         vref[i] = ref_peak
#     else:
#         vref[i] = 0.0

## TRIANGULAR SIGNAL
fsignal = 60
ref_peak = 915
vref = np.zeros(t.size)
chunks = int(Fsampling / (2 * fsignal))
how_many = len(vref) / (2 * chunks)

for j in range (int(how_many)):
    start = j * 2 * chunks
    print(start)
    step = 1
    for i in range(start, start + chunks, 1):
        vref[i] = int(step * ref_peak / chunks)
        step += 1


## SINUSOIDAL SIGNAL
# fsignal = 60
# ref_peak = 915
# vref = np.zeros(t.size)
# for i in range(np.size(vref)):
#     vref[i] = int(ref_peak * np.sin(2 * np.pi * fsignal * tiempo_de_simulacion * (i/t.size)))

# for i in range(np.size(vref)):
#     if vref[i] <= 0.0:
#         vref[i] = 0.0



vout_plant_adc = np.zeros(t.size)
error = np.zeros(t.size)
d = np.zeros(t.size)
vin_plant_d = np.zeros(t.size)
vout_plant = np.zeros(t.size)
mode = np.zeros(t.size)

if Show_System_Inputs == True:
    fig, ax = plt.subplots()
    ax.set_title('Vref')
    ax.set_xlabel('t [s]')
    ax.grid()
    ax.plot(t, vref, 'y')    
    plt.tight_layout()
    plt.show()


if PID_Single_Stage == True:
    b_plant = np.transpose(filter_rm_dig_zoh.num)
    a_plant = np.transpose(filter_rm_dig_zoh.den)

    vin_plant = np.ones(t.size)
    vout_plant = np.zeros (t.size)    
    recur_plant = RecursiveTF(b_plant, a_plant)
    # for i in range(t.size):
    #     vout_plant_rm_method2[i] = recur_plant.newOutput(vin_plant[i])
    
    max_pwm_pts = 950

    kp = 256
    ki = 6
    kd = 0
    pid = PID_int(1,1, 128)
    pid.changeParamsFromK(kp, ki, kd)
    pid.showParamsFromK(kp, ki, kd, Fsampling)

    for i in range(1, len(vsupply)):
        ###################################################
        # primero calculo el error, siempre punto a punto #
        ###################################################
        if vref[i] == 0:
            error[i] = 0
            d[i] = 0
            pid.resetFilter()
            vout_plant[i] = 0
            recur_plant.resetFilter()
            vout_plant_adc[i] = 0
            
        else:
            error[i] = vref[i] - vout_plant_adc[i-1]

            #############################
            # aplico Digital Controller #
            #############################
            d[i] = pid.newOutput(error[i])
            # print('error: ' + str(error[i]) + ' d: ' + str(d[i]) + ' i: ' + str(i))

            #floor check
            if d[i] < 0:
                if d[i] < -max_pwm_pts:
                    d[i] = -max_pwm_pts

            #roof check
            if d[i] > max_pwm_pts:
                d[i] = max_pwm_pts
            
            vout_plant[i] = recur_plant.newOutput(d[i] / 1000.0)
            # vout_plant[i] = recur_plant.newOutput(500 / 1000.0)

            vout_plant_adc[i] = Adc12Bits (vout_plant[i])



if Respuesta_CloseLoop_All_Inputs_Digital == True:     
    fig, ax1 = plt.subplots()
    ax1.set_title(f'ref_peak={ref_peak}')
    ax1.grid()
    ax1.plot(t, vref, 'r', label="sp")
    ax1.plot(t, vout_plant_adc, 'y', label="out_adc")
    ax1.plot(t, error, 'g', label="error")
    ax1.plot(t, d, 'm', label="duty")
    ax1.legend(loc='upper left')

    # ax2.plot(t, vin_setpoint, 'y', label="sp")
    # ax.stem(t, vout_plant)
    # ax2.plot(t, vout_plant, 'y', label="out")
    # ax2.plot(t, vin_plant_d, 'm', label="in")

    # # ax.set_ylim(ymin=-10, ymax=360)
    # ax2.legend(loc='upper left')
    plt.tight_layout()
    plt.show()

# print (d)
# print (error)

current = 1.0
vsense = CurrentToVoltageSense(current)
adc_pts = Adc12Bits(vsense)
print('voltage for ' + str(current) + 'A -> ' + str(vsense) + 'V -> adc: ' + str(adc_pts))
vsense = 1.0
current = VoltageSenseToCurrent(vsense)
adc_pts = Adc12Bits(vsense)
print('current for ' + str(vsense) + 'V -> ' + str(current) + 'A -> adc: ' + str(adc_pts))

