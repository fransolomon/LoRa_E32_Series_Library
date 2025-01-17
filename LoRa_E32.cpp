/*
 * EBYTE LoRa E32 Series
 * https://www.mischianti.org/category/my-libraries/lora-e32-devices/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Renzo Mischianti www.mischianti.org All right reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "LoRa_E32.h"

LoRa_E32::LoRa_E32(byte rxPin, byte txPin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;
    SoftwareSerial* mySerial = new SoftwareSerial((uint8_t)this->rxPin, (uint8_t)this->txPin); // "RX TX" // @suppress("Abstract class cannot be instantiated")
    this->ss = mySerial;
    this->hs = NULL;

    this->bpsRate = bpsRate;
}
LoRa_E32::LoRa_E32(byte rxPin, byte txPin, byte auxPin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->auxPin = auxPin;
    SoftwareSerial* mySerial = new SoftwareSerial((uint8_t)this->rxPin, (uint8_t)this->txPin); // "RX TX" // @suppress("Abstract class cannot be instantiated")
    this->ss = mySerial;
    this->hs = NULL;

    this->bpsRate = bpsRate;
}
LoRa_E32::LoRa_E32(byte rxPin, byte txPin, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->auxPin = auxPin;

    this->m0Pin = m0Pin;
    this->m1Pin = m1Pin;

    SoftwareSerial* mySerial = new SoftwareSerial((uint8_t)this->rxPin, (uint8_t)this->txPin); // "RX TX" // @suppress("Abstract class cannot be instantiated")
    this->ss = mySerial;
    this->hs = NULL;

    this->bpsRate = bpsRate;
}

LoRa_E32::LoRa_E32(HardwareSerial* serial, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->ss = NULL;
    this->hs = serial;

    this->bpsRate = bpsRate;
}
LoRa_E32::LoRa_E32(HardwareSerial* serial, byte auxPin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->auxPin = auxPin;

    this->ss = NULL;
    this->hs = serial;

    this->bpsRate = bpsRate;
}
LoRa_E32::LoRa_E32(HardwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->auxPin = auxPin;

    this->m0Pin = m0Pin;
    this->m1Pin = m1Pin;

    this->ss = NULL;
    this->hs = serial;

    this->bpsRate = bpsRate;
}

LoRa_E32::LoRa_E32(SoftwareSerial* serial, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->ss = serial;
    this->hs = NULL;

    this->bpsRate = bpsRate;
}
LoRa_E32::LoRa_E32(SoftwareSerial* serial, byte auxPin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->auxPin = auxPin;

    this->ss = serial;
    this->hs = NULL;

    this->bpsRate = bpsRate;
}
LoRa_E32::LoRa_E32(SoftwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate){
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->auxPin = auxPin;

    this->m0Pin = m0Pin;
    this->m1Pin = m1Pin;

    this->ss = serial;
    this->hs = NULL;

    this->bpsRate = bpsRate;
}

bool LoRa_E32::begin(){
	if (this->auxPin != 0) {
		pinMode(this->auxPin, INPUT);
		DEBUG_PRINTLN("Init AUX pin!");
	}
	if (this->m0Pin != 0) {
		pinMode(this->m0Pin, OUTPUT);
		DEBUG_PRINTLN("Init M0 pin!");
		digitalWrite(this->m0Pin, HIGH);

	}
	if (this->m1Pin != 0) {
		pinMode(this->m1Pin, OUTPUT);
		DEBUG_PRINTLN("Init M1 pin!");
		digitalWrite(this->m1Pin, HIGH);

	}

    DEBUG_PRINTLN("Begin ex");
    if (this->hs){
        DEBUG_PRINTLN("Begin Hardware Serial");

    	this->serialDef.begin(*this->hs, this->bpsRate);
	}else if (this->ss){
        DEBUG_PRINTLN("Begin Software Serial");

		this->serialDef.begin(*this->ss, this->bpsRate);
	}	else{
        DEBUG_PRINTLN("Begin Software Serial Pin");
        SoftwareSerial* mySerial = new SoftwareSerial((int)this->rxPin, (int)this->txPin); // "RX TX" // @suppress("Abstract class cannot be instantiated")
        this->ss = mySerial;

//		SoftwareSerial mySerial(this->rxPin, this->txPin);
        DEBUG_PRINT("RX Pin: ");
        DEBUG_PRINT((int)this->rxPin);
        DEBUG_PRINT("TX Pin: ");
        DEBUG_PRINTLN((int)this->txPin);

		this->serialDef.begin(*this->ss, this->bpsRate);
	}

    this->serialDef.stream->setTimeout(1000);
    Status status = setMode(MODE_0_NORMAL);
    return status==SUCCESS;
}

/*

Utility method to wait until module is doen tranmitting
a timeout is provided to avoid an infinite loop

*/

