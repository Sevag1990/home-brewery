/*
   Main class to handle the mashshedule
*/

#include "Arduino.h"
#include "MashSchedule.h"

void MashSchedule::Default()
{
	 //  _temp = 0; //temporary variable for the temperature
		// _time = 1;
		// //_steps = arrSize/2;
		// difTime = 0;
		// curStarted = false;
	sensors.begin();
	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);
	pinMode(RELAY4, OUTPUT);
	//Send request to get Temperature
	//sensors.requestTemperatures();
	//CurrentTemp = sensors.getTempCByIndex(0);

	pinMode(buttonInput,OUTPUT);
    pinMode(led, OUTPUT);
    
	lcd.Begin();
	lcd.Print("Waiting for program");
	//lcd.Print("Current Temp: ", 1); //Bortkommenterad pågrund av att den redan gör detta i funktionen receive()
	//lcd.Print(String(CurrentTemp));	
}

void MashSchedule::Receive()
{
	String content = "";

	/*while (Serial.available()){ 
		content = Serial.readString();
	}*/
	content = serialStr.Read();

	if(content != "")
	{
		int data[maxStep]; //The results will be stored here
		int i = 0;

		while(content != "")
		{
			int index = content.indexOf(",");		//We find the next comma

			if(index < 0)
			{							//När man läst ut allt ur content så ballar index ur, och då breakar vi ur loopen och nollställer content.
				content = "";
				break;
			}

			if(i==0)
			{
				name = content.substring(0,index);
			}
			else
			{
				data[i] = atol(content.substring(0,index).c_str()); //Extract the number
			}

			content = content.substring(index+1); //Remove the number from the string
			i++;
		}

		arrSize = i-2;
		_steps = arrSize/2;
		arr = new int [arrSize];
		int check = name.length();
		int checkSum = data[i-1];

		for(int j=0; j < arrSize; j++)
		{
		    arr[j] = data[j+1];
		    check += arr[j];
		}

		if(check == checkSum)
		{
			lcd.Print("Paused             ");
			lcd.Print("Mashschemed uploaded", 1);
			loaded = true;	
		}
		else
		{
			lcd.Print("Fail",1);
			loaded = false;
		}
	}

	sensors.requestTemperatures();
	CurrentTemp = sensors.getTempCByIndex(0);
	lcd.Print("Current temp: ", 2);
	lcd.Print(String(CurrentTemp), 2, 14);
	lcd.Print(String(degree), 2, 16);
	lcd.Print("C", 2, 17);

	state  = digitalRead(buttonInput);

	if(loaded && state == HIGH)
	{	randomSeed(now());
		setTime(0,0,0,0,0,0);
		lcd.Print("                    ", 0); //Just temporary, will instead implement a function clear() on the display class
		lcd.Print("                    ", 1);
		lcd.Print("                    ", 2);
		
	    _temp = 0; //temporary variable for the temperature
		_time = 1;
		//_steps = arrSize/2;
		difTime = 0;
		curStarted = false;
	    _step=1;
	    
		while(loaded)
		{			
			Start();
		}
	}
}

void MashSchedule::Start()
{	
	totTime = now();
	curTime = totTime - difTime;

	if(second() % 2 == 0) //Updates the temp every two seconds
	{
		sensors.requestTemperatures();
		CurrentTemp = sensors.getTempCByIndex(0);
	}

	lcd.Print("Totalt:");
	lcd.Print(String(minute(totTime)), 0, 7);
	lcd.Print("min ", 0, 9);
	lcd.Print("  ", 0, 14);
	lcd.Print(String(second(totTime)), 0, 14);
	lcd.Print(" sek", 0, 16);

	if(!curStarted && CurrentTemp >= arr[_temp]) //Starts counting when the current temp is right
	{		
		difTime = totTime;
		curTime = totTime - difTime;
		curStarted = true;
		someFlag=false;
	}
	lcd.Print("Aktuellt:", 1);
	lcd.Print(String(CurrentTemp), 1, 9);
	lcd.Print(String(degree), 1, 11);
	lcd.Print("C ", 1, 12);
//	lcd.Print("  ", 1, 14); //we did this later down for minutes

	lcd.Print("Steg ", 2);
	lcd.Print(String(_step), 2, 5);
	lcd.Print(": ",2,6);
	lcd.Print(String(arr[_temp]),2,8);
	lcd.Print("C ",2,10);
	lcd.Print(String(arr[_time]),2,12);
	lcd.Print(" min",2,14);

	if(_step < _steps)
	{
		lcd.Print("Steg ", 3);
		lcd.Print(String(_step+1), 3, 5);
		lcd.Print(": ",3,6);
		lcd.Print(String(arr[_temp+2]),3,8);
		lcd.Print("C ",3,10);
		lcd.Print(String(arr[_time+2]),3,12);
		lcd.Print(" min",3,14);
	}

	if (minute(curTime) <= 0 && curStarted) //Changing between min and sec on the current step
	{
		lcd.Print(String(second(curTime)), 1, 14);
		lcd.Print(" sek", 1, 16);
	}
	else if(curStarted)
	{
		lcd.Print(" ", 1, 15);
		lcd.Print(String(minute(curTime)), 1, 14);
		lcd.Print(" min", 1, 16);
	}

	if(CurrentTemp >= arr[_temp] && curStarted) //For the relay
	{	TurnOff();
		//digitalWrite(led,LOW);
	}

	else
	{
		//digitalWrite(led,HIGH);
		
		if(!someFlag)
		{
		Random();
		}
		else
		{
		AllOn();
		
		}
	}

	if(minute(curTime) >= arr[_time] && (_steps) > _step && curStarted)
	{
		_step += 1;
		_temp += 2;
		_time += 2;
		someFlag=true;
		curStarted = false;
		difTime = 0;

		lcd.Print("                    ",1);
		lcd.Print("                    ",3);
	}	
}

void MashSchedule::AllOn(){
	digitalWrite(RELAY1,HIGH);
	digitalWrite(RELAY2,HIGH);
	digitalWrite(RELAY3,HIGH);
	digitalWrite(RELAY4,HIGH);
	someFlag=false;
}

void MashSchedule::Random(){
	if (someFlag_2)
	{
	
	randNumber = random(4);
	someFlag_2=false;
	}
	switch (randNumber)
	{
		
		case  0:
		digitalWrite(RELAY1,HIGH); 
		           
		break;
		case 1:
		digitalWrite(RELAY2,HIGH);           // Turns ON Relays 2
								         
		break;
		case 2:
		digitalWrite(RELAY3,HIGH);           // Turns ON Relays 3
		break;        
		case 3:
		digitalWrite(RELAY4,HIGH);           // Turns ON Relays 4
		break;   
		
	
	
}
}
void MashSchedule::TurnOff(){
	someFlag_2=true;
	digitalWrite(RELAY1,LOW);
	digitalWrite(RELAY2,LOW);
	digitalWrite(RELAY3,LOW);
	digitalWrite(RELAY4,LOW);
}