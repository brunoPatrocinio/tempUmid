# tempUmid
A prototype to collect data from de Temperature and Humidity sensor and send the data to a Node JS server by an API.

This program is set to be used with ESP 32 . It' was loaded to an Esp 32 Wroom.
See the design schematics to know how to use and connect the PINS.

**********************************************************************************************************************************************************************************************
**Components List**
* Esp32 Wroom;
* Jumper Wires;
* SD Card Reader
* DHT 11 Temperature and Humidity Sensor;
* Display LCD 16x2 - With I2C interface.
**********************************************************************************************************************************************************************************************
**Important Note**
To make the Esp32 send the data to the server, it uses the REST architecture.
So it mount the URL and send data.
It needs to connect to the WiFi network as well.

**Note 2**
Format a Micro Sd card to FAT32.
After that, create a file name called "data.txt" and put in the roof directory of the sd card.
inside the file type the information the program needs to load from the Sd card. Starting at the first line and finishing at line 5.(SSID, password, ip, port and route).
like this:
yourNetwork
yourPassword
ipNumber  (Example: 192.168.0.17)
portNumber
RouteName (Example: /data)

The program at the esp32 loads the data from the file, fill the necessary variables, connect to the WiFi and mount the URL to send data to the Node server.
***********************************************************************************************************************************************************************************************
After set the pins correctly, it's time to set the node.js server into the PC.
Make sure to have all the dependencies to run.
**See the Package.json to confirm.**
***********************************************************************************************************************************************************************************************
Download the server files, open the VScode.
**In the index.js file, make sure the port is the same that you load from the Sd Card file.**
************************************************************************************************************************************************************************************************
**Note 3**
**The node.js part is not well developed, but it works. Feel yourself free to make improvements and make better than me. :)**
*************************************************************************************************************************************************************************************************
