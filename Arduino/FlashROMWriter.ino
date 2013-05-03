#define VCC (23)
#define GND (52)

#define ADDRESS_0 (42)
#define ADDRESS_1 (40)
#define ADDRESS_2 (36)
#define ADDRESS_3 (38)
#define ADDRESS_4 (32)
#define ADDRESS_5 (34)
#define ADDRESS_6 (28)
#define ADDRESS_7 (30)
#define ADDRESS_8 (35)
#define ADDRESS_9 (37)
#define ADDRESS_10 (41)
#define ADDRESS_11 (39)
#define ADDRESS_12 (26)
#define ADDRESS_13 (33)
#define ADDRESS_14 (31)
#define ADDRESS_15 (24)
#define ADDRESS_16 (22)
#define ADDRESS_17 (27)

#define DATA_0 (44)
#define DATA_1 (48)
#define DATA_2 (46)
#define DATA_3 (50)
#define DATA_4 (53)
#define DATA_5 (51)
#define DATA_6 (49)
#define DATA_7 (45)

#define RESET (25)
#define CE (47)
#define OE (43)
#define WE (29)

void setDataPinMode(int mode) {
  if (mode == OUTPUT) {
    pinMode(DATA_0, OUTPUT);
    pinMode(DATA_1, OUTPUT);
    pinMode(DATA_2, OUTPUT);
    pinMode(DATA_3, OUTPUT);
    pinMode(DATA_4, OUTPUT);
    pinMode(DATA_5, OUTPUT);
    pinMode(DATA_6, OUTPUT);
    pinMode(DATA_7, OUTPUT);
  } 
  else {
    pinMode(DATA_0, INPUT_PULLUP);
    pinMode(DATA_1, INPUT_PULLUP);
    pinMode(DATA_2, INPUT_PULLUP);
    pinMode(DATA_3, INPUT_PULLUP);
    pinMode(DATA_4, INPUT_PULLUP);
    pinMode(DATA_5, INPUT_PULLUP);
    pinMode(DATA_6, INPUT_PULLUP);
    pinMode(DATA_7, INPUT_PULLUP);
  }
}

