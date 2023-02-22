// Автор: Мамаев Александр (alxmamaev@mail.ru)
// GitHub: https://github.com/alxmamaev/servduino


// Библиотека для работы с сетевой картой
#include <EtherCard.h>

// Буффер сетевой карты
byte Ethernet::buffer[200];

// Адрес сайта на который будем стучаться (в моем случае он запускается на одном с сервером IP)
const char website[] PROGMEM = "https://dev-pipe.leananalistic.com.ua/api/test"; //Замените на ip адрес вашего компьютера

// IP адрес сервера.
static byte websiteip[] = { 91,222,67,100 }; //Замените на ip адрес вашего компьютера
static uint32_t timer;

// Действия после успешной отправки данных

//Мак-адрес устройства
static byte mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

//функция колбэка (нелбходима для обраюотки полученных данных)
static void callback(byte status, word off, word len) {
    Serial.println(">>>");
    Ethernet::buffer[off + 300] = 0;
    Serial.print((const char*)Ethernet::buffer + off);
    Serial.println("...");
}
//{\"SN\":23,\"HO\":1,\"MI\":2,\"SE\":5,\"A\":0,\"B\":0,\"C\":0,\"D\":0,\"E\":0,\"F\":0,\"G\":0,\"H\":0,\"I\":0.00,\"J\":0.00,\"K\":0.00,\"L\":0.00,\"M\":0.00,\"N\":0.00}
// Функция отправки данных на сервер
void send_package() {
    ether.packetLoop(ether.packetReceive());
    const char* data = "{\"SN\":23,\"HO\":1,\"MI\":2,\"SE\":5,\"A\":0,\"B\":0,\"C\":0,\"D\":0,\"E\":0,\"F\":0,\"G\":0,\"H\":0,\"I\":0.00,\"J\":0.00,\"K\":0.00,\"L\":0.00,\"M\":0.00,\"N\":0.00}";
    if (millis() > timer) {
        Serial.println(F("<<PING"));
        timer = millis() + 5000;
        ether.browseUrl(PSTR("/"),data , website, callback);
    }
}

void setup() {
    Serial.begin(9600);

    if (ether.begin(sizeof Ethernet::buffer, mac) == 0)
        Serial.println(F("Failed to access Ethernet controller"));
    else
        Serial.println(F("Successfully to access Ethernet controller"));
    if (!ether.dhcpSetup())
        Serial.println(F("DHCP failed"));
    else
        Serial.println(F("Successfully DHCP"));

    memcpy(ether.hisip, websiteip, sizeof(websiteip));
    ether.printIp(F("SRV: "), ether.hisip);
}

void loop() {
    //отправляем данные
    send_package();
}