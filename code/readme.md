# Components

`Communicator.ino`

Asks the AboveWater Arduino to gather data and send it back. Receives the data over serial, and uses it to construct a JSON string. This string is then sent to the server via a WiFi client.

`AboveWater.ino`

Asks the BelowWater Arduino for a temperature reading on request from the Communicator ESP8266\. With this data point, and configuration variables read from the SD card it generates a line of CSV and sends it to both a file on the aforementioned SD and the Huzzah ESP8266.

`BelowWater.ino`

Takes a temperature reading on request from the AboveWater Arduino, and sends it back via serial.

`_config.txt`

Stores headers and metadata used for formatting and reporting data entries.

`SetDateTime.ino`

In order to reflect the proper date and time we can use this file a single time to set the DeadOn RTC with the current values.
