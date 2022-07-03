
#include <Arduino.h>
#include <Time.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoQueue.h>

using namespace std;
struct list {


    struct node {
        node* p_next;
        String data;
        node(String _data = String(), node* _p_next = nullptr) {
            this->data = _data;
            this->p_next = _p_next;
        }
    };

    int length;
    node* head;
public:
    int getleng() {
        return length;
    }
    
    list()
    {
        length = 0;
        head = nullptr;
    }


    ~list()
    {
    }

    void push_back(String _data)
    {
        if (head == nullptr) {
            head = new node(_data);
        }
        else {
            node* tmp = this->head;
            while (tmp->p_next != nullptr)
            {
                tmp = tmp->p_next;
            }
            tmp->p_next = new node(_data);
        }
        length++;
    }


    /*String operator[](int _ind)
    {
        node* tmp = this->head;
        if (_ind <= length) {
            int count = 0;
            while (tmp != nullptr) {
                if (count == _ind)return tmp->data;
                tmp = tmp->p_next;
                count++;
            }
        }
        else
        {
            return tmp->data;
        }

    }*/

    String get_by_index(int _ind) {
        String res;
        node* tmp = this->head;
        if (_ind <= length) {
            int count = 0;
            while (tmp != nullptr) {
                if (count == _ind)res = tmp->data;
                tmp = tmp->p_next;
                count++;
            }
        }
        else
        {
            res = tmp->data;
        }
        return res;
    }

    void pop_front()
    {
        node* tmp = head;
        head = head->p_next;
        delete tmp;
        length--;
    }

    void clear()
    {
        while (length)
        {
            pop_front();
        }
    }


    void push_front(String _data)
    {
        head = new node(_data, head);
        length++;
    }


    void insert(String _data, int ind)
    {
        if (ind == 0) {
            push_front(_data);
        }
        else {
            node* tmp = this->head;
            for (int i = 0; i < ind - 1; ++i) {
                tmp = tmp->p_next;
            }
            node* n_node = new node(_data, tmp->p_next);
            tmp->p_next = n_node;
        }
        length++;
    }



    void remove_at(int index)
    {
        node* tmp = this->head;
        for (int i = 0; i < index - 1; i++)
        {
            tmp = tmp->p_next;
        }
        node* tmp2 = tmp->p_next;
        tmp->p_next = tmp2->p_next;
        delete tmp2;
        length--;
    }


    void pop_back()
    {
        node* tmp = this->head;
        for (int i = 0; i < length - 1; i++)
        {
            tmp = tmp->p_next;
        }
        node* tmp2 = tmp->p_next;
        tmp->p_next = nullptr;
        delete tmp2;
        length--;
    }
};

///////////////////////////////////////////////////////////////////// static varibles
//vector<String> json_buffer = vector<String>();
//list<String> json_buffer = list<String>();
list json_buffer = list();
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
        if(!first_start) first_start = true;
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
int buttom = 0;
int code_target = 200;
const String SN = "1111";
int max_size_buffer = 180;
int max_size_bufferString = 1;

///////////////////////////////////////////////////////////////////// static varibles


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
                    String payload = https.getString();
                    //Serial.println(payload);
                    if (httpCode == 200)timelocal.set_time(payload);
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
int sendArray(String url) {
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
            int httpCode = https.POST(arr_json_buffer + ']');
            // httpCode will be negative on error
            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                //Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    //Serial.println(payload);
                    if (httpCode == 200)timelocal.set_time(payload);
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

    //setTime(8, 29, 0, 1, 1, 11); // set time to Saturday 8:29:00am Jan 1 2011
    WiFi.mode(WIFI_STA);

    
    //WiFiMulti.addAP("DANA", "11160045");
    //WiFiMulti.addAP("kormotech", "a9GyeUce");
    //WiFiMulti.addAP("ASUS", "donperes");
    WiFiMulti.addAP("AP", "donperes");
    if (SyncTime(urlTime) == 200)Serial.println("!!!sync time!!!");
    else timelocal.first_start = false;
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
void Post(String msg,unsigned long time_loss) {
    
    for (int i = 0; i < 4; i++)
    {
        if (i > 2) {
            BuffString(msg, time_loss);
            break;
        }
       Serial.print("sendData run ");
       int code = sendData(msg, dev_urlPostData);
       Serial.println(code);
       if (code == code_target)break;
        else Serial.println("; resend ");
    }
    
}
void Buff(String msg,unsigned long time_loss) {
    if (json_buffer.getleng() >= max_size_buffer) {
        Serial.println("STACK OVERFLOW");
    }
    else
    {
        json_buffer.push_back(msg);
        Serial.println("Add to bufer");
        timelocal.Add_seconds(((millis() - time_loss) / 1000) + 2);
        Serial.print("Add_seconds "); Serial.println((millis() - time_loss) / 1000);
    }
    
}
void BuffString(String msg, unsigned long time_loss) { 
    if (arr_json_buffer_size < max_size_bufferString) {
        if (arr_json_buffer.length() < 100) {
            arr_json_buffer += msg;
            arr_json_buffer_size++;
        }
        else {
            arr_json_buffer += ',' + msg;
            arr_json_buffer_size++;
        }
        Serial.println("Add to string bufer");
    }
    else
    {
        Serial.print("STACK OVERFLOW ");
        Serial.println(arr_json_buffer.length());
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
void Flush(unsigned long time_loss) {
    for (int i = 0; i < json_buffer.getleng(); i++)
    {   
       
        String msg = json_buffer.get_by_index(i);
        Serial.print("Flush  ");
        Serial.print("[");
        Serial.print(i);
        Serial.print("] code = ");
        for (int j = 0; j < 3; j++)
        {
            if (time_loss + 40000 < millis()) {
                for (int k = 0; k < 2; k++)
                {
                    Serial.print("SyncTime run ");
                    int code = SyncTime(urlTime);
                    Serial.println(code);
                    if (code == code_target)break;
                    else Serial.println(" resync ");   
                }
                break;
            }
            Serial.print("sendData run ");
            int code = sendData(msg, dev_urlPostData);
            Serial.println(code);
            if (code == code_target) {
                json_buffer.pop_front();
                break;
            }
            else Serial.println(" resend ");
        }
    }
}
int my_event = 9;
Time my_time;
bool conn_flag = false;
bool conn_flag2 = false;
void loop() {
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
        
        //if (timelocal.second == 0)
        if (timelocal.second == my_time.second)
        {
            my_time.Add_seconds(my_event);
                    //String data_from_counter = Read_Serial(SN);
                    String data_from_counter = Read_Serial_FAKE(SN);
                    unsigned long time_loss = millis();
                    if (WiFiMulti.run() == WL_CONNECTED) {
                        conn_flag2 = true;
                        int code = Test_connection(dev_urlTestConnection);
                        Serial.print("TestConn run "); Serial.println(code);
                        if (code == code_target) {
                            if (arr_json_buffer_size > 0) {
                                int flush_code = FlushString(time_loss);
                                if(flush_code == code_target)Post(data_from_counter, time_loss);
                                else BuffString(data_from_counter, time_loss);
                            }
                            else Post(data_from_counter,time_loss);
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
        SyncTime(urlTime);
    }
}