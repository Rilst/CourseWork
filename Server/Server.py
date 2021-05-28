import pymysql.cursors  
import socket, threading, hashlib, uuid, json, time

def GetLogins(login, conn):
    row = None
    try:
        with conn.cursor() as cursor:
            sql = "SELECT idUsers FROM users Where Logins = %s"
            cursor.execute(sql, (login))
            row = cursor.fetchone()
            if row != None:
                return row['idUsers']
            else:
                return None
    except: 
        print("Connection to BD failed")
        
def RegistrationBD(login, password, conn):
    try:
        cursor = conn.cursor()
        sql = "INSERT INTO `users`(`Logins`, `Passwords`) VALUES (%s, %s)"
        print ("Insert Grade:", login, password)
        cursor.execute(sql, (login, password))
        conn.commit()
        return True
    except: 
        print("Connection to BD failed")
        return False

def AuthBD(login, password, conn):
    try:
        cursor = conn.cursor()
        row = None
        sql = "SELECT * FROM users WHERE logins = %s AND passwords = %s"
        cursor.execute(sql, (login, password))
        row = cursor.fetchone()

        if row != None:
            print("Auth Successful")
            return True
        else:
            print("Auth failed")
            return False
    except: 
        print("Connection to BD failed")
        return False

def GenerateSID():
    return uuid.uuid4().hex

def UpdateSID(login, sid, conn):
    try:
        cursor = conn.cursor()
        sql = "UPDATE users SET session_id = %s WHERE logins = %s"
        cursor.execute(sql, (sid, login))
        conn.commit()
        return True
    except: 
        print("Connection to BD failed")
        return False
  
def DeleteSID(login, conn):
    try:
        cursor = conn.cursor()
        sql = "UPDATE users SET session_id = %s WHERE logins = %s"
        cursor.execute(sql, (0, login))
        conn.commit()
        return True
    except: 
        print("Connection to BD failed")
        return False

def GetSID(login, conn):
    row = None
    try:
        with conn.cursor() as cursor:
            sql = "SELECT session_id FROM users Where Logins = %s"
            cursor.execute(sql, (login))
            row = cursor.fetchone()
            if row != None:
                return row['session_id']
            else:
                return None
    except: 
        print("Connection to BD failed 123")

def registration(data, connBD, connUser):
    if GetLogins(data["login"], connBD) is None:
        if RegistrationBD(data["login"], data["password"], connBD):
            connUser.send(json.dumps({'type': 'reg', 'success': True}).encode('utf-8'))
            print("Registration:" , data["login"])
        else:
            connUser.send(json.dumps({'type': 'reg', 'success': False}).encode('utf-8'))
    else:
        connUser.send(json.dumps({'type': 'reg', 'success': False}).encode('utf-8'))

def login(data, usersLogins, connBD, connUser):
    if usersLogins.get(data["login"]) is not None:
        connUser.send(json.dumps({'type': 'auth', 'success': False, 'reason': 'UserIsAuthorized'}).encode('utf-8'))
    else:
        if AuthBD(data["login"], data["password"], connBD):
            session_id = GenerateSID()
            connUser.send(json.dumps({'type': 'auth', 'success': True, 'session_id': session_id}).encode('utf-8'))
            UpdateSID(data["login"], session_id, connBD)
            return True
        else:
            connUser.send(json.dumps({'type': 'auth', 'success': False, 'reason': 'WrongPassOrLogin'}).encode('utf-8'))
            return False

