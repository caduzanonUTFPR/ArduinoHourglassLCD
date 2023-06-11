# ArduinoHourglassLCD
 Digital Hourglass with Arduino - Uses LCD display to set the time.
## Components in use:
 - Arduino Uno
 - 2 8x8 LED Matrices with MAX7219 controller;
 - 20x4 LED Display;
 - 2 pushbuttons (Pull-Up resistor is done internally);
 - MPU6050 Accelerometer;
 - Passive buzzer (not needed for basic functionality).
## How to use:
### Necessary Arduino Libraries:
In order to be able to compile the code, you will need to install the "LiquidCrystal", "Wire" and "MPU6050_tockn" libraries. The LedControl.h and Delay.h files are included in the folder, I advise you to download the entire folder and compile from there.
### Circuit connections:
Here's a simplified diagram showing all the connections needed to get it working. Note that the buzzer is an optional part and can be removed.
![](/Diagrama-EN.jpg)
## How it works:
You turn it on, a message on the LCD display appears, as it is calibrating the accelerometer. When it finishes, the loop starts. If the hourglass is "standing", it'll start counting. It has three modes, that can be switched into by pressing the button in pin 3:
- Set Minutes
- Set Seconds
- Hourglass
### Set Minutes:
This mode will make it possible to increase or decrease the value stored in the Minutes part. To increase, give the button in pin 2 a press. To decrease, give the button in pin 3 a 'short' press. Note that the duration of a long press is determined by the variable "DEBOUNCE_THRESHOLD" on the code. The time values will be displayed on the LCD display. 
### Set Seconds:
Same behavior as Set Minutes, but deals with the value stored in the Seconds. Default time is set to 20 seconds, you may change that to whatever you want. I picked a 'small' value to allow for quick inspection of the functions. 
### Hourglass:
The default and main mode. This is where all the action takes place - "particles" fall from the top matrix to the bottom matrix, which are determined by the accelerometer reading. A set of functions is used to tell if the "particles" can move or not, allowing you to "lay" the hourglass sideways and see no movement. 

## Original "Inspiration":
The original code comes from [this project](https://www.instructables.com/Arduino-Hourglass/). The main difference between that version and this one is the renaming of a few functions, the presence of the MPU6050 and the addition of the LCD display. 

## To-Do:
- [ ] Translate this README to Brazilian Portuguese
- [ ] Translate the text shown in the LCD display and Serial prints to English
- [ ] Check all three boxes when everything is done :D