/*
 * Copyright 2007-2008 (c) Erik Sjodin, eriksjodin.net
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
 * included in all copies or substantial portions of the Software.
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
 
#ifndef OF_ARDUINO_H
#define OF_ARDUINO_H

#include "ofStandardFirmata.h"

		#define SYSEX_SERVO_ATTACH                      0x00
		#define SYSEX_SERVO_DETACH                      0x01
		#define SYSEX_SERVO_WRITE                       0x02
		#define SYSEX_SHT_ATTACH                        0x03
		#define SYSEX_SHT_MESSAGE                       0x04
		#define SYSEX_WIRE_BEGIN                        0x05
		#define SYSEX_WIRE_SEND                         0x06

		#define ARD_SHT									0x05
		#define ARD_WIRE								0x06
		
/**
        This class extend ofStandardFirmata and provides additional functionality like servo support through SysEx messages.
		It is intended to be used with the Firmata v2 library, the GlueFirmata firmware and additional ofArduino libraries, the latter can be found at
		http://arduinoglue.googlecode.com 
**/


class ofArduino: public ofStandardFirmata{

        public:
                ofArduino();

                ~ofArduino();

				int connect(string device, int baud=57600);
                // opens a serial port connection to the arduino
				// Note: GlueFirmata is set to use 57600 bps by default since 115200 (the firmata default) has proven to be unreliable
				
				// -- servo
			    void sendServo(int pin, int value, bool force=false);
                // pin: 9, 10
				// the pin has to have a servo attached
				
                void sendServoAttach(int pin, int minPulse=544, int maxPulse=2400, int angle=180);
				// pin: 9, 10
                // attaches a servo to a pin

                void sendServoDetach(int pin);
				// pin: 9, 10
                // detaches a servo from a pin, the pin mode remains as OUTPUT
				
				int getServo(int pin);
				// returns the last set servo value for a pin if the pin has a servo attached
				
				// --- wire
				void sendWireBegin(int address=-1);
				// 
				
				void sendWireBeginTransmission(int address);
				//
				
				void sendWire(int byte);
				// uses to bits to send wire data
				
				void sendWireEndTransmission();
				// 
				
				// --- sht
				void sendShtAttach(int dataPin, int clockPin);
                // attaches a sht sensor
				
				void sendShtDetach(int dataPin, int clockPin);
                // attaches a sht sensor
				
				void sendShtRequest();
                // requests data from an attached sht sensor
				
				float getTemp();
				float getHumidity();
				
				// --- blinkM
				void sendBlinkMAttach();
				void sendBlinkMStopScript(int address);
				void sendBlinkMDisableStartupScript(int address);
				void sendBlinkMSetRGB(int address, int r, int g, int b);
				void sendBlinkMFadeToRGB(int address, int r, int g, int b);
				void sendBlinkMFadeSpeed(int address, int speed);
				// time to do fade in, 255 = instanteneous
				void sendBlinkMSetAddress(int address);
				// program the address of a blinkM
				void sendBlinkMPlayScript(int address, int script, int times, int pos);
				
		protected:
		
				void processSysExData(vector<unsigned char> data);
				
				int _servoValue[ARD_TOTAL_DIGITAL_PINS];
                // the last set servo values
				
				float _temp;
				// the last received temperature
				
				float _humidity;
				// the last received humidity
				
};

#endif