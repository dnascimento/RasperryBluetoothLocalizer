RasperryBluetoothLocalizer
==========================

Bluetooth Devices Location and Domotic Action:
Detects bluetooth devices, calcule the distance and execute domotic tasks on smart room.
ex: Mobile1 is 4m from sensor1 then open the door.


Centralized Architecture:
Sensor (C and BlueZ):
-> spotter.c -> Main program: Single threaded: Inq the mobiles bluetooth to get RSS value. POST this value to central server.

WebServer (PHP and JS):
-> new_data.php -> receive data from sensor, check the distance (the distance calculation formula is here) and execute all pendent actions.
-> index.php -> display the mobile location and actions management


Executor (Java):
-> WebServer invokes a Java library to execute commands on domotics. This library is available at: 
https://github.com/dnascimento/matlab_java_iSimplex


This is just a introduction...

any question: dario.nascimento@gmail.com