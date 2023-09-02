import numpy as np
import sys
import matplotlib.pyplot as plt
from scipy import signal


class PID_float:
    def __init__(self, b_params, a_params):
        self.b_pid = b_params
        self.a_pid = a_params
        self.resetFilter()

        
    def changeParams(self, b_params, a_params):
        self.b_pid = b_params
        self.a_pid = a_params

        
    def resetFilter(self):
        self.error_z2 = 0.0
        self.error_z1 = 0.0
        self.new_output_z1 = 0.0


    def newOutput(self, new_input):
        error = new_input
        new_output = self.b_pid[0] * error \
                     + self.b_pid[1] * self.error_z1 \
                     + self.b_pid[2] * self.error_z2 \
                     - self.a_pid[1] * self.new_output_z1

        self.new_output_z1 = new_output
        self.error_z2 = self.error_z1
        self.error_z1 = error

        return new_output


class PID_int:
    def __init__(self, b_params, a_params, divider):
        self.b_pid = np.array(b_params, dtype=np.int16)
        self.a_pid = np.array(a_params, dtype=np.int16)
        self.div = np.int16(divider)
        print(f'b_pid: {self.b_pid} dtype: {self.b_pid.dtype}')
        print(f'a_pid: {self.a_pid} dtype: {self.a_pid.dtype}')
        print(f'divider: {self.div} dtype: {self.div.dtype}')        
        self.resetFilter()

        
    def changeParams(self, b_params, a_params):
        self.b_pid = np.array(b_params, dtype=np.int16)
        self.a_pid = np.array(a_params, dtype=np.int16)
        print(f'params to b_pid: {self.b_pid} dtype: {self.b_pid.dtype}')
        print(f'params to a_pid: {self.a_pid} dtype: {self.a_pid.dtype}')

        
    def resetFilter(self):
        self.error_z2 = np.int16(0)
        self.error_z1 = np.int16(0)
        self.new_output_z1 = np.int16(0)


    def newOutput(self, new_input):
        error = np.int32(new_input)
        new_output = np.int32(0)
        
        new_output = self.b_pid[0] * error / self.div
        new_output += self.b_pid[1] * self.error_z1 / self.div
        new_output += self.b_pid[2] * self.error_z2 / self.div
        new_output -= self.a_pid[1] * self.new_output_z1

        self.new_output_z1 = np.int16(new_output)
        self.error_z2 = self.error_z1
        self.error_z1 = error

        return np.int16(new_output)

    
    def windowingLastOutput(self, last_output):
        self.new_output_z1 = np.int16(last_output)
    
    
    def showParamsFromK (self, kp, ki, kd, Fsampling):
        f0 = ki * Fsampling / (kp * 6.28)
        if kd != 0:
            f1 = (kp * 10) / (kd * Fsampling * 6.28)
        else:
            f1 = 'none'

        gain = kp / self.div
        print(f"f0 = {f0} f1 = {f1} plateu gain = {gain}")

        
    def changeParamsFromK (self, kp, ki, kd):
        k1 = kp + ki + kd
        k2 = -kp - 2*kd
        k3 = kd
        b_pid_int = [k1, k2, k3]
        a_pid_int = [1, -1]
        self.changeParams(b_pid_int, a_pid_int)


def phelp ():
    print('use six arguments: f/i kp ki kd div fs')
    print('    f/i float or integer pid')
    print('    kp ki kd float or integer')
    print('    div divider on integer pid, else use 1')
    print('    fs sampling frequency')

    
if __name__ == "__main__":
    if len(sys.argv) != 7:
        phelp()
        
    else:
        if sys.argv[1] == 'f':
            print('not implemented yet!')
        elif sys.argv[1] == 'i':
            pid = PID_int(1,1,int(sys.argv[5]))
            kp = int(sys.argv[2])
            ki = int(sys.argv[3])
            kd = int(sys.argv[4])
            fs = int(sys.argv[6])
            
            pid.changeParamsFromK(kp, ki, kd)
            pid.showParamsFromK(kp, ki, kd, fs)

            b_float = pid.b_pid / pid.div
            a_float = [1., -1.]
            print(b_float)
            print(a_float)
            w, h = signal.freqz(b_float, a_float, 2*fs)
            # w, h = signal.freqz(b_float, a_float)            
            fig = plt.figure(1)
            fig.clf()
            plt.title('Digital filter frequency response')

            # plt.semilogx(w, 20 * np.log10(abs(h)), 'b')
            # plt.xlabel('Frequency rad/sample')
            plt.semilogx(w*fs/np.pi, 20 * np.log10(abs(h)), 'b')
            plt.xlabel('Frequency Hz')
            plt.ylabel('Amplitude [dB]', color='b')

            plt.grid()
            plt.show()
            plt.draw()
            
        else:
            phelp()
            
