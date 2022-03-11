# How to prepare the development environment

## Set up the Arduino IDE

Setup the Arduino IDE on your system. Make sure you install at least version 1.8.10, since older versions may not support all required features.

When installed, just add Arduino executable to the PATH so the makefiles can make use of it

### Linux

On Linux, your distribution's package manager probably includes the Arduino IDE, but it may be an out-of-date version. If your distribution installs a version of Arduino before 1.8.10 or so, you'll need to install the Arduino IDE following the instructions [on the wiki](https://github.com/keyboardio/Kaleidoscope/wiki/Arduino-Setup-Linux).

## Download hardware platform, including library source code

To be able to compile the FW with Arduino you need the hardware platform installed in your IDE, to do this go to boards manager

![Untitled](https://kaleidoscope.readthedocs.io/en/latest/_images/open-boards-manager.png)

And add the SAMD board support. 

Then clone this repository inside the Arduino folder in the Documents folder of your user (windows & mac)

```bash
git clone https://github.com/Dygmalab/KaleidoscopeTest.git hardware/dygma/samd
```

## Download the Raise Firmware

Download the firmware repo in a folder of your choosing

```
git clone <https://github.com/Dygmalab/Raise-Firmware.git>
```

# Build and flash the firmware

Before you begin, make sure your Raise is connected to your computer.

### The MakeFile

The updated makefile now supports Windows, Mac & Linux

### Previous steps for Windows

First install dependencies with [chocolately](https://chocolatey.org/install) using:

```
choco install make

```

### Configure the MakeFile

```
make config
```

Will show the current configuration of the makefile.

Modify it to fit your current environment.

### Run the make file

```
cd Raise-Firmware
make flash
```

# Additional information

[Focus API KeyMap](https://github.com/Dygmalab/Raise-Firmware/blob/master/FOCUS_API.MD)

[MakeFile documentation](https://github.com/Dygmalab/Raise-Firmware/blob/master/MAKEFILE.MD)
