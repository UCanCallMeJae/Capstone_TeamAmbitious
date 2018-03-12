from flask import *
from s2d import *
import time
app = Flask(__name__) #Create objects



@app.route('/')
def index():
        return render_template('trashcan3.html') #Render the page

@app.route('/LEDLvl')
def LED():
        send("LED\n") #Command sent using send() from s2d
        return render_template('trashLvl.html', response=receiveResponse())

@app.route('/TrashLvl')
def TrashLvl():
        send("trashLvl\n")
        time.sleep(0.2);
        send("gTrashLvl\n")
        return render_template('trashLvl.html', response=receiveResponse())

