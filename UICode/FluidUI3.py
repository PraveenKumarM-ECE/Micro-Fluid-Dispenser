import sys
import serial
import serial.tools.list_ports
from datetime import datetime
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QComboBox, QPushButton, QLineEdit, QTextEdit, QGroupBox,
    QGridLayout, QCheckBox
)


# ---------------- Serial Thread ----------------
class SerialReader(QThread):
    data_received = pyqtSignal(str)

    def __init__(self, ser):
        super().__init__()
        self.ser = ser
        self.running = True

    def run(self):
        while self.running:
            if self.ser.in_waiting:
                try:
                    text = self.ser.readline().decode(errors="ignore")
                    self.data_received.emit(text)
                except:
                    pass

    def stop(self):
        self.running = False
        self.quit()
        self.wait()


# ---------------- Main UI ----------------
class FluidUI(QWidget):
    def __init__(self):
        super().__init__()

        self.ser = None
        self.thread = None

        self.setWindowTitle("Micro Fluid Dispenser")
        self.setMinimumSize(850, 00)

        main = QVBoxLayout()
        self.setLayout(main)

        # TOP BAR
        top = QHBoxLayout()

        top.addWidget(QLabel("Port:"))

        self.portCombo = QComboBox()
        self.refreshPorts()
        top.addWidget(self.portCombo)

        self.connectBtn = QPushButton("Connect")
        self.connectBtn.clicked.connect(self.toggleConnection)
        top.addWidget(self.connectBtn)

        self.status = QLabel("Disconnected")
        self.status.setStyleSheet("color: red; font-weight: bold;")
        top.addWidget(self.status)

        top.addStretch()
        main.addLayout(top)

        # MID
        mid = QHBoxLayout()
        main.addLayout(mid)

        # LEFT PANEL
        leftPanel = QVBoxLayout()

        # ---- Pre Pumping ----
        preBox = QGroupBox("Pre Pumping (Milli sec)")
        preLayout = QGridLayout()
        preBox.setStyleSheet("""
QGroupBox { border: 0; font-weight: bold; margin-top: 10px; }
QGroupBox::title { color: #333; font-size: 14px; }
QLabel { font-size: 12px; }
QLineEdit { border: 1px solid #888; border-radius: 3px; padding: 4px; }
""")

        preLayout.addWidget(QLabel("ON:"), 0, 0)
        self.preOn = QLineEdit()
        preLayout.addWidget(self.preOn, 0, 1)

        preLayout.addWidget(QLabel("OFF:"), 1, 0)
        self.preOff = QLineEdit()
        preLayout.addWidget(self.preOff, 1, 1)

        preBox.setLayout(preLayout)
        leftPanel.addWidget(preBox)

        # ---- Main Pumping ----
        mainBox = QGroupBox("Main Pumping (Milli sec)")
        mainLayout = QGridLayout()
        mainBox.setStyleSheet("""
QGroupBox { border: 0; font-weight: bold; margin-top: 10px; }
QGroupBox::title { color: #333; font-size: 14px; }
QLabel { font-size: 12px; }
QLineEdit { border: 1px solid #888; border-radius: 3px; padding: 4px; }
""")

        mainLayout.addWidget(QLabel("ON:"), 0, 0)
        self.mainOn = QLineEdit()
        mainLayout.addWidget(self.mainOn, 0, 1)

        mainLayout.addWidget(QLabel("OFF:"), 1, 0)
        self.mainOff = QLineEdit()
        mainLayout.addWidget(self.mainOff, 1, 1)

        mainBox.setLayout(mainLayout)
        leftPanel.addWidget(mainBox)

        mid.addLayout(leftPanel)

        # -------------------------------------------------------------
        # TABLE MIDDLE
        # -------------------------------------------------------------
        tableBox = QGroupBox("")
        tableLayout = QGridLayout()
        tableLayout.setContentsMargins(4, 4, 4, 4)
        tableLayout.setHorizontalSpacing(6)
        tableLayout.setVerticalSpacing(4)

        tableBox.setStyleSheet("""
    QGroupBox {
        font-size: 15px;
        font-weight: bold;
        color: #222;
        border: 1px solid rgba(0,0,0,0.2);
        border-radius: 8px;
        margin-top: 6px;
        padding: 6px;
        background: rgba(255,255,255,0.45);
    }
    QLabel {
        font-size: 13px;
        color: #333;
        padding: 2px;
    }
    QLineEdit {
        font-size: 13px;
        padding: 3px;
        background: rgba(255,255,255,0.7);
        border: 1px solid rgba(0,0,0,0.25);
        border-radius: 5px;
        min-height: 18px;
    }
""")

        labels = [
            "PON30", "POFF30", "MON30", "MOFF30",
            "PON60", "POFF60", "MON60", "MOFF60",
            "TOT (Cycles)"
        ]

        self.valueFields = {}

        for i, label in enumerate(labels):
            lbl = QLabel(label)
            box = QLineEdit()
            box.setReadOnly(True)

            tableLayout.addWidget(lbl, i, 0)
            tableLayout.addWidget(box, i, 1)

            self.valueFields[label] = box

        tableBox.setLayout(tableLayout)
        mid.addWidget(tableBox)

        # RIGHT SIDE BUTTONS
      
        rightPanel = QVBoxLayout()

        self.readBtn = QPushButton("READ")
        self.readBtn.clicked.connect(lambda: self.sendText("READ"))
        rightPanel.addWidget(self.readBtn)
        self.readBtn.setStyleSheet("""
        QPushButton {
	border-color: rgb(85, 255, 255);
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #f2f2f2, stop:1 #d6d6d6);
    border: 2px solid #888;
    border-radius: 5px;
    padding: 5px;
    color: black;
    font-weight: bold;
    box-shadow: 2px 2px 4px #999;
}

QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #d6d6d6, stop:1 #f2f2f2);
    border: 2px solid #555;
    padding-top: 7px;
    padding-left: 7px;
}
QPushButton:hover{
    background-color: rgb(140, 140, 140);}
                                     """)


        self.stsBtn = QPushButton("STS")
        self.stsBtn.clicked.connect(lambda: self.sendText("STS"))
        self.stsBtn.setStyleSheet("""
        QPushButton {
	border-color: rgb(85, 255, 255);
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #f2f2f2, stop:1 #d6d6d6);
    border: 2px solid #888;
    border-radius: 5px;
    padding: 5px;
    color: black;
    font-weight: bold;
    box-shadow: 2px 2px 4px #999;
}

QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #d6d6d6, stop:1 #f2f2f2);
    border: 2px solid #555;
    padding-top: 7px;
    padding-left: 7px;
}
QPushButton:hover{
    background-color: rgb(140, 140, 140);}
                                     """)                           
        rightPanel.addWidget(self.stsBtn)

        self.resetBtn = QPushButton("RESET CYCLE")
        self.resetBtn.clicked.connect(lambda: self.sendText("TOT RST"))
        self.resetBtn.setStyleSheet("""
        QPushButton {
	border-color: rgb(85, 255, 255);
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #f2f2f2, stop:1 #d6d6d6);
    border: 2px solid #888;
    border-radius: 5px;
    padding: 5px;
    color: black;
    font-weight: bold;
    box-shadow: 2px 2px 4px #999;
}

QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #d6d6d6, stop:1 #f2f2f2);
    border: 2px solid #555;
    padding-top: 7px;
    padding-left: 7px;
}
QPushButton:hover{
    background-color: rgb(140, 140, 140);}
                                     """)

        rightPanel.addWidget(self.resetBtn)

        rightPanel.addSpacing(20)

        # --------- NEW UPDATE BUTTON ---------
        self.updateBtn = QPushButton("UPDATE")
        self.updateBtn.clicked.connect(self.sendUpdate)
        self.updateBtn.setStyleSheet("""
QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 rgb(150, 255, 255), stop:1 rgb(85, 255, 255));
    border: 2px solid rgb(0, 200, 200);
    border-radius: 6px;
    padding: 6px 12px;
    color: black;
    font-weight: bold;
}

QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 rgb(85, 255, 255), stop:1 rgb(150, 255, 255));
    border: 2px solid rgb(0, 150, 150);
    padding-top: 8px;
    padding-left: 8px;
}

QPushButton:hover {
    background-color: rgb(59, 179, 179);
}
""")


        
        self.resetBtn = QPushButton("RESET CYCLE")
        self.resetBtn.clicked.connect(lambda: self.sendText("TOT RST"))
        self.resetBtn.setStyleSheet("""
        QPushButton {
	border-color: rgb(85, 255, 255);
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #f2f2f2, stop:1 #d6d6d6);
    border: 2px solid #888;
    border-radius: 5px;
    padding: 5px;
    color: black;
    font-weight: bold;
    box-shadow: 2px 2px 4px #999;
}

QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #d6d6d6, stop:1 #f2f2f2);
    border: 2px solid #555;
    padding-top: 7px;
    padding-left: 7px;
}
QPushButton:hover{
    background-color: rgb(140, 140, 140);}
                                     """)
        rightPanel.addWidget(self.updateBtn)
        # --------------------------------------

        # Mode 30/60
        modeBox = QGroupBox("Mode")
        modeLayout = QVBoxLayout()
        modeBox.setStyleSheet("""
QGroupBox {
    border: 0;
    font-weight: bold;
    margin-top: 18px;      /* increased to avoid title blocking */
}

QGroupBox::title {
    subcontrol-origin: margin;
    padding: 0 4px;
    color: #333;
    font-size: 14px;
    background: transparent;
}

QLabel {
    font-size: 12px;
}

QComboBox {
    border: 1px solid #888;
    border-radius: 3px;
    padding: 4px;
    background: white;
}
""")

        self.modeSelect = QComboBox()
        self.modeSelect.addItems(["30", "60"])
        modeLayout.addWidget(self.modeSelect)
        modeBox.setLayout(modeLayout)

        rightPanel.addWidget(modeBox)
        rightPanel.addStretch()

        mid.addLayout(rightPanel)


        # LOG LABEL + TIMESTAMP OPTION
        logTop = QHBoxLayout()
        logTop.addWidget(QLabel("Log:"))

        self.timeCheck = QCheckBox("Timestamp")
        self.timeCheck.setChecked(True)
        logTop.addWidget(self.timeCheck)

        logTop.addStretch()
        main.addLayout(logTop)

        # LOG BOX
        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.log.setStyleSheet("background:#000; color:#0f0; font-family: Consolas; font-size:14px;")
        main.addWidget(self.log)

        # SEND BOX
        bottom = QHBoxLayout()
        self.input = QLineEdit()
        bottom.addWidget(self.input)

        self.sendBtn = QPushButton("Send")
        self.sendBtn.clicked.connect(self.manualSend)
        bottom.addWidget(self.sendBtn)

        main.addLayout(bottom)

    # SERIAL FUNCTIONS

    def refreshPorts(self):
        self.portCombo.clear()
        for p in serial.tools.list_ports.comports():
            self.portCombo.addItem(p.device)

    def toggleConnection(self):
        if self.ser is None:
            self.connectSerial()
        else:
            self.disconnectSerial()

    def connectSerial(self):
        try:
            port = self.portCombo.currentText()
            self.ser = serial.Serial(port, 9600, timeout=0.1)

            self.thread = SerialReader(self.ser)
            self.thread.data_received.connect(self.updateLog)
            self.thread.start()

            self.status.setText("Connected")
            self.status.setStyleSheet("color: green; font-weight: bold;")
            self.connectBtn.setText("Disconnect")

        except Exception as e:
            self.log.append(f"Error: {e}")

    def disconnectSerial(self):
        if self.thread:
            self.thread.stop()

        if self.ser:
            self.ser.close()

        self.ser = None
        self.thread = None

        self.status.setText("Disconnected")
        self.status.setStyleSheet("color: red; font-weight: bold;")
        self.connectBtn.setText("Connect")

    # ---------------- Update Log + Fill Table ----------------
    def updateLog(self, text):

        if self.timeCheck.isChecked():
         stamp = datetime.now().strftime("%H:%M:%S")
         self.log.append(f"{stamp}  {text.strip()}")
        else:
         self.log.append(text)


        t = text.strip()

        # Extract ONLY the number
        for key in self.valueFields:
            base = key.split()[0]      # e.g. "PON30"
            if t.startswith(base):
                if "=" in t:
                    val = t.split("=", 1)[1].strip()   # Only after '='
                    self.valueFields[key].setText(val)

    # ----------------------------------------------------------

    def sendText(self, txt):
        if self.ser:
            self.ser.write((txt + "\n").encode())
            if self.timeCheck.isChecked():
                stamp = datetime.now().strftime("%H:%M:%S")



    def manualSend(self):
        t = self.input.text().strip()
        if t:
            self.sendText(t)
            self.input.clear()

    # UPDATE BUTTON
    def sendUpdate(self):
        pre_on  = self.preOn.text().strip() or "1000" 
        pre_off = self.preOff.text().strip() or "1000"
        main_on = self.mainOn.text().strip() or "1000"
        main_off = self.mainOff.text().strip() or "1000"
        mode = self.modeSelect.currentText()

        commands = [
            f"PON{mode} {pre_on}",
            f"POFF{mode} {pre_off}",
            f"MON{mode} {main_on}",
            f"MOFF{mode} {main_off}"
        ]

        for cmd in commands:
            self.sendText(cmd)
            QThread.msleep(100)


# ---------------- Main App ----------------
if __name__ == "__main__":
    from PyQt5 import QtCore, QtWidgets
    QtWidgets.QApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling)
    app = QtWidgets.QApplication(sys.argv)
    win = FluidUI()
    win.show()
    sys.exit(app.exec_())
