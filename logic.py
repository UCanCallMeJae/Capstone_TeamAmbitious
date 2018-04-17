import pymysql
from s2d import *
import time
import os
import sys
timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
moistureLevel = "0"

#SQL Prep
db = pymysql.connect("127.0.0.1", "pi", "pi", "Capstone")
cursor = db.cursor()

def trashLevel():
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

def moistureLevel():
	send("2waterLvl\n".encode())
	time.sleep(10)
	send("2water\n".encode())
	try:
		one, two = receiveResponse().decode().split(",")
		currentLevel = two
		moistureLevel = two
	except (ValueError, UnboundLocalError):
		currentLevel = "000"
		moistureLevel = "00"
	sql = "INSERT INTO WATER (Level, Time) VALUES (%s, %s)"
	try:
		cursor.execute(sql, (currentLevel, timestamp))
	except:
		db.rollback()
		print("ERROR")


def waterCycle(currentLevel):
	currentWaterLevel = currentLevel
	if currentWaterLevel > 30:
		shouldWater = false
	else:
		shouldWater = true
	if shouldWater:
		send("2RELAY\n".encode())
		time.sleep(20)
		send("2RELAY\n".encode())
	else:
		print("No need to water")



trashLevel()
moistureLevel()
waterCycle(moistureLevel)

sys.exit()
