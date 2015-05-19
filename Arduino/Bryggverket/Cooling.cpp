/*
   Main class to handle the coolingshedule
*/

#include "Arduino.h"
#include "Cooling.h"

Cooling::Cooling(int id, int temp)
{
	setTime(0,0,0,0,0,0);
	_temp = temp;
	_id = id;
    lcd.Begin();
    sensors.requestTemperatures();
	CurrentTemp = sensors.getTempCByIndex(0) + Calibrator;
}

boolean Cooling::Start()
{
	totTime = now();

	if(second() % 2 == 0) //Updates the temp every two seconds
	{
		sensors.requestTemperatures();
		CurrentTemp = sensors.getTempCByIndex(0) + Calibrator;
	}

	lcd.cooling(totTime, CurrentTemp, _temp);

	digitalWrite(COOLINGPUMP,HIGH);

	if(second() == 1)
	{
		Serial.print("cooling,"+String(_id)+","+String(minute(totTime)+hour(totTime)*60)+","+CurrentTemp+"/");
	}

	if(CurrentTemp <= _temp)
	{
		relay.AllLow();
		digitalWrite(BUZZER, HIGH);
		delay(500);
		digitalWrite(BUZZER, LOW);
		return false;
	}

	return true;
}

void Cooling::Unpause()
{
	setTime(totTime);
}