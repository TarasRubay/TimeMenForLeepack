/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
//const uint8_t fingerprint[20] = { 0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3 };
const char fingerprint[] = "4924e700d512b302b7e87be2c6ffb35d467f1acc";

ESP8266WiFiMulti WiFiMulti;

void setup() {

    Serial.begin(9600);
    // Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("DANA", "11160045");
    //WiFiMulti.addAP("kormotech", "a9GyeUce");
    //WiFiMulti.addAP("ASUS", "donperes");
}

int sendDataTest(String message) {
    unsigned long time = millis();
    if ((WiFiMulti.run() == WL_CONNECTED)) {

        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

        //client->setFingerprint(fingerprint);
        // Or, if you happy to ignore the SSL certificate, then use the following line instead:
         client->setInsecure();

        HTTPClient https;

        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://pipe.leananalistic.com.ua/api/fromesp")) {  // HTTPS

            Serial.print("[HTTPS] POST...\n");

            https.addHeader("Content-Type", "application/json");
            https.addHeader("User-Agent", "Arduino");
            https.addHeader("Accept", "*/*");
            https.addHeader("Connection", "keep-alive");
            int httpCode = https.POST(message);
            // httpCode will be negative on error
            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);
                    https.end();
                    return httpCode;
                }
            }
            else {
                Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
                https.end();
                return 3;
            }
            https.end();
        }
        else {
            Serial.printf("[HTTPS] Unable to connect\n");
            return 2;
        }


    }
    return 1;
    //Serial.println(millis() - time);
}


//1#15#15#15#15
//public int SerialNumber{ get; set; }
//public int CountFirst{ get; set; }
//public int CountSecond{ get; set; }
//public int CountFirstDemage{ get; set; }
//public int CountSecondDemage{ get; set; }
void loop() {

    String msg = Serial.readString();
    //Serial.println("online");
    if (msg.length() != 0) {
        int try_send = 4;
        int code = sendDataTest(msg);
        Serial.print(code);
        Serial.println(" - code");
        if (code != 200) {
            for (int i = 0; i <= try_send; i++)
            {
            code = sendDataTest(msg);
            if (code == 200)break;
            Serial.print(code);
            Serial.print(" -code  try send");
            Serial.println(i);
            delay(1000);
            }
        }
        else
            Serial.printf("[HTTPS] send ok\n");
    }
}

