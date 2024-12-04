#include "Timer.h"
//#include "Adafruit_LiquidCrystal.h"
// Depending on the LiquidCrystal library you are able to install, it might be:
#include "LiquidCrystal.h"
#include "pitches.h"

// Sound Variables  
int buzzer = 8;
int sw = 10;
int cursLR = 0;
int cursUD = 0;
int select = 0;
int startMenu = 1;
int playPause = 0;
int noteCount = 0;

// == Song 1 ==
int song1[] = {
NOTE_E4, NOTE_C5, NOTE_B1, NOTE_F3, NOTE_C4, 
NOTE_A4, NOTE_A4, NOTE_GS5, NOTE_C5, NOTE_CS4, 
NOTE_AS4, NOTE_C5, NOTE_DS4, NOTE_CS5, NOTE_GS4, 
NOTE_C3, NOTE_E3, NOTE_DS5, NOTE_D4, NOTE_D3
};
int song1_time[] = {
2, 1, 2, 1, 1, 4, 8, 16, 8, 4, 4, 1, 8, 4, 2, 4, 4, 16, 4, 2
};

// == Song 2 ==

int song2[] = {
  NOTE_FS5, NOTE_D2, NOTE_DS5, NOTE_G2, NOTE_B3, 
  NOTE_C2, NOTE_G5, NOTE_D6, NOTE_CS5, NOTE_AS4, 
  NOTE_DS6, NOTE_D3, NOTE_CS4, NOTE_E5, NOTE_DS6,
   NOTE_E4, NOTE_B4, NOTE_F4, NOTE_E6, NOTE_DS4
};

int song2_time[] = {
  2, 2, 4, 8, 1, 8, 4, 4, 16, 8, 2, 4, 16, 8, 2, 4, 16, 4, 8, 1
};

// == Song 3 == 

int song3[] = {
  NOTE_A5, NOTE_D4, NOTE_D6, NOTE_DS3, NOTE_G4, 
  NOTE_B2, NOTE_F2, NOTE_A3, NOTE_AS2, NOTE_B5, 
  NOTE_C6, NOTE_C3, NOTE_GS3, NOTE_G2, NOTE_FS5, 
  NOTE_AS4, NOTE_GS2, NOTE_CS3, NOTE_C3, NOTE_AS2
};

int song3_time[] = {
  1, 2, 16, 4, 16, 2, 16, 1, 1, 2, 1, 8, 2, 16, 8, 1, 16, 4, 1, 2
};

//int songs[] = {1,2,3};

// LCD variables
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Depending on the LiquidCrystal library you are able to install, it might be:
 LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Task Structure Definition
typedef struct task {
   int state;                  // Tasks current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;


const unsigned char tasksNum = 4;
task tasks[tasksNum]; // We have 4 tasks

// Task Periods

const unsigned long periodLCDOutput = 100;
const unsigned long periodJoystickInput = 100;
const unsigned long periodSoundOutput = 100;
const unsigned long periodController = 500;


// GCD 
const unsigned long tasksPeriodGCD = 100;

// Task Function Definitions
int TickFct_LCDOutput(int state);
int TickFct_JoystickInput(int state);
int TickFct_SoundOutput(int state);
int TickFct_Controller(int state);

// Task Enumeration Definitions
enum LO_States {LO_init, LO_MenuOptionA, LO_MenuOptionB, LO_MenuOptionC, LO_MenuOptionD, displaySong};
enum JI_States {JI_init, StartJ, Left, Right, Up, Down, Select, PlayJ, PauseJ};
enum SO_States {SO_init, SO_SoundOn, SO_SoundOff};
enum C_States {C_init, C_T1};



void TimerISR() { // TimerISR actually defined here
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
     if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = 0;
     }
     tasks[i].elapsedTime += tasksPeriodGCD;
  }
}


