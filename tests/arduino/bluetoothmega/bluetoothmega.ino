// Main application entry point 
void setup()
{
    // Begin communicating with the Serial1 interface
    Serial.begin(9600);
    Serial1.begin(57600);
 
    Serial.println("Serial start!");
}
 
// Main application loop
const int msgLength = 64;
char msg[msgLength];
int msgIndex = 0;
void loop()
{

    if (Serial.available())
        Serial1.print((char)Serial.read());
}

void serialEvent1()
{
    if (Serial1.available())
    {
        msg[msgIndex++] = (char)Serial1.read();

        if (msg[msgIndex - 1] == '\r')
        {
            Serial.print("finished: "); Serial.println(msg);
            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}
