/*
 * Bluetooth.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: cgerstle
 */

#include "Bluetooth.h"

#include "../config/Enums.h"
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_UART.h>

namespace util {

Adafruit_BluefruitLE_UART ble(Serial4, 0);

Bluetooth::Bluetooth(config::Properties *properties) {
	this->properties = properties;
}

void Bluetooth::init() {
	Serial.print("setting up bluetooth...");

	/* Initialise the module */
	ble.echo(false);
	if (!ble.begin(false))
	{
	    Serial.println("Couldn't find BLE, make sure it's in CoMmanD mode & check wiring?");
	}
	Serial.println("done.");
	Serial.print("waiting for bluetooth connection...");

	while (!ble.isConnected()) {
	    delay(500);
	}
	Serial.println("connected.");

	ble.sendCommandCheckOK("AT+HWModeLED=BLUEART");
	ble.setMode(BLUEFRUIT_MODE_DATA);

	Serial.println("bluetooth setup complete.");
}

void Bluetooth::listen() {
    while (ble.available())
    {
    	int temp = ble.read();
        msg[msgIndex++] = temp;

        if (msg[msgIndex - 1] == '\r')
        {
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                char *pKey = msg;
                *pValue = '\0';
                pValue++;

                processMessage(atoi(pKey), pValue);
				ble.print("K");
            }

            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}

bool Bluetooth::processMessage(int key, char *value)
{
	Serial.printf("setting %d=%s\r\n", key, value);

	// in an ideal world, this would fail if the enum doesn't exist
	Preferences p = static_cast<Preferences>(key);
	properties->set(p, value);

	return true;
}

void Bluetooth::parseColor(char *colorString, CHSV *color)
{
    char *value = colorString;
    char *pComma = strchr(colorString, ',');
    *pComma = '\0';

    (*color).hue = byte(atoi(value));

    value = pComma + 1;
    pComma = strchr(value, ',');
    *pComma = '\0';

    (*color).sat = byte(atoi(value));
    (*color).val = byte(atoi(pComma + 1));
}

Bluetooth::~Bluetooth() {
}

} /* namespace util */
