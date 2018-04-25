
import pymysql
from flask import *
from werkzeug import secure_filename
from s2d import *
import time
import os
timestamp = time.strftime("%Y-%m-%d %H:%M:%S")


app = Flask(__name__) #Create objects
app.secret_key = os.urandom(12)


#SQL Prep
db = pymysql.connect("127.0.0.1", "pi", "pi", "Capstone")
cursor = db.cursor()

#Path for uploaded files
UPLOAD_FOLDER = '/home/pi/Desktop/Capstone/Capstone_TeamAmbitious'

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
#		sql1 = "SELECT * FROM WATER ORDER BY id DESC LIMIT 1"
#		sql2 = "SELECT * FROM TRASH ORDER BY id DESC LIMIT 1"
#		cursor.execute(sql1)
#		waterVal = cursor.fetchone()
#		cursor.execute(sql2)
#		trashVal = cursor.fetchone()
#		waterTup = []
#		trashTup = []
#		for value in waterVal:
#			waterTup.append(value)
#			if waterVal[1] < 10:
#				level = 
				
        	return render_template('home.html')
@app.route('/irrigation.html')
def irrigationIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM WATER ORDER BY id DESC LIMIT 15"
		cursor.execute(sql)
		values = cursor.fetchall()
		valLevel = []
		timeVal = []
		newTup = []
		for level in values:
			valLevel.append(level[1])
			if level[1] >= 30:
				level += ("#00d95a",) #Green
				newTup.append(level)
			if level[1] > 10 and level[1] <=29:
				level += ("#ff6a00",) #Amber
				newTup.append(level)
			if level[1] <= 10:
				level += ("#ff0016",) #Red
				newTup.append(level)
		for time in values:
			timeVal.append(time[2])
		return render_template('irrigation.html', values=valLevel, time=timeVal, elements=newTup)

@app.route('/irrigationQuery')
def irrigationStoreVal():
		if not session.get('logged_in'):
			return render_template('login.html')
		else:
			send("2waterLvl\n".encode())
			time.sleep(2)
			send("2water\n".encode())
			try:
				one, two = receiveResponse().decode().split(",")
				currentLevel = two
			except (ValueError, UnboundLocalError):
				currentLevel = "000"
				print("Error getting moisture value")
			sql = "INSERT INTO WATER (Level, Time) VALUES (%s, %s)"
			try:
				cursor.execute(sql, (currentLevel, timestamp))
				db.commit()
			except:
				db.rollback()
				print("Error storing moist value")
			return irrigationIndex()
@app.route('/login', methods=['POST'])
def do_admin_login():
	if request.form['password'] == 'password' and request.form['username'] == 'admin':
		session['logged_in'] = True
	return index()
@app.route('/logout')
def logout():
	session['logged_in'] = False
	return render_template('login.html')

@app.route('/modernized.html')
def modernizedIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM uploads"
		sql2 = "SELECT * FROM events"
		cursor.execute(sql)
		data = cursor.fetchall()
		cursor.execute(sql2)
		events = cursor.fetchall()
		print(events)
		return render_template('modernized.html', img=data, events=events)


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
		return administrator()
@app.route('/2LEDLvl')
def LED2():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		send('2LED\n'.encode())
		try:
			one, two = receiveResponse().decode().split(",")
		except:
			two="ERROR"
			print("Error sending 2LED")
			pass
		return administrator()

@app.route('/trashcan.html')
def trashCanIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM TRASH ORDER BY id DESC LIMIT 15"
		cursor.execute(sql)
		values = cursor.fetchall()
		valLevel = []
		timeVal = []
		newTup = []
		for level in values:
			valLevel.append(level[1])
			if level[1] < 30:
				level += ("#ff0016",) #Red
				newTup.append(level)
			if level[1] >= 31 and level[1] <=59:
				level += ("#ff6a00",) #Amber
				newTup.append(level)
			if level[1] >= 60:
				level += ("#00d95a",) #Green
				newTup.append(level)
		for time in values:
			timeVal.append(time[2])
		return render_template('trashcan.html', values=valLevel, time=timeVal, elements=newTup)


