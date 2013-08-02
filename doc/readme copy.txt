
###########################################################################
##### Documentação da rede Mesh+Sensores Bluetooth para MIT Portugal  #####
#####																  #####
##### Author: Dário Nascimento										  #####
##### Date: 18 Feb 2013 										      #####
###########################################################################


LOGIN:
user: pi
password: mit


########################## Wireless ######################################

Os dongle USB de Wireless são RTL8188CUS da Realtek. Os drivers que existem de origem com o raspbian são incompletos por isso temos de compilar os drivers.


Usar mesh em raspberry:
Instalar os drivers da wireless:
http://www.raspberrypi.org/phpBB3/viewtopic.php?f=66&t=6737&start=25

apt-get install git wget unzip wireless-tools wpasupplicant

rpi-update
reboot
#find and remove all previous instances of 8192cu.ko and then depmod
find / -type f -name "8192cu.ko" -exec rm -f {} \;
depmod -a





1º Sacar o source do kernel para compilar modulos:
mkdir /root/src
cd /root/src
git clone –depth 1 https://github.com/raspberrypi/linux.git
ln -s linux /lib/modules/3.1.9+/build
cd /lib/modules/3.1.9+/build
ln -s arch/arm arch/armv6l
make mrproper
gzip -dc /proc/config.gz > .config
make prepare
make modules_prepare
make && make modules

#get wi-fi driver source from
http://218.210.127.131/downloads/downloadsView.aspx?Langid=1&PNid=48&PFid=48&Level=5&Conn=4&DownTypeID=3&GetDown=false&Downloads=true#2772


cp /path/to/RTL819xCU_USB_linux_v3.4.2_3727.20120404.zip /root/src/
cd /root/src
unzip RTL819xCU_USB_linux_v3.4.2_3727.20120404.zip
cd RTL8188C_8192C_8192D_USB_linux_v3.4.2_3727.20120404
sh install.sh



##################################### Depois da configuração ########################################

modprobe batman-adv

Se for a pen da RALINK (que diz daemon), então: (se for realtek não precisa dos próximos 2 passos)
iw dev wlan0 del
iw phy phy0 interface add me0 type ibss



ifconfig me0 mtu 1528
iwconfig me0 enc off
iwconfig me0 mode ad-hoc essid MeshMIT ap 02:11:87:95:2F:00  channel 1
batctl if add me0

ifconfig me0 10.0.0.3 netmask 255.255.255.0 up
ifconfig bat0 192.168.1.13 netmask 255.255.255.0 up
route add default gw 192.168.1.1 bat0


######################################### Módulo com DHCP #####################################################
O nó com o RALINK tem um servidor DHCP para toda a rede...

O nó atribui IP's na subnet 12.0.0.0/24 entre 1 e 100

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


Base de dados:
user: root
password: mit




########## popular a base de dados
mysql -uroot -p
create database mit;

CREATE USER 'mit'@'localhost' IDENTIFIED BY 'mit';

GRANT ALL PRIVILEGES ON *.* TO 'mit'@'%'  WITH GRANT OPTION;

mysql -umit -p mit < db.dump



############################################# Arquitectura da solução #################################################

O sistema funciona com o modelo de repositório. Os dados são recebidos pela interface PHP (servidor aplicacional)...

São guardados na tabela sensorData. Guarda apenas as entradas mais recentes de cada sensor.


O processador, calcula a distância a que está de cada sensor
Esta versão não utiliza java, só PHP+JS...








