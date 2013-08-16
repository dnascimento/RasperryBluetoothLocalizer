Rasperry Bluetooth Localizer
==========================

Bluetooth Devices Location and Domotic Action:
Detects bluetooth devices, calcule the distance and execute domotic tasks on smart room.
ex: Mobile1 is 4m from sensor1 then open the door.
All sensors are connected via wireless.

Centralized Architecture:
========
Sensor (C and BlueZ):
- spotter.c: Main program: Single threaded: Inq the mobiles bluetooth to get RSS value. POST this value to central server.

WebServer (PHP and JS):
- new_data.php: receive data from sensor, check the distance (the distance calculation formula is here) and execute all pendent actions.
- index.php: display the mobile location and actions management




Executor (Java):
-> WebServer invokes a Java library to execute commands on domotics. This library is available at: 
https://github.com/dnascimento/matlab_java_iSimplex


This is just a introduction...

How to Add a new sensor
=======
-  Make a hard copy
-  diskutil list
-  diskutil unmountDisk /dev/disk******
-  SD ---> PC
-  dd if=/dev/disk********* of=SD_MIT_Working_v3.img
-  PC ----> SD 
-  dd of=/dev/disk********* if=SD_MIT_Working_v3.img

Deploy
-  Turn it on. It will join the network MIT_AP. 
-  Login server (192.168.20.1)
-  nmap 192.168.20.0/24
-  check which is the new address and ssh to new device
-  change the run.c at last line to the new sensor id (it's the last number)
-  make
-  sudo reboot

and ready to go

Add new entry in database at server (sensors_list table).

any question: dario.nascimento@gmail.com