def sendinvite(data, connBD, connUser, usersLogins, usersConnect, usersInChat):
    if (usersLogins.get(data["user"]) is not None) and (data["session_id"] == GetSID(usersConnect.get(connUser), connBD)):
        if data["type"] == "Invite":
            if usersConnect.get(connUser) != data["user"]:
                s = usersLogins.get(data["user"])
                s.send(json.dumps({'type': 'Invite', 'user': usersConnect.get(connUser), 'key': data["key"]}).encode('utf-8'))
            else:
                connUser.send(json.dumps({'type': 'DeclineInvite', 'user': data["user"], 'reason': 'The user is busy'}).encode('utf-8'))
        elif data["type"] == "AcceptInvite":
                s = usersLogins.get(data["user"])
                s.send(json.dumps({'type': 'AcceptInvite', 'user': usersConnect.get(connUser), 'key': data["key"]}).encode('utf-8'))
                usersInChat[usersConnect.get(connUser)] = data["user"]
                usersInChat[data["user"]] = usersConnect.get(connUser)
        elif data["type"] == "DeclineInvite":
            s = usersLogins.get(data["user"])
            s.send(json.dumps({'type': 'DeclineInvite', 'user': usersConnect.get(connUser), 'reason': data["reason"]}).encode('utf-8'))
    else:
        connUser.send(json.dumps({'type': 'DeclineInvite', 'user': data["user"], 'reason': 'The user with this username was not found'}).encode('utf-8'))

def sendmessage(data, connBD, connUser, usersLogins, usersConnect, usersInChat):
    if (usersInChat.get(usersConnect.get(connUser)) is not None) and (data["session_id"] == GetSID(usersConnect.get(connUser), connBD)):
        s = usersLogins[usersInChat.get(usersConnect.get(connUser))]
        s.send(json.dumps({'type': 'message', 'user': usersConnect.get(connUser), 'message': data["message"]}).encode('utf-8'))
    else:
        connUser.send(json.dumps({'type': 'messageError'}).encode('utf-8'))

def sendimage(data, connBD, connUser, usersLogins, usersConnect, usersInChat):
    if (usersInChat.get(usersConnect.get(connUser)) is not None) and (data["session_id"] == GetSID(usersConnect.get(connUser), connBD)):
        s = usersLogins[usersInChat.get(usersConnect.get(connUser))]
        s.send(json.dumps({'type': 'image', 'user': usersConnect.get(connUser), 'image': data["image"]}).encode('utf-8'))
    else:
        connUser.send(json.dumps({'type': 'messageError'}).encode('utf-8'))
      
def accepting(sock, usersLogins, usersConnect, usersInChat, connBD):
    while True:
        connUser, addr = sock.accept()
        print('Connected to:', addr[0], ':', addr[1])
        packet = lastpacket(time.time())
        thread_1 = threading.Thread(target = Connection, args= (connUser, usersLogins, usersConnect, usersInChat, packet, connBD, sock))
        thread_1.start()

def check_connection(connUser, usersLogins, usersConnect, usersInChat, packet, connBD, sock):
    while True:
        try:
            connUser.send(json.dumps({'type': 'check'}).encode('utf-8'))
            
            if time.time() - packet.packet > 60:
                if usersConnect.get(connUser) is not None:
                    if usersInChat.get(usersConnect[connUser]) is not None:
                        usersLogins[usersInChat[usersConnect[connUser]]].send(json.dumps({'type': 'DisconnectFromChat', 'user': usersConnect.get(connUser)}).encode('utf-8'))
                        print(usersInChat)
                        del usersInChat[usersInChat[usersConnect[connUser]]]
                        del usersInChat[usersConnect[connUser]]
                        print(usersInChat)
                    temp = usersLogins[usersConnect.get(connUser)]
                    del usersLogins[usersConnect.get(connUser)]
                    del usersConnect[temp]
                    print("Connection lose")
                break
        except:
            if usersConnect.get(connUser) is not None:
                if usersInChat.get(usersConnect[connUser]) is not None:
                    usersLogins[usersInChat[usersConnect[connUser]]].send(json.dumps({'type': 'DisconnectFromChat', 'user': usersConnect.get(connUser)}).encode('utf-8'))
                    print(usersInChat)
                    del usersInChat[usersInChat[usersConnect[connUser]]]
                    del usersInChat[usersConnect[connUser]]
                    print(usersInChat)
                temp = usersLogins[usersConnect.get(connUser)]
                del usersLogins[usersConnect.get(connUser)]
                del usersConnect[temp]
                print("Disconnected 0")
                break
            else:
                print("Disconnected 0")
                break
        time.sleep(15)
    connUser.close()

