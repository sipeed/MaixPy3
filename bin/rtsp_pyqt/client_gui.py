from PyQt5.QtWidgets import QHBoxLayout, QLabel, QSizePolicy, QVBoxLayout
from PyQt5.QtWidgets import QMainWindow, QWidget, QPushButton
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import pyqtSignal, QTimer
from PIL.ImageQt import ImageQt

from client import Client

DEFAULT_FPS = 40

class ClientWindow(QMainWindow):
    _update_image_signal = pyqtSignal()

    def __init__(
            self,
            file_name: str,
            host_address: str,
            host_port: int,
            rtp_port: int,
            parent=None):
        super(ClientWindow, self).__init__(parent)
        self.video_player = QLabel()
        self.setup_button = QPushButton()
        self.play_button = QPushButton()
        self.pause_button = QPushButton()
        self.tear_button = QPushButton()
        self.error_label = QLabel()

        self._media_client = Client(file_name, host_address, host_port, rtp_port)
        self._update_image_signal.connect(self.update_image)
        self._update_image_timer = QTimer()
        self._update_image_timer.timeout.connect(self._update_image_signal.emit)

        self.init_ui()

    def init_ui(self):
        self.setWindowTitle("Client")

        self.setup_button.setEnabled(True)
        self.setup_button.setText('Setup')
        self.setup_button.clicked.connect(self.handle_setup)

        self.play_button.setEnabled(False)
        self.play_button.setText('Play')
        self.play_button.clicked.connect(self.handle_play)

        self.pause_button.setEnabled(False)
        self.pause_button.setText('Pause')
        self.pause_button.clicked.connect(self.handle_pause)

        self.tear_button.setEnabled(False)
        self.tear_button.setText('Teardown')
        self.tear_button.clicked.connect(self.handle_teardown)

        self.error_label.setSizePolicy(
            QSizePolicy.Preferred,
            QSizePolicy.Maximum)

        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)

        control_layout = QHBoxLayout()
        control_layout.setContentsMargins(0, 0, 0, 0)
        control_layout.addWidget(self.setup_button)
        control_layout.addWidget(self.play_button)
        control_layout.addWidget(self.pause_button)
        control_layout.addWidget(self.tear_button)

        layout = QVBoxLayout()
        layout.addWidget(self.video_player)
        layout.addLayout(control_layout)
        layout.addWidget(self.error_label)

        central_widget.setLayout(layout)
        
        self.handle_setup()
        self.handle_play()

    def update_image(self):
        if not self._media_client.is_receiving_rtp:
            return
        frame = self._media_client.get_next_frame()
        if frame is not None:
            pix = QPixmap.fromImage(ImageQt(frame[0]).copy())
            self.video_player.setPixmap(pix)

    def handle_setup(self):
        self._media_client.establish_rtsp_connection()
        self._media_client.send_setup_request()
        self.setup_button.setEnabled(False)
        self.play_button.setEnabled(True)
        self.tear_button.setEnabled(True)
        self._update_image_timer.start(1000//DEFAULT_FPS) # VideoStream.DEFAULT_FPS

    def handle_play(self):
        self._media_client.send_play_request()
        self.play_button.setEnabled(False)
        self.pause_button.setEnabled(True)

    def handle_pause(self):
        self._media_client.send_pause_request()
        self.pause_button.setEnabled(False)
        self.play_button.setEnabled(True)

    def handle_teardown(self):
        self._media_client.send_teardown_request()
        self.setup_button.setEnabled(True)
        self.play_button.setEnabled(False)
        self.pause_button.setEnabled(False)
        exit(0)

    def handle_error(self):
        self.play_button.setEnabled(False)
        self.pause_button.setEnabled(False)
        self.tear_button.setEnabled(False)
        self.error_label.setText(f"Error: {self.media_player.errorString()}")
