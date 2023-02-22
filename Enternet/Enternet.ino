#include <EtherCard.h>

// your variable

#define PATH    "example.php"
#define VARIABLE    "test"

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "www.leananalistic.com.ua/";

byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;

void setup() {
    Serial.begin(57600);
    Serial.println("\n[webClient]");

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
        Serial.println("Failed to access Ethernet controller");
    if (!ether.dhcpSetup())
        Serial.println("DHCP failed");

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

    if (!ether.dnsLookup(website))
        Serial.println("DNS failed");

    ether.printIp("SRV: ", ether.hisip);
}

void loop() {
    ether.packetLoop(ether.packetReceive());

    if (millis() > timer) {
        timer = millis() + 10000;

        byte sd = stash.create();
        stash.print("variable=");
        stash.print(VARIABLE);
        stash.print("&action=Submit");
        stash.save();

        // generate the header with payload - note that the stash size is used,
        // and that a "stash descriptor" is passed in as argument using "$H"
        Stash::prepare(PSTR("POST http://$F/$F.csv HTTP/1.0" "\r\n"
            "Host: $F" "\r\n"
            "Content-Length: $D" "\r\n"
            "Content-Type: application/x-www-form-urlencoded" "\r\n"
            "\r\n"
            "$H"),
            website, PSTR(PATH), website, stash.size(), sd);

        // send the packet - this also releases all stash buffers once done
        ether.tcpSend();
    }
}