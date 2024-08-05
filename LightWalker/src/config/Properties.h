/*
 * Properties.h
 *
 *  Created on: Nov 9, 2018
 *      Author: cgerstle
 */

#ifndef CONFIG_PROPERTIES_H_
#define CONFIG_PROPERTIES_H_

#include <map>
#include "Enums.h"
#include "FastLED.h"

namespace config {

class Properties {
public:
	Properties();
	virtual ~Properties();
    void set(Preferences p, char *value);
    char* getString(Preferences p, char *defaultValue);
    int getInt(Preferences p, int defaultValue);
    float getFloat(Preferences p, float defaultValue);
    bool getBool(Preferences p, bool defaultValue);
    void getCHSV(Preferences p, CHSV *dest);

private:
    std::map<Preferences, char*> properties;
    void parseColor(CHSV *dest, char *colorString);
};

} /* namespace config */

#endif /* CONFIG_PROPERTIES_H_ */
