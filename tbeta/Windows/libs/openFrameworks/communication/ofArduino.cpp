/*
 * Copyright 2007-2008 (c) Erik Sjodin, eriksjodin.net
 *
 * Devloped at: The Interactive Institutet / Art and Technology, 
 * OF Lab / Ars Electronica
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial _portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
 
#include "ofArduino.h"


ofArduino::ofArduino():ofStandardFirmata(){
	for (int i=0; i<ARD_TOTAL_DIGITAL_PINS; ++i) {
		_servoValue[i] = -1;
	}
	_temp = -1;
	_humidity = -1;
}

ofArduino::~ofArduino() {
}

int ofArduino::connect(string device, int baud){
	return ofStandardFirmata::connect(device, baud);
}

void ofArduino::sendServo(int pin, int value, bool force){
	if(_digitalPinMode[pin]==ARD_SERVO && (_servoValue[pin]!=value || force)){
		sendByte(FIRMATA_START_SYSEX);
		sendByte(SYSEX_SERVO_WRITE);
		sendByte(pin);
		sendValueAsTwo7bitBytes(value);
		sendByte(FIRMATA_END_SYSEX);
		_servoValue[pin]=value;
	}
}
 
void ofArduino::sendServoAttach(int pin, int minPulse, int maxPulse, int angle) {
	sendByte(FIRMATA_START_SYSEX);
	sendByte(SYSEX_SERVO_ATTACH);
	sendByte(pin);
	sendValueAsTwo7bitBytes(minPulse);
	sendValueAsTwo7bitBytes(maxPulse);
	sendByte(FIRMATA_END_SYSEX);
	_digitalPinMode[pin]=ARD_SERVO;
}
 
void ofArduino::sendServoDetach(int pin) {
	sendByte(FIRMATA_START_SYSEX);
	sendByte(SYSEX_SERVO_DETACH);
	sendByte(pin);
	sendByte(FIRMATA_END_SYSEX);
	_digitalPinMode[pin]=ARD_OUTPUT;
}

int ofArduino::getServo(int pin){
	if(_digitalPinMode[pin]==ARD_SERVO)
		return _servoValue[pin];
	else
		return -1;
}

void ofArduino::sendWireBegin(int address) {
	sendAnalogPinReporting(4, ARD_OFF);
	sendAnalogPinReporting(5, ARD_OFF);
	sendByte(FIRMATA_START_SYSEX);
	sendByte(SYSEX_WIRE_BEGIN);
	if(address>-1)
		sendByte(address);
	_digitalPinMode[20]=ARD_WIRE;
	_digitalPinMode[21]=ARD_WIRE;
	sendByte(FIRMATA_END_SYSEX);
}

void ofArduino::sendWireBeginTransmission(int address) {
	if(_digitalPinMode[20]==ARD_WIRE && _digitalPinMode[21]==ARD_WIRE){
		sendByte(FIRMATA_START_SYSEX);
		sendByte(SYSEX_WIRE_SEND);
		sendByte(address);
	}
}

void ofArduino::sendWire(int byte) {
	if(_digitalPinMode[20]==ARD_WIRE && _digitalPinMode[21]==ARD_WIRE)
		sendValueAsTwo7bitBytes(byte);
}

void ofArduino::sendWireEndTransmission() {
	if(_digitalPinMode[20]==ARD_WIRE && _digitalPinMode[21]==ARD_WIRE)
		sendByte(FIRMATA_END_SYSEX);
}

void ofArduino::sendShtAttach(int dataPin, int clockPin) {
	sendByte(FIRMATA_START_SYSEX);
	sendByte(SYSEX_SHT_ATTACH);
	sendByte(dataPin);
	sendByte(clockPin);
	sendByte(FIRMATA_END_SYSEX);
	_digitalPinMode[dataPin]=ARD_SHT;
	_digitalPinMode[clockPin]=ARD_SHT;
}

void ofArduino::sendShtDetach(int dataPin, int clockPin) {
	_digitalPinMode[dataPin]=ARD_OUTPUT;
	_digitalPinMode[clockPin]=ARD_OUTPUT;
}

void ofArduino::sendShtRequest() {
	sendByte(FIRMATA_START_SYSEX);
	sendByte(SYSEX_SHT_MESSAGE);
	sendByte(FIRMATA_END_SYSEX);
}

float ofArduino::getTemp(){
		return _temp;
}

float ofArduino::getHumidity(){
		return _humidity;
}

void ofArduino::sendBlinkMAttach(){
	sendWireBegin();				
}

void ofArduino::sendBlinkMStopScript(int address){
	sendWireBeginTransmission(address);	
	sendWire('o');
	sendWireEndTransmission();				
}

void ofArduino::sendBlinkMDisableStartupScript(int address){
	sendWireBeginTransmission(address);	
	sendWire('B');
	sendWire(0);
	sendWire(0);
	sendWire(0);
	sendWire(10);
	sendWire(0);
	sendWireEndTransmission();				
}

void ofArduino::sendBlinkMSetRGB(int address, int r, int g, int b){
	sendWireBeginTransmission(address);
	sendWire('n');
	sendWire(r);
	sendWire(g);
	sendWire(b);
	sendWireEndTransmission();
}

void ofArduino::sendBlinkMFadeToRGB(int address, int r, int g, int b){
	sendWireBeginTransmission(address);
	sendWire('c');
	sendWire(r);
	sendWire(g);
	sendWire(b);
	sendWireEndTransmission();
}

void ofArduino::sendBlinkMFadeSpeed(int address, int speed){
	sendWireBeginTransmission(address);
	sendWire('f');
	sendWire(speed);
	sendWireEndTransmission(); 
}

void ofArduino::sendBlinkMSetAddress(int address){
	sendWireBeginTransmission(0); // general call
	sendWire('A');
	sendWire(address);
	sendWire(0xD0);
	sendWire(0x0D);
	sendWire(address);
	sendWireEndTransmission();
}

void ofArduino::sendBlinkMPlayScript(int address, int script, int times, int pos){
	sendWireBeginTransmission(address); 
	sendWire('p');
	sendWire(script);
	sendWire(times);
	sendWire(pos);
	sendWire(address);
}
				
// sysex data is assumed to be 8-bit bytes split into two 7-bit bytes.
void ofArduino::processSysExData(vector<unsigned char> data){
	      
	vector<unsigned char>::iterator it;
	unsigned char buffer;
	int i = 1;
				   
	// act on reserved sysEx messages (extended commands) or trigger SysEx event...
	switch(data.front()) { //first byte in buffer is command
		case SYSEX_SHT_MESSAGE:
			it = data.begin();
			it++; // skip the first byte, which is the sht message command
			_temp = *it;
			it++;
			_temp += (float)*it / 100;
			it++;
			_humidity = *it;
			it++;
			_humidity+= (float)*it/100;
		break;
		default: // the message isn't in this class extended command set, pass it...
			ofStandardFirmata::processSysExData(data);
		break;
			   
	}
}

