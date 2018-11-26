/*
 * Properties.cpp
 *
 *  Created on: Nov 9, 2018
 *      Author: cgerstle
 */

#include "Properties.h"

namespace config {

Properties::Properties() {
}

Properties::~Properties() {
}

void Properties::set(Preferences p, char *value) {
	//Serial.printf("setting preference %d = '%s'\n", p, value);

	// see if property already set, if so re-write the string
	std::map<Preferences, char*>::iterator it = properties.find(p);
	if (it != properties.end()) {
		if (it-> second != NULL)
			strncpy(it->second, value, 32);
	}
	else {
		char *newVal = new char[32];
		strncpy(newVal, value, 32);
		properties[p] = newVal;
	}
}

char* Properties::getString(Preferences p, char *defaultValue) {
	std::map<Preferences, char*>::iterator it = properties.find(p);
	if (it != properties.end()) {
		return it->second;
	}

	return defaultValue;
}

int Properties::getInt(Preferences p, int defaultValue) {
	char *strVal = getString(p, NULL);

	if (strVal != NULL) {
		return atoi(strVal);
	}

	return defaultValue;
}

float Properties::getFloat(Preferences p, float defaultValue) {
	char *strVal = getString(p, NULL);

	if (strVal != NULL) {
		return atof(strVal);
	}

	return defaultValue;
}

bool Properties::getBool(Preferences p, bool defaultValue) {
	char *strVal = getString(p, NULL);

	if (strVal != NULL) {
		if (strncmp(strVal, "1", 1) == 0)
			return true;
		else
			return false;
	}

	return defaultValue;
}

void Properties::getCHSV(Preferences p, CHSV *dest) {
	char *strVal = getString(p, NULL);

	if (strVal != NULL) {
		parseColor(dest, strVal);
	}
}

char mutableColorString[32];
void Properties::parseColor(CHSV *dest, char *colorString) {
	strcpy(mutableColorString, colorString);
	char *hue = mutableColorString;
	char *pComma = strchr(mutableColorString, ',');
	*pComma = '\0';

	dest->hue = atoi(hue);

	hue = pComma + 1;
	pComma = strchr(hue, ',');
	*pComma = '\0';

	dest->sat = atoi(hue);
	dest->val = atoi(pComma + 1);
}

} /* namespace config */
