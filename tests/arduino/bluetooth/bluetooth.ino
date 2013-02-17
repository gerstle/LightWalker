/*******************************************\
 BlueSMiRF I/O Example
 
 Sends and recieves single-bytes between the
 client-USB connection and the tethered
 Bluetooth device.
 
 Jeremy Bridon
 jgbridon@gmail.com
 

 // <gerstle> modified heavily 17feb2013
\********************************************/
 
// Included for serial communication
#include <SoftwareSerial.h>

// Define pins you're using for serial communication
// for the BlueSMiRF connection
#define TXPIN 11
#define RXPIN 10
 
// Create an instance of the software serial object
SoftwareSerial bluetooth(RXPIN, TXPIN);
 
// Main application entry point 
void setup()
{
    // Define the appropriate input/output pins
    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);

    // Begin communicating with the bluetooth interface
    Serial.begin(9600);
    bluetooth.begin(57600);
 
    // Say we are starting the serial com
    Serial.println("Serial start!");
    //bluetooth.print("Serial start!\r");
}
 
// Main application loop
const int msgLength = 64;
char msg[msgLength];
int i = 0;
void loop()
{
    if (bluetooth.available())
    {
        //Serial.println((char)bluetooth.read());
        i = 0;
        msg[i++] = (char)bluetooth.read();
        while ((msg[i - 1] != '\r') && (msg[i - 1] > 0x0) && (msg[i - 1] <= 0x7F))
        {
            msg[i++] = (char)bluetooth.read();
            //Serial.println(msg);
        }

        Serial.print("command: ");
        Serial.println(msg);

        for (int j = 0; ((j < i) && (j < msgLength)); j++)
            msg[j] = '\0';
    }
    if (Serial.available())
        bluetooth.print((char)Serial.read());
}
