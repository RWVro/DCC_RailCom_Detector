# DCC-RailCom_Detector
 
Note Serial USB port speed is 500000 bits per second.

Do not rename Port2 because of the ESP32 microControler.

+++++


I have noticed that locomotives from some manufacturers
do not answer if you request the address with address 0.
The solution is as follows:

Increase the address by 1 and test again, stop when the address is found
