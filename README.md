# Gyroscopic Remote Control Car
## ECE5780_MiniProject
Nicholas Eloff

Carter Edginton

Beverly Yee

Krishna Dhekar

# High-Level Description
This project's goal was to convert an average off the shelf remote control car and convert it to a gyroscopic controled car. 

This was accomplished by interfacing our STM32F-Discovery board (henceforth referred to as the STM32 board) to the transmitter from the remote. We first attempted to interface with the motors of the car but we were not able to provide enough current to run them. We then tried to use the circuit board of the car itself and this proved to be futile. After discussions and ideas from our TA we were able to interface with the board of the transmitter. We accomplished this by soldering wires onto each of the leads (left = orange wire, right = red wire, forward = yellow wire, backward = brown wire). We also soldered a wire that acted as ground (black wire).

![RemotePCB](https://github.com/NicholasEloff/ECE5780_MiniProject/assets/16803011/b2a3dba2-d4d0-482f-93d2-ccb04cbe7aed)

The next task was making sure that the STM32 board would be able output the correct voltage that we needed. As we were using the oscilloscope, we found that the transmitter board would need an input of 5V--the 3.3 was not sufficient to run it. After a little bit of research, we found that there are certain pins on the STM32 that output 5V. All we needed to do was add a couple of pull-up resistors and enable the pins.

The longest and most tedious part of this project was getting our gyroscope code to work. We had some previous code that was mostly working, but it took a lot of time for us to figure it out. The code was flashing the LEDs constantly and as soon as we took out the code that toggled them, everything would break. Carter suggested we just turn the LED's off rather than comment out the code and this solved our problem. 

We also encountered a problem that the gyroscope would desync very quickly and we had to add some code to rectify this. After this problem was solved, we needed to hook the gyroscope to the 5v pins that were wired to the transmitter leads. This proved to be challenging in making this work. Beverly was the MVP when it came to figuring out the gyroscope code. She was able to figure out the bugs that were in the orginal code and hook everything up to the car. She spent multiple hours debugging and trying new things to get it to work.

In the end we were able to accomplish our original goal of making a car that was controlled by hand movements with data coming from the gyroscope. It was a fun, frustrating, and good project to learn from. 

# How to Setup/Use
You will need the transmitter and RC car, a STM32F072RBT6 microcontroller, a breadboard (or two), jumper wires, a handful of 1k resistors, and sufficient batteries--4 AA batteries for the car and a 9V battery to power the transmitter.
From the transmitter PCB, there will be 4 bendable contacts (2 of them will be connected to each other). Underneath each of these flexible parts, there is a smaller contact on the pcb which looks like a staple. To either one of the bendable contacts(it will work for either one, and they do not need to be connected), you will solder a jumper wire to one of them, and connect the other end to a GND pin. 

To each of these four "staples", solder a jumper wire to them and connect them to a breadboard. for each of those, connect a 1k resistor in series to one of the 5v tolerant pins (in our code, we used PB6, PB7, PB8, PB9). You will initiate the gyroscope via I2C communication by setting up 2 pins and adding 1k resistors to them. See below for the pin connections.

![image](https://github.com/NicholasEloff/ECE5780_MiniProject/assets/16803011/3d1bb5d1-fec3-4310-9a81-fb23b0542e35)

Our development environment for this project was Keil uVision5. You can open the project file to open all the files and be able to download the built files to the board. 

To use the project, have the car on and powered, have the transmitter powered with a 9 volt battery, and have the STM32 powered. 

Once the board has been reset, then you can tilt the board in 4 directions to send commands to the RC car. The corresponding light will depend on the direction you rotated the board.

Once a light turns on, then the transmitter should be sending information to the RC car commanding it to turn or drive. 
