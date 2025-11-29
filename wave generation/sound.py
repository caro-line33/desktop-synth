import serial
import sounddevice as sd
import numpy as np

fs = 44100 #Sampling Rate
frequencies = [0] * 2
print(frequencies)

PORT = 'COM5'
BAUD = 38400

ser = serial.Serial(PORT, BAUD, timeout = 0.1)
print("listening for press....")

stream = sd.OutputStream(samplerate = fs, channels = 1)
stream.start()

button_pressed = [False]



def callback(outdata, frames, time, status):
    t = (np.arange(frames) + callback.sample_index) / fs
    if button_pressed[0] == True:
        waveform = 0
        for frequency in frequencies:
            waveform += 0.5 * np.sin(2 * np.pi * float(frequency) * t) 
        outdata[:] = waveform.reshape(-1, 1)
    else:
        outdata[:] = np.zeros((frames, 1))
    callback.sample_index += frames

callback.sample_index = 0
stream = sd.OutputStream(channels=1, samplerate = fs, blocksize=256, callback=callback)
stream.start()
    


while True:
    try:
        line = ser.readline().decode().split(' ')
        print(len(line))
        i = 0
        if len(line) <= 1:
            button_pressed[0] = False
            frequencies = [0] * 2
        else:
            button_pressed[0] = True
            for i in range(len(line) - 1):
                 print(line[i])
                 frequencies[i] = line[i]

        print(frequencies, button_pressed)


    except:
        pass
