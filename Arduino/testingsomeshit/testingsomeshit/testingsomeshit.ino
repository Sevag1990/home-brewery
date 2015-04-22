#include <Time.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
LiquidCrystal_I2C lcd(0x27,20,4);
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  255
boolean chek=true;

//Defines our Pin
#define ONE_WIRE_BUS A0

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
int CurrentTemp;

int buttonInput = 2;
int total;
int aktuell;

time_t t=0;

void setup()
{
	Serial.begin(9600);
	lcd.init();
	lcd.backlight();
	sensors.begin();
	time_t pctime = 0;
	
	

	  /* add setup code here, setup code runs once when the processor starts */

}

void loop()
{
	sensors.requestTemperatures();
	CurrentTemp =sensors.getTempCByIndex(0);
	lcd.print("Total: ");lcd.print(minute());lcd.print(" min "); lcd.print(second());lcd.print(" sek");  
	 
	
	
	//clear the screen
	
	
			
			
	if (Serial.available())
	{
	{	if(chek)
		t = now();
		chek=false;
	}
	
		aktuell=now()-t;
			if (minute(aktuell)==0)
			{
			 lcd.setCursor(0,1);
			 lcd.print("Aktuellt: ");lcd.print(CurrentTemp);lcd.print("C");lcd.print((char)223);lcd.print(second(aktuell)); lcd.print(" sek");
			  
			 }
			 else{
				  lcd.setCursor(0,1);
			 	 lcd.print("Aktuellt: ");lcd.print(CurrentTemp);lcd.print("C");lcd.print((char )223); lcd.print(minute(aktuell));lcd.print(" min");
			 }
			 
		 }
		
		
		
	
	
	delay(2000);
	lcd.clear();
	

}