/*
 Name:		TimeMenForLeepack.ino
 Created:	2/5/2022 10:35:31 PM
 Author:	Rubay-PC
*/

//#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "GQbktYuA_REdqQFtKji4-Df-VaEcuIR6";

char ssid[] = "DANA";
char pass[] = "11160045";

#define BLYNK_PRINT Serial

WidgetTerminal terminal(V1);
BLYNK_WRITE(V1)
{
    
    
}

void setup()
{
    // Debug console
    Serial.begin(9600);

     Blynk.begin(auth, ssid, pass);
   // Blynk.begin("eCIWL8JuGLRo9FMibxWmnEkKZn4iWWCeNG+utp8Qk6M=", ssid, pass, "ArduinoTimeMen.azure-devices.net");
    terminal.clear();

}

void loop()
{
    Blynk.run();
    String msg = Serial.readString();
    if (msg.length() != 0) {
    terminal.print(msg);
    Serial.print(msg);
    terminal.flush();
    delay(1000);
    terminal.print("NULL");
    terminal.flush();
    }
    
}