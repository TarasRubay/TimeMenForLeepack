#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>


using namespace std;

#define TRIGGER_PIN 2
bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )
String arr_json_buffer = "[";
int arr_json_buffer_size = 0;
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
    unsigned long get_millis() { return _time_step; }
    void Print() {

        Serial.printf("% d", this->hour);
        Serial.printf(":% d", this->minute);
        Serial.printf(":% d", this->second);
        Serial.printf("  buff size = % d\n", arr_json_buffer_size);
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
        if (!first_start) first_start = true;
        Serial.print(">>time sync>> ");
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
String urlTime = "https://pipe.leananalistic.com.ua/api/syncdatetime/time";
String urlPostData = "https://pipe.leananalistic.com.ua/api/fromesp";
String urlTestConnection = "https://pipe.leananalistic.com.ua/api/fromesp";
//testing
String dev_urlTime = "https://dev-pipe.leananalistic.com.ua/api/syncdatetime/time";
String dev_urlPostData = "https://dev-pipe.leananalistic.com.ua/api/test";
String dev_urlTestConnection = "https://dev-pipe.leananalistic.com.ua/api/fromesp";
String dev_urlPostArray = "https://dev-pipe.leananalistic.com.ua/api/test/arraybuffer";
String dev_urlPostTestString = "https://dev-pipe.leananalistic.com.ua/api/test/teststring";
String dev_urlGetTestString = "https://dev-pipe.leananalistic.com.ua/api/test/getpost";
int buttom = 0;
int code_target = 200;
const String SN = "2";
int max_size_bufferString = 30;

///////////////////////////////////////////////////////////////////// static varibles
//1 - not connection to wifi
//2 - https fail begining to url
//3 - https code < 0
//<200 - response from server
int sendData(String message, String url) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;
        if (https.begin(*client, url)) {  // HTTPS
            https.addHeader("Content-Type", "application/json");
            https.addHeader("User-Agent", "Arduino");
            https.addHeader("Accept", "*/*");
            https.addHeader("Connection", "keep-alive");
            int httpCode = https.POST(message);//"{\"value\":\"lfkdjkwelifjwepoifjw;eofjwe\"}"
            //String msg = "{\"value\":\"" + message + "\"}";
            //int httpCode = https.POST(msg);//"{\"value\":\"lfkdjkwelifjwepoifjw;eofjwe\"}"
            //Serial.print(httpCode);
            //Serial.print(" ");
            //Serial.println(msg);
            if (httpCode > 0) {
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    if (httpCode == 200)timelocal.set_time(payload);
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
//1 - not connection to wifi
//2 - https fail begining to url
//3 - https code < 0
//<200 - response from server
int sendArray(String url) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;
        if (https.begin(*client, url)) {  // HTTPS
            https.addHeader("Content-Type", "application/json");
            https.addHeader("User-Agent", "Arduino");
            https.addHeader("Accept", "*/*");
            https.addHeader("Connection", "keep-alive");
            int httpCode = https.POST(arr_json_buffer + ']');
            Serial.print(httpCode);
            Serial.print(" ");
            Serial.println(arr_json_buffer + ']');
            if (httpCode > 0) {
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    if (httpCode == 200)timelocal.set_time(payload);
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
                    if (httpCode == 200)timelocal.set_time(payload);
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
    Serial.begin(9600);
    //Serial.setDebugOutput(true);

    pinMode(buttom, OUTPUT);
    Serial.println();
    Serial.println();
    Serial.println();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
    Serial.begin(9200);
    Serial.setDebugOutput(true);
    delay(3000);
    Serial.println("\n Starting");

    pinMode(TRIGGER_PIN, INPUT);

    if (wm_nonblocking) wm.setConfigPortalBlocking(false);
    int customFieldLength = 40;
    const char* custom_radio_str = "<br/><label for='customfieldid'>Custom Field Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='customfieldid' value='2'> Two<br><input type='radio' name='customfieldid' value='3'> Three";
    new (&custom_field) WiFiManagerParameter(custom_radio_str); // custom html input

    wm.addParameter(&custom_field);
    wm.setSaveParamsCallback(saveParamCallback);

    std::vector<const char*> menu = { "wifi","info","param","sep","restart","exit" };
    wm.setMenu(menu);

    // set dark theme
    wm.setClass("invert");

    wm.setConfigPortalTimeout(30); // auto close configportal after n seconds
    bool res;
    res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

    if (!res) {
        Serial.println("Failed to connect or hit timeout");
    }
    else {
        Serial.println("connected...yeey :)");

    }
    if (SyncTime(urlTime) == 200)Serial.println("!!!sync time!!!");
    else timelocal.first_start = false;
}

void checkButton() {
    if (digitalRead(TRIGGER_PIN) == LOW) {
        delay(50);
        if (digitalRead(TRIGGER_PIN) == LOW) {
            Serial.println("Button Pressed");
            delay(3000); // reset delay hold
            if (digitalRead(TRIGGER_PIN) == LOW) {
                Serial.println("Button Held");
                Serial.println("Erasing Config, restarting");
                wm.resetSettings();
                ESP.restart();
            }
            Serial.println("Starting config portal");
            wm.setConfigPortalTimeout(120);
            
            if (!wm.startConfigPortal("Lean_SN-2", "password")) {
                Serial.println("failed to connect or hit timeout");
                delay(3000);
            }
            else {
                Serial.println("connected...yeey :)");
            }
        }
    }
}
String getParam(String name) {
    String value;
    if (wm.server->hasArg(name)) {
        value = wm.server->arg(name);
    }
    return value;
}

void saveParamCallback() {
    Serial.println("[CALLBACK] saveParamCallback fired");
    Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
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
String Read_Serial_FAKE(String SN) {
    String ho = String(timelocal.hour);
    String mi = String(timelocal.minute);
    String se = String(timelocal.second);

    return "{\"SN\":" + SN
        + ",\"HO\":" + ho
        + ",\"MI\":" + mi
        + ",\"SE\":" + se
        + ",\"A\":0,\"B\":0,\"C\":0,\"D\":0,\"E\":0,\"F\":0,\"G\":0,\"H\":0,\"I\":0.00,\"J\":0.00,\"K\":0.00,\"L\":0.00,\"M\":0.00,\"N\":0.00}";
}
void Post(String msg, unsigned long time_loss) {

    for (int i = 0; i < 4; i++)
    {
        if (i > 2) {
            BuffString(msg, time_loss);
            break;
        }
        Serial.print("sendData run ");
        int code = sendData(msg, dev_urlPostData);
        //int code = sendData(msg, dev_urlPostTestString); //for testing
        Serial.println(code);
        if (code == code_target)break;
        else Serial.println("; resend ");
    }

}

void BuffString(String msg, unsigned long time_loss) {
    if (arr_json_buffer_size < max_size_bufferString) {
        if (arr_json_buffer.length() < 100) {
            if (msg.length() > 80) {
                arr_json_buffer += msg;
                arr_json_buffer_size++;
            }
            else Serial.println("small size length");
        }
        else {
            if (msg.length() > 50) {
                arr_json_buffer += ',' + msg;
                arr_json_buffer_size++;
            }
            else Serial.println("small size length");
        }
        Serial.println("Add to string bufer");
    }
    else
    {
        Serial.print("STACK OVERFLOW ");
        Serial.println(arr_json_buffer.length());
        Read_Serial(SN);
    }
    timelocal.Add_seconds(((millis() - time_loss) / 1000) + 2);
    Serial.print("Add_seconds "); Serial.println((millis() - time_loss) / 1000);
}
int FlushString(unsigned long time_loss) {
    Serial.print("Flush  ");
    Serial.print("size[");
    Serial.print(arr_json_buffer_size);
    Serial.println("]");
    for (int i = 0; i < 4; i++)
    {
        Serial.print("sendArray run ");
        delay(1000);
        int code = sendArray(dev_urlPostArray);
        //int code = sendArray(dev_urlPostTestString);//for send test string
        Serial.println(code);
        if (code == code_target) {
            arr_json_buffer.~String();
            arr_json_buffer = "[";
            arr_json_buffer_size = 0;
            return code;
        }
        else Serial.println(" resend ");
    }
    return 0;
}
int my_event = 9;
Time my_time;
bool conn_flag = false;
bool conn_flag2 = false;
void loop() {
    if (wm_nonblocking) wm.process(); // avoid delays() in loop when non-blocking and other long running code  
    checkButton();
    if (conn_flag && conn_flag2) {
        Serial.println("emercy sync time");
        SyncTime(urlTime);
        conn_flag = false;
        conn_flag2 = false;
    }

    if (timelocal.first_start)
    {
        if (conn_flag) {
            if (millis() + 1000 > timelocal.get_millis()) {
                Serial.print("emercy add seconds ");
                Serial.println(timelocal.get_millis());
                timelocal.Print();
                timelocal.Add_seconds(1);
            }
        }
        timelocal.Run();

        //if (timelocal.second == my_time.second)
        if (timelocal.second == 0)
            //if (timelocal.second == 0 || timelocal.second == 20 || timelocal.second == 40)
        {
            //my_time.Add_seconds(my_event);
            String data_from_counter = Read_Serial(SN);
            //String data_from_counter = Read_Serial_FAKE(SN);
            unsigned long time_loss = millis();
            if (WiFiMulti.run() == WL_CONNECTED) {
                conn_flag2 = true;
                int code = Test_connection(dev_urlTestConnection);
                Serial.print("TestConn run "); Serial.println(code);
                if (code == code_target) {
                    if (arr_json_buffer_size > 0) {
                        int flush_code = FlushString(time_loss);
                        if (flush_code == code_target)Post(data_from_counter, time_loss);
                        else BuffString(data_from_counter, time_loss);
                    }
                    else Post(data_from_counter, time_loss);
                }
                else {
                    BuffString(data_from_counter, time_loss);
                }
            }
            else {
                BuffString(data_from_counter, time_loss + 1000);
                conn_flag = true;
                conn_flag2 = false;
            }
            data_from_counter.~String();

        }
    }
    else
    {
        Serial.println("try to sync time");
        if (SyncTime(urlTime) == code_target)Read_Serial(SN);
    }
 }