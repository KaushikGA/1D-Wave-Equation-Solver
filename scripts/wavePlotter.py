import csv, time, shutil
import glob, os, os.path
import numpy as np
import matplotlib.pyplot as plt

os.chdir(r'C:\Temp\data')
with open('simulation_Parameters.txt') as f:
    lines = f.readlines()
    L=float(lines[0].split()[1])
    waveLegth=float(lines[1].split()[1])
    waveSpeed=float(lines[2].split()[1])
    CFL=float(lines[3].split()[1])
    T=float(lines[4].split()[1])
dx=waveLegth*0.01
dt = CFL * dx / waveSpeed;
discretization=np.arange(0, L, dx)

for timestepIndex in range(int(T/dt)):
    fileName='time_' + str(timestepIndex)
    fileTitle='time = ' + str(float(timestepIndex*dt)) ;
    fileNameWithExt=fileName+'.csv'
    imgNameWithExt=fileName+'.png'
    filePath = os.path.join(os.getcwd(),'CSV_data', fileNameWithExt)
    with open(filePath, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            dataArr=np.zeros(len(row)) 
            for val in range(len(row)-1):
                if ( len(row[val+1])):
                 dataArr[val]=float(row[val+1])
    plt.figure().set_figheight(5)
    plt.plot(discretization,dataArr,'-k', label='Wave', linewidth=2 )
    plt.ylim([-1, 2])
    plt.axvline(x = 0, color = 'r', ls='--', label = 'Periodic boundary')
    plt.axvline(x = L , color = 'r', ls='--')
    plt.legend(loc='upper center')
    plt.title(fileTitle)
    plt.savefig( os.path.join(os.getcwd(), 'Images' , imgNameWithExt ) ,dpi=300)
    plt.clf()
    plt.close()
