#include <Arduino.h>
#include <SPI.h>
#include "SimpleModbusMaster.h"

#define connection_error_led 13


#define baud 19200
#define timeout 1000
#define polling 200 // the scan rate

#define retry_count 10 

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 7

enum
{
  PACKET1,
  PACKET2,
  // leave this last entry
  TOTAL_NO_OF_PACKETS
};

// Create an array of Packets for modbus_update()
Packet packets[TOTAL_NO_OF_PACKETS];

packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];

unsigned int volt[4];
unsigned int freq[4];

unsigned long timer;

float f_2uint_float(unsigned int uint1, unsigned int uint2) {    // reconstruct the float from 2 unsigned integers

  union f_2uint {
    float f;
    uint16_t i[2];
  };

  union f_2uint f_number;
  f_number.i[0] = uint1;
  f_number.i[1] = uint2;

  return f_number.f;

}


void setup()
{
  
  packet1->id = 1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 3027;
  packet1->no_of_registers = 2;
  packet1->register_array = volt;

  packet2->id = 1;
  packet2->function = READ_HOLDING_REGISTERS;
  packet2->address = 3109;
  packet2->no_of_registers = 2;
  packet2->register_array = freq;
  
  modbus_configure(baud, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
  
  pinMode(connection_error_led, OUTPUT);
  timer = millis();
}



void loop()
{
  unsigned int connection_status = modbus_update(packets);
  
  if (connection_status != TOTAL_NO_OF_PACKETS)
  {
    digitalWrite(connection_error_led, HIGH);
   
  }
  else
    digitalWrite(connection_error_led, LOW);
 
  long newTimer = millis();
  if(newTimer -  timer >= 5000){
    Serial.println();
    Serial.print("VOLTAGE : ");

    Serial.print(f_2uint_float(volt[1],volt[0]));
    Serial.println(" V");

    Serial.println();
    Serial.print("Frequncy : ");
    Serial.print(f_2uint_float(freq[1],freq[0]));
    Serial.println(" Hz");
    
    timer = newTimer;
  }

  
}