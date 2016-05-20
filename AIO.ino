#include "Arduino.h"
#define RELAY1 7
#if !defined(SERIAL_PORT_MONITOR)
  #error "Arduino version not supported. Please update your IDE to the latest version."
#endif

#if defined(SERIAL_PORT_USBVIRTUAL)
  // Shield Jumper on HW (for Leonardo and Due)
  #define port SERIAL_PORT_HARDWARE
  #define pcSerial SERIAL_PORT_USBVIRTUAL
#else
  // Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
  #include "SoftwareSerial.h"
  SoftwareSerial port(12, 13);
  #define pcSerial SERIAL_PORT_MONITOR
#endif

#include "EasyVR.h"

#include <stdio.h>
#include <string.h>
#include <DS1302.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display
uint8_t RST_PIN = 5; //RST pin attach to
uint8_t SDA_PIN = 6; //IO pin attach to
uint8_t SCL_PIN = 8; //clk Pin attach to
/* Create buffers */
char buf[50];
char day[10];
String comdata = "";
int numdata[7] ={ 0}, j = 0, mark = 0;
/* Create a DS1302 object */
DS1302 rtc(RST_PIN, SDA_PIN, SCL_PIN);//create a variable type of DS1302

void print_time()
{
  /* Get the current time and date from the chip */
  Time t = rtc.time();
  /* Name the day of the week */
  memset(day, 0, sizeof(day));
  switch (t.day)
  {
    case 1: 
    strcpy(day, "Sunday"); 
    break;
    case 2: 
    strcpy(day, "Monday"); 
    break;
    case 3: 
    strcpy(day, "Tuesday"); 
    break;
    case 4: 
    strcpy(day, "Wednesday"); 
    break;
    case 5: 
    strcpy(day, "Thursday"); 
    break;
    case 6: 
    strcpy(day, "Friday"); 
    break;
    case 7: 
    strcpy(day, "Saturday"); 
    break;
  }
  /* Format the time and date and insert into the temporary buffer */
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  /* Print the formatted string to serial so we can see the time */
  Serial.println(buf);
  lcd.setCursor(3,0);
  lcd.print(t.yr);
  lcd.print("-");
  lcd.print(t.mon/10);
  lcd.print(t.mon%10);
  lcd.print("-");
  lcd.print(t.date/10);
  lcd.print(t.date%10);
  lcd.print(" ");
  lcd.setCursor(4,1);
  lcd.print(day);
  lcd.setCursor(6,2);
  lcd.print(t.hr);
  lcd.print(":");
  lcd.print(t.min/10);
  lcd.print(t.min%10);
  lcd.print(":");
  lcd.print(t.sec/10);
  lcd.print(t.sec%10);
  lcd.setCursor(6,3);
  lcd.print("Hey Fayadh");
  
}


EasyVR easyvr(port);

//Groups and Commands
enum Groups
{
  GROUP_0  = 0,
  GROUP_1  = 1,
};

enum Group0 
{
  G0_ARDUINO = 0,
};

enum Group1 
{
  G1_THANKS = 0,
  G1_TIME = 1,
  G1_ON = 2,
  G1_OFF = 3,
};


int8_t group, idx;

void setup()
{
  // setup PC serial port
  pcSerial.begin(9600);

  rtc.write_protect(false);
  rtc.halt(false);
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 
  Time t(2016, 5, 19, 13, 25, 00, 5);//initialize the time
  /* Set the time and date on the chip */
  rtc.time(t);

  pinMode(RELAY1, OUTPUT); //set Relay pin mode

  // bridge mode?
  int mode = easyvr.bridgeRequested(pcSerial);
  switch (mode)
  {
  case EasyVR::BRIDGE_NONE:
    // setup EasyVR serial port
    port.begin(9600);
    // run normally
    pcSerial.println(F("---"));
    pcSerial.println(F("Bridge not started!"));
    break;
    
  case EasyVR::BRIDGE_NORMAL:
    // setup EasyVR serial port (low speed)
    port.begin(9600);
    // soft-connect the two serial ports (PC and EasyVR)
    easyvr.bridgeLoop(pcSerial);
    // resume normally if aborted
    pcSerial.println(F("---"));
    pcSerial.println(F("Bridge connection aborted!"));
    break;
    
  case EasyVR::BRIDGE_BOOT:
    // setup EasyVR serial port (high speed)
    port.begin(115200);
    // soft-connect the two serial ports (PC and EasyVR)
    easyvr.bridgeLoop(pcSerial);
    // resume normally if aborted
    pcSerial.println(F("---"));
    pcSerial.println(F("Bridge connection aborted!"));
    break;
  }

  while (!easyvr.detect())
  {
    Serial.println("EasyVR not detected!");
    delay(1000);
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER; //<-- start group (customize)
}

void action();

void loop()
{
  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)

  Serial.print("Say a command in Group ");
  Serial.println(group);
  easyvr.recognizeCommand(group);

  do
  {
    // can do some processing while waiting for a spoken command
  }
  while (!easyvr.hasFinished());
  
  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  idx = easyvr.getWord();
  if (idx >= 0)
  {
    // built-in trigger (ROBOT)
    // group = GROUP_X; <-- jump to another group X
    return;
  }
  idx = easyvr.getCommand();
  if (idx >= 0)
  {
    // print debug message
    uint8_t train = 0;
    char name[32];
    Serial.print("Command: ");
    Serial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train))
    {
      Serial.print(" = ");
      Serial.println(name);
    }
    else
      Serial.println();
	// beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // perform some action
    action();
  }
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      Serial.println("Timed out, try again...");
    int16_t err = easyvr.getError();
    if (err >= 0)
    {
      Serial.print("Error ");
      Serial.println(err, HEX);
    }
  }
  /*add the data to comdata when the serial has data */
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    
    mark = 1;
  }
  /* Use a comma to separate the strings of comdata,
  and then convert the results into numbers to be saved in the array numdata[] */
  if(mark == 1)
  {
    Serial.print("You inputed : ");
    Serial.println(comdata);
    for(int i = 0; i < comdata.length() ; i++)
    {
      if(comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13)
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
      }
    }
    /* The converted numdata add up to the time format, then write to DS1302*/
    Time t(numdata[0], numdata[1], numdata[2], numdata[3], numdata[4], numdata[5], numdata[6]);
    rtc.time(t);
   mark = 0;
    j=0;
    /* clear comdata ，in order to wait for the next input */
    comdata = String("");
    /* clear numdata */
    for(int i = 0; i < 7 ; i++) numdata[i]=0;
  }
  /* print the current time */
  print_time();
  
}

void action()
{
    switch (group)
    {
    case GROUP_0:
      switch (idx)
      {
      case G0_ARDUINO:
      
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        group = GROUP_1;
        break;
      }
      break;
    case GROUP_1:
      switch (idx)
      {
      case G1_THANKS:
      lcd.noBacklight();
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_TIME:
      lcd.backlight();
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_ON:
      digitalWrite(RELAY1,0);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_OFF:
      digitalWrite(RELAY1,1);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      }
      break;
    }
}
