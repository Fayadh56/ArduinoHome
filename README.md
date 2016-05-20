# ArduinoProjects
Some of my home automation projects involving the arduino micro-controller  

The first project here, was something I've always wanted to do. Control the lights in my house using voice commands, to do that I used an 8 Channel Relay connected to AC mains, an Arduino Uno, and the Easy VR 3 voice recognition shield from sparkfun. 

The provided Easy VR software (windows only) was a tremendous help in coding the actual voice commands due to the built in code gnerator. 

If you'd like to know more please visit the following links,
http://www.veear.eu/tag/easyvr-3/
https://www.sparkfun.com/products/13316
http://www.veear.eu/files/EasyVR-Commander.zip (easy VR commander software)
https://github.com/RoboTech-srl/EasyVR-Arduino/releases/download/v1.6/EasyVR-Arduino-library-1.6.zip (the required library)
http://www.veear.eu/files/EasyVR%203%20User%20Manual%201.0.11.pdf (users manual for the shield)

My plans moving forward, are to incorporate some kind of security/password to trigger the start of the voice recognition, and to be able to incorporate several more functions to it. Which is what I tried with the All in one code I have on this repo, 
as I tried incorporating an LCD display, and a RTC that is also triggered by voice commands. 

However the arduino board started processing really slow, and I need to refine the code a bit more before I get a fully working prototype. 

