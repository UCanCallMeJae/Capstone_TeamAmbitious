from flask import *
from s2d import * #serial2Duinov1.01.py needs to be in the same directory
app = Flask(__name__) #Create objects



@app.route('/trash')
def index():
        send("LED\n")
        return render_template('trashcan3.html')

@app.route('/trash?LED=trigger')
def LED():
        send("LED\n")
        return render_template('trashcan3.html')
