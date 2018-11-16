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

int ServoPinOffset = 10;
Servo SEM[4];
bool AciveDI[13] = {0};
bool AciveAI[5] = {0};
byte n=0;
byte bv=0;


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
      //Serial.print(cmd);
  	  if (cmd == ' ') 
  	    continue;
      else if (cmd == '#') 
      {
        Serial.print("ASP Ver: 0.1.0"); 
      }
  	  else if (cmd == 'c')
      {
        delay(20);
        //Serial.print("Config ");
        
    		cmd = Serial.read();
        //Serial.print(cmd);

    		if (cmd == 'i')       //set as digital input with pullup
    		{
    			n = Serial.parseInt();  //number
          //Serial.print(n);
          //Serial.print(" as DIn Pullup\n");
    			pinMode(n, INPUT_PULLUP);
    			AciveDI[n-1]=1;
    		}
    		else if (cmd == 'I')       //set as digital input
    		{
    			n = Serial.parseInt();  //number
          //Serial.print(n);
          //Serial.print(" as DIn\n");
    			pinMode(n, INPUT);
    			AciveDI[n-1]=1;
    		}
    		else if (cmd == 'a')       //set as analog input
    		{
          n = Serial.parseInt();  //number
          //Serial.print(n);
          //Serial.print(" as AIn\n");
    			AciveAI[n]=1;
    		}
    		else if (cmd == 'o')       //set as output
    		{
    			n = Serial.parseInt();  //number
          //Serial.print(n);
          //Serial.print(" as Out\n");
    			pinMode(n, OUTPUT);
    		}
    		else if (cmd == 'e')       //servo
    		{
    			n = Serial.parseInt();  //number
          //Serial.print(n);
          //Serial.print(" as Servo\n");
    			int idx = n-1;
    			SEM[idx].attach(ServoPinOffset+idx);
    		}
      }
      else if (cmd == 'g')
      {
    		Serial.print("{ \"DI\":[");
        for (int i=2; i<=13; i++)
    		{
    				Serial.print(digitalRead(i));
    				Serial.print(",");
  		  }
  		  Serial.print("], ");
  		
    		Serial.print(" \"AI\":[");
        for (int i=0; i<5; i++)
    		{
    			Serial.print(map(analogRead(i),0,1024,0,100));
    			Serial.print(",");
    		}
    		Serial.print("]}\n");
      }
      else if (cmd == 's')       //set
      {
        //Serial.print("Set ");
        n = Serial.parseInt();  //number
		    //byte v = Serial.parseInt();  //value
        digitalWrite(n, 1);
        //Serial.print(n);
        //Serial.print('\n');
      }
      else if (cmd == 'r')       //set
      {
        //Serial.print("ReSet ");
        n = Serial.parseInt();  //number
        //byte v = Serial.parseInt();  //value
        digitalWrite(n, 0);
        //Serial.print(n);
        //Serial.print('\n');
      }
      //Motor via L293
      else if (cmd == 'm')       //set
      {
        //Serial.print("Motor ");
        n = Serial.parseInt();  //number
        //Serial.print(n);
        //Serial.print(';');
        int v = Serial.parseInt();  //number
        //Serial.print(v);
        //Serial.print('\n');
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
      else 
      {
       Serial.print("No Command:");
       Serial.print(cmd); 
      }
    }
  }
}
