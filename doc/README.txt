***************************************************************
*															  *
*	Sensor de Bluetooth com modulo de comunicação RPC		  *
*	@author: Dário Nascimento - INESC-ID 					  *
*	@contact: dfrnascimento@gmail.com						  *
*	@version: v1											  *
*	@date: 23-AUG-2012										  *
*															  *
***************************************************************

Este programa é constituido por 6 módulos:
	1 - Programa de leitura no sensor em C (ver README do mesmo)
	2 - RPC Binder (servidor RPC)
	3 - Cliente RPC
	4 - Controlador Central
	5 - Actuador de domótica
	6 - Google Webtool Kit (GWT)


Como correr o programa?
Antes de mais, tem de utilizar o módulo RPCBinder, para tal, tem de ter autorizaçao de administrador:
"sudo ant run"

Já no módulo MIT, fazer "ant gwt-compile" para gerar os ficheiros do GWT. 
Copiar o .war para o 





O ponto de arranque do programa, quando com GWT é: ist.mit.roomACL.server.MITControlerServiceImpl, no método init.
Neste método, é verificado se existe um servidor RPC Binder e instanciado o ControlServer.

O ControlServer é o módulo central, é onde os dados são guardados, solicitados e processados.
O método newData() é invocado pelo cliente RPC (server.rpcModule.RPCServer) sempre que um sensor envia uma nova amostra.


A cada sensor, é associado uma tarefa que consiste numa acção a realizar caso um determinado telemóvel esteja a uma certa distância desse 
mesmo sensor. Por isso, sempre que é recebida a amostra, é verificado quais as tarefas desse sensor que devem ser realizadas.

O controlo da domótica é feito nos packages "domotics" e "controlFunctions". O package domotics contém uma representação virtual
de todos os dispositivos de domótica da sala.  Para actuar sobre esses dispotivos, é enviado um pacote HTTP através das "controlFunctions".

