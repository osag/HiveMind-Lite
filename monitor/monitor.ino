/* --- Libraries --- */
#include <DHT.h>
#include <SD.h>
#include "stdio.h"

/* --- Pins --- */
#define SD_PIN 10
#define DHT_INTERNAL_PIN A0
#define DHT_EXTERNAL_PIN A1

/* --- Values --- */
#define DHT_TYPE DHT11
#define BAUD 9600
#define CHARS 8
#define BUFFER 128
#define DIGITS 4
#define PRECISION 2
#define INTERVAL 1000

/* --- Global --- */
Sd2Card card;
SdVolume volume;
SdFile root;
char int_C[CHARS];
char int_RH[CHARS];
char ext_C[CHARS];
char ext_RH[CHARS];
char output[BUFFER];
DHT internal(DHT_INTERNAL_PIN, DHT_TYPE);
DHT external(DHT_EXTERNAL_PIN, DHT_TYPE);

/* --- Setup --- */
void setup() {
  Serial.begin(9600);
  pinMode(SD_PIN, OUTPUT);
  while (!card.init(SPI_HALF_SPEED, SD_PIN)) {
    Serial.println("Initialization failed. Things to check:");
    Serial.println("* Is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* Did you change the chipSelect pin to match your shield or module?");
  } 
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }
  uint32_t volumesize;  
  volumesize = volume.blocksPerCluster();
  volumesize *= volume.clusterCount();
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);
  internal.begin();
  external.begin();
}

/* --- Loop --- */
void loop() {
  String dataString = "";
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  dtostrf(get_ext_RH(), DIGITS, PRECISION, ext_RH); 
  dtostrf(get_ext_C(), DIGITS, PRECISION, ext_C);
  dtostrf(get_int_RH(), DIGITS, PRECISION, int_RH);
  dtostrf(get_int_C(), DIGITS, PRECISION, int_C);
  sprintf(output, "{'Internal_C':%s, 'External_C':%s, 'Internal_RH':%s, 'External_RH':%s}", int_C, ext_C, int_RH, ext_RH);
  Serial.println(output);
  dataFile.print(output);
  dataFile.close();
  delay(INTERVAL);
}

/* --- Get Internal Humidity --- */
float get_int_RH() {
  float val = internal.readHumidity();
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

/* --- Get Internal Temperature --- */
float get_int_C() {
  float val = internal.readTemperature(); //  Serial.println(val);
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

/* --- Get External Humidity --- */
float get_ext_RH() {
  float val = external.readHumidity();
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

/* --- Get External Temperature --- */
float get_ext_C() {
  float val = external.readTemperature();
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}
