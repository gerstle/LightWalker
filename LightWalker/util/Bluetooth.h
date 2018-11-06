/*
 * Bluetooth.h
 *
 *  Created on: Mar 26, 2016
 *      Author: cgerstle
 */

#ifndef UTIL_BLUETOOTH_H_
#define UTIL_BLUETOOTH_H_

#include "../lw/LW.h"


namespace util {

class Bluetooth {
public:
	Bluetooth();
	virtual ~Bluetooth();

	void init(LW *lw);
	void listen();
private:
	bool executeCommand(int key, char *value, int valueLen);
	void parseColor(char *colorString, CHSV *color);

	static const int msgLength = 128;
	char msg[msgLength];
	int msgIndex = 0;
	char *pKey = NULL;
	char *pValue = '\0';
	LW *lw;

};

} /* namespace util */

#endif /* UTIL_BLUETOOTH_H_ */
