import Launch_simu as ls
import numpy as np
import matplotlib.pyplot as plt
import os



###########################################
maxPktPerHour  = 400
stepPktPerHour = 5
Nnodes = np.array([250])
simulationTime = 3600
periodSenderUnique = 100
runs = 50

pktPerHour   = np.arange(1, maxPktPerHour, step= stepPktPerHour, dtype='float')
periodSender = 3600/pktPerHour
params = {
    'nDevices': [250],
    'simulationTime': [simulationTime],#♣[round(i, 4) for i in periodSender],
    'radius': [6250],
    'appPeriod': [periodSender], #lambda p: ["aloha"] if p["radius"] == 0 else ["goursaud"],
    'logProfile': [1],
    'packetSize': [50],
    'maxReceptionPaths': [50],
}

Result_FileName = 'Test'
script = ['LoRaTest','aloha-throughput']
###########################################

get_AllRun=True

if __name__ == '__main__':
    ls.launch_simu(script[0], 
                params, 
                Result_FileName, 
                15,
                runs=runs, 
                optimized= True,
                show_progess=True,
                get_AllRun=get_AllRun,
                max_processes=8
                )
