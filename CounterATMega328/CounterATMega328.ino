/*
 Name:		CounterATMega328.ino
 Created:	2/6/2022 3:45:35 AM
 Author:	Rubay-PC
*/
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <stdio.h>

const byte rxPin = 9;
const byte txPin = 6;

// set up a new serial object
SoftwareSerial mySerial(rxPin, txPin);
int buttom = 2;
int count = 0;
void setup() {
	Serial.begin(56000);
	mySerial.begin(56000);

	pinMode(11, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(buttom, INPUT);
	pinMode(3, INPUT);
}
void CountRunTimeNew(unsigned long timeUpdate) {
	bool flagFirst = true;
	bool flagSecond = true;
	bool flagThird = true;
	bool flagFour = true;
	bool flag1 = false;
	bool flag2 = false;
	int countSpeedFirst = 0;
	int countSpeedSecond = 0;
	unsigned long timeRun = millis() + timeUpdate;
	int third = 0;
	int four = 0;
    //int third = analogRead(A2);
	while (millis() != timeRun)
	{
		/*flag1 = 0;
		flag2 = 0;
		if (analogRead(A0) > analogRead(A2))flag1 = true;
		if(analogRead(A1) > analogRead(A3))flag2 = true;*/
		if (!digitalRead(3) == flagFirst) {
			flagFirst = !flagFirst;
			if(!flagFirst)countSpeedFirst++;
		}

		if (!digitalRead(5) == flagSecond) {
			flagSecond = !flagSecond;
			if (!flagSecond)countSpeedSecond++;
		}
		if (digitalRead(A2) == flagThird) {
			flagThird = !flagThird;
			if (!flagThird) {
				third++;
				digitalWrite(11, HIGH);
			}
		}

		if  (digitalRead(A3) == flagFour) {
			flagFour = !flagFour;
			if (!flagFour) {
				four++;
				digitalWrite(10, HIGH);
			}
		}
		/*Serial.print(analogRead(A0));
		Serial.print(' ');
		Serial.print(analogRead(A1));
		Serial.print(' ');
		Serial.print(analogRead(A2));
		Serial.print(' ');
		Serial.println(analogRead(A3));*/

		digitalWrite(10, LOW);
		digitalWrite(11, LOW);
	}

	SendSimpleText(countSpeedFirst, countSpeedSecond, third, four);
}
void CountTime(long int timeLaps) {
	long int timeRun = 0;
	long int countSpeed = 0;
	bool flag = true;
	long int b = 0;
	for (long int i = 0; i < timeLaps; i++)
	{
	//	if (digitalRead(buttom))timeRun++;
		if (digitalRead(3) == flag) {
			flag = !flag;
			countSpeed++;
			delay(1);
		}
		else
		{
			delay(1);
		}
	}

	SendJsonData(timeRun, (countSpeed - 1) / 2);
}
void CountRunTime(long int timeUpdate) {
	int stoppedAfter = 1600;
	int countRequestFirst = 0;
	int countRequestSecond = 0;
	long int timeStop = 0;
	long int counter = 0;
	bool flagFirst = true;
	bool flagSecond = true;
	int countSpeedFirst = 0;
	int countSpeedSecond = 0;
	for (int long i = 0; i < timeUpdate; i++)
	{
		if (!digitalRead(3) == flagFirst) {
			flagFirst = !flagFirst;
			countSpeedFirst++;
		}
		
		if (!digitalRead(5) == flagSecond) {
			flagSecond = !flagSecond;
			countSpeedSecond++;
		}

			if (!digitalRead(3)) {
				countRequestFirst++;
				counter = 0;
			}

			if (!digitalRead(5)) {
				countRequestSecond++;
				counter = 0;
			}
			
			if (digitalRead(5) && digitalRead(3))counter++;
			if (counter > stoppedAfter)timeStop++;
		
		delay(1);
	}
			
	//SendJsonDataNew(1, countRequestFirst, countRequestSecond, timeStop + stoppedAfter,countSpeedFirst/2,countSpeedSecond/2);
	SendJsonDataNew(1, timeStop + stoppedAfter,countSpeedFirst,countSpeedSecond);
}
void SendSimpleText(int countFirst, int countSecond, int third, int four) {
	/*StaticJsonDocument<256> doc;
	doc["Serial"] = 2;
	doc["First"] = countFirst;
	doc["Second"] = countSecond;
	doc["millis"] = millis();*/
	//serializeJson(doc,Serial);
	//serializeJson(doc,mySerial);//{"CF":0,"CS":0}
	Serial.print("{\"Sn\":");
	Serial.print(1);
	Serial.print(",\"A\":");
	Serial.print(countFirst);
	Serial.print(",\"B\":");
	Serial.print(countSecond);
	Serial.print(",\"C\":");
	Serial.print(third);
	Serial.print(",\"D\":");
	Serial.print(four);
	Serial.print("}");

	mySerial.print("{\"Sn\":");
	mySerial.print(1);
	mySerial.print(",\"A\":");
	mySerial.print(countFirst);
	mySerial.print(",\"B\":");
	mySerial.print(countSecond);
	mySerial.print(",\"C\":");
	mySerial.print(third);
	mySerial.print(",\"D\":");
	mySerial.print(four);
	mySerial.print("}");
	//Serial.println('#');
	//Serial.println(millis());
}
void SendJsonDataNew(int serialNumber, int countRequestFirst,int countRequestSecond,long int timeStop, int avgSpeedF, int avgSpeedS) {
	digitalWrite(LED_BUILTIN, HIGH);
	
	StaticJsonDocument<512> doc;
	doc["SN"] = serialNumber;
	doc["CRF"] = countRequestFirst;
	doc["CRS"] = countRequestSecond;
	doc["TS"] = timeStop;
	doc["AVGF"] = avgSpeedF;
	doc["AVGS"] = avgSpeedS;
	//serializeJson(doc, Serial);
	serializeJson(doc, mySerial);
	digitalWrite(LED_BUILTIN, LOW);
	
}
void SendJsonDataNew(int serialNumber, long int timeStop, int avgSpeedF, int avgSpeedS) {
	digitalWrite(LED_BUILTIN, HIGH);

	StaticJsonDocument<256> doc;
	doc["SN"] = serialNumber;
	doc["TS"] = timeStop;
	doc["AVGF"] = avgSpeedF;
	doc["AVGS"] = avgSpeedS;
	StaticJsonDocument<256> doc2;
	String a;
	serializeJson(doc, a);
	doc2["jsondata"] =  a;
	//{"jsondata": "{SN:32, TS : 1600, AVGF : 9, AVGS : 7}"}
	//serializeJson(doc2, Serial);
	serializeJson(doc2, mySerial);
	digitalWrite(LED_BUILTIN, LOW);

}
void SendJsonData(long int timeRun, long int countPerTime) {
	digitalWrite(LED_BUILTIN, HIGH);
	analogWrite(11, 30);
	StaticJsonDocument<256> doc;
	doc["timeRun"] = timeRun;
	doc["countPerTime"] = countPerTime;
	serializeJson(doc, Serial);
	serializeJson(doc, mySerial);
	digitalWrite(LED_BUILTIN, LOW);
	analogWrite(11, 0);
}
void SampleJson() {
	if (!digitalRead(buttom)) {
		count++;
		digitalWrite(LED_BUILTIN, HIGH);
		delay(500);
		StaticJsonDocument<256> doc;
		doc["name"] = "nano";
		doc["count"] = count;
		// String msg = "Hello from nano ";
		serializeJson(doc, Serial);
		serializeJson(doc, mySerial);
		//Serial.println(msg + count);
		//mySerial.print(msg + count);
		delay(1000);
	}
	digitalWrite(LED_BUILTIN, LOW);
}
void loop() {
	
	CountRunTimeNew(60000);
	
	
}
