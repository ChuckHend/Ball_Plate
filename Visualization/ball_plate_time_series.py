from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import pyqtgraph as pg
from pyqtgraph.ptime import time
import serial

## updated 10/19/2016

app = QtGui.QApplication([])

p = pg.plot(title="Ball and Plate: Live", labels= {'left': 'Position', 'bottom':'Time (milliseconds)'})

p.setYRange(0, 1000, padding = None)    # fix y axis range

l = pg.LegendItem((100,60), (60,10))        # args are (size, position)
l.setParentItem(p.graphicsItem())           # ??


spX = p.plot()              #  plot object for x setpoint
spX.setPen(pg.mkPen('r', width=3))   #  set line color red

posX = p.plot()             #  plot object for x position
posX.setPen(pg.mkPen('r', width=3,  style=QtCore.Qt.DotLine))  #  set line color green

spY = p.plot()              #  plot object for y setpoint
spY.setPen(pg.mkPen('w', width=3))   #  set line color white

posY = p.plot()             #  plot object for y position
posY.setPen(pg.mkPen('w', width=3, style=QtCore.Qt.DotLine))  #  set line color yellow

l.addItem(spX, 'X Set Point')
l.addItem(posX, 'X Position')
l.addItem(spY, 'Y Set Point')
l.addItem(posY, 'Y Position')

dataSPX = []                #  array for x set point
dataXPOS = []               #  array for x position
dataSPY = []                #  array for y set point
dataYPOS = []               #  array for y position
time = []                   #  array for x axis labels

raw=serial.Serial('COM5', 115200)      #  in stream from serial

def update():
    global data, spX, posX, spY, posY, xSPdata, xPOSdata, ySPdata, yPOSdata, time, timedata
    inData = raw.readline()
    data = inData.split(",")    #   split by comma, data from arduino is comma separated

    dataSPX.append(float(data[0]))
    dataXPOS.append(float(data[1]))
    dataSPY.append(float(data[2]))
    dataYPOS.append(float(data[3]))
    time.append(float(data[4]))               # keep time as a string to for x axis lables


    xSPdata = np.array(dataSPX, dtype='float64')
    xPOSdata = np.array(dataXPOS, dtype='float64')
    ySPdata = np.array(dataSPY, dtype='float64')
    yPOSdata = np.array(dataYPOS, dtype='float64')
    timeData = np.array(time, dtype='float64')


    spX.setData(timeData,xSPdata)
    posX.setData(timeData,xPOSdata)
    spY.setData(timeData,ySPdata)
    posY.setData(timeData,yPOSdata)

    app.processEvents()

    if len(xSPdata) > 100:
        dataSPX.pop(0)
        dataXPOS.pop(0)
        dataSPY.pop(0)
        dataYPOS.pop(0)
        time.pop(0)

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(0)


if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
            QtGui.QApplication.instance().exec_()