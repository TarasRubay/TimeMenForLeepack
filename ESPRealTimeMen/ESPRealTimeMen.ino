#include <Arduino.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

using namespace std;


struct Time
{
private:
    unsigned long _time_step;
    void _add_one_step() {
        _time_step += 1000;
        if (this->second + 1 > 59) {
            if (this->minute + 1 > 59) {
                if (this->hour + 1 > 23) {
                    this->hour = 0;
                    this->minute = 0;
                    this->second = 0;
                }
                else
                {
                    this->hour = this->hour + 1;
                    this->minute = 0;
                    this->second = 0;
                }
            }
            else {
                this->minute = this->minute + 1;
                this->second = 0;
            }
        }
        else this->second = this->second + 1;
    }
public:
    int hour;
    int minute;
    int second;

    void Print() {
        Serial.printf("% d", this->hour);
        Serial.printf(":% d", this->minute);
        Serial.printf(":% d\n", this->second);
        Serial.flush();
    }
    
    void Run() {

        if (_time_step == millis()) {
            _add_one_step();
            //Serial.printf("sec: %d\n", this->second);
            //Serial.flush();
            
            
            this->Print();
        }
        
    }
    void set_time(String playload) {
        //15:37:03
        //01234567
        int aa = playload.substring(0,1).toInt() * 10;
        int ab = playload.substring(1,2).toInt();
        int ba = playload.substring(3,4).toInt() * 10;
        int bb = playload.substring(4,5).toInt();
        int ca = playload.substring(6,7).toInt() * 10;
        int cb = playload.substring(7).toInt();
        this->hour = aa + ab;
        this->minute = ba + bb;
        this->second = ca + cb;
        _time_step = millis() + 1000;
    }
    Time operator++()
    {
        if (this->second + 1 > 59) {
            if (this->minute + 1 > 59) {
                if (this->hour + 1 > 23) {
                    this->hour = 0;
                    this->minute = 0;
                    this->second = 0;
                    return *this;
                }
                else
                {
                    this->hour = this->hour + 1;
                    this->minute = 0;
                    this->second = 0;
                    return *this;
                }
            }
            else {
                this->minute = this->minute + 1;
                this->second = 0;
                return *this;
            }
        }
        else
        {
            this->second = this->second + 1;
            return *this;
        }
    }
    Time operator++(int val)
    {
        Time tmp(*this);
        if (this->second + 1 > 59) {
            if (this->minute + 1 > 59) {
                if (this->hour + 1 > 23) {
                    this->hour = 0;
                    this->minute = 0;
                    this->second = 0;
                    return tmp;
                }
                else
                {
                    this->hour = this->hour + 1;
                    this->minute = 0;
                    this->second = 0;
                    return tmp;
                }
            }
            else {
                this->minute = this->minute + 1;
                this->second = 0;
                return tmp;
            }
        }
        else
        {
            this->second = this->second + 1;
            return tmp;
        }
    }
};
Time timelocal;
ESP8266WiFiMulti WiFiMulti;
String urlTime = "https://dev-pipe.leananalistic.com.ua/api/syncdatetime/time";
String urlPostData = "https://pipe.leananalistic.com.ua/api/fromesp";
String urlTestConnection = "https://pipe.leananalistic.com.ua/api/fromesp";
String urlTest = "https://dev-pipe.leananalistic.com.ua/api/test";
int buttom = 0;

