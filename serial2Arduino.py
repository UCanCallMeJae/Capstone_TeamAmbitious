import serial #Import our serial library

ser = serial.Serial('/dev/ttyACM0', 9600) #Create the object (path to port, baud rate)

ser.flushInput() #Flush the serial input

print ser.portstr #Print our port

def getCommand(): #Function to get our command
	command = raw_input("Please enter a command: ")
	return command

def send():
	command = getCommand()
	command += '\n' #Add a termination character
	ser.write(command) #Send to the Arduino
	ser.flushInput()
while True:
	send()

