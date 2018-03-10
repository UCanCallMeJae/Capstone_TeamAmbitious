
import serial #Import our serial library

ser = serial.Serial('/dev/ttyACM0', 9600) #Create the object (path to port, baud rate)

ser.flushInput() #Flush the serial input

print ser.portstr #Print our port

def constructCommand(): #Function to get our command
        command = raw_input("Please enter a command: ")
        command += '\n' #Termination signals end of your command
        return command

def send(command):
        ser.write(command) #Send to the Arduino
        ser.flushInput()
        print('Command sent via serial ---> ' + command)
def exit(excuse):
        print(excuse)
        quit()
def receiveResponse():
        debug("Reading serial line")
        response = ser.readline()
        return response
def debug(description):
        print(description)


#while True:
#        command = constructCommand()
 #       if command == "exit\n":
               #exit("User requested application close.")
   #     else:
        #        send(command)
           #     print(receiveResponse())




