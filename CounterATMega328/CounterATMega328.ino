/*
 Name:		CounterATMega328.ino
 Created:	2/6/2022 3:45:35 AM
 Author:	Rubay-PC
*/
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const byte rxPin = 9;
const byte txPin = 6;

// set up a new serial object
SoftwareSerial mySerial(rxPin, txPin);
int buttom = 2;
int count = 0;
void setup() {
	Serial.begin(9600);
	mySerial.begin(9600);

	pinMode(11, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(buttom, INPUT);
	pinMode(3, INPUT);
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
	CountRunTime(58000);
	//CountTime(5000);
}
