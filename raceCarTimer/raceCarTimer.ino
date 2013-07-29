#include <LiquidCrystal.h>

const int clockInt = 0;            // digital pin 2 is now interrupt 0

//the pin that transmits the clock wave
const int wavePin = 3;

const int resetPin = 9;

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
  
  pinMode(resetPin, INPUT);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  resetRace();  
  
  // if laser is in place already - be ready for the race from the get-go!
  if(getLightReading() >= LASER_LIGHT_READING_MIN)
  {
    readyForRace();
  }
}

void readyForRace()
{
  writeTextInLine("Hold steady!", 0);
  printCurrentTime();
  detachInterrupt(clockInt);
  delay(3000);     
  attachInterrupt(clockInt, clockCounter, RISING); 
  writeTextInLine("Race ready!", 0);
  
  raceStatus = RACE_READY;  
}

void resetRace(){
  raceStatus = BEFORE_RACE;
  writeTextInLine("Set up race", 0);
  printCurrentTime();
}

void resetTime(){
  minutes = 0;
  seconds = 0;
  milliSeconds = 0;
}

void writeTextInLine(String text, int lineNumber)
{
  clearLcdLine(lineNumber);
  lcd.setCursor(0,lineNumber);
  lcd.print(text);
}

void clearLcdLine(int lineNumber){
  lcd.setCursor(0, lineNumber);
  lcd.print("                ");
}


/*
* Here is where all the main time-keeping is coming from
* is called on each interrupt - that means "called each 'clockSpeed times' a second"
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

int getLightReading() 
{
  return analogRead(photoPin);
}

/*
* Initializes the race
*/
void setupRaceCondition()
{
  
}

void loop() 
{  
}
