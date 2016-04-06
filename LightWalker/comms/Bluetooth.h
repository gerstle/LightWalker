/*
 * Bluetooth.h
 *
 *  Created on: Mar 26, 2016
 *      Author: cgerstle
 */

#ifndef COMMS_BLUETOOTH_H_
#define COMMS_BLUETOOTH_H_

#include "../lw/LW.h"

namespace comms {

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
	//static const unsigned char* one_str;
	char msg[msgLength];
	int msgIndex = 0;
	char *pKey = NULL;
	char *pValue = '\0';
	LW *lw;

};

} /* namespace comms */

#endif /* COMMS_BLUETOOTH_H_ */
