from PyQt5 import QtWidgets,QtCore
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import QApplication, QMainWindow, QLineEdit, QListWidget, QMessageBox, QComboBox, QLabel, QPlainTextEdit
from PyQt5.QtCore    import QThread, pyqtSignal, QTimer
import sys
import socket
import threading
import time
import signal

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

class Thread(QThread):

    msgSig = pyqtSignal(str)
    lobbySig = pyqtSignal()
    updateLobbySig = pyqtSignal(str)
    loadRoomSig = pyqtSignal()
    updateRoomSig = pyqtSignal(str)
    QuestionSig = pyqtSignal(str)
    FinalRankingSig = pyqtSignal(str)

    def __init__(self):
        super(Thread, self).__init__()

    def run(self):
        while True:
            self.buffer = sock.recv(1024).decode('ascii')
            print(self.buffer)
            if "Your nick was set" in self.buffer or "So you left our room, take care, mate" in self.buffer:
                self.lobbySig.emit()
            elif "Room has been created"in self.buffer or "Welcome to Room" in self.buffer:
                self.loadRoomSig.emit()
            elif self.buffer[:2]=='%:':
                self.updateLobbySig.emit(self.buffer[2:])
            elif self.buffer[:2]=='#:':
                self.updateRoomSig.emit(self.buffer[2:])
            elif self.buffer[:1]=='?':
                self.QuestionSig.emit(self.buffer[1:])
            elif self.buffer[:1]=='~':
                self.FinalRankingSig.emit(self.buffer[1:])
            elif "U decided to leave Server" in self.buffer:
                sys.exit()
            else:
                self.msgSig.emit(self.buffer)