@app.route('/TrashLvl')
def trashLvl():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		send("trashLvl\n".encode())
		time.sleep(1)
		send("gTrashLvl\n".encode())
		try:
			one, two = receiveResponse().decode().split(",")
			currentLevel = two
		except (ValueError, UnboundLocalError):
			currentLevel = "000"
		sql = "INSERT INTO TRASH (Level, Time) VALUES (%s, %s)"
		try:
			cursor.execute(sql, (currentLevel, timestamp))
			db.commit()
		except:
			db.rollback()
			print("ERROR")
		return trashCanIndex()
@app.route('/RelayState')
def relay():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		send("2RELAY\n".encode())
		return irrigationIndex()

@app.route('/uploader', methods = ['POST'])
def upload_file():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		if request.method == 'POST':
			f = request.files['file']
			file_name = f.filename
			print(file_name)
			img_path = os.path.join(UPLOAD_FOLDER, 'static', secure_filename(f.filename))
			f.save(img_path)
			print(img_path)
			sql = "INSERT INTO uploads (image_path, image_name) VALUES (%s, %s)"
			try:
				cursor.execute(sql, (img_path, file_name))
				db.commit()
			except:
				db.rollback()
				print("ERROR")
		return administrator()

@app.route('/edit')
def edit_images():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM uploads"
		cursor.execute(sql)
		data = cursor.fetchall()
		return render_template('edit.html', img=data)
@app.route('/editEvents')
def editEvents():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM events"
		try:
			cursor.execute(sql)
			events = cursor.fetchall()
		except:
			db.rollback()
			print("Error fetching")
			events = "000"
		return render_template('editevents.html', events=events)
@app.route('/submitEvents', methods=['POST'])
def submiteditEvents():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		response = request.form.getlist('eventCheck')
		for event in response:
			sql = "DELETE FROM events WHERE id IN (%s)"
			try:
				cursor.execute(sql, (event))
				db.commit()
				print("Event removed")
			except:
				db.rollback()
				print("Error removing events")
		return administrator()
@app.route('/editDB', methods=['POST'])
def makeChanges():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		response = request.form.getlist('img')
		for img in response:
			sql = "SELECT * FROM uploads WHERE id = %s"
			cursor.execute(sql, (img))
			img_name = cursor.fetchall()
			sql2 = "DELETE FROM uploads WHERE id IN (%s)"
			image_path = img_name[0][1]
			print(image_path)
			try:
				cursor.execute(sql2, (img))
				db.commit()
				os.remove(image_path)
				print("REMOVED")
			except:
				db.rollback()
				print("Error removing")
		return administrator()

@app.route('/administration.html')
def administrator():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM WATER ORDER BY id DESC LIMIT 15"
		cursor.execute(sql)
		elements = cursor.fetchall()
		valLevel = []
		timeVal = []
		newTup = []
		for value in elements:
			valLevel.append(value[1])
			if value[1] >= 30 and value[1] <= 69:
				value +=("#00d95a",) #Green
				newTup.append(value)
			if value[1] > 10 and value[1] <= 29:
				value +=("#ff6a00",) #Amber
				newTup.append(value)
			if value[1] >= 70 or value[1] <= 10:
				value +=("#ff0016",) #Red
				newTup.append(value)
		for time in elements:
			timeVal.append(time[2])
		sql2 = "SELECT * FROM TRASH ORDER BY id DESC LIMIT 1"
		cursor.execute(sql2)
		trashLvl = cursor.fetchone()
		print(trashLvl)
		if trashLvl[1] < 30:
			arrowPos = "0%"
			arrowCol = "#ff0016"
		if trashLvl[1] >= 31 and trashLvl[1] <= 59:
			arrowPos = "33%"
			arrowCol = "#ff6a00"
		if trashLvl[1] >= 60:
			arrowPos = "57%"
			arrowCol = "#00d95a"
		return render_template('administration.html', values=newTup, arrowPos=arrowPos, arrowCol=arrowCol)

@app.route('/addevent', methods=['POST'])
def addevent():
	date = request.form['date']
	event = request.form['event']
	print(date)
	print(event)
	sql = "INSERT INTO events (date, event) VALUES (%s, %s)"
	try:
		cursor.execute(sql, (date, event))
		db.commit()
	except:
		db.rollback()
		print("ERROR adding event")
	return render_template('administration.html')

if __name__ == "__main__":
	app.secret_key = os.urandom(12)
	app.run(debug=True, host='0.0.0.0')
