from ledBlink import PiThing
from flask import *

app = Flask(__name__) #Create objects
pi_thing = PiThing()

VALUE = "100"


@app.route("/trash")
def index():
	return render_template('trashcan.html')

@app.route("/trash/checkVolume")
def ledState():
	return VALUE





