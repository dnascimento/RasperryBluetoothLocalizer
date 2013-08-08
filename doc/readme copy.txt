
###########################################################################
##### Documentação da rede Mesh+Sensores Bluetooth para MIT Portugal  #####
#####																  #####
##### Author: Dário Nascimento										  #####
##### Date: 18 Feb 2013 										      #####
###########################################################################

################################# BLUETOOTH ###################################################################
O módulo de bluetooth é baseado na stack BlueZ...
Para utilizar, instale:
sudo apt-get install dbus libdbus-1-3 libdbus-1-dev libdbus-glib-1-dev glibc-2.13-1 libssl-dev curl  libcurl3-dev   libyajl-dev libyajl2  bluez gir1.2-glib-2.0 libcap-ng0 libgirepository-1.0-1 python-dbus python-dbus-dev python-gi bluetooth libusb-dev libudev-dev libical-dev libreadline6-dev   libbluetooth-dev









######################################## SQL Server + Processor #######################################################
Ainda que a rede seja Mesh, tem um único ponto de falha nesta versão. Por simplificação, há um único ponto cordenador de dados e de processamento. Todos recolhem dados mas apenas 1 unidade trata-os.
Essa unidade é novamente o RALINK, com IP 12.0.0.1. 

Todos os sensores recolhem dados e enviam para essa unidade por POST HTML. (usa o porto 80 por causa de firewalls)


Para instalar o servidor:

sudo apt-get install apache2  php5-mysql php5 mysql-server mysql-client mysql-common openjdk-6-jdk




########## popular a base de dados
mysql -uroot -p
create database mit;

CREATE USER 'mit'@'localhost' IDENTIFIED BY 'mit';

GRANT ALL PRIVILEGES ON *.* TO 'mit'@'%'  WITH GRANT OPTION;

mysql -uroot -p mit < db.sql



############################################# Arquitectura da solução #################################################

O sistema funciona com o modelo de repositório. Os dados são recebidos pela interface PHP (servidor aplicacional)...

São guardados na tabela sensorData. Guarda apenas as entradas mais recentes de cada sensor.


O processador, calcula a distância a que está de cada sensor
Esta versão não utiliza java, só PHP+JS...








