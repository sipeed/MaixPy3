
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import pyqtSignal, QTimer
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5 import QtCore, QtGui, QtWidgets
from client import Client
from PIL.ImageQt import ImageQt
from mainGui import Ui_Mainui
import random
import cv2
import numpy as np
import os
pictureDir = './pictures'
videosDir = './videos'



videoPath = "./videos/"
picturePath  = "./pictures/"
DEFAULT_FPS = 40
version = "v 0.0.1"
auther = "Adancurusul"
size = (480,360)
fps = 40
class mainWinLogic(QWidget,Ui_Mainui):
    _update_image_signal = pyqtSignal()
    def __init__(self):
        super(mainWinLogic,self).__init__()
        self.fileName = '/dev/camera'
        self.hostPort = 18811
        self.rtpPort = 18813
        self.setupUi(self)
        op = QtWidgets.QGraphicsOpacityEffect()
        # 设置透明度的值，0.0到1.0，最小值0是透明，1是不透明
        op.setOpacity(1)
        self.label.setGraphicsEffect(op)
        self.setWindowTitle('Maix2VideoPlayer')
        self.label_3.setText(version)
        #self.setWindowFlags(QtCore.Qt.WindowMinimizeButtonHint)
        self.stillRunning = 0
        self.isConnected = 0
        self.recordingState = 0

        self.imageNow = None
        #self._media_client = Client(self.fileName, self.hostAddress, self.hostPort, self.rtpPort)
        self.pauseButton.setEnabled(False)
        self.stopButton.setEnabled(True)
        self.photoButton.setEnabled(False)
        self.playButton.setEnabled(False)
        self.recordButton.setEnabled(False)
        self.setupButton.clicked.connect(self.setupHandler)
        self.playButton.clicked.connect(self.playHandler)
        self.pauseButton.clicked.connect(self.pauseHandler)
        self.photoButton.clicked.connect(self.photoHandler)
        self.stopButton.clicked.connect(self.stopHandler)

        self.recordButton.clicked.connect(self.recordHandler)
        try :
            with open("winconf", "r") as confF:
                self.ipInput.setText(confF.read())
        except:
            print("no config file")

    def closeEvent(self, event):
        reply = QMessageBox.question(self,'GUI',"Stop and exit?？",QMessageBox.Yes | QMessageBox.No,QMessageBox.No)
        if reply == QMessageBox.Yes:
            if self.isConnected:
                self._media_client.send_teardown_request()
                self.setupButton.setEnabled(True)
                self.playButton.setEnabled(False)
                self.pauseButton.setEnabled(False)
                self._update_image_timer.stop()
                self.checkTimer.stop()
                event.accept()
            else:
                event.accept()
        else:
            event.ignore()

    def startRecording(self):
        '''
        recoring
        Add each frame to the video
        :return:
        '''

        timeStr = self.getRandomStr()
        self.video = cv2.VideoWriter(videoPath + timeStr + ".avi", cv2.VideoWriter_fourcc('I', '4', '2', '0'), fps,
                                     size)
        self.recordingState = 1
    def getRandomStr(self):
        timeNow = QDateTime.currentDateTime()
        unixTime = timeNow.toSecsSinceEpoch()
        timeStr = str(unixTime) + "-" + str(random.randint(0, 10))
        return  timeStr
    def stopRecording(self):
        '''
        stop recording
        :return:
        '''
        self.recordingState = 0
        self.video.release()
        cv2.destroyAllWindows()

    def qtpixmap2Cvimg(self,qtpixmap):

        qimg = qtpixmap.toImage()
        temp_shape = (qimg.height(), qimg.bytesPerLine() * 8 // qimg.depth())
        temp_shape += (4,)
        ptr = qimg.bits()
        ptr.setsize(qimg.byteCount())
        result = np.array(ptr, dtype=np.uint8).reshape(temp_shape)
        result = result[..., :3]

        return result
    def recordHandler(self):
        if not self.recordingState:
            self.recordButton.setText("stop")
            self.startRecording()
        else:

            self.recordButton.setText("record")
            self.stopRecording()
        '''size = (480, 360)
        fps = 40'''

    def stopHandler(self):
        '''
        stop and quit
        '''
        reply = QMessageBox.information(self, "quit", "sure to stop and quit?", QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
        if reply == QMessageBox.Yes :
            if self.isConnected :
                self._media_client.send_teardown_request()
                self.setupButton.setEnabled(True)
                self.playButton.setEnabled(False)
                self.pauseButton.setEnabled(False)
                self._update_image_timer.stop()
                self.checkTimer.stop()
                exit(0)
            else :
                exit(0)


    def photoHandler(self):
        '''
        take a picture
        '''
        print(type(self.imageNow))

        timeStr = self.getRandomStr()

        self.imageNow.save(picturePath+timeStr+".png")
    def pauseHandler(self):
        '''
        send a pause request
        '''
        self._media_client.send_pause_request()
        self.pauseButton.setEnabled(False)
        self.playButton.setEnabled(True)
        self.checkTimer.stop()
    def playHandler(self):
        '''
        Paused playback
        '''
        self._media_client.send_play_request()
        self.playButton.setEnabled(False)
        self.pauseButton.setEnabled(True)

        self.checkTimer.start(2000)

    def setupHandler(self):
        '''
        setup
        '''
        self.hostAddress = self.ipInput.text()
        with open("winconf","w+") as confF:
            confF.write(self.hostAddress)
        self.stillRunning = 1
        self.checkTimer = QTimer()
        self.checkTimer.timeout.connect(self.checkTimerHandler)
        self.checkTimer.start(2000)
        self._media_client = Client(self.fileName, self.hostAddress, self.hostPort, self.rtpPort)
        self._media_client.establish_rtsp_connection()
        self._media_client.send_setup_request()

        self.setupButton.setEnabled(False)
        self.playButton.setEnabled(True)
        self.stopButton.setEnabled(True)
        #self.pauseButton.setEnabled(True)
        self.recordButton.setEnabled(True)
        self.photoButton.setEnabled(True)
        self._update_image_signal.connect(self.updateImage)
        self._update_image_timer = QTimer()
        self._update_image_timer.timeout.connect(self._update_image_signal.emit)
        self._update_image_timer.start(1000 // DEFAULT_FPS)  # VideoStream.DEFAULT_FPS
        self.isConnected = 1
        self.playHandler()
    def checkTimerHandler(self):
        '''
        Check to see if it is still working
        '''
        if not self.stillRunning :
            self.setupButton.setEnabled(True)
            QtWidgets.QMessageBox.critical(self, "error", "Failed to receive, please check and try again")
            self._update_image_timer.stop()
            self.checkTimer.stop()
        print("into handler")
        self.stillRunning = 0
    def updateImage(self):
        if not self._media_client.is_receiving_rtp:
            return
        frame = self._media_client.get_next_frame()
        if frame is not None:
            self.stillRunning = 1
            self.imageNow = ImageQt(frame[0]).copy()
            pix = QPixmap.fromImage(self.imageNow)
            self.label.setPixmap(pix)

            if self.recordingState:
                imgA = QtGui.QPixmap(self.imageNow)
                img2Store = self.qtpixmap2Cvimg(imgA)
                self.video.write(img2Store)
def ifPathExist():
    if not os.path.exists(pictureDir):
        os.makedirs(pictureDir)
    if not os.path.exists(videosDir):
        os.makedirs(videosDir)

if __name__ == '__main__':
    ifPathExist()
    app=QApplication(sys.argv)
    win = mainWinLogic()
    win.show()
    sys.exit(app.exec_())
