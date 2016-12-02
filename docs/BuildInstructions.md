# Sensor Software

## Underwater Arduino

```
- Arduino IDE
    - The most recent version of the belowWater software
    - FTDI to USB cable
    - Arduino Pro Mini
    - A computer with a USB port
```

Plug the FTDI to USB cable in to your Arduino, so that when the Arduino is right-side-up, the cable has the green wire on the left and the black wire on the right.

The Arduino has BLK and GRN written next to their respective pins, so it's easy to get this arrangement right.

Open the Arduino IDE and pull up the belowWater file.

Under the "Tools" menu:

Select the "Board:" option, and change that to "Arduino Pro or Pro Mini".

Make sure that the "Processor:" says "ATmega328 (5V, 16MHz)".

Change "Port:" to whichever COM port your cable is plugged into on the computer.

Hit the upload button and you're done!

## ESP8266 Wifi Chip

```
- Arduino IDE
    - The most recent version of the ClientConnect software
    - FTDI to USB cable
    - HUZZAH ESP8266 Breakout
    - A computer with a USB port
```

Plug the FTDI to USB cable into the HUZZAH, so that when the HUZZAH is right-side-up, the cable has the green wire on the left and the black wire on the right.

Open the ClientConnect file in the Arduino IDE.

Under the "Tools" menu:

Select the "Board:" option, and change that to "Adafruit HUZZAH ESP8266"

Make sure the "CPU Frequency:" is set to 80 MHz, the "Flash size:" is set to 4M (3M SPIFFS), and "Upload speed:" is set to 115200.

Finally, make sure the "Port:" option is set to your correct COM port.

You must put the board into Bootloader mode by pressing and holding the GPIO button, pressing and releasing the reset button, then releasing the GPIO button.

The red LED above the GPIO button should be dimly lit if you did it correctly.

Press the upload button, and wait for the upload process to complete.

## Above Water Arduino

```
- Arduino IDE
  - The most recent version of the aboveWater software
  - FTDI to USB cable
  - Arduino Pro Mini
  - A computer with a USB port
```

Plug the FTDI to USB cable in to your Arduino, so that when the Arduino is right-side-up, the cable has the green wire on the left and the black wire on the right.

The Arduino has BLK and GRN written next to their respective pins, so it's easy to get this arrangement right.

Open the belowWater file in the IDE.

Under the "Tools" menu:

Select the "Board:" option, and change that to "Arduino Pro or Pro Mini".

Make sure that the "Processor:" says "ATmega328 (5V, 16MHz)".

Change "Port:" to whichever COM port your cable is plugged into on the computer.

If you're setting up the project for the first time, you'll need to program the real-time clock. To do so, refer to this segment from the README file on your microSD card (next page).

Press the upload button and wait for the upload process to complete.

## DeadOn Real Time Clock

The RTC Time is in GMT time. Currently, if you update the code it will NOT reset the clock. This is because the segment to program the clock reads:

The first line makes it so the time will only be reset if the RTC turns off or malfunctions

If you need to reset the time for any reason:

Put your computer in GMT (UTC) time to make sure the clock functions correctly

Remove the exclamation point from the inside of the if statement, so it reads

Immediately replace the exclamation point, so it looks like it originally did, and then re-upload that code. This makes the clock not reset every time the device restarts.

# Hardware

## Wiring
