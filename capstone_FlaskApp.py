
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
				level += ("#00d95a",)
				newTup.append(level)
			if level[1] > 10 and level[1] <=29:
				level += ("#ff6a00",)
				newTup.append(level)
			if level[1] <= 10:
				level += ("#ff0016",)
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

@app.route('/modernized.html')
def modernizedIndex():
	if not session.get('logged_in'):
		return render_template('login.html')
	else:
		sql = "SELECT * FROM uploads"
		cursor.execute(sql)
		data = cursor.fetchall()
		return render_template('modernized.html', img=data)


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
		return render_template('administration.html')


if __name__ == "__main__":
	app.secret_key = os.urandom(12)
	app.run(debug=True, host='0.0.0.0')
