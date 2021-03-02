from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow, QLineEdit, QListWidget
import sys
from communication import ClientCommunication

class MainWindow(QMainWindow, ClientCommunication):

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi()

    def setupUi(self):
        self.setObjectName("Main")
        self.resize(640, 480)

        self.QtStack = QtWidgets.QStackedLayout()

        self.stack1 = QtWidgets.QWidget()
        self.stack2 = QtWidgets.QWidget()
        self.stack3 = QtWidgets.QWidget()

        self.loginUi()
        self.lobbyUi()
        self.roomUi()

        self.QtStack.addWidget(self.stack1)
        self.QtStack.addWidget(self.stack2)
        self.QtStack.addWidget(self.stack3)
    
    def loginUi(self):
        self.stack1.resize(640, 480)
        self.PushButton1 = QtWidgets.QPushButton(self.stack1)
        self.PushButton1.setText("LOGIN")
        self.PushButton1.move(290, 380)
        self.PushButton1.clicked.connect(self.loginClicked)
        self.IpBox = QLineEdit(self.stack1)
        self.IpBox.move(260, 150)
        self.PortBox = QLineEdit(self.stack1)
        self.PortBox.move(260, 200)
        self.NicknameBox = QLineEdit(self.stack1)
        self.NicknameBox.move(260, 250)

    def lobbyUi(self):
        self.stack2.resize(640, 480)
        self.ListWidget = QListWidget(self.stack2)
        self.ListWidget.resize(300, 200)
        self.ListWidget.addItem("0 TEST0")
        self.ListWidget.addItem("1 TEST1")
        self.ListWidget.addItem("2 TEST2")
        self.ListWidget.move(180, 100)
        self.JoinButton = QtWidgets.QPushButton(self.stack2)
        self.JoinButton.setText("Join room")
        self.JoinButton.move(230, 380)
        self.JoinButton.clicked.connect(self.joinClicked)
        self.CreateRoomButton = QtWidgets.QPushButton(self.stack2)
        self.CreateRoomButton.setText("Create room")
        self.CreateRoomButton.move(350, 380)
        self.CreateRoomButton.clicked.connect(self.createRoomClicked)

    def roomUi(self):
        self.stack3.resize(640, 480)
    
    def loginClicked(self):
        self.establishConnection(str(self.IpBox.text()),int(self.PortBox.text()))
        self.setNickname(str(self.NicknameBox.text()))
        self.QtStack.setCurrentIndex(1)

    def joinClicked(self):
        self.selectedRoom = self.ListWidget.currentItem().text().split(' ')[0]
        self.joinRoom(self.selectedRoom)

    def createRoomClicked(self):
        self.createRoom()

def main():
    app = QApplication(sys.argv)
    showMain = MainWindow()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()