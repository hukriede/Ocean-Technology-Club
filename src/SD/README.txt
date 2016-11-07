Here is a file containing notes that are important to editing, compiling, running, and maintaining code for the Mote OT Club. 
This guide assumes that you know basic programming skills, basic knowledge of the Arduino programming language, and how to work the Arduino IDE.
If you make a change, note it in the changelog.txt file on your designated microSD card. 

- If an error occurs, always refer back to the original file that you were intending on changing. Always save a copy of this file and rename it to make changes to. 

- To reprogram the arduino, simply plug in the ftdi cable, make sure nothing else is connected to its RX and TX pins, select "Arduino Pro or Pro Mini" from the board drop down, and your correct COM port. 

- To reprogram the ESP8266, you must put the board into bootloader mode and then follow the same steps listed above. Make sure when you select your board, you select the option for the HUZZAH board. 
	- To put your HUZZAH into bootloader mode: 
		- Make sure it is powered up
		- Press and hold the GPIO button 
		- While holding this button, press and release the RESET button
		- Once you have released the reset button, you can release the GPIO button
	- To confirm your board is in bootloader mode, check to see if the red on-board GPIO LED is lit dimly
	- If you have any questions or concerns, look online at the ESP8266 forums and you'll find plenty of tips and tricks

- The RTC Time is in GMT time. Currently, if you update the code it will NOT reset the clock. This is because the segment to program the clock reads:
    if (!RTC.isrunning()) {
      // following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
  The first line makes it so the time will only be reset if the RTC turns off or malfunctions 

	- If you need to reset the time for any reason: 
		- put your computer in GMT (UTC) time to make sure the clock functions correctly
		- remove the exclamation point from the inside of the if statement, so it reads if(RTC.isrunning())
		- upload the code to the arduino with the RTC attached
		- change the line back so it contains the exclamation point again, like if(!RTC.isrunning())
		- upload the code again

- The _config file is formatted so that each item is in order, so it is received correctly by the ESP8266
	- if you add any information to the _config.txt, add it to the end of the first line
	- do not wrap the content in spaces
	- begin each string with a comma and end it with a comma
		- example: if you wanted to write hello, write ",hello,", not ", hello , "
	- DO NOT remove any of the current information from the _config file

- If you add information to _config, follow the above rules, and then add a description on the 3rd line of the file, where the other descriptors are 
	- this allows other people to identify what each variable means

- Once you add another field to _config, you must make that change to the ClientConnect.ino file, and re-upload that program to the ESP8266
	- In the getJSON() method, the data[] array length must be incremented by the number of fields added to _config
	- In the getJSON() method, another field must be added to the "systems" nested JSON. 
		- to do this, add: 
		  systems["DESCRIPTION OF FIELD"] = data[POSITION IN ARRAY];

		- if your field is an integer:
			- change data[POSITION IN ARRAY] to data[POSITION IN ARRAY].toInt()
		- if your field is a float/double:
			- change data[POSITION IN ARRAY] to data[POSITION IN ARRAY].toFloat()

