/*
 Name:		CounterATMega328.ino
 Created:	2/6/2022 3:45:35 AM
 Author:	Rubay-PC
*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <stdio.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print  ("0.10 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}

void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print  ("2 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}

//A,B,C,D,E,F,G,H,I,J,K,L,M,N
const byte rxPin = 3;
const byte txPin = 5;
bool flagButt = true;
int ReaderButt;
int A = 0;
int B = 0;
int C = 0;
int D = 0;
int E = 0;
int F = 0;
int G = 0;
int H = 0;
float I = 0;
float J = 0;
float K = 0;
float L = 0;
float M = 0;
float N = 0;
int ReaderA = 0;
int ReaderB = 0;
int ReaderC = 0;
int ReaderD = 0;
int ReaderE = 0;
int ReaderF = 0;
int ReaderG = 0;
int ReaderH = 0;
int ReaderI = 0;
int ReaderJ = 0;
int ReaderK = 0;
int ReaderL = 0;
int ReaderM = 0;
int ReaderN = 0;
bool flagA = true;
bool flagB = true;
bool flagC = true;
bool flagD = true;
bool flagE = true;
bool flagF = true;
bool flagG = true;
bool flagH = true;
bool flagI = true;
bool flagJ = true;
bool flagK = true;
bool flagL = true;
bool flagM = true;
bool flagN = true;
// set up a new serial object
SoftwareSerial mySerial(rxPin, txPin);
int buttom = 12;
bool ADXL345conn = true;
void setup(void) {
#ifndef ESP8266
	while (!Serial); // for Leonardo/Micro/Zero
#endif
	Serial.begin(9600);
	mySerial.begin(9600);
	for (uint8_t t = 15; t > 0; t--) {
		Serial.print("[SETUP] WAIT:  ");
		Serial.println(t);
		Serial.flush();
		delay(1000);
	}
	if (!accel.begin())
	{
		/* There was a problem detecting the ADXL345 ... check your connections */
		Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
		ADXL345conn = false;
	}
	else
	{
		accel.setRange(ADXL345_RANGE_16_G);
		displaySensorDetails();
		displayDataRate();
		displayRange();
	}
	
	pinMode(2, INPUT);
	pinMode(4, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	pinMode(11, INPUT);
	pinMode(10, INPUT);
	pinMode(9, INPUT);
	pinMode(6, INPUT);
	
	
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(buttom, INPUT);
	
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
	mySerial.print(2);
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
void CountTipa(bool writeSerial) {
	ReaderButt = digitalRead(buttom);
	if (ReaderButt == flagButt) {
		flagButt = !flagButt;
		if (flagButt) {
			digitalWrite(LED_BUILTIN, HIGH);
			SendTipaJson(writeSerial);
			delay(1);
			digitalWrite(LED_BUILTIN, LOW);
		}
	}
	ReaderA = digitalRead(2);// 1
	ReaderB = digitalRead(4);// 2
	ReaderC = digitalRead(7);// 1
	ReaderD = digitalRead(8);// 2
	ReaderE = digitalRead(11);// 2
	ReaderF = digitalRead(10);// 2
	ReaderG = digitalRead(9);// 2
	ReaderH = digitalRead(6);// 2
	if (ReaderA == flagA) {
		flagA = !flagA;
		if (flagA)A++;
	}
	if (ReaderB == flagB) {
		flagB = !flagB;
		if (flagB)B++;
	}
	if (ReaderC == flagC) {
		flagC = !flagC;
		if (flagC)C++;
	}
	if (ReaderD == flagD) {
		flagD = !flagD;
		if (flagD)D++;
	}
	if (ReaderE == flagE) {
		flagE = !flagE;
		if (flagE)E++;
	}
	if (ReaderF == flagF) {
		flagF = !flagF;
		if (flagF)F++;
	}
	if (ReaderG == flagG) {//A,B,C,D,E,F,G,H,I,J,K,L,M,N
		flagG = !flagG;
		if (flagG)G++;
	}
	if (ReaderH == flagH) {
		flagH = !flagH;
		if (flagH)H++;
	}
	Accelerometer(ADXL345conn);
}
void SendTipaJson(bool writeSerial) {
	if (writeSerial) {
	/*Serial.print("{\"SN\":");
	Serial.print(2);
	Serial.print(",\"HO\":");
	Serial.print(22);
	Serial.print(",\"MI\":");
	Serial.print(22);
	Serial.print(",\"SE\":");
	Serial.print(22);*/
	Serial.print(",\"A\":");
	Serial.print(A);
	Serial.print(",\"B\":");
	Serial.print(B);
	Serial.print(",\"C\":");
	Serial.print(C);
	Serial.print(",\"D\":");
	Serial.print(D);
	Serial.print(",\"E\":");
	Serial.print(E);
	Serial.print(",\"F\":");
	Serial.print(F);
	Serial.print(",\"G\":");
	Serial.print(G);
	Serial.print(",\"H\":");
	Serial.print(H);
	Serial.print(",\"I\":");
	Serial.print(I);
	Serial.print(",\"J\":");
	Serial.print(J);
	Serial.print(",\"K\":");
	Serial.print(K);
	Serial.print(",\"L\":");
	Serial.print(L);
	Serial.print(",\"M\":");
	Serial.print(M);
	Serial.print(",\"N\":");
	Serial.print(N);
	Serial.print("}");
	}
	/*
	mySerial.print("{\"SN\":");
	mySerial.print(2222);
	mySerial.print(",\"HO\":");
	mySerial.print(3);
	mySerial.print(",\"MI\":");
	mySerial.print(33);
	mySerial.print(",\"SE\":");
	mySerial.print(33);*/
	mySerial.print(",\"A\":");
	mySerial.print(A);
	mySerial.print(",\"B\":");
	mySerial.print(B);
	mySerial.print(",\"C\":");
	mySerial.print(C);
	mySerial.print(",\"D\":");
	mySerial.print(D);
	mySerial.print(",\"E\":");
	mySerial.print(E);
	mySerial.print(",\"F\":");
	mySerial.print(F);
	mySerial.print(",\"G\":");
	mySerial.print(G);
	mySerial.print(",\"H\":");
	mySerial.print(H);
	mySerial.print(",\"I\":");
	mySerial.print(I);
	mySerial.print(",\"J\":");
	mySerial.print(J);
	mySerial.print(",\"K\":");
	mySerial.print(K);
	mySerial.print(",\"L\":");
	mySerial.print(L);
	mySerial.print(",\"M\":");
	mySerial.print(M);
	mySerial.print(",\"N\":");
	mySerial.print(N);
	mySerial.print("}");
	//mySerial.end();
	A = 0;
	B = 0;
	C = 0;
	D = 0;
	E = 0;
	F = 0;
	G = 0;
	H = 0;
	I = 0;
	J = 0;
	K = 0;
	L = 0;
	M = 0;
	N = 0;
}
void Accelerometer(bool conn) {
	if (conn) {
	sensors_event_t event;
	accel.getEvent(&event);
	// x y z
	// I J K +
	// L M N -
	if (I == 0 && L == 0)I = L = event.acceleration.x;
	if (J == 0 && M == 0)J = M = event.acceleration.y;
	if (K == 0 && N == 0)K = N = event.acceleration.z;
	if (event.acceleration.x > I)I = event.acceleration.x;
	if (event.acceleration.x < L)L = event.acceleration.x;
	if (event.acceleration.y > J)J = event.acceleration.y;
	if (event.acceleration.y < M)M = event.acceleration.y;
	if (event.acceleration.z > K)K = event.acceleration.z;
	if (event.acceleration.z < N)N = event.acceleration.z;
	}
	
}

void loop(void) {
	//CountRunTime(10000);
	CountTipa(true);
}
