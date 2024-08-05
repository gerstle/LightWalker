0. sloeber
1. new arduino project
2. use this directory as workspace
3. make sure teensy hardware is setup under private hardware
4. Use the arduino | add library to project stuff for anything not recognized... fastled, adafruit ble, etc
5. I also had to add `-lstdc++` to this line in my teensy `boards.txt` file: `teensy36.build.flags.libs=-larm_cortexM4lf_math -lm -lstdc++`

# 2024

How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-teensy/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Upload firmware for the specific environment
$ pio run -e teensy36 --target upload

# Clean build files
$ pio run --target clean
```