def Connection(connUser, usersLogins, usersConnect, usersInChat, packets, connBD, sock):
    
    thread_2 = threading.Thread(target = check_connection, args= (connUser, usersLogins, usersConnect, usersInChat, packets, connBD, sock))
    thread_2.start()
    
    while(True):
        try:
            data = connUser.recv(2048).decode('utf-8')
            
            packets.packet = time.time()
            
                
            print('data:', data)
            recievedData = data[data.find('{'):]
            print('recievedData:', recievedData)

            while len(recievedData) < int(data[:data.find('{')]):
                packet = connUser.recv(int(data[:data.find('{')]) - len(data)).decode('utf-8')
                recievedData += packet
        except:
            if usersConnect.get(connUser) is not None:
                if usersInChat.get(usersConnect[connUser]) is not None:
                    usersLogins[usersInChat[usersConnect[connUser]]].send(json.dumps({'type': 'DisconnectFromChat', 'user': usersConnect.get(connUser)}).encode('utf-8'))
                    print(usersInChat)
                    del usersInChat[usersInChat[usersConnect[connUser]]]
                    del usersInChat[usersConnect[connUser]]
                    print(usersInChat)
                temp = usersLogins[usersConnect.get(connUser)]
                del usersLogins[usersConnect.get(connUser)]
                del usersConnect[temp]
                print("Disconnected 1")
            break
            
        try:
            JsonData = json.loads(recievedData)
            if isinstance(JsonData, dict) and JsonData["type"] == "auth":
                if login(JsonData, usersLogins, connBD, connUser):
                    usersLogins[JsonData["login"]] = connUser
                    usersConnect[connUser] = JsonData["login"]
            elif isinstance(JsonData, dict) and JsonData["type"] == "reg":
                registration(JsonData, connBD, connUser)
            elif isinstance(JsonData, dict) and (JsonData["type"] == "Invite" or JsonData["type"] == "AcceptInvite" or JsonData["type"] == "DeclineInvite"):
                sendinvite(JsonData, connBD, connUser, usersLogins, usersConnect, usersInChat)
            elif isinstance(JsonData, dict) and JsonData["type"] == "message":
                sendmessage(JsonData, connBD, connUser, usersLogins, usersConnect, usersInChat)
            elif isinstance(JsonData, dict) and JsonData["type"] == "image":
                sendimage(JsonData, connBD, connUser, usersLogins, usersConnect, usersInChat)
            elif not JsonData:
                print("not data")
                break
        except:
            if usersConnect.get(connUser) is not None:
                if usersInChat.get(usersConnect[connUser]) is not None:
                    usersLogins[usersInChat[usersConnect[connUser]]].send(json.dumps({'type': 'DisconnectFromChat', 'user': usersConnect.get(connUser)}).encode('utf-8'))
                    print(usersInChat)
                    del usersInChat[usersInChat[usersConnect[connUser]]]
                    del usersInChat[usersConnect[connUser]]
                    print(usersInChat)
                temp = usersLogins[usersConnect.get(connUser)]
                del usersLogins[usersConnect.get(connUser)]
                del usersConnect[temp]
            break
    connUser.close()
    print("Disconnected 2")
    
connectionBD = pymysql.connect(host='localhost',
                               user='root',
                               password='',                             
                               db='messenger_database',
                               charset='utf8mb4',
                               cursorclass=pymysql.cursors.DictCursor)

class lastpacket:
    def __init__(self, packet):
        self.packet = packet

usersLogins = {}
usersConnect = {}
usersInChat = {}

host = '192.168.1.105'
port = 8080

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(20)
print("Socket listening")
thread = threading.Thread(target = accepting, args=(s, usersLogins, usersConnect, usersInChat, connectionBD))
thread.start()

        