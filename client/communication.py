import socket
import threading

class Communication():
    def __init__(self):
        self.ip = "127.0.0.1"
        self.port = 8081
        self.sock = None

    def connect(self, ip, port):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((ip, port))
            self.sock = sock
        except:
            print("Connection error")
    def writeData(self, data):
        data = bytes(data, encoding='ascii')
        self.sock.send(data)

class ClientCommunication(Communication):
    def __init__(self):
        super(ClientCommunication, self).__init__()
        self.buffer = None
    
    def establishConnection(self, ip, port):
        if self.sock is None:
            self.connect(ip, port)
            self.th = threading.Thread(target=self.listen)
            self.th.start()

    def listen(self):
        while True:
            self.buffer = self.sock.recv(1024).decode('utf-8')
            print(self.buffer)
            if "Your nick was set" in self.buffer:
                None


    def setNickname(self, name):
        self.writeData(f"@{name}:")

    def serverDisconnect(self):
        self.writeData('#:')

    def createRoom(self):
        self.writeData('*:')

    def blabla(self):
        self.writeData('_:')

    def joinRoom(self, id):
        self.writeData(f">{id}:")

    def ready(self):
        self.writeData('!:')

    def leaveRoom(self):
        self.writeData('<:')
    
    def changeCategory(self, id):
        self.writeData('${id}:')

    def playerLimit(self, limit):
        self.writeData('&{limit}:')

    def questionAmount(self, limit):
        self.writeData('?{limit}:')

