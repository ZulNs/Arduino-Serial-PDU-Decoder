/*
  Serial PDU Decoder
 
 Decodes a line of PDU hex text to 7-bits readable text.
 Entered text must be terminate by a newline or a carriage return or both characters.
 
 Created 19 August 2017
 @Gorontalo, Indonesia
 by ZulNs
 
 This example code is in the public domain.
 */

char strBuffer[9];  // for storing max 8 chrs plus null termination chr
byte chrCtr = 0;
byte blockCtr = 0;
boolean isPdu = false;
boolean isFirstByte = true;

void setup() {
  Serial.begin(115200);    // the GPRS baud rate 
  Serial.println(F("*** Serial PDU Decoder by ZulNs ***"));
  Serial.println();
  strBuffer[8] = 0;
}

void loop() {
  byte chr;
  boolean pduHex;
  
  while (Serial.available()) {
    chr = Serial.read();
    /*
    if (chr == '\n' || chr == '\r') {
      if (blockCtr > 0) {
        strBuffer[blockCtr] = 0;
        decodePdu();
        Serial.print(strBuffer);
        //writeStr();
        chrCtr += blockCtr;
        blockCtr = 0;
      }
      if (chrCtr > 0) {
        Serial.print(F("\nLength: "));
        Serial.print(chrCtr, DEC);
        Serial.print(F(" chrs\n\n"));
        chrCtr = 0;
      }
    }
    else {
      strBuffer[blockCtr++] = chr;
      if (blockCtr == 7) {
        decodePdu();
        Serial.print(strBuffer);
        //writeStr();
        chrCtr += 8;
        blockCtr = 0;
      }
    }
    */
    if (chr >= 'a' && chr <= 'f')
      chr -= 32; // make upper case
    pduHex = chr >= '0' && chr <= '9' || chr >= 'A' && chr <= 'F';
    if (isPdu || pduHex) {
      if (!pduHex) {
        isPdu = false;
        isFirstByte = true;
        if (blockCtr > 0) {
          strBuffer[blockCtr] = 0;
          decodePdu();
          Serial.print(strBuffer);
          //writeStr();
          chrCtr += blockCtr;
          blockCtr = 0;
        }
        if (chrCtr > 0) {
          Serial.print(F("\nLength: "));
          Serial.print(chrCtr, DEC);
          Serial.print(F(" chrs\n\n"));
          chrCtr = 0;
        }
      }
      else
        isPdu = true;
      if (isPdu) {
        chr -= 48;
        if (chr > 9)
          chr -= 7;
        if (isFirstByte)
          strBuffer[blockCtr] = chr << 4;
        else {
          strBuffer[blockCtr++] |= chr;
          if (blockCtr == 7) {
            decodePdu();
            Serial.print(strBuffer);
            //writeStr();
            chrCtr += 8;
            blockCtr = 0;
          }
        }
        isFirstByte = !isFirstByte;
      }
    }
  }
}

void decodePdu() {
  byte ctr = 0;
  uint16_t wrd;
  while (strBuffer[ctr] != 0) {
    wrd = strBuffer[ctr] << 8 | lowByte(wrd);
    wrd >>= 8 - ctr;
    strBuffer[ctr] = lowByte(wrd) & 0x7F;
    wrd >>= ctr++;
    if (ctr == 7) {
      strBuffer[ctr++] = lowByte(wrd) >> 1;
      return;
    }
  }
}

void writeStr() {
  byte ctr = 0;
  while (strBuffer[ctr] != 0)
    write2Console(strBuffer[ctr++]);
}

void write2Console(char chr) {
  if (chr == '\t')
    Serial.print(F("\\t"));
  else if (chr == '\n')
    Serial.print(F("\\n"));
  else if (chr == '\n')
    Serial.print(F("\\r"));
  Serial.print(chr);
}

