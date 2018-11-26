/*
 * Bluetooth.h
 *
 *  Created on: Mar 26, 2016
 *      Author: cgerstle
 */

#ifndef UTIL_BLUETOOTH_H_
#define UTIL_BLUETOOTH_H_

#include "../config/Properties.h"

namespace util {

class Bluetooth {
public:
	Bluetooth(config::Properties *properties);
	virtual ~Bluetooth();

	void init();
	void listen();
private:
	bool processMessage(int key, char *value);
	void parseColor(char *colorString, CHSV *color);

	static const int msgLength = 128;
	char msg[msgLength];
	int msgIndex = 0;
	char *pValue = '\0';

	config::Properties *properties;
};

} /* namespace util */

#endif /* UTIL_BLUETOOTH_H_ */
