-----------
Description
-----------
Program to read and display kml or xml data (from Geonaute KeyMaze 700 or Garmin ForeRunner 305), usely used for running data.
Data are displayed on Google map.
Using ZedGraph (http://sourceforge.net/projects/zedgraph/) for the charts and a c dll (RS200 project) to read data from Geonaute KeyMaze 700 (serial data).

-------------
Configuration
-------------
- change the google map key api (https://developers.google.com/maps/?hl=fr) inside the map.html file (line 8)
- if using a Keymaze 700, change the mode parameter inside the config.ini to 1 (mode=1). It will display a new tab to get the data from the watch.
- change the DATA_DIRECTORY parameter with the path where your xml or kml are stored.
- change the serial port and serial speed if necessary.
- copy the map.ini file and config.ini into the executable folder
