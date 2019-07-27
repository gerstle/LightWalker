0. sloeber
1. new arduino project
2. use this directory as workspace
3. make sure teensy hardware is setup under private hardware
4. Use the arduino | add library to project stuff for anything not recognized... fastled, adafruit ble, etc
5. I also had to add `-lstdc++` to this line in my teensy `boards.txt` file: `teensy36.build.flags.libs=-larm_cortexM4lf_math -lm -lstdc++`

