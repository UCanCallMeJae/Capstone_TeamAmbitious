
import pymysql
from flask import *
from s2d import *
import time
import os
timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

app = Flask(__name__) #Create objects


#SQL Prep
db = pymysql.connect("127.0.0.1", "pi", "pi", "Capstone")
cursor = db.cursor()


@app.route('/')
def index():
#       return "Hello"
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	return render_template('home.html') #Render the page
@app.route('/home.html')
def homeindex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	return render_template('home.html')
@app.route('/irrigation.html')
def irrigationIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	return render_template('irrigation.html')

@app.route('/login', methods=['POST'])
def do_admin_login():
	if request.form['password'] == 'password' and request.form['username'] == 'admin':
		session['logged_in'] = True	
	return index()

@app.route('/modernized.html')
def modernizedIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	return render_template('modernized.html')


@app.route('/LEDLvl')
def LED():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	send('LED\n'.encode()) #Command sent using send() from s2d
        	try:
			one, two = receiveResponse().decode().split(",")
		except (ValueError, UnboundLocalError):
			two = "ERROR"
			pass
#        print(parseResponse)
#        parseResponse.split(".")
#        print(parseResponse)
#       return "Command Sent"
        	return render_template('trashcan.html', response=two)

@app.route('/trashcan.html')
def trashCanIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	return render_template('trashcan.html')

@app.route('/TrashLvl')
def TrashLvl():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
        	send("trashLvl\n".encode())
        	time.sleep(1);
        	send("gTrashLvl\n".encode())
        	try:
			one, two = receiveResponse().decode().split(",")
        		currentLevel = two
		except (ValueError, UnboundLocalError):
			currentLevel = "000"
#        	parseResponse = receiveResponse()
#        	parseResponse.split(".", 1)
#        	print(parseResponse)
        	sql = "INSERT INTO TRASH (Level, Time) VALUES (%s, %s)"
        	try:
                	cursor.execute(sql, (currentLevel, timestamp))
                	db.commit()
        	except:
                	db.rollback()
                	print("ERROR")
        	return render_template('trashcan.html', response=currentLevel)
@app.route('/RelayState')
def relay():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		send("RELAY\n".encode())
		return render_template('trashcan.html', response=receiveResponse())

if __name__ == "__main__":
	app.secret_key = os.urandom(12)
	app.run(debug=True, host='0.0.0.0', port=5000)