void addressFlash(unsigned long address) {
  digitalWrite(ADDRESS_0,  ((address & 0x000001) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_1,  ((address & 0x000002) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_2,  ((address & 0x000004) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_3,  ((address & 0x000008) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_4,  ((address & 0x000010) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_5,  ((address & 0x000020) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_6,  ((address & 0x000040) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_7,  ((address & 0x000080) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_8,  ((address & 0x000100) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_9,  ((address & 0x000200) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_10, ((address & 0x000400) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_11, ((address & 0x000800) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_12, ((address & 0x001000) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_13, ((address & 0x002000) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_14, ((address & 0x004000) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_15, ((address & 0x008000) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_16, ((address & 0x010000) == 0) ? LOW : HIGH);
  digitalWrite(ADDRESS_17, ((address & 0x020000) == 0) ? LOW : HIGH);
}

void commandFlash(unsigned long address, unsigned char data) {
  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);
  setDataPinMode(OUTPUT);

  addressFlash(address);

  // address latching occurs 'falling edge'
  digitalWrite(WE, LOW);
    
  digitalWrite(DATA_0, ((data & 0x01) == 0) ? LOW : HIGH);
  digitalWrite(DATA_1, ((data & 0x02) == 0) ? LOW : HIGH);
  digitalWrite(DATA_2, ((data & 0x04) == 0) ? LOW : HIGH);
  digitalWrite(DATA_3, ((data & 0x08) == 0) ? LOW : HIGH);
  digitalWrite(DATA_4, ((data & 0x10) == 0) ? LOW : HIGH);
  digitalWrite(DATA_5, ((data & 0x20) == 0) ? LOW : HIGH);
  digitalWrite(DATA_6, ((data & 0x40) == 0) ? LOW : HIGH);
  digitalWrite(DATA_7, ((data & 0x80) == 0) ? LOW : HIGH);

  // data latching occurs 'rising edge'
  digitalWrite(WE, HIGH);
}

void resetFlash() {
  digitalWrite(RESET, LOW);
  delayMicroseconds(10);
  
  digitalWrite(RESET, HIGH);
  delayMicroseconds(10);

  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(CE, LOW);
}

void waitForData(unsigned char value) {
  unsigned char data = 0;
  
  setDataPinMode(INPUT);
  while (true) {
    digitalWrite(OE, LOW);
    data  = digitalRead(DATA_0) == HIGH ? 0x01 : 0;
    data |= digitalRead(DATA_1) == HIGH ? 0x02 : 0;
    data |= digitalRead(DATA_2) == HIGH ? 0x04 : 0;
    data |= digitalRead(DATA_3) == HIGH ? 0x08 : 0;
    data |= digitalRead(DATA_4) == HIGH ? 0x10 : 0;
    data |= digitalRead(DATA_5) == HIGH ? 0x20 : 0;
    data |= digitalRead(DATA_6) == HIGH ? 0x40 : 0;
    data |= digitalRead(DATA_7) == HIGH ? 0x80 : 0;
    digitalWrite(OE, HIGH);

    if (data == value) {
      return;
    }
    
    delay(10);
  }
}

void eraseFlash() {  
  commandFlash(0x5555, 0xAA);
  commandFlash(0x2AAA, 0x55);
  commandFlash(0x5555, 0x80);
  commandFlash(0x5555, 0xAA);
  commandFlash(0x2AAA, 0x55);
  commandFlash(0x5555, 0x10);

  waitForData(0xFF);

  digitalWrite(WE, HIGH);
  digitalWrite(CE, LOW);
}

void writeFlash(unsigned long address, unsigned char data) {
  if (data == 0xff) {
    return;
  }
  
  commandFlash(0x5555, 0xAA);
  commandFlash(0x2AAA, 0x55);
  commandFlash(0x5555, 0xA0);
  commandFlash(address, data);
  
  // typical 7, max. 200
  waitForData(data);
}

unsigned char readFlash(unsigned long address) {
  unsigned char data = 0;
  
  addressFlash(address);
  digitalWrite(CE, LOW);
  digitalWrite(OE, LOW);

  setDataPinMode(INPUT);
  
  data |= digitalRead(DATA_0) == HIGH ? 0x01 : 0;
  data |= digitalRead(DATA_1) == HIGH ? 0x02 : 0;
  data |= digitalRead(DATA_2) == HIGH ? 0x04 : 0;
  data |= digitalRead(DATA_3) == HIGH ? 0x08 : 0;
  data |= digitalRead(DATA_4) == HIGH ? 0x10 : 0;
  data |= digitalRead(DATA_5) == HIGH ? 0x20 : 0;
  data |= digitalRead(DATA_6) == HIGH ? 0x40 : 0;
  data |= digitalRead(DATA_7) == HIGH ? 0x80 : 0;

  digitalWrite(OE, HIGH);
  
  return data;
}

void setup() {
  pinMode(RESET, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(CE, OUTPUT);
  
  pinMode(VCC, OUTPUT);
  pinMode(GND, OUTPUT);

  pinMode(ADDRESS_0, OUTPUT);
  pinMode(ADDRESS_1, OUTPUT);
  pinMode(ADDRESS_2, OUTPUT);
  pinMode(ADDRESS_3, OUTPUT);
  pinMode(ADDRESS_4, OUTPUT);
  pinMode(ADDRESS_5, OUTPUT);
  pinMode(ADDRESS_6, OUTPUT);
  pinMode(ADDRESS_7, OUTPUT);
  pinMode(ADDRESS_8, OUTPUT);
  pinMode(ADDRESS_9, OUTPUT);
  pinMode(ADDRESS_10, OUTPUT);
  pinMode(ADDRESS_11, OUTPUT);
  pinMode(ADDRESS_12, OUTPUT);
  pinMode(ADDRESS_13, OUTPUT);
  pinMode(ADDRESS_14, OUTPUT);
  pinMode(ADDRESS_15, OUTPUT);
  pinMode(ADDRESS_16, OUTPUT);
  pinMode(ADDRESS_17, OUTPUT);

  setDataPinMode(OUTPUT);

  digitalWrite(RESET, HIGH);
  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(CE, LOW);

  digitalWrite(GND, LOW);
  digitalWrite(VCC, HIGH);
  
  Serial.begin(19200);

  /* initialization finished */

  resetFlash();
  Serial.println("writer initialized.");
  Serial.println("commands:");
  Serial.println("DEL : delete chip");
  Serial.println("WRTaaaaaaaadd...256bytes...dd : write data");
  Serial.println("DMP : dump chip");
  Serial.println("CRC : checksum chip");
}

boolean crc_table_initialized = false;
unsigned long crc_table[256];
unsigned long crc32;
void make_crc_table(void) {
    for (unsigned long i = 0; i < 256; i++) {
        unsigned long c = i;
        for (int j = 0; j < 8; j++) {
            c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
        }
        crc_table[i] = c;
    }
}

void crc32_init() {
    if (! crc_table_initialized) {
        make_crc_table();
        crc_table_initialized = true;
    }
    
    crc32 = 0xFFFFFFFF;
}

void crc32_calc(unsigned char data) {
    crc32 = crc_table[(crc32 ^ data) & 0xFF] ^ (crc32 >> 8);
}

unsigned long crc32_get() {
    return crc32 ^ 0xFFFFFFFF;
}

char buffer[256];

void readAll() {
  while (Serial.available() > 0) {
    Serial.read();
  }
} 

void loop() {
  if (Serial.readBytes(buffer, 3) == 3) {
    if (strncmp("WRT", buffer, 3) == 0) {
      unsigned long address;
      
      // get address
      if (Serial.readBytes(buffer, 4) != 4) {
        Serial.println("write failed.");
        Serial.flush();
        return;
      }
      
      address = 0;//(unsigned long)((unsigned long)buffer[0] << 24L);
      address |= (unsigned long)((buffer[1] & 0xffUL) << 16UL);
      address |= (unsigned long)((buffer[2] & 0xffUL) << 8UL);
      address |= ((unsigned long)(buffer[3] & 0xffUL));
      address &= 0x003ffffUL;
      
      Serial.println(address >> 16UL, DEC);
      Serial.println(address & 0xFFFF, DEC);

      // get data 256 bytes
      if (Serial.readBytes(buffer, 256) != 256) {
        Serial.println("write failed.");
        Serial.flush();
        return;
      }
      
      for (unsigned long i = 0; i < 256; i++) {
        writeFlash((unsigned long)(address + i), buffer[i]);
      }
      Serial.println("write completed!");
    } else if (strncmp("DEL", buffer, 3) == 0) {
      resetFlash();
      eraseFlash();
      resetFlash();
      Serial.println("chip erase completed!");
    } else if (strncmp("CRC", buffer, 3) == 0) {
      crc32_init();
      for (unsigned long i = 0; i < 262144; i++) {
        crc32_calc(readFlash(i));
      }
      Serial.print("CRC:");
      Serial.print(crc32_get(), HEX);
      Serial.println();
      Serial.println("crc calculation completed!");
    } else if (strncmp("DMP", buffer, 3) == 0) {
      for (unsigned long i = 0; i < 262144; i++) {
        Serial.print(readFlash(i), HEX);
        Serial.print(" ");
        if (i % 256 == 255) {
          Serial.println();
        }
      }
      Serial.println();
      Serial.println("dump completed!");
    }
  }
  Serial.flush();
}