Status LoRa_E32::waitCompleteResponse(unsigned long timeout, unsigned int waitNoAux) {

	Status result = SUCCESS;

	unsigned long t = millis();

	// make darn sure millis() is not about to reach max data type limit and start over
	if (((unsigned long) (t + timeout)) == 0){
		t = 0;
	}

	// if AUX pin was supplied and look for HIGH state
	// note you can omit using AUX if no pins are available, but you will have to use delay() to let module finish
	if (this->auxPin != 0) {
		while (digitalRead(this->auxPin) == LOW) {
			if ((millis() - t) > timeout){
				result = ERR_TIMEOUT;
				DEBUG_PRINTLN("Timeout error!");
				return result;
			}
		}
		DEBUG_PRINTLN("AUX HIGH!");
	}
	else {
		// if you can't use aux pin, use 4K7 pullup with Arduino
		// you may need to adjust this value if transmissions fail
		this->managedDelay(waitNoAux);
		DEBUG_PRINTLN("Wait no AUX pin!");
	}


	// per data sheet control after aux goes high is 2ms so delay for at least that long)
	this->managedDelay(20);

	return result;
}

/*

delay() in a library is not a good idea as it can stop interrupts
just poll internal time until timeout is reached

*/


void LoRa_E32::managedDelay(unsigned long timeout) {

	unsigned long t = millis();

	// make darn sure millis() is not about to reach max data type limit and start over
	if (((unsigned long) (t + timeout)) == 0){
		t = 0;
	}

	while ((millis() - t) < timeout) 	{ 	}

}

/*

Method to indicate availability

*/

int LoRa_E32::available(unsigned long timeout) {
//	unsigned long t = millis();
//
//	// make darn sure millis() is not about to reach max data type limit and start over
//	if (((unsigned long) (t + timeout)) == 0){
//		t = 0;
//	}
//
//	if (this->auxPin != 0) {
//		if (digitalRead(this->auxPin) == HIGH){
//			return 0;
//		}else{
//			while (digitalRead(this->auxPin) == LOW) {
//				if ((millis() - t) > timeout){
//					DEBUG_PRINTLN("Timeout error!");
//					return 0;
//				}
//			}
//			DEBUG_PRINTLN("AUX HIGH!");
//			return 2;
//		}
//	}else{
		return this->serialDef.stream->available();
//	}
}

/*

Method to indicate availability

*/

void LoRa_E32::flush() {
	this->serialDef.stream->flush();
}


void LoRa_E32::cleanUARTBuffer()
{
//  bool IsNull = true;

  while (this->available())
  {
//    IsNull = false;

    this->serialDef.stream->read();
  }
}


/*

Method to send a chunk of data provided data is in a struct--my personal favorite as you
need not parse or worry about sprintf() inability to handle floats

TTP: put your structure definition into a .h file and include in both the sender and reciever
sketches

NOTE: of your sender and receiver MCU's are different (Teensy and Arduino) caution on the data
types each handle ints floats differently

*/

Status LoRa_E32::sendStruct(void *structureManaged, uint16_t size_) {
		Status result = SUCCESS;

		uint8_t len = this->serialDef.stream->write((uint8_t *) structureManaged, size_);
		if (len!=size_){
			result = ERR_DATA_SIZE_NOT_MATCH;
		}

		result = this->waitCompleteResponse(1000);
		if (result != SUCCESS) return result;

		this->cleanUARTBuffer();

		return result;
}


