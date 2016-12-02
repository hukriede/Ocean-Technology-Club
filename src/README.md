# Components

`AboveWater.ino`

Program that records sensor and system data to a CSV. 

This data is then sent to a the ESP8266 for handling,
and to an SD card for datalogging.

`BelowWater.ino`

This code allows for the under-water temperature to be retrieved and sent to the Serial port.

`ClientConnect.ino`

Receives data from the Serial port and inserts the data into a JSON String. This string is then sent to a WiFi client.

`data/_config.txt`

Header and metadata used for formatting and reporting data entries.