void LCDWriteLines(String line1, String line2) {
  lcd.clear();          
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

// Task Function Definitions

int menuOption = 0;
int songOption = 0;




// Task 1
/*
int cursLR = 0;
int cursUD = 0;
int select = 0;
int startMenu = 0;
int playPause = 0;
*/
int TickFct_LCDOutput(int state) {
  switch (state) { // State Transitions
    case LO_init:
      state = LO_MenuOptionA;
      LCDWriteLines("Song 1  Song 2", "Song 3  Start");
      //lcd.cursor();
      lcd.blink();
    break;
    case LO_MenuOptionA:
      if((select == 1) && (songOption != 0)) {
        state = displaySong;
        //LCDWriteLines("Song 1  Song 2", "Song 3  Start");
        //lcd.setCursor(6, 1);
        //lcd.blink();
      }
      else if((cursUD == 0) && (cursLR == 1))
      {
        state = LO_MenuOptionB;
      }
      else if((cursUD == 1) && (cursLR == 0))
      {
        state = LO_MenuOptionC;
      }
      else if((cursUD == 1) && (cursLR == 1))
      {
        state = LO_MenuOptionD;
      }
    break;
    case LO_MenuOptionB:
      if((cursUD == 0) && (cursLR == 0))
      {
        state = LO_MenuOptionA;
      }
      else if((cursUD == 1) && (cursLR == 0))
      {
        state = LO_MenuOptionC;
      }
      else if((cursUD == 1) && (cursLR == 1))
      {
        state = LO_MenuOptionD;
      }

      //LCDWriteLines("Lab 7", "Sound System");
      
    break;
    case LO_MenuOptionC:
      if((cursUD == 0) && (cursLR == 0))
      {
        state = LO_MenuOptionA;
      }
      else if((cursUD == 1) && (cursLR == 1))
      {
        state = LO_MenuOptionD;
      }
      else if((cursUD == 0) && (cursLR == 1))
      {
        state = LO_MenuOptionB;
      }
    break;

      case LO_MenuOptionD:
      if((cursUD == 0) && (cursLR == 0))
      {
        state = LO_MenuOptionA;
      }
      else if((cursUD == 1) && (cursLR == 0))
      {
        state = LO_MenuOptionC;
      }
      else if((cursUD == 0) && (cursLR == 1))
      {
        state = LO_MenuOptionB;
      }
    break;

    case displaySong:
      if(startMenu == 0)
      {
        state = LO_init;
      }

    break;
  }

  switch (state) { // State Actions
    case LO_init:

    break;
    case LO_MenuOptionA:
      //lcd.setCursor(7, 1);
      //lcd.blink();
      if(songOption == 0)
      {
        LCDWriteLines("Song 1  Song 2", "Song 3  Start");
      }

    break;
    case LO_MenuOptionB:
      //lcd.setCursor(0, 1);
      //lcd.blink();
      if(songOption == 3)
      {
        LCDWriteLines("Song 1  Song 2", "Song 3* Start");
      }
      else if(songOption == 0)
      {
        LCDWriteLines("Song 1  Song 2", "Song 3  Start");
      }
    break;
    case LO_MenuOptionC:
      //lcd.setCursor(7, 0);
      //lcd.blink();
      if(songOption == 2)
      {
        LCDWriteLines("Song 1 *Song 2", "Song 3  Start");
      }
      else if(songOption == 0)
      {
        LCDWriteLines("Song 1  Song 2", "Song 3  Start");
      }
    break;
    case LO_MenuOptionD:
      //lcd.setCursor(6, 0);
      //lcd.blink();
      if(songOption == 1)
      {
        LCDWriteLines("Song 1* Song 2", "Song 3  Start");
      }
      else if(songOption == 0)
      {
        LCDWriteLines("Song 1  Song 2", "Song 3  Start");
      }
    break;
    case displaySong:
      //LCDWriteLines("Current Song", "Pause  Play");
      if(playPause == 0)
      {
        if(songOption == 1)
        {
          LCDWriteLines("Playing Song 1", "Pause  *Play");
        }
        else if(songOption == 2)
        {
          LCDWriteLines("Playing Song 2", "Pause  *Play");
        }
        else if(songOption == 3)
        {
          LCDWriteLines("Playing Song 3", "Pause  *Play");
        }
        
      }
      else if(playPause == 1)
      {
        if(songOption == 1)
        {
          LCDWriteLines("Playing Song 1", "Pause*  Play");
        }
        else if(songOption == 2)
        {
          LCDWriteLines("Playing Song 2", "Pause*  Play");
        }
        else if(songOption == 3)
        {
          LCDWriteLines("Playing Song 3", "Pause*  Play");
        }
      }
    break;
  }
  return state;
}



// Task 2
int TickFct_JoystickInput(int state) {

  switch (state) { // State Transitions
    case JI_init:
      state = StartJ;
    break;
    case StartJ:
      if(analogRead(A0) < 400)
      {
        state = Left;
      }
      else if(analogRead(A1)> 700)
      {
        state = Up;
      }
      else if(analogRead(A1) < 400)
      {
        state = Down;
      }
    break;
    case Left:
      if(select == 1)
      {
        state = Select;
      }
      if(analogRead(A0) > 700)
      {
        state = Right;
      }
      else if(analogRead(A1) < 300)
      {
        state = Down;
      }
      else if(analogRead(A1) > 700)
      {
        state = Up;
      }
    break;
    case Right:
      if(select == 1)
      {
        state = Select;
      }
      if(analogRead(A0) < 300)
      {
        state = Left;
      }
      else if(analogRead(A1) < 300)
      {
        state = Down;
      }
      else if(analogRead(A1) > 700)
      {
        state = Up;
      }
    break;
    case Up:
      if(select == 1)
      {
        state = Select;
      }
      if(analogRead(A0) < 300)
      {
        state = Left;
      }
      else if(analogRead(A0) > 700)
      {
        state = Right;
      }
       else if(analogRead(A1) < 300)
      {
        state = Down;
      }
    break;

    case Down:
      if(select == 1)
      {
        state = Select;
      }
      if(analogRead(A0) < 300)
      {
        state = Left;
      }
      else if(analogRead(A0) > 700)
      {
        state = Right;
      }
      else if(analogRead(A1) > 700)
      {
        state = Up;
      }
    break;

    case Select:
      if(startMenu == 0)
      {
        state = StartJ;
      }
      else
      {
        state = PlayJ;
      }
    break;
    case PlayJ:
      if(startMenu == 0)
      {
        state = StartJ;
      }
      if(analogRead(A0) < 300)
      {
        state = PauseJ;
      }
    break;
    case PauseJ:
      if(startMenu == 0)
      {
        state = StartJ;
      }
      if(analogRead(A0) > 700)
      {
        state = PlayJ;
      }
    break;
  }

   switch (state) { // State Actions
    case StartJ:
      cursLR = 0;
      cursUD = 0;
      menuOption = 0;
      select = 0;
      startMenu = 1;
      noteCount = 0;
      songOption = 0;

      lcd.setCursor(7, 1);
      lcd.blink();
      //songOption = 0;
      //LCDWriteLines("Lab 7", "Sound System");
    break;
    case Left:

      //int input = analogRead(A0);
      cursLR = 1;
      if(cursUD == 0)
      {
        lcd.setCursor(6, 1);
        lcd.blink();
      }
      else
      {
        lcd.setCursor(6, 0);
        lcd.blink();
      }
      if(digitalRead(sw) == 0)
      {
        if((cursUD == 0) && (select == 0))
        {
          songOption = 3;
        }
        else if(select == 0)
        {
          songOption = 1;
        }
      }
      //songOption = 1;
    break;
    case Right:
      cursLR = 0;
      if(cursUD == 0) 
      {
        lcd.setCursor(7, 1);
        lcd.blink();
      }
      else
      {
        lcd.setCursor(7, 0);
        lcd.blink();
      }
      if(digitalRead(sw) == 0)
      {
        if((cursUD == 0) && (select == 0))
        {
          if(songOption != 0)
          {
            select = 1;
          }
          
        }
        else if(select == 0)
        {
          songOption = 2;
        }
      }
      //int input = analogRead(A0);
     
    break;

    case Up:
      cursUD = 1;
        if(cursLR == 0)
      {
        lcd.setCursor(7, 0);
        lcd.blink();
      }
      else
      {
        lcd.setCursor(6, 0);
        lcd.blink();
      }

      if(digitalRead(sw) == 0)
      {
        if((cursLR == 0)&&(select == 0))
        {
          songOption = 2;
        }
        else  if((select == 0))
        {
          songOption = 1;
        }
      }
    break;

    case Down:

      cursUD = 0;
       if(cursLR == 0)
      {
        lcd.setCursor(7, 1);
        lcd.blink();
      }
      else
      {
        lcd.setCursor(6, 1);
        lcd.blink();
      }


      if(digitalRead(sw) == 0)
      {
        if((cursLR == 0) &&(select == 0))
        {
          if(songOption != 0)
          {
            select = 1;
          }
        }
        else if((select == 0))
        {
          songOption = 3;
        }
      }
    break;

    case Select:

    break;

    case PlayJ:
      lcd.setCursor(7, 1);
      lcd.blink();
      if(digitalRead(sw) == 0)
      {
        playPause = 0;
        select = 1;
      }
      if(noteCount == 10)
      {
        startMenu = 0;
      }
    break;
      
    case PauseJ:
      lcd.setCursor(5, 1);
      lcd.blink();
      if(digitalRead(sw) == 0)
      {
        playPause = 1;
        select = 0;
      }
      if(noteCount == 10)
      {
        startMenu = 0;
      }
    break;
  }



  return state;
}



// Sound Output
int counter = 0;
int note = 0;
int TickFct_SoundOutput(int state) {
  switch (state) { // State Transitions
    case SO_init:
      state = SO_SoundOn;
    break;

    
    case SO_SoundOn:
      if((songOption == 1) && (select ==1))
      {
        if(counter >= song1_time[note]) {
         state = SO_SoundOff;
         note++;
         noteCount++;
         counter = 0;
         note = note % 20;
        }
      }
      else if((songOption == 2)&& (select ==1))
      {
        if(counter >= song2_time[note]) {
         state = SO_SoundOff;
         note++;
         noteCount++;
         counter = 0;
         note = note % 20;
        }
      }
      else if((songOption ==3)&& (select ==1))
      {
        if(counter >= song2_time[note]) {
         state = SO_SoundOff;
         note++;
         noteCount++;
         counter = 0;
         note = note % 20;
        }
      }
      
    break;


    case SO_SoundOff:
      state = SO_SoundOn;
    break;
    
  }
   switch (state) { // State Actions
    case SO_SoundOn:
    /*if( analogRead(A0) < 400)
    {
      if(songOption != 0)
      {
        songOption--;
        delay(1000);
      }
    }
    else if(analogRead(A0) > 800)
    {
      if(songOption != 2)
      {
        songOption++;
        delay(1000);
      }
    }*/
    

    if((songOption == 1) && (select ==1))
    {
      tone(buzzer, song1[note], periodSoundOutput * song1_time[note]);
      counter++;
    }
    else if((songOption ==2) && (select ==1))
    {
      tone(buzzer, song2[note], periodSoundOutput * song2_time[note]);
      counter++;
    }
    else if((songOption == 3) && (select ==1))
    {
      tone(buzzer, song3[note], periodSoundOutput * song3_time[note]);
      counter++;
    }
      
    break;
    case SO_SoundOff:
      noTone(buzzer);
    break;

  }
  return state;
}

// Task 4 (Unused)
int TickFct_Controller(int state) {
  switch (state) { // State Transitions
    case 0:
    break;
  }

   switch (state) { // State Actions
    case 0:
    break;
  }
  return state;
}



void InitializeTasks() {
  unsigned char i=0;
  tasks[i].state = LO_init;
  tasks[i].period = periodLCDOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_LCDOutput;
  ++i;
  tasks[i].state = JI_init;
  tasks[i].period = periodJoystickInput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_JoystickInput;
  ++i;
  tasks[i].state = SO_init;
  tasks[i].period = periodSoundOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_SoundOutput;
  ++i;
  tasks[i].state = C_init;
  tasks[i].period = periodController;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Controller;

}

void setup() {
  
  // Initialize Inputs
  // put your setup code here, to run once:
  InitializeTasks();
  pinMode(sw, INPUT_PULLUP);
  TimerSet(tasksPeriodGCD);
  TimerOn();
  Serial.begin(9600);
  // Initialize Outputs
  lcd.begin(16, 2);
  //lcd.setCursor(7, 1);

}

void loop() {
  // put your main code here, to run repeatedly:
  // Task Scheduler with Timer.h
  //TickFct_JoystickInput();
  //Serial.println(songOption);
  //Serial.println(analogRead(A0));
  //Serial.println(analogRead(A1));
  //Serial.println(menuOption);
  delay(500);
  
  //LCDWriteLines("Lab 7", "Sound System");
  //Serial.println(digitalRead(sw));
  Serial.println(cursLR);
  Serial.println(cursUD);
  //while(!TimerFlag) {}
  //TimerFlag = 0;
  
  
}