/*

Method to get a chunk of data provided data is in a struct--my personal favorite as you
need not parse or worry about sprintf() inability to handle floats

TTP: put your structure definition into a .h file and include in both the sender and reciever
sketches

NOTE: of your sender and receiver MCU's are different (Teensy and Arduino) caution on the data
types each handle ints floats differently

*/


Status LoRa_E32::receiveStruct(void *structureManaged, uint16_t size_) {
	Status result = SUCCESS;

	uint8_t len = this->serialDef.stream->readBytes((uint8_t *) structureManaged, size_);
	if (len!=size_){
		result = ERR_DATA_SIZE_NOT_MATCH;
	}
	if (result != SUCCESS) return result;

	result = this->waitCompleteResponse(1000);
	if (result != SUCCESS) return result;

	DEBUG_PRINT("Available buffer: ");
	DEBUG_PRINT(len);
	DEBUG_PRINT(" structure size: ");
	DEBUG_PRINTLN(size_);

	return result;
}

/*

method to set the mode (program, normal, etc.)

*/

Status LoRa_E32::setMode(uint8_t mode) {

	// data sheet claims module needs some extra time after mode setting (2ms)
	// most of my projects uses 10 ms, but 40ms is safer

	this->managedDelay(40);

	if (this->m0Pin == 0 && this->m1Pin == 0) {
		DEBUG_PRINTLN(F("The M0 and M1 pins is not set, this mean that you are connect directly the pins as you need!"))
	}else{
		switch (mode)
		{
		  case MODE_0_NORMAL:
			// Mode 0 | normal operation
			digitalWrite(this->m0Pin, LOW);
			digitalWrite(this->m1Pin, LOW);
			DEBUG_PRINTLN("MODE NORMAL!");
			break;
		  case MODE_1_WAKE_UP:
			digitalWrite(this->m0Pin, HIGH);
			digitalWrite(this->m1Pin, LOW);
			DEBUG_PRINTLN("MODE WAKE UP!");
			break;
		  case MODE_2_POWER_SAVING:
			digitalWrite(this->m0Pin, LOW);
			digitalWrite(this->m1Pin, HIGH);
			DEBUG_PRINTLN("MODE POWER SAVING!");
			break;
		  case MODE_3_SLEEP:
			// Mode 3 | Setting operation
			digitalWrite(this->m0Pin, HIGH);
			digitalWrite(this->m1Pin, HIGH);
			DEBUG_PRINTLN("MODE PROGRAM/SLEEP!");
			break;
		  default:
			return ERR_INVALID_PARAM;
		}
	}
	// data sheet says 2ms later control is returned, let's give just a bit more time
	// these modules can take time to activate pins
	this->managedDelay(40);

	// wait until aux pin goes back low
	Status res = this->waitCompleteResponse(1000);

	return res;
}

void LoRa_E32::writeProgramCommand(PROGRAM_COMMAND cmd){
	  uint8_t CMD[3] = {cmd, cmd, cmd};
	  uint8_t size = this->serialDef.stream->write(CMD, 3);
	  DEBUG_PRINTLN(size);
	  this->managedDelay(50);  //need ti check
}

