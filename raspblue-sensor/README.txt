***************************************************************
*															  *
*	Sensor de Bluetooth com modulo de comunicação RPC		  *
*	@author: Dário Nascimento - INESC-ID 					  *
*	@contact: dfrnascimento@gmail.com						  *
*	@version: v1											  *
*	@date: 23-AUG-2012										  *
*															  *
***************************************************************

Este é o módulo de leitura de bluetooth através do sensor.
Este módulo é constituido por 2 submodulos:
	- Client RPC
	- Sensor
Prepara-te para muita hora de programação "C" avançada...

O código para o sensor está em spotter. Para compilar:
 	make
 	make plugins  - (compila os ficheiros de plugins bluetooth)
 
 O programa começa no spotter.c. É criada uma thread com um 
 intervalo de 10 segundos. Essa thread é iniciada no inq.c.
 O inq.c é onde são enviados os comandos para o "hci" (google it) 
 de modo a colocar o bluetooth no modo de inquisição para que 
 os dispositivos próximos lhe respondam.
 O reporter.c, processa os eventos (respostas) dos dispositivos
 que foram questionados e guarda os valores.
 
 Recomendo vivamente que veja a sequência de comandos enviados ao
 hci (controlador/bus do bluetooth) através da instrução: hci_send_cmd
 Para mais detalhes sobre os comandos, deve ler a especificação do 
 bluetooth v4 que está em anexo.
 
 Este sistema foi testado com a versão 4 do blueZ que é uma verdadeira
 carga de trabalhos para instalar porque tem de ser cross-compiled (compilada
 no PC linux e não na placa ARM) e tem imensas dependências... Por isso,
 é disponibilizada uma versão já compilada no docs.
 
 
 