/*
Autour: Furkan Metin OĞUZ
Date:2021
*/

#include <Adafruit_MAX31856.h>

#include <BLEDevice.h>

#include <BLEServer.h>

#include <BLEUtils.h>

#include <BLE2902.h>

BLEServer * pServer = NULL;
BLECharacteristic * pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "325cd0da-210c-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID "3d58b4ea-210c-11ec-9621-0242ac130002"

Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(17, 16, 5, 4);
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer * pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer * pServer) {
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

    if (!maxthermo.begin()) {

        while (1) delay(10);
    }

    maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);

    Serial.print("Thermocouple type: ");
    switch (maxthermo.getThermocoupleType()) {
    case MAX31856_TCTYPE_B:
        Serial.println("B Type");
        break;
    case MAX31856_TCTYPE_E:
        Serial.println("E Type");
        break;
    case MAX31856_TCTYPE_J:
        Serial.println("J Type");
        break;
    case MAX31856_TCTYPE_K:
        Serial.println("K Type");
        break;
    case MAX31856_TCTYPE_N:
        Serial.println("N Type");
        break;
    case MAX31856_TCTYPE_R:
        Serial.println("R Type");
        break;
    case MAX31856_TCTYPE_S:
        Serial.println("S Type");
        break;
    case MAX31856_TCTYPE_T:
        Serial.println("T Type");
        break;
    case MAX31856_VMODE_G8:
        Serial.println("Voltage x8 Gain mode");
        break;
    case MAX31856_VMODE_G32:
        Serial.println("Voltage x8 Gain mode");
        break;
    default:
        Serial.println("Unknown");
        break;
    }

    maxthermo.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    BLEDevice::init("Microzerr temperature");

    pServer = BLEDevice::createServer();
    pServer -> setCallbacks(new MyServerCallbacks());

    BLEService * pService = pServer -> createService(SERVICE_UUID);

    pCharacteristic = pService -> createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );

    pCharacteristic -> addDescriptor(new BLE2902());

    pService -> start();

    BLEAdvertising * pAdvertising = BLEDevice::getAdvertising();
    pAdvertising -> addServiceUUID(SERVICE_UUID);
    pAdvertising -> setScanResponse(false);
    pAdvertising -> setMinPreferred(0x0);
    BLEDevice::startAdvertising();

}

void loop() {

    maxthermo.triggerOneShot();
    delay(500);
    char txString[8];
    Serial.println(maxthermo.readThermocoupleTemperature());
    dtostrf(maxthermo.readThermocoupleTemperature(), 1, 2, txString);
    pCharacteristic -> setValue(txString);
    pCharacteristic -> notify();

}