ResponseStructContainer LoRa_E32::getConfiguration(){
	MODE_TYPE prevMode = this->mode;

	ResponseStructContainer rc;
	rc.status.code = this->setMode(MODE_3_PROGRAM);
	if (rc.status.code!=SUCCESS) return rc;

	this->writeProgramCommand(READ_CONFIGURATION);

	rc.data = malloc(sizeof(Configuration));
	rc.status.code = this->receiveStruct((uint8_t *)rc.data, sizeof(Configuration));
	if (rc.status.code!=SUCCESS) return rc;

	DEBUG_PRINTLN("----------------------------------------");
	DEBUG_PRINT(F("HEAD BIN INSIDE: "));  DEBUG_PRINT(((Configuration *)rc.data)->HEAD, BIN);DEBUG_PRINT(" ");DEBUG_PRINT(((Configuration *)rc.data)->HEAD, DEC);DEBUG_PRINT(" ");DEBUG_PRINTLN(((Configuration *)rc.data)->HEAD, HEX);
	DEBUG_PRINTLN("----------------------------------------");

	rc.status.code = this->setMode(prevMode);
	if (rc.status.code!=SUCCESS) return rc;

//	this->printParameters(*configuration);

	if (0xC0 != ((Configuration *)rc.data)->HEAD && 0xC2 != ((Configuration *)rc.data)->HEAD){
		rc.status.code = ERR_HEAD_NOT_RECOGNIZED;
	}

//	rc.data = configuration;
	return rc;
}

ResponseStatus LoRa_E32::setConfiguration(Configuration configuration, PROGRAM_COMMAND saveType){
	MODE_TYPE prevMode = this->mode;

	ResponseStatus rc;
	rc.code = this->setMode(MODE_3_PROGRAM);
	if (rc.code!=SUCCESS) return rc;

	this->writeProgramCommand(READ_CONFIGURATION);

	configuration.HEAD = saveType;

	rc.code = this->sendStruct((uint8_t *)&configuration, sizeof(Configuration));
	if (rc.code!=SUCCESS) return rc;

	DEBUG_PRINTLN("----------------------------------------");
	DEBUG_PRINT(F("HEAD BIN INSIDE: "));  DEBUG_PRINT(configuration.HEAD, BIN);DEBUG_PRINT(" ");DEBUG_PRINT(configuration.HEAD, DEC);DEBUG_PRINT(" ");DEBUG_PRINTLN(configuration.HEAD, HEX);
	DEBUG_PRINTLN("----------------------------------------");

	rc.code = this->setMode(prevMode);
	if (rc.code!=SUCCESS) return rc;

//	this->printParameters(*configuration);

	if (0xC0 != configuration.HEAD && 0xC2 != configuration.HEAD){
		rc.code = ERR_HEAD_NOT_RECOGNIZED;
	}

	return rc;
}

ResponseStructContainer LoRa_E32::getModuleInformation(){
	MODE_TYPE prevMode = this->mode;

	ResponseStructContainer rc;
	rc.status.code = this->setMode(MODE_3_PROGRAM);
	if (rc.status.code!=SUCCESS) return rc;

	this->writeProgramCommand(READ_MODULE_VERSION);

	struct ModuleInformation *moduleInformation = (ModuleInformation *)malloc(sizeof(ModuleInformation));
	rc.status.code = this->receiveStruct((uint8_t *)moduleInformation, sizeof(ModuleInformation));
	if (rc.status.code!=SUCCESS) {
		return rc;
	}

	rc.status.code = this->setMode(prevMode);
	if (rc.status.code!=SUCCESS) return rc;

//	this->printParameters(*configuration);

	if (0xC3 != moduleInformation->HEAD){
		rc.status.code = ERR_HEAD_NOT_RECOGNIZED;
	}

	DEBUG_PRINTLN("----------------------------------------");
	DEBUG_PRINT(F("HEAD BIN INSIDE: "));  DEBUG_PRINT(moduleInformation->HEAD, BIN);DEBUG_PRINT(" ");DEBUG_PRINT(moduleInformation->HEAD, DEC);DEBUG_PRINT(" ");DEBUG_PRINTLN(moduleInformation->HEAD, HEX);

	DEBUG_PRINT(F("Model no.: "));  DEBUG_PRINTLN(moduleInformation->model, HEX);
	DEBUG_PRINT(F("Version  : "));  DEBUG_PRINTLN(moduleInformation->version, HEX);
	DEBUG_PRINT(F("Features : "));  DEBUG_PRINTLN(moduleInformation->features, HEX);
	DEBUG_PRINTLN("----------------------------------------");

	rc.data = moduleInformation; // malloc(sizeof (moduleInformation));

	return rc;
}


