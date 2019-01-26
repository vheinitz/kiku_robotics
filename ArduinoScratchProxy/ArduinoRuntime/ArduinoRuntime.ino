/**
Arduino run-time system for controlling the I/O from outside via the serial interface
Author: Valentin Heinitz, 2018-10-10

Description:
Intender for the use in a children-course for technics and programming. The usual kits for such 
courses - LEGO MindStorm was not affordable. The Scratch programming language for children and arduino, "glued" 
together by python, seems to be a promising alternative.

The Scratch programming language is capable of exchanging messages over TCP. It opens a server port and sends all defined variables
on the connection of the client. This initial message can be used to configure the arduino board using a name convention:

i<Num> - Digital Input, Pull Up
I<Num> - Digital Input, No Pull Up
o<Num> - Digital Output
a<Num> - Analog Input
t1 STepper Motor on D2-D5
t2 STepper Motor on D6-D9
t3 STepper Motor on D10-D13
e1 SErver Motor on Pin 10
e2 SErver Motor on Pin 11
e3 SErver Motor on Pin 12
e4 SErver Motor on Pin 13


*/

#include <Servo.h>

void stepper_do(int num, int steps);

int ServoPinOffset = 10;
Servo SEM[4];
bool AciveDI[13] = {0};
bool AciveAI[5] = {0};
byte n=0;
byte bv=0;
int DEBUG=0;

void setup() {
  Serial.begin(38400);        
  for (int p=2; p <=13; p++)
    pinMode(p, INPUT_PULLUP);
}

// g - get all Input

byte cmd=0;
void loop() 
{  
  while (1)
  {
    if (Serial.available() > 0) 
    {      
      cmd = Serial.read();
      if (DEBUG) Serial.print(cmd);
      if (cmd == ' ' || cmd == 13 || cmd == 10) 
       continue;
          
      else if (cmd == '#') 
      {
        Serial.print("ASP Ver: 0.1.0"); 
      }
      else if (cmd == 'c')
      {
        delay(20);
        if (DEBUG) Serial.print("Config ");
        
            cmd = Serial.read();
            if (DEBUG) Serial.println(cmd);

            if (cmd == 'i')       //set as digital input with pullup
            {
                n = Serial.parseInt();  //number
                if (DEBUG) Serial.print(n);
                if (DEBUG) Serial.print(" as DIn Pullup\n");
                pinMode(n, INPUT_PULLUP);
                AciveDI[n-1]=1;
            }
            else if (cmd == 'I')       //set as digital input
            {
                n = Serial.parseInt();  //number
                if (DEBUG) Serial.print(n);
                if (DEBUG) Serial.print(" as DIn\n");
                pinMode(n, INPUT);
                AciveDI[n-1]=1;
            }
            else if (cmd == 'a')       //set as analog input
            {
                n = Serial.parseInt();  //number
                if (DEBUG) Serial.print(n);
                if (DEBUG) Serial.print(" as AIn\n");
                AciveAI[n]=1;
            }
            else if (cmd == 'o')       //set as output
            {
                n = Serial.parseInt();  //number
                if (DEBUG) Serial.print(n);
                if (DEBUG) Serial.print(" as Out\n");
                pinMode(n, OUTPUT);
            }
            else if (cmd == 'e')       //servo
            {
                n = Serial.parseInt();  //number
                if (DEBUG) Serial.print(n);
                if (DEBUG) Serial.print(" as Servo\n");
                int idx = n-1;
                SEM[idx].attach(ServoPinOffset+idx);
            }
      }
      else if (cmd == 'g')
      {
        Serial.print("{ \"DI\":[");
        for (int i=0; i<=13; i++)
        {
            Serial.print(digitalRead(i));
            Serial.print(",");
        }
        Serial.print("], ");
          
        Serial.print(" \"AI\":[");
        for (int i=0; i<=7; i++)
            {
                Serial.print(map(analogRead(i),0,1024,0,100));
                Serial.print(",");
            }
            Serial.print("]}\n");
      }
      else if (cmd == 's')       //set
      {
        if (DEBUG) Serial.print("Set ");
        n = Serial.parseInt();  //number
        pinMode(n, OUTPUT);
        digitalWrite(n, 1);
        if (DEBUG) Serial.print(n);
        if (DEBUG) Serial.print('\n');
      }
      else if (cmd == 'r')       //set
      {
        if (DEBUG) Serial.print("ReSet ");
        n = Serial.parseInt();  //number
        pinMode(n, OUTPUT);
        digitalWrite(n, 0);
        if (DEBUG) Serial.print(n);
        if (DEBUG) Serial.print('\n');
      }
      //Motor via L293
      else if (cmd == 'm')       //motor
      {
        if (DEBUG) Serial.print("Motor ");
        n = Serial.parseInt();  //number
        if (DEBUG) Serial.print(n);
        if (DEBUG) Serial.print(';');
        int v = Serial.parseInt();  //number
        if (DEBUG) Serial.print(v);
        if (DEBUG) Serial.print('\n');
        if (n==1)
        {
          pinMode(2, OUTPUT);
          pinMode(3, OUTPUT);
          if (v==0)
          {
             digitalWrite(2, 0);
             digitalWrite(3, 0);
          }
          else if (v==1)
          {
             digitalWrite(2, 1);
             digitalWrite(3, 0);
          }
          else if (v==2)
          {
             digitalWrite(2, 0);
             digitalWrite(3, 1);
          }
        }
        else if (n==2)
        {
          pinMode(4, OUTPUT);
          pinMode(5, OUTPUT);
          if (v==0)
          {
             digitalWrite(4, 0);
             digitalWrite(5, 0);
          }
          else if (v==1)
          {
             digitalWrite(4, 1);
             digitalWrite(5, 0);
          }
          else if (v==2)
          {
             digitalWrite(4, 0);
             digitalWrite(5, 1);
          }
        }
      }
      //Stepper Motor
      else if (cmd == 't')     
      {
          if (DEBUG) Serial.print("Stepper ");
          int n = Serial.parseInt();  //stepper id
          if (DEBUG) Serial.print(n);
          if (DEBUG) Serial.print(';');
          int s = Serial.parseInt();  //steps
          if (DEBUG) Serial.print(s);
          if (DEBUG) Serial.print('\n');
  
        stepper_do(n,s);
      }
      else if (cmd == 'd')     
      {
          DEBUG=1;
      }
      else 
      {
         if (DEBUG) Serial.print("No Command:");
         if (DEBUG) Serial.print(cmd); 
      }
    }
  }
}

