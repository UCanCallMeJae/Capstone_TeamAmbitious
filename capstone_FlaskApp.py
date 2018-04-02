
import pymysql
from flask import *
from s2d import *
import time
timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

app = Flask(__name__) #Create objects


#SQL Prep
db = pymysql.connect("127.0.0.1", "pi", "pi", "Capstone")
cursor = db.cursor()


@app.route('/')
def index():
#       return "Hello"
        return render_template('home.html') #Render the page
@app.route('/home.html')
def homeindex():
        return render_template('home.html')
@app.route('/irrigation.html')
def irrigationIndex():
        return render_template('irrigation.html')

@app.route('/modernized.html')
def modernizedIndex():
        return render_template('modernized.html')


@app.route('/LEDLvl')
def LED():
        send('LED\n'.encode()) #Command sent using send() from s2d
        one, two = receiveResponse().decode().split(",")
#        print(parseResponse)
#        parseResponse.split(".")
#        print(parseResponse)
#       return "Command Sent"
        return render_template('trashcan.html', response=two)

@app.route('/trashcan.html')
def trashCanIndex():
        return render_template('trashcan.html')

@app.route('/TrashLvl')
def TrashLvl():
        send("trashLvl\n".encode())
        time.sleep(1);
        send("gTrashLvl\n".encode())
        one, two = receiveResponse().decode().split(",")
        currentLevel = two
#        parseResponse = receiveResponse()
#        parseResponse.split(".", 1)
#        print(parseResponse)
        sql = "INSERT INTO TRASH (Level, Time) VALUES (%s, %s)"
        try:
                cursor.execute(sql, (currentLevel, timestamp))
                db.commit()
        except:
                db.rollback()
                print("ERROR")
        return render_template('trashcan.html', response=two)
@app.route('/RelayState')
def relay():
	send("RELAY\n".encode())
	return render_template('trashcan.html', response=receiveResponse())