ResponseStatus LoRa_E32::resetModule(){
	MODE_TYPE prevMode = this->mode;

	ResponseStatus status;
	status.code = this->setMode(MODE_3_PROGRAM);
	if (status.code!=SUCCESS) return status;

	this->writeProgramCommand(WRITE_RESET_MODULE);

	status.code = this->waitCompleteResponse(1000);
	if (status.code!=SUCCESS) return status;


	status.code = this->setMode(prevMode);
	if (status.code!=SUCCESS) return status;

	return status;
}

ResponseContainer LoRa_E32::receiveMessage(){
	ResponseContainer rc;
	rc.status.code = SUCCESS;
	rc.data = this->serialDef.stream->readString();
	this->cleanUARTBuffer();
	if (rc.status.code!=SUCCESS) {
		return rc;
	}

//	rc.data = message; // malloc(sizeof (moduleInformation));

	return rc;
}
ResponseStructContainer LoRa_E32::receiveMessage(const uint8_t size){
	ResponseStructContainer rc;

	rc.data = malloc(size);
	rc.status.code = this->receiveStruct((uint8_t *)rc.data, size);
	this->cleanUARTBuffer();
	if (rc.status.code!=SUCCESS) {
		return rc;
	}

//	rc.data = message; // malloc(sizeof (moduleInformation));

	return rc;
}

ResponseStatus LoRa_E32::sendMessage(const void *message, const uint8_t size){
	ResponseStatus status;
	status.code = this->sendStruct((uint8_t *)message, size);
	if (status.code!=SUCCESS) return status;

	return status;
}
ResponseStatus LoRa_E32::sendMessage(const String message){
	DEBUG_PRINT("Send message: ");
	DEBUG_PRINT(message);
	byte size = message.length(); // sizeof(message.c_str())+1;
	DEBUG_PRINT(" size: ");
	DEBUG_PRINTLN(size);
	char messageFixed[size];
	memcpy(messageFixed,message.c_str(),size);

	ResponseStatus status;
	status.code = this->sendStruct((uint8_t *)&messageFixed, size);
	if (status.code!=SUCCESS) return status;

	free(messageFixed);
	return status;
}

ResponseStatus LoRa_E32::sendFixedMessage(byte ADDH, byte ADDL, byte CHAN, const String message){
//	DEBUG_PRINT("String/size: ");
//	DEBUG_PRINT(message);
//	DEBUG_PRINT("/");
	byte size = message.length(); // sizeof(message.c_str())+1;
//	DEBUG_PRINTLN(size);
//
//	#pragma pack(push, 1)
//	struct FixedStransmissionString {
//		byte ADDH = 0;
//		byte ADDL = 0;
//		byte CHAN = 0;
//		char message[];
//	} fixedStransmission;
//	#pragma pack(pop)
//
//	fixedStransmission.ADDH = ADDH;
//	fixedStransmission.ADDL = ADDL;
//	fixedStransmission.CHAN = CHAN;
//	char* msg = (char*)message.c_str();
//	memcpy(fixedStransmission.message, (char*)msg, size);
////	fixedStransmission.message = message;
//
//	DEBUG_PRINT("Message: ");
//	DEBUG_PRINTLN(fixedStransmission.message);
//
//	ResponseStatus status;
//	status.code = this->sendStruct((uint8_t *)&fixedStransmission, sizeof(fixedStransmission));
//	if (status.code!=SUCCESS) return status;
//
//	return status;
	char messageFixed[size];
	memcpy(messageFixed,message.c_str(),size);
	return this->sendFixedMessage(ADDH, ADDL, CHAN, (uint8_t *)messageFixed, size);
}
ResponseStatus LoRa_E32::sendBroadcastFixedMessage(byte CHAN, const String message){
	return this->sendFixedMessage(0xFF, 0xFF, CHAN, message);
}

typedef struct fixedStransmission
{
	byte ADDH = 0;
	byte ADDL = 0;
	byte CHAN = 0;
	unsigned char message[];
}FixedStransmission;

