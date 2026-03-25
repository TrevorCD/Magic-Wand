/* esp32.ino
 *
 * device side program for magic wand project
 * implements BLE server for communication between stm32 and host
 *
 *------------------------------------------------------------------------------
 *   Copyright 2026 Trevor B. Calderwood
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *----------------------------------------------------------------------------*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define DEBUG 1
#define READ_TIMEOUT_MS 1000

/* Global BLE variables */
BLECharacteristic *pCharacteristic;
BLEServer *pServer;
BLEService *pService;
BLEAdvertising * pAdvertising;

const char * serviceUUID = "3cd00375-4415-4fe2-aa41-42bd35f1c526";
const char * characteristicUUID = "cc84a98c-36be-4fe1-8345-be620545fd34";

/* Global connection state variables */
int connected;
unsigned long lastReadTime = 0;

/* BLE Callbacks */
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      connected = 1;
      lastReadTime = millis(); // for initial read timeout time
      #if DEBUG
      Serial.println("Connected");
      #endif
    }

    void onDisconnect(BLEServer* pServer) {
      pAdvertising->start();  // restart advertising
      #if DEBUG
      Serial.println("Disconnected");
      Serial.println("Starting advertising");
      #endif
    }
};

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pCharacteristic) {
    #if DEBUG
    Serial.println("read callback");
    #endif
    // reset read timeout
    lastReadTime = millis();
    // get new value from stme32

    // set new value
    /// for testing:
    pCharacteristic->setValue(1);
  }
};

void setup()
{
  #if DEBUG
  Serial.begin(9600);
  #endif
  // pin setup

  // BLE setup
  BLEDevice::init("ESP32MagicWand");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  pService = pServer->createService(serviceUUID);
  
  pCharacteristic = pService->createCharacteristic(
    characteristicUUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->setCallbacks(new CharacteristicCallbacks());
  pCharacteristic->setValue(1);
  pService->start();

  connected = 0; 

  pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  #if DEBUG
  Serial.println("Advertising started");
  #endif

}

void loop()
{
  if(connected && (lastReadTime + READ_TIMEOUT_MS > millis()))
  {
    connected = 0;
    pAdvertising->start();
    #if DEBUG
    Serial.println("Read timeout occured");
    Serial.println("Advertise start");
    #endif
  }
}
