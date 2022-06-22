#include <Arduino.h>
#include <Time.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoQueue.h>

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
    bool first_start;
    void Print() {
        Serial.printf("% d", this->hour);
        Serial.printf(":% d", this->minute);
        Serial.printf(":% d\n", this->second);
        Serial.flush();
    }
    void Add_seconds(int sec) {
        for (int i = 0; i < sec; i++)
        {
            _add_one_step();
        }
    }
    void Run() {

        if (_time_step == millis()) {
            _add_one_step();
            this->Print();
        }
        
    }
    void set_time(String playload) {
        //1:37:03
        //01234567
        vector<int> dat;
        String t = "";
        for (int i = 0; i < playload.length(); i++)
        {
            t += playload[i];
            if (playload[i] == ':') {
                dat.push_back(t.toInt());
                t = "";
            }
        }
        this->hour = dat[0];
        this->minute = dat[1];
        this->second = dat[2];
        t.clear();
        dat.clear();
        
        _time_step = millis() + 1000;
        if(!first_start) first_start = true;
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

vector<String> json_buffer = vector<String>();
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
                    //Serial.printf("sync time: \n", payload);                    
                    //Serial.printf("sync time\n");
                    if(httpCode == 200)timelocal.set_time(payload);
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
    Serial.setDebugOutput(true);
  
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
    if (SyncTime(urlTime) != 200)timelocal.first_start = false;
}
String Read_Serial(String SN) {
    String ho = String(timelocal.hour);
    String mi = String(timelocal.minute);
    String se = String(timelocal.second);
    digitalWrite(buttom, LOW);
    delay(1);
    String msg = Serial.readString();
    delay(1);
    digitalWrite(buttom, HIGH);
    return "{\"SN\":" + SN
        + ",\"HO\":" + ho
        + ",\"MI\":" + mi
        + ",\"SE\":" + se
        + msg;
}
void Post(String msg) {
    Serial.print("sendData return code "); Serial.println(sendData(msg, urlTest));
    Serial.print("SyncTime return code "); Serial.println(SyncTime(urlTime));
}
void Buff(String msg,unsigned long time_loss) {
    json_buffer.push_back(msg);
    Serial.println("Add to bufer");
    timelocal.Add_seconds(((millis() - time_loss) / 1000) + 2);
    Serial.print("Add_seconds "); Serial.println((millis() - time_loss) / 1000);
}
void Flush() {
    for (int i = 0; i < json_buffer.size(); i++)
    {   
        String msg = json_buffer[i];
        Serial.print("Flush  ");
        Serial.print(sendData(msg, urlTest));
        Serial.println(msg);
    }
    json_buffer.clear();
}
const String SN = "4";
void loop() {
    if (timelocal.first_start)
    {
        timelocal.Run();
        
        //if (timelocal.second == 0 || timelocal.second == 20 || timelocal.second == 40)
        if (timelocal.second == 0)
        {
            String data_from_counter = Read_Serial(SN);
            unsigned long time_loss = millis();
            int code = Test_connection(urlTestConnection);
            Serial.print("Test_connection return code"); Serial.println(code);
            if (code == 200) {
                if (!json_buffer.empty())Flush();
                Post(data_from_counter);
            }
            else {
                Buff(data_from_counter, time_loss);
            }
        }

    }
        else SyncTime(urlTime);
}

