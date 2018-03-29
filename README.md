# Capstone_TeamAmbitious
Capstone project code, materials, and misc.

-------------------------------------------------------------------------
Notes & Troubleshooting: |
-------------------------

- CSS and other styling files should be kept within /static/styles/ 
  - Where / is the directory from which the main Flask application is run
- render_template() checks for HTML files in /templates/
  - Where / is the directory from which the main Flask application is run
  
- Ensure that the master Arduino is plugged into the correct port (/dev/ACM* for Arduino devices) corresponding to the port defined in S2D.py


- If Flask complains about another instance already running:
  - Enter: ps -fA | grep -e "flask" 
    -This will show you any instances running
      -Go ahead and kill the process using: sudo kill [process#]



- If master transmitter reports Tx Failed:
 - Check the antennas (are they bent, soldering is okay?)

-------------------------------------------------------------------------
Pin Mappings| 
------------  

              -----
Arduino Pin --> NRF24L01+ Pin:  
  3.3v --> 1
  GND --> 2
  8 --> 3 (CSN)
  7 --> 4 (CE)
  ~11 --> 5 (MISO)
  ~12 --> 6 (SCK)
  ~13 --> 8 (MOSI)
 
 
  
  
-------------------------------------------------------------------------
