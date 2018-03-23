
from flask import *
from s2d import *
import time
app = Flask(__name__) #Create objects



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
#       return "Command Sent"
        return render_template('trashcan.html', response=receiveResponse())

@app.route('/trashcan.html')
def trashCanIndex():
        return render_template('trashcan.html')

@app.route('/TrashLvl')
def TrashLvl():
        send("trashLvl\n".encode())
        time.sleep(1);
        send("gTrashLvl\n".encode())
        return render_template('trashcan.html', response=receiveResponse())
@app.route('/RelayState')
def relay():
	send("RELAY\n".encode())
	return render_template('trashcan.html', response=receiveResponse())
