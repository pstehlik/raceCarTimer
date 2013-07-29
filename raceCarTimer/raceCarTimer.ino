#include <LiquidCrystal.h>

// digital pin 2 is interrupt 0 - make sure to have pin 2 (which is the interrupt) connected to pin 3 (which is the clock)
const int clockInt = 0;

//the pin that transmits the clock wave
const int wavePin = 3;

//how many times the click should tick each secon (millisecond resolution)
const int clockSpeed = 1000;

//read the light input from this analog pin
const int photoPin = 0; 

const int LASER_LIGHT_READING_MIN = 1000;

const int MIN_SEC_RACE_TIME = 3;

int milliSeconds = -1; // counts rising edge clock signals and as the clockSpeed it 1000 - also the milli second counter
int seconds = -1;
int minutes = -1;

const int BEFORE_RACE = -1;
const int RACE_READY = 0;
const int RACE_RUNNING = 1;
const int RACE_OVER = 2;

int raceStatus = BEFORE_RACE;

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

void setup() 
{
  Serial.begin(9600);
  //listen to the clock wave on interrupt with the given speed
  attachInterrupt(clockInt, clockCounter, RISING);
  pinMode(photoPin, INPUT);  
  pinMode(wavePin, OUTPUT);
  //sets the clock speed to send on the respective pin
  //basically connect the wavePin to the clockInt and the wave coming from here
  //is read by the interrupt above
  tone(wavePin, clockSpeed);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  resetRace();  
  
  // if laser is in place already - be ready for the race from the get-go!
  if(getLightReading() >= LASER_LIGHT_READING_MIN)
  {
    readyForRace();
  }
}

/*
* Sets status, timers and all that to the 'ready to race' state
* A break in the light source should then start the actual race
*/
void readyForRace()
{
  writeTextInLine("Hold steady!", 0);
  resetTime();
  printCurrentTime();
  detachInterrupt(clockInt);
  delay(3000);     
  attachInterrupt(clockInt, clockCounter, RISING); 
  writeTextInLine("Race ready!", 0);
  
  raceStatus = RACE_READY;  
}

/*
* Sets status, timers and all that to the initial state - when not even a light-source is pointed
* at the sensor
*/
void resetRace(){
  raceStatus = BEFORE_RACE;
  writeTextInLine("Set up race", 0);
  printCurrentTime();
}

/*
* Resets all time keeping variables
*/
void resetTime(){
  minutes = 0;
  seconds = 0;
  milliSeconds = 0;
}

/*
* Clears the line 'lineNumber'
* And then fills it with the 'text'
*/
void writeTextInLine(String text, int lineNumber)
{
  clearLcdLine(lineNumber);
  lcd.setCursor(0,lineNumber);
  lcd.print(text);
}

/*
* Fills the line 'lineNumber' in the LCD with spaces
*/
void clearLcdLine(int lineNumber){
  lcd.setCursor(0, lineNumber);
  lcd.print("                ");
}


/*
* Here is where all the main time-keeping is coming from and basically the 'main method' that is responsible
* for printing out time and keeping the status of the current race etc.
* 
* It is called on each interrupt - that means "called each 'clockSpeed times' a second"
*/
void clockCounter() 
{ 
  if(raceStatus == BEFORE_RACE)
  {
    if(getLightReading() >= LASER_LIGHT_READING_MIN)
    {
      resetTime();
      readyForRace();
    }  
    return;
  } else if(raceStatus == RACE_READY) 
  {
    if(getLightReading() < LASER_LIGHT_READING_MIN)
    {
      writeTextInLine("Race running!", 0);
      clearLcdLine(1);
      raceStatus = RACE_RUNNING;
    }
  }
  
  if(raceStatus == RACE_RUNNING)
  {
    if(MIN_SEC_RACE_TIME <= seconds && getLightReading() < LASER_LIGHT_READING_MIN)
    {
      detachInterrupt(clockInt);
      raceStatus == RACE_OVER;      
      if(MIN_SEC_RACE_TIME == seconds && milliSeconds < 500)
      {
        writeTextInLine("Race time <=3s", 0);
        writeTextInLine("Please repeat", 1);        
      } else {
        writeTextInLine("Final time", 0);
        printCurrentTime();        
      }
    } else {
      milliSeconds ++; // with each clock rise add 1 (1000th of a second)
    
      if(milliSeconds % 500 == 0)
      {
        printCurrentTime();
      }
      
      if(milliSeconds >= clockSpeed - 1) 
      {         
        seconds ++;          // after one clock cycle, increase the seconds
        milliSeconds = 0;
      }
    }
  }
}

/*
* Prints the current clock time to serial as well as the LCD (on the second line)
*/
void printCurrentTime()
{
  Serial.print("Clock at:");
  Serial.println(minutes);
  Serial.println(seconds);
  Serial.println(milliSeconds);
  
  lcd.setCursor(0,1);
  lcd.print(minutes);
  lcd.print("m ");
  lcd.print(seconds);
  lcd.print("s ");
  lcd.print(milliSeconds);
  lcd.print("ms ");
}

/*
* Convenience method for reading the light sensor
*/
int getLightReading() 
{
  return analogRead(photoPin);
}


void loop() 
{  
}