void stepper_do(int num, int steps)
{
  
  int i1 = 2;
  int i2 = 3;
  int i3 = 4;
  int i4 = 5;
  int T = 1200;
  
  if(num == 1)
  {
    i1=2; i2=3; i3=4; i4=5;  
    if ( steps < 0 )
    {
       i1=5; i2=4; i3=3; i4=2; 
    }
  }
  else if(num == 2)
  {
    i1=6; i2=7; i3=8; i4=9;
    if ( steps < 0 )
    {
       i1=9; i2=8; i3=7; i4=6;  
    }
  }
  else if(num == 3)
  {
    i1=10; i2=11; i3=12; i4=13;
    if ( steps < 0 )
    {
       i1=13; i2=12; i3=11; i4=10;  
    }
  }

  pinMode(i1, OUTPUT);
  pinMode(i2, OUTPUT);
  pinMode(i3, OUTPUT);
  pinMode(i4, OUTPUT);
  
  delayMicroseconds(100000);
  
  for ( int i=0; i < abs(steps); i++ )
  {
    digitalWrite(i1, LOW); 
    digitalWrite(i2, LOW); 
    digitalWrite(i3, LOW); 
    digitalWrite(i4, LOW);
    delayMicroseconds(T);
    digitalWrite(i1, LOW); 
    digitalWrite(i2, LOW); 
    digitalWrite(i3, LOW); 
    digitalWrite(i4, HIGH);
    delayMicroseconds(T);
    digitalWrite(i1, LOW); 
    digitalWrite(i2, LOW); 
    digitalWrite(i3, HIGH); 
    digitalWrite(i4, HIGH);
    delayMicroseconds(T);
    digitalWrite(i1, LOW); 
    digitalWrite(i2, LOW); 
    digitalWrite(i3, HIGH); 
    digitalWrite(i4, LOW);
    delayMicroseconds(T);
    digitalWrite(i1, LOW); 
    digitalWrite(i2, HIGH); 
    digitalWrite(i3, HIGH); 
    digitalWrite(i4, LOW);
    delayMicroseconds(T);
    digitalWrite(i1, LOW); 
    digitalWrite(i2, HIGH); 
    digitalWrite(i3, LOW); 
    digitalWrite(i4, LOW);
    delayMicroseconds(T);
    digitalWrite(i1, HIGH); 
    digitalWrite(i2, HIGH); 
    digitalWrite(i3, LOW); 
    digitalWrite(i4, LOW);
    delayMicroseconds(T);
    digitalWrite(i1, HIGH); 
    digitalWrite(i2, LOW); 
    digitalWrite(i3, LOW); 
    digitalWrite(i4, LOW);
    delayMicroseconds(T);
  }
  digitalWrite(i1, LOW); 
  digitalWrite(i2, LOW); 
  digitalWrite(i3, LOW); 
  digitalWrite(i4, LOW);
}