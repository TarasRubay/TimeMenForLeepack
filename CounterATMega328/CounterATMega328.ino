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
	Serial.begin(9600);
	mySerial.begin(9600);

	pinMode(3, INPUT);
	pinMode(5, INPUT);
	pinMode(10, INPUT);
	pinMode(11, INPUT);
	
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
	int third = 0;
	int four = 0;
	unsigned long timeRun = millis() + timeUpdate;
	while (millis() != timeRun)
	{
		
		if (digitalRead(3) == flagFirst) {
			countSpeedFirst++;
			flagFirst = !flagFirst;
		}

		if (digitalRead(5) == flagSecond) {
			countSpeedSecond++;
			flagSecond = !flagSecond;
		}


		/*if (!digitalRead(10) == flagThird) {
			flagThird = !flagThird;
			if (flagThird) {
				third++;
			}
		}

		if  (!digitalRead(11) == flagFour) {
			flagFour = !flagFour;
			if (flagFour) {
				four++;
			}
		}*/
		
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
	bool flagFirst = true;
	bool flagSecond = true;
	bool flagThird = true;
	bool flagFour = true;
	int countSpeedFirst = 0;
	int countSpeedSecond = 0;
	int countSpeedThird = 0;
	int countSpeedFour = 0;
	int FirstReader;
	int SecondReader;
	int ThirdReader;
	int FourReader;
	for (int long i = 0; i <= timeUpdate; i++)
	{
		FirstReader = digitalRead(5);// 1
		SecondReader = digitalRead(3);// 2
		ThirdReader = digitalRead(11);// 1
		FourReader = digitalRead(10);// 2
		/*long xaxaxa = 444444;
		long xaxax = 444444;
		long xaxa = 444444;
		long xax = 444444;
		long xa = 444444;
		long x = 444444;*/
		delayMicroseconds(969);

		if (FirstReader == flagFirst) {
			flagFirst = !flagFirst;
			if(flagFirst)countSpeedFirst++;
		}
		if (SecondReader == flagSecond) {
			flagSecond = !flagSecond;
			if(flagSecond)countSpeedSecond++;
		}
		if (ThirdReader == flagThird) {
			flagThird = !flagThird;
			if (flagThird)countSpeedThird++;
		}
		if (FourReader == flagFour) {
			flagFour = !flagFour;
			if (flagFour)countSpeedFour++;
		}

	}
	SendSimpleText(countSpeedFirst,countSpeedSecond,countSpeedThird,countSpeedFour);
}
void SendSimpleText(int countFirst, int countSecond, int third, int four) {
	
	/*Serial.print("{\"Sn\":");
	Serial.print(1);
	Serial.print(",\"A\":");
	Serial.print(countFirst);
	Serial.print(",\"B\":");
	Serial.print(countSecond);
	Serial.print(",\"C\":");
	Serial.print(third);
	Serial.print(",\"D\":");
	Serial.print(four);
	Serial.println("}");
	*/
	/*Serial.print(countFirst);
	Serial.print(countSecond);
	Serial.print(third);
	Serial.print(four);*/
	//Serial.println(millis());


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
	
	//CountRunTimeNew(3750); 
	//CountRunTime(2000);
	CountRunTime(60000);
	//60013 501 503 472 506
	
	
}
