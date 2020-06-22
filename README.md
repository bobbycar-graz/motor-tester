# Motor Tester Program

You can use this program to test motors connected to an ESC board that speaks [0xFEEDC0DE64](https://github.com/0xFEEDC0DE64)'s [Bobby Car Protocol](https://github.com/bobbycar-graz/bobbycar-protocol).

After starting the program, the left motor will spin up to maximum speed (1000) in steps of 100, using FOC Voltage mode with a soft current limit of 2 A.

## Compiling

```
mkdir build && cd build/
cmake ..
make
```

## Running

Example:
```
./motor-tester /dev/ttyUSB0
```

The default baud rate is 38400. If you require a different baud rate, you can specify the baud rate as the second parameter, ex.:
```
./motor-tester /dev/ttyUSB0 115200
```

The program will continuously output the current state of the motors received from the device in the following format:
```
left : S 21.3980 E00 H100
right: S  0.0000 E00 H110
```

Here, `S 21.3980` represents the speed in km/h at a wheel diameter of 16.5 cm. `E00` shows the current error number. `H100` shows the current state of the hall sensors.
