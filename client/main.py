from PyQt5 import QtCore, QtGui, QtWidgets
import socket

class Ui_LoginWindow(object):
    def setupUi(self, LoginWindow):
        LoginWindow.setObjectName("LoginWindow")
        LoginWindow.resize(549, 398)
        self.centralwidget = QtWidgets.QWidget(LoginWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(240, 220, 75, 23))
        self.pushButton.setObjectName("pushButton")
        self.pushButton.clicked.connect(self.loginclick)                                                                    
        self.lineEdit = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit.setGeometry(QtCore.QRect(220, 80, 113, 20))
        self.lineEdit.setToolTip("")
        self.lineEdit.setObjectName("lineEdit")
        self.lineEdit_2 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_2.setGeometry(QtCore.QRect(220, 120, 113, 20))
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.lineEdit_3 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_3.setGeometry(QtCore.QRect(220, 160, 113, 20))
        self.lineEdit_3.setObjectName("lineEdit_3")
        LoginWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(LoginWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 549, 21))
        self.menubar.setObjectName("menubar")
        LoginWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(LoginWindow)
        self.statusbar.setObjectName("statusbar")
        LoginWindow.setStatusBar(self.statusbar)

        self.retranslateUi(LoginWindow)
        QtCore.QMetaObject.connectSlotsByName(LoginWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.pushButton.setText(_translate("MainWindow", "Login"))
        self.lineEdit.setPlaceholderText(_translate("MainWindow", "IP"))
        self.lineEdit_2.setPlaceholderText(_translate("MainWindow", "port"))
        self.lineEdit_3.setPlaceholderText(_translate("MainWindow", "name"))
    
    def loginclick(self):
        ip = self.lineEdit.text()
        port = int(self.lineEdit_2.text())
        return ip,port

class Ui_LobbyWindow(object):
    def setupUi(self, LobbyWindow):
        LobbyWindow.setObjectName("LobbyWindow")
        LobbyWindow.resize(549, 398)
        self.centralwidget = QtWidgets.QWidget(LobbyWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.listWidget = QtWidgets.QListWidget(self.centralwidget)
        self.listWidget.setGeometry(QtCore.QRect(100, 20, 331, 271))
        self.listWidget.setObjectName("listWidget")
        self.pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(160, 310, 75, 23))
        self.pushButton.setObjectName("pushButton")
        self.pushButton_2 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_2.setGeometry(QtCore.QRect(270, 310, 75, 23))
        self.pushButton_2.setObjectName("pushButton_2")
        LobbyWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(LobbyWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 549, 21))
        self.menubar.setObjectName("menubar")
        LobbyWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(LobbyWindow)
        self.statusbar.setObjectName("statusbar")
        LobbyWindow.setStatusBar(self.statusbar)

        self.retranslateUi(LobbyWindow)
        QtCore.QMetaObject.connectSlotsByName(LobbyWindow)

    def retranslateUi(self, LobbyWindow):
        _translate = QtCore.QCoreApplication.translate
        LobbyWindow.setWindowTitle(_translate("LobbyWindow", "LobbyWindow"))
        self.pushButton.setText(_translate("LobbyWindow", "Join Lobby"))
        self.pushButton_2.setText(_translate("LobbyWindow", "Create Lobby"))

def connect(ip, port):
    s = socket.socket()
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
        s.connect((ip, port))
        print ("Socket successfully created")
    except socket.error as err:  
        print ("socket creation failed with error %s" %(err))

class Controller:
    def __init__(self):
        pass

    def show_login(self):
        self.login = QtWidgets.QMainWindow()
        ui = Ui_LoginWindow()
        ui.setupUi(self.login)
        self.login.show()

    def show_lobby(self):
        self.lobby = QtWidgets.QMainWindow()
        ui = Ui_LobbyWindow()
        ui.setupUi(self.lobby)
        self.lobby.show()


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    controller = Controller()
    controller.show_login()
    controller.show_lobby()
    sys.exit(app.exec_())