FixedStransmission *init_stack(int m){
	FixedStransmission *st = (FixedStransmission *)malloc(sizeof(FixedStransmission)+m*sizeof(int));
    return st;
}

ResponseStatus LoRa_E32::sendFixedMessage( byte ADDH,byte ADDL, byte CHAN, const void *message, const uint8_t size){
//	#pragma pack(push, 1)
//	struct FixedStransmission {
//		byte ADDH = 0;
//		byte ADDL = 0;
//		byte CHAN = 0;
//		unsigned char message[];
//	} fixedStransmission;
//	#pragma pack(pop)


	FixedStransmission *fixedStransmission = init_stack(size);

//	STACK *resize_stack(STACK *st, int m){
//	    if (m<=st->max){
//	         return st; /* Take sure do not kill old values */
//	    }
//	    STACK *st = (STACK *)realloc(sizeof(STACK)+m*sizeof(int));
//	    st->max = m;
//	    return st;
//	}

	fixedStransmission->ADDH = ADDH;
	fixedStransmission->ADDL = ADDL;
	fixedStransmission->CHAN = CHAN;
//	fixedStransmission.message = &message;

	memcpy(fixedStransmission->message,(unsigned char*)message,size);

	ResponseStatus status;
	status.code = this->sendStruct((uint8_t *)fixedStransmission, size+3);
	if (status.code!=SUCCESS) return status;

	free(fixedStransmission);

	return status;
}
ResponseStatus LoRa_E32::sendBroadcastFixedMessage(byte CHAN, const void *message, const uint8_t size){
	return this->sendFixedMessage(0xFF, 0xFF, CHAN, message, size);
}

ResponseContainer LoRa_E32::receiveInitialMessage(uint8_t size){
	ResponseContainer rc;
	rc.status.code = SUCCESS;
	char buff[size];
	uint8_t len = this->serialDef.stream->readBytes(buff, size);
	if (len!=size) {
		rc.status.code = ERR_DATA_SIZE_NOT_MATCH;
		return rc;
	}

	rc.data = buff; // malloc(sizeof (moduleInformation));

	return rc;
}

#define KeeLoq_NLF		0x3A5C742E

unsigned long LoRa_E32::encrypt(unsigned long data)
{
  unsigned long x = data;
  unsigned long r;
  int keyBitNo, index;
  unsigned long keyBitVal,bitVal;

  for (r = 0; r < 528; r++)
  {
    keyBitNo = r & 63;
    if(keyBitNo < 32)
      keyBitVal = bitRead(this->halfKeyloqKey,keyBitNo); // key low
    else
      keyBitVal = bitRead(this->halfKeyloqKey, keyBitNo - 32);// key hight
    index = 1 * bitRead(x,1) + 2 * bitRead(x,9) + 4 * bitRead(x,20) + 8 * bitRead(x,26) + 16 * bitRead(x,31);
    bitVal = bitRead(x,0) ^ bitRead(x, 16) ^ bitRead(KeeLoq_NLF,index) ^ keyBitVal;
    x = (x>>1) ^ bitVal<<31;
  }
  return x;
}

unsigned long LoRa_E32::decrypt(unsigned long data)
{
  unsigned long x = data;
  unsigned long r;
  int keyBitNo, index;
  unsigned long keyBitVal,bitVal;

  for (r = 0; r < 528; r++)
  {
    keyBitNo = (15-r) & 63;
    if(keyBitNo < 32)
      keyBitVal = bitRead(this->halfKeyloqKey,keyBitNo); // key low
    else
      keyBitVal = bitRead(this->halfKeyloqKey, keyBitNo - 32); // key hight
    index = 1 * bitRead(x,0) + 2 * bitRead(x,8) + 4 * bitRead(x,19) + 8 * bitRead(x,25) + 16 * bitRead(x,30);
    bitVal = bitRead(x,31) ^ bitRead(x, 15) ^ bitRead(KeeLoq_NLF,index) ^ keyBitVal;
    x = (x<<1) ^ bitVal;
  }
  return x;
 }