//1 - not connection to wifi
//2 - https fail begining to url
//3 - https code < 0
//<200 - response from server
int sendData(String message,String url) {
    //Serial.println(message);
    if ((WiFiMulti.run() == WL_CONNECTED)) {

        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

        //client->setFingerprint(fingerprint);
        // Or, if you happy to ignore the SSL certificate, then use the following line instead:
        client->setInsecure();

        HTTPClient https;

        //Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, url)) {  // HTTPS

            //Serial.print("[HTTPS] POST...\n");

            https.addHeader("Content-Type", "application/json");
            https.addHeader("User-Agent", "Arduino");
            https.addHeader("Accept", "*/*");
            https.addHeader("Connection", "keep-alive");
            int httpCode = https.POST(message);
            // httpCode will be negative on error
            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                //Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    //String payload = https.getString();
                    //Serial.println(payload);
                    https.end();
                    return httpCode;
                }
            }
            else {
                //Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
                https.end();
                return 3;
            }
            https.end();
        }
        else {
            //Serial.printf("[HTTPS] Unable to connect\n");
            return 2;
        }


    }
    return 1;
    
}
//1 - not connection to wifi
//2 - https fail begining to url
//3 - https code < 0
//<200 - response from server
int SyncTime(String url) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;
        if (https.begin(*client, url)) {  // HTTPS
            https.addHeader("Content-Type", "application/json");
            https.addHeader("User-Agent", "Arduino");
            https.addHeader("Accept", "*/*");
            https.addHeader("Connection", "keep-alive");
            int httpCode = https.GET();
            if (httpCode > 0) {
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    //Serial.printf("pay load: \n", payload);                    
                    //Serial.printf("sync time\n");
                    timelocal.set_time(payload);
                    https.end();
                    return httpCode;
                }
            }
            else {
                https.end();
                return 3;
            }
            https.end();
        }
        else {
            return 2;
        }
    }
    return 1;
}
int Test_connection(String url) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;
        if (https.begin(*client, url)) {  // HTTPS
            https.addHeader("Content-Type", "application/json");
            https.addHeader("User-Agent", "Arduino");
            https.addHeader("Accept", "*/*");
            https.addHeader("Connection", "keep-alive");
            int httpCode = https.GET();
            if (httpCode > 0) {
                    https.end();
                    return httpCode;
                
            }
            else {
                https.end();
                return 3;
            }
            https.end();
        }
        else {
            return 2;
        }
    }
    return 1;
}
void setup() {
    //digitalWrite(buttom, HIGH);
    //Serial.setDebugOutput(true);
    Serial.begin(9600);
    // Serial.setDebugOutput(true);
  
    pinMode(buttom, OUTPUT);
    Serial.println();
    Serial.println();
    Serial.println();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    //setTime(8, 29, 0, 1, 1, 11); // set time to Saturday 8:29:00am Jan 1 2011
    WiFi.mode(WIFI_STA);

    
    WiFiMulti.addAP("DANA", "11160045");
    //WiFiMulti.addAP("kormotech", "a9GyeUce");
    //WiFiMulti.addAP("ASUS", "donperes");
    //WiFiMulti.addAP("AP", "donperes");
    SyncTime(urlTime);
    
}

String SN = "2";
void loop() {
    timelocal.Run();
    
    /*Serial.printf("sec: % d\n", timelocal.second);
    Serial.end();*/
    //Serial.println(timelocal.second);
    
    
 /*   if (timelocal.second == 1)
    {
        digitalWrite(buttom, LOW);
        String msg = Serial.readString();

    }
    else
    {
        digitalWrite(buttom, HIGH);
    }*/
    if (timelocal.second == 1)
    {
        int code = Test_connection(urlTestConnection);
        if (code == 200) {
            digitalWrite(buttom, LOW);
            delay(1);
            String msg = Serial.readString();
            delay(1);
            digitalWrite(buttom, HIGH);
            digitalWrite(buttom, HIGH);
            digitalWrite(buttom, HIGH);
            String ho = String(timelocal.hour);
            String mi = String(timelocal.minute);
            String se = String(timelocal.second);
                /*sendData(
                    "{\"SN\":" + SN 
                    + ",\"HO\":"+ "5"
                    + ",\"MI\":" + "10"
                    + ",\"SE\":" + "0"
                    + msg, urlTest);
            */
            int code = sendData(msg, urlTest);
            Serial.println(code);
            SyncTime(urlTime);
        }
    }else
    {
        digitalWrite(buttom, HIGH);
    }
    //,"A":5,"B":4,"C":56,"D":40,"E":0,"F":0,"G":0,"H":0}
     
}

