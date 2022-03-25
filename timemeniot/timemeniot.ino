/*
 * asksensors http request
 * description: connect esp8266 to asksensors over http
 *  author: https://asksensors.com, 2018-2019
 *  github: https://github.com/asksensors/asksensors-esp8266-api
 */

#include <arduino.h>
#include <esp8266wifi.h>
#include <esp8266wifimulti.h>
#include <esp8266httpclient.h>

 // user config: todo
const char* wifi_ssid = "ASUS";             // ssid
const char* wifi_password = "...................";         // wifi
const char* apikeyin = "...................";      // api key in
const unsigned int writeinterval = 25000;   // write interval (in ms)

// asksensors config.
string host = "http://api.asksensors.com";         // asksensors api host name

esp8266wifimulti wifimulti;

void setup() {

    serial.begin(115200);
    serial.println("*****************************************************");
    serial.println("********** program start : connect esp8266 to asksensors over http");
    serial.println("wait for wifi... ");
    serial.print("********** connecting to wifi : ");
    serial.println(wifi_ssid);
    wifi.begin(wifi_ssid, wifi_password);
    while (wifi.status() != wl_connected) {
        delay(500);
        serial.print(".");
    }
    serial.println("");
    serial.println("-> wifi connected");
    serial.println("-> ip address: ");
    serial.println(wifi.localip());

}


void loop() {
    // wait for wifi connection
    if (wifi.status() == wl_connected) {

        httpclient http;

        serial.print("[http] begin...\n");

        // create a url for the request
        string url = "";
        url += host;
        url += "/write/";
        url += apikeyin;
        url += "?module1=";
        url += random(10, 100);

        serial.print("********** requesting url: ");
        serial.println(url);
        http.begin(url); //http

        serial.println("> request sent to asksensors");

        serial.print("[http] get...\n");
        // start connection and send http header
        int httpcode = http.get();

        // httpcode will be negative on error
        if (httpcode > 0) {
            // http header has been send and server response header has been handled
            serial.printf("[http] get... code: %d\n", httpcode);

            // file found at server
            if (httpcode == http_code_ok) {
                string payload = http.getstring();
                serial.println(payload);
            }
        }
        else {
            serial.printf("[http] get... failed, error: %s\n", http.errortostring(httpcode).c_str());
        }

        http.end();

        serial.println("********** end ");
        serial.println("*****************************************************");
    }

    delay(writeinterval);   // wait for writeinterval
}