class MainWindow(QMainWindow):

    def __init__(self, *args, **kwargs):
        self.currentMenu = 0
        self.ip = "127.0.0.1"
        self.port = 8081
        self.connected = False
        self.gameStatus = False
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi()
        self.thread = Thread()
        self.thread.msgSig.connect(self.msgBox)
        self.thread.lobbySig.connect(self.switchLobby)
        self.thread.updateLobbySig.connect(self.updateLobby)
        self.thread.loadRoomSig.connect(self.switchRoom)
        self.thread.updateRoomSig.connect(self.updateRoom)
        self.thread.QuestionSig.connect(self.gameQuestion)
        self.thread.FinalRankingSig.connect(self.showFinalRanking)
        self.counter=QTimer()
        self.counter.timeout.connect(self.countdown)

    def beforeQuit(self):
        print("Exiting")

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

        self.msg = QMessageBox()

        #self.QtStack.setCurrentIndex(2)
    
    def loginUi(self):
        self.stack1.resize(640, 480)

        self.PushButton1 = QtWidgets.QPushButton(self.stack1)
        self.PushButton1.setText("ENTER MIDDLEEARTH")
        self.PushButton1.move(150, 180)
        self.PushButton1.resize(345, 100)
        self.PushButton1.clicked.connect(self.loginClicked)

        self.ExitButton = QtWidgets.QPushButton(self.stack1)
        self.ExitButton.setText("EXIT")
        self.ExitButton.move(540, 20)
        self.ExitButton.clicked.connect(self.exitServer)
        self.ExitButton.setVisible(False)
        self.ExitButton.setEnabled(False)

        self.SetNickButton = QtWidgets.QPushButton(self.stack1)
        self.SetNickButton.setText("Set nickname")
        self.SetNickButton.move(220, 300)
        self.SetNickButton.resize(200, 50)
        self.SetNickButton.clicked.connect(self.setNick)
        self.SetNickButton.setVisible(False)
        self.SetNickButton.setEnabled(False)

        # self.IpLabel = QLabel(self.stack1)
        # self.IpLabel.setText("IP:")
        # self.IpLabel.move(240, 153)

        # self.IpBox = QLineEdit(self.stack1)
        # self.IpBox.move(260, 150)
        # self.IpBox.setText("127.0.0.1")

        # self.PortBox = QLineEdit(self.stack1)
        # self.PortBox.move(260, 200)
        # self.PortBox.setText("8081")

        # self.PortLabel = QLabel(self.stack1)
        # self.PortLabel.setText("Port:")
        # self.PortLabel.move(228, 203)

        self.NicknameBox = QLineEdit(self.stack1)
        self.NicknameBox.move(270, 200)
        self.NicknameBox.setText("YourSUPERName")
        self.NicknameBox.setVisible(False)

        self.NicknameLabel = QLabel(self.stack1)
        self.NicknameLabel.setText("Nickname:")
        self.NicknameLabel.move(195, 203)
        self.NicknameLabel.setVisible(False)

    def lobbyUi(self):
        self.stack2.resize(640, 480)

        self.ListWidget = QListWidget(self.stack2)
        self.ListWidget.resize(300, 200)
        self.ListWidget.move(180, 100)

        self.ExitButton = QtWidgets.QPushButton(self.stack2)
        self.ExitButton.setText("EXIT")
        self.ExitButton.move(540, 20)
        self.ExitButton.clicked.connect(self.exitServer)

        self.RoomsListLabel = QLabel(self.stack2)
        self.RoomsListLabel.setText("Rooms list:")
        self.RoomsListLabel.move(180, 80)

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

        self.QuestionText = QPlainTextEdit(self.stack3)
        self.QuestionText.move(50, 30)
        self.QuestionText.resize(350,200)

        self.ExitButton = QtWidgets.QPushButton(self.stack3)
        self.ExitButton.setText("EXIT")
        self.ExitButton.move(540, 20)
        self.ExitButton.clicked.connect(self.exitServer)

        self.TimeLeftLabel = QLabel(self.stack3)
        self.TimeLeftLabel.setText("Time left")
        self.TimeLeftLabel.move(460, 320)

        self.TimeCounterLabel = QLabel(self.stack3)
        self.TimeCounterLabel.setText("00:00")
        self.TimeCounterLabel.move(460, 350)
        self.TimeCounterLabel.setFont(QFont('sans-serif', 15))

        self.AnswerAButton = QtWidgets.QPushButton(self.stack3)
        self.AnswerAButton.move(50, 250)
        self.AnswerAButton.resize(350,30)
        self.AnswerAButton.clicked.connect(lambda: self.answerQuestion("1"))

        self.AnswerBButton = QtWidgets.QPushButton(self.stack3)
        self.AnswerBButton.move(50, 300)
        self.AnswerBButton.resize(350,30)
        self.AnswerBButton.clicked.connect(lambda: self.answerQuestion("2"))

        self.AnswerCButton = QtWidgets.QPushButton(self.stack3)
        self.AnswerCButton.move(50, 350)
        self.AnswerCButton.resize(350,30)
        self.AnswerCButton.clicked.connect(lambda: self.answerQuestion("3"))

        self.AnswerDButton = QtWidgets.QPushButton(self.stack3)
        self.AnswerDButton.move(50, 400)
        self.AnswerDButton.resize(350,30)
        self.AnswerDButton.clicked.connect(lambda: self.answerQuestion("4"))

        self.ReadyButton = QtWidgets.QPushButton(self.stack3)
        self.ReadyButton.setText("Ready")
        self.ReadyButton.move(500, 320)
        self.ReadyButton.clicked.connect(self.setReady)
        
        self.LeaveButton = QtWidgets.QPushButton(self.stack3)
        self.LeaveButton.setText("Leave room")
        self.LeaveButton.move(500, 360)
        self.LeaveButton.clicked.connect(self.exitRoom)

        self.playerListWidget = QListWidget(self.stack3)
        self.playerListWidget.resize(160, 200)
        self.playerListWidget.move(460, 100)

        self.QuestionsLimitLabel = QLabel(self.stack3)
        self.QuestionsLimitLabel.move(50, 85)
        self.QuestionsLimitLabel.resize(200, 15)

        self.QuestionsLimitBox = QLineEdit(self.stack3)
        self.QuestionsLimitBox.move(50, 100)

        self.QuestionsLimitButton = QtWidgets.QPushButton(self.stack3)
        self.QuestionsLimitButton.setText("Set")
        self.QuestionsLimitButton.move(200, 99)
        self.QuestionsLimitButton.clicked.connect(self.setQuestionsLimit)

        self.PlayersLimitBox = QLineEdit(self.stack3)
        self.PlayersLimitBox.move(50, 150)

        self.PlayersLimitLabel = QLabel(self.stack3)
        self.PlayersLimitLabel.move(50, 135)
        self.PlayersLimitLabel.resize(200, 15)

        self.PlayersLimitButton = QtWidgets.QPushButton(self.stack3)
        self.PlayersLimitButton.setText("Set")
        self.PlayersLimitButton.move(200, 149)
        self.PlayersLimitButton.clicked.connect(self.setPlayersLimit)

        self.QuestionCategoryLabel = QLabel(self.stack3)
        self.QuestionCategoryLabel.move(50, 185)
        self.QuestionCategoryLabel.resize(200, 15)

        self.QuestionListBox = QComboBox(self.stack3)
        self.QuestionListBox.addItem("life")
        self.QuestionListBox.addItem("music")
        self.QuestionListBox.addItem("sport")
        self.QuestionListBox.resize(130,20)
        self.QuestionListBox.move(50, 200)

        self.QuestionListButton = QtWidgets.QPushButton(self.stack3)
        self.QuestionListButton.setText("Set")
        self.QuestionListButton.move(200, 199)
        self.QuestionListButton.clicked.connect(self.setCategory)

    def msgBox(self, msg):
        QMessageBox.about(self, "", msg)
    
    def loginClicked(self):
        self.connect('127.0.0.1', 8081)
        if self.connected:
            self.PushButton1.setEnabled(False)
            self.PushButton1.setVisible(False)
            self.ExitButton.setEnabled(True)
            self.ExitButton.setVisible(True)
            self.NicknameBox.setVisible(True)
            self.NicknameLabel.setVisible(True)
            self.SetNickButton.setVisible(True)
            self.SetNickButton.setEnabled(True)

    def setNick(self):
        if self.connected == True:
            self.setNickname(str(self.NicknameBox.text()))

    def joinClicked(self):
        self.selectedRoom = str(int(self.ListWidget.currentItem().text().split(' ')[0]) - 1)
        self.joinRoom(self.selectedRoom)

    def createRoomClicked(self):
        self.createRoom()

    def connect(self, ip, port):
        try:
            sock.connect((ip, port))
            self.thread.start()
            self.connected = True
        except:
            print("Connection error")
            self.msgBox("Connection error")

    def writeData(self, data):
        data = bytes(data, encoding='ascii')
        sock.send(data)

    def lobbyRequest(self):
        self.getRoomList()

    def setNickname(self, name):
        self.writeData(f"@{name}:")

    def serverDisconnect(self):
        self.writeData('#:')

    def exitServer(self):
        if self.connected == True:
            self.serverDisconnect()
        sys.exit()

    def createRoom(self):
        self.writeData('*:')

    def joinRoom(self, id):
        self.writeData(f">{id}:")

    def ready(self):
        self.writeData('!:')

    def leaveRoom(self):
        self.writeData('<:')
    
    def changeCategory(self, cat):
        print(f"${cat}:")
        self.writeData(f"${cat}:")

    def playerLimit(self, limit):
        self.writeData(f"&{limit}:")

    def questionAmount(self, limit):
        self.writeData(f"?{limit}:")

    def addPoints(self, points):
        self.writeData(f"%{points}:")
    
    def getRoomList(self):
        while(self.currentMenu == 1):
            self.writeData('_:')
            time.sleep(1)

    def getRoomUpdate(self):
        while(self.currentMenu == 2):
            self.writeData('_:')
            time.sleep(1)

    def switchLobby(self):
        self.lobbyUpdateThread = threading.Thread(target=self.getRoomList)
        self.currentMenu = 1
        self.QtStack.setCurrentIndex(self.currentMenu)
        self.lobbyUpdateThread.start()

    def switchRoom(self):
        self.ReadyButton.setEnabled(True)
        self.gameStatus = False
        self.currentMenu = 2
        self.QtStack.setCurrentIndex(self.currentMenu)
        self.roomUpdateThread = threading.Thread(target=self.getRoomUpdate)
        self.roomUpdateThread.start()
        self.showSettingsUi(True)
        self.showGameUi(False)

    def setReady(self):
        self.ready()
        self.ReadyButton.setEnabled(False)
    
    def exitRoom(self):
        self.gameStatus = False
        self.leaveRoom()
    
    def updateLobby(self, rooms):
        selectedRoom = self.ListWidget.row(self.ListWidget.currentItem())
        self.ListWidget.clear()
        rooms = rooms.split(':\n')
        if rooms[0] != '':
            for room in rooms[:-1]:
                room = room.split(':')
                self.ListWidget.addItem(f"{room[0]}   Category:{room[1]}   Players:{room[2]}/{room[3]}   Status:{room[4]}")
        if selectedRoom >= 0:
            self.ListWidget.setCurrentRow(selectedRoom)

    def updateRoom(self, data):
        print("udate")
        if self.gameStatus == False:
            self.playerListWidget.clear()
            data = data.split(':')
            self.QuestionsLimitLabel.setText(f"Question limit: Actual({data[1]})")
            self.PlayersLimitLabel.setText(f"Players limit: Actual({data[2]})")
            self.QuestionCategoryLabel.setText(f"Category: Actual({data[0]})")
            for player in data[3:]:
                self.playerListWidget.addItem(player)

    def setPlayersLimit(self):
        self.playerLimit(self.PlayersLimitBox.text())
        self.PlayersLimitBox.clear()

    def setQuestionsLimit(self):
        self.questionAmount(self.QuestionsLimitBox.text())
        self.QuestionsLimitBox.clear()
    
    def setCategory(self):
        self.changeCategory(self.QuestionListBox.currentText())

    def showSettingsUi(self,s:bool):
        self.QuestionsLimitLabel.setVisible(s)
        self.QuestionsLimitBox.setVisible(s)
        self.QuestionsLimitButton.setVisible(s)
        self.PlayersLimitLabel.setVisible(s)
        self.PlayersLimitBox.setVisible(s)
        self.PlayersLimitButton.setVisible(s)
        self.QuestionCategoryLabel.setVisible(s)
        self.QuestionListBox.setVisible(s)
        self.QuestionListButton.setVisible(s)

    def showGameUi(self,s:bool):
        self.QuestionText.setVisible(s)
        self.TimeLeftLabel.setVisible(s)
        self.TimeCounterLabel.setVisible(s)
        self.AnswerAButton.setVisible(s)
        self.AnswerBButton.setVisible(s)
        self.AnswerCButton.setVisible(s)
        self.AnswerDButton.setVisible(s)

    def answerButtons(self, s:bool):
        self.AnswerAButton.setEnabled(s)
        self.AnswerBButton.setEnabled(s)
        self.AnswerCButton.setEnabled(s)
        self.AnswerDButton.setEnabled(s)
    
    def gameQuestion(self, buffer):
        self.gameStatus = True
        buffer = buffer.split('#')
        ranking = buffer[1]
        buffer = buffer[0].split(':')
        question = buffer[0]
        answers = buffer[1].split('?')
        self.correctAnswer = buffer[2][0]
        self.QuestionText.clear()
        self.QuestionText.insertPlainText(question)
        self.AnswerAButton.setText(answers[0])
        self.AnswerBButton.setText(answers[1])
        self.AnswerCButton.setText(answers[2])
        self.AnswerDButton.setText(answers[3])
        self.showSettingsUi(False)
        self.answerButtons(True)
        self.showGameUi(True)
        self.updatePlayerRanking(ranking)
        self.timeLeft = 10
        self.TimeCounterLabel.setText(f"00:10")
        self.counter.start(1000)
    
    def countdown(self):
        if self.timeLeft > 0:
            self.timeLeft -= 1
            self.TimeCounterLabel.setText(f"00:0{self.timeLeft}")
        else:
            self.counter.stop()
            self.answerButtons(False)
    
    def answerQuestion(self, answer):
            self.answerButtons(False)
            print(answer)
            print(self.correctAnswer)
            print(answer == self.correctAnswer)
            if answer == self.correctAnswer:
                self.addPoints(self.timeLeft)
                print(f"Odp {self.timeLeft}")

    def updatePlayerRanking(self, ranking):
        ranking = ranking.replace('\t','').replace(':','').split('\n')
        ranking = list(filter(None, ranking))
        self.playerListWidget.clear()
        for player in ranking:
            self.playerListWidget.addItem(player)
        print(ranking)

    def showFinalRanking(self, ranking):
        self.gameStatus = False
        self.ReadyButton.setEnabled(True)
        self.msgBox(ranking)


    
def main():
    app = QApplication(sys.argv)
    showMain = MainWindow()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()