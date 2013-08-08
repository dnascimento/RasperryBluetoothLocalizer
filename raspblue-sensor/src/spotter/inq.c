/*
 * Bluetooth inquisition - Copyright Michael John Wensley 2005 2006
 *
 *
 * Featuring new aggressive, vicious scanning.
 *
 * https://www.bluetooth.org/spec/
 * BT Spec: http://www.bluetooth.org/foundry/adopters/document/Core_v2.0_EDR/en/1/Core_v2.0_EDR.zip
 * http://standards.ieee.org/getieee802/802.15.html
 *
 *
 * Unfortunatly we can't get closer to bluetooth than the HCI layer... so...
 *
 * do inquiry -notice that all multi-octet paramaters are in reverse octet order.
 * hcitool cmd 01 0001 33 8b 9e 30 00
 *                     ^  ^  ^  ^  ^---- maximum responses (0x00 = no limit) 0x01-0xFF means 1-255 maximum responses
 *                     |  |  |  +------- timeout in 1.28 second units (bluetooth hops) 0x01-0x30
 *                     +--+--+---------- LAP as 338b9e "GIAC" meaning I want all devices that can, to respond.
 * and 338b9e is mentioned here too: https://www.bluetooth.org/foundry/assignnumb/document/baseband
 * and subsequently moved to https://www.bluetooth.org/assigned-numbers/
 *
 * This tells the bluetooth dongle to do continuous inquiries until stopped with 0004 below
 *
 * perfect for the inquisition version 2 `_^ (see section 7.1.3 of the BT spec)
 * hcitool cmd 01 0003 0A 00 09 00 33 8b 9e 08 00
 *                     ^  ^  ^  ^  ^  ^  ^  ^  ^---- maximum responses like as above.
 *                     |  |  |  |  |  |  |  +------- length time in units to follow the inquiry channel hop sequence.
 *                     |  |  |  |  +--+--+---------- LAP (the magic number is "GIAC") meaning I want all devices.
 *                     |  |  +--+------------------- min_period between scans (must be bigger than length)
 *                     +--+------------------------- max_period between scans (must be bigger than min_period)
 * stop the inquisition... used on exit or to query for other things.
 * hcitool cmd 01 0004
 *
 * hcidump -X
 * xx [ mm mm mm mm mm mm 01 00 00 dd dd dd cc cc ]
 * ^^   ^^^^^^^^^^^^^^^^^ ^^ ^^ ^^ ^^^^^^^^ ^^^^^
 *  |                   |  |  |  |        |     +-- clock offset
 *  |                   |  |  |  |        +-------- device class
 *  |                   |  |  |  +----------------- Page Scan Mode
 *  |                   |  |  +-------------------- Page Scan Period Mode
 *  |                   |  +----------------------- Page Scan Repetition Mode
 *  |                   +-------------------------- IEEE MAC Address
 *  +---------------------------------------------- Number of responses
 *
 * return RSSI (signal stength) with inqury results would be nice too...
 * hcitool cmd 03 0045 01				//Inquiry Result format with RSSI  (pag. 632 of specification)
 */


/** Biblioteca para inquição de dados ao bluetooth*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <libintl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <sys/socket.h>
#include <netdb.h>
#include <getopt.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/md5.h>

#include "inq.h"
#include "location.h"

#define maxi(a,b) ((a)>(b)?(a):(b))
#define mini(a,b) ((a)<(b)?(a):(b))

#define FINGER_FILE "bt_sensor.cfg"		//local onde guarda os dados


void (* sensor_result)(SensorData *);
static pthread_t sense_thread;

extern char **environ;
extern char *optarg;

static int p_inquiry = 0;
static int run = 0;
static int done = 0;
int debug = 0;

static unsigned char buf[HCI_MAX_FRAME_SIZE];
static unsigned char controlbuf[100];
struct timeval inquiry_complete;

short do_finger = 0;
short num_fingers;		//contador de impressoes
short finger_min[20];
short finger_max[20];

static int dd;	//device



//Print sensor summary data
static int print_data(SensorData * data){
	int i, aux = 0;	
	printf("Num nodes: %d\n",data->RSS.node_number);
	for (i=0; i<data->RSS.node_number; i++){
		if (i>0) aux=1;
		printf("Node: %s  --  Distance: %f\n", data->RSS.nodes + i*(MD5_DIGEST_LENGTH*2+1) + aux, data->RSS.rss[i]);
	}
	return 1;
}

/**
 * This decodes output from the dongle. Generally BlueZ would carbon-copy all the dongle's output
 * to all the programs that have a raw socket to the dongle open, thusly ensuring no-one
 * has to miss anything interesting :-)
 *
 * However, this means we must detect precicely the things we are interested in, as the
 * user might have other programs causing HCI events as well as our periodic inquiry,
 * such as open connections, sdp, etc. This also means you can usually leave inq running
 * when interrogating a discovered device with hcitool.
 *
 */
static int decode(int len, struct timeval tstamp) {
	char ieeeaddr[18];
	struct timeval resultime;

	int i;
	if (len < 1) {
		perror("Packet too small");
		return EXIT_SUCCESS;
	}
	switch (buf[0]) {
		case HCI_EVENT_PKT:
	        if (len < 3) {
        	        fprintf(stderr, gettext("Event so short I cannot read the OGF/OCF numbers\n"));
        	        return EXIT_FAILURE;
	        }
	        if (len != (buf[2] + 3)) {
	                fprintf(stderr, gettext("Event parameter size does not match buffer read.\n"));
	                return EXIT_FAILURE;
       		}
		/* now we don't need to refer to len again... */
		switch (buf[1]) {
			case EVT_INQUIRY_COMPLETE:
				/* this means the inquiry is complete, purge the database for the next */
				inquiry_complete = tstamp;
				SensorData * data = reporter_swap();			//processar todos os dados, convertendo-os!!!!!
				debug && print_data(data);
				if (sensor_result != NULL)
					sensor_result(data);
				if (buf[2] != 1) {
					fprintf(stderr, gettext("Inquiry Complete event length wrong.\n"));
				}
				errno = bt_error(buf[3]);
				if (errno > 0) {
					perror("Inquiry Complete");
					return EXIT_FAILURE;
				}
				debug && printf("-\n");
				fflush(stdout);
				done=1;
				break;




			case EVT_INQUIRY_RESULT:
				/* we can infer the distance/interference from the time taken for a response */
				/* got a result but without any devices in it (!) */
				if (buf[2] == 0)
					break;

				if (buf[2] != ((buf[3] * sizeof(inquiry_info)) + 1)) {
					fprintf(stderr, gettext("Inquiry Result event length wrong.\n"));
					return EXIT_FAILURE;
				}

				/* finding the time since the beginning of this inq
				 * gives us an unreliable hint about how far away it is
				 * RSSI is better for that, of course.
				 */
				timersub(&tstamp,&inquiry_complete,&resultime);

				for (i = 0; i < buf[3]; i++) {
					debug && printf("@ %8lu:%06lu ... ", resultime.tv_sec, resultime.tv_usec);
					reporter_add((inquiry_info*)&buf[4 + i * sizeof(inquiry_info)]);
				}
				/* flush so netcat6 or whatever else that is on standard out, gets the events */
				fflush(stdout);
				break;
			case EVT_REMOTE_NAME_REQ_COMPLETE:
				if (buf[2] != sizeof(evt_remote_name_req_complete)) {
					fprintf(stderr, gettext("Name Request Complete event length wrong.\n"));
					return EXIT_FAILURE;
				}
				errno = bt_error(((evt_remote_name_req_complete*)&buf[3])->status);
				debug && fprintf(stderr, gettext("Name complete Status %s\n"), strerror(errno));
				ba2str( &(((evt_remote_name_req_complete*)&buf[3])->bdaddr), ieeeaddr);
				if (((evt_remote_name_req_complete*)&buf[3])->status == 0) {
					printf("!name %s ", ieeeaddr);
					for (i = 0; i < (buf[2] - 7); i++) {
						if ( (((evt_remote_name_req_complete*)&buf[3])->name[i]) == 0)
							break;
						if ( (((evt_remote_name_req_complete*)&buf[3])->name[i]) < 32) {
							printf("\\x%02x", (((evt_remote_name_req_complete*)&buf[3])->name[i]));
						} else {
							putchar((((evt_remote_name_req_complete*)&buf[3])->name[i]));
						}
					}
				} else {
					printf("!namefail %s %d", ieeeaddr, ((evt_remote_name_req_complete*)&buf[3])->status);
				}
				printf("\n");
				break;


			case EVT_CMD_COMPLETE:
				/* it can be bigger, depending on the opcode */
				if (buf[2] < sizeof(evt_cmd_complete)) {
									fprintf(stderr, gettext("Command Complete event length wrong."));
									return EXIT_FAILURE;
							}

				/* ogf is upper 6 bits, ocf lower 10 bits to make opcode */
				switch (((evt_cmd_complete*)&buf[3])->opcode) {						//quando terminar a inquisicao periodica
					case cmd_opcode_pack(OGF_LINK_CTL,OCF_PERIODIC_INQUIRY):
					errno = bt_error(buf[3 + sizeof(evt_cmd_complete)]);
					debug && fprintf(stderr, gettext("Periodic inquiry requested: %s\n"), strerror(errno));		//indica se conseguiu com sucesso
					if (buf[2] != (sizeof(evt_cmd_complete) + 1)) {
						fprintf(stderr, gettext("Periodic Inquiry Command Complete event length wrong.\n"));
						return EXIT_FAILURE;
					}
					if (buf[3 + sizeof(evt_cmd_complete)] == HCI_COMMAND_DISALLOWED) {
						fprintf(stderr, gettext("\
	The Dongle is in a state that refused setting periodic mode.\n\
	It may, of course, already be in periodic mode (You -SIGKILL'd me earlier?)\n\
	so I will just pretend that it is.\n\
	"));
						p_inquiry = 1;
					} else {
						if (errno > 0) {
							perror(gettext("Can't Start Periodic mode."));
							return EXIT_FAILURE;
						}
						if (inquiry_complete.tv_sec == 0) inquiry_complete = tstamp;
						p_inquiry = 1;
					}
					break;
					case cmd_opcode_pack(OGF_LINK_CTL,OCF_EXIT_PERIODIC_INQUIRY):
					errno = bt_error(buf[3 + sizeof(evt_cmd_complete)]);
					debug && fprintf(stderr, gettext("Exit Periodic inquiry requested: %s\n"), strerror(errno));
									if (buf[2] != (sizeof(evt_cmd_complete) + 1)) {
											fprintf(stderr, gettext("Exit Periodic Inquiry Command Complete event length wrong.\n"));
						return EXIT_FAILURE;
									}
									if (buf[3 + sizeof(evt_cmd_complete)] == HCI_COMMAND_DISALLOWED) {
						fprintf(stderr, gettext("The Dongle is in a state that refused clearing periodic mode.\nIt may, of course, have been unset by another program.\n so I will just pretend that it is.\n"));
						p_inquiry = 0;
									} else {
											errno = bt_error(buf[3 + sizeof(evt_cmd_complete)]);
											if (errno > 0) {
													perror(gettext("Can't Stop Periodic mode."));
													system("hciconfig hci0 reset");
													return EXIT_FAILURE;
											}
						p_inquiry = 0;
									}
					break;
				}

				break;




			case EVT_CMD_STATUS:
				if (buf[2] != sizeof(evt_cmd_status)) {
									perror(gettext("Command Status event length wrong.\n"));
									return EXIT_FAILURE;
							}
				errno = bt_error(((evt_cmd_status*)&buf[3])->status);
				switch (((evt_cmd_status*)&buf[3])->opcode) {
					case cmd_opcode_pack(OGF_LINK_CTL,OCF_CREATE_CONN):
					if (debug) {
						fprintf(stderr, gettext("Device Connect requested: %s\n"), strerror(errno));
					}
					break;
					case cmd_opcode_pack(OGF_LINK_CTL,OCF_DISCONNECT):
					if (debug) {
						fprintf(stderr, gettext("Device Disconnect Requested: %s\n"), strerror(errno));
					}
					break;
					case cmd_opcode_pack(OGF_LINK_CTL,OCF_REMOTE_NAME_REQ):
					printf("!namereq %d\n", ((evt_cmd_status*)&buf[3])->status);
					break;
					default:
					if (debug) {
						fprintf(stderr, gettext("HCI opcode %d requested: %s\n"), (((evt_cmd_status*)&buf[3])->opcode), strerror(errno));
					}
				}

				break;


			/* Recebeu uma respsota com RSSI (é o comum, e vai descodificar)
			 *
			 */
			case EVT_INQUIRY_RESULT_WITH_RSSI:
				if (buf[2] == 0)
					break;

				if (buf[2] != ((buf[3] * sizeof(inquiry_info_with_rssi)) + 1)) {
					perror(gettext("Inquiry Result with RSSI event length wrong."));
					return EXIT_FAILURE;
				}

				timersub(&tstamp,&inquiry_complete,&resultime);

				for (i = 0; i < buf[3]; i++) {
					debug && printf("@ %8lu:%06lu ... ", resultime.tv_sec, resultime.tv_usec);
					//debug && printf("\n\n BEFORE REPORTER \n");
					reporter_add_with_rssi((inquiry_info_with_rssi*)&buf[4 + i * sizeof(inquiry_info_with_rssi)]);
					//debug && printf("\n\n AFTER REPORTER \n");
				}
				fflush(stdout);
				break;
			}
		break;
	}
	return EXIT_SUCCESS;
}

static void exquisition(int sig) {
}

/**
 * The Inquisition... sets up dongle to scan
 * continuously, and continuously reads its output
 */
static int inquisition(int dd, int length) {
    fd_set readSet;
    fd_set writeSet;
    fd_set exceptSet;
	char inbuf[100];
	char ieeeaddr[18];
	
	int i;
	unsigned int j, k, l;
	uint8_t dir;
	int len;
	periodic_inquiry_cp infoarea;
	periodic_inquiry_cp *info = &infoarea;

	write_inquiry_mode_cp inqmodearea;		//TODO DARIO
	write_inquiry_mode_cp *inqmode = &inqmodearea; //TODO DARIO

	remote_name_req_cp getnamearea;
	remote_name_req_cp *getname = &getnamearea;
	
	struct hci_filter flt;
	/* hci_event_hdr *hdr; */
	struct iovec iov;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct timeval tstamp;
	
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = exquisition;

	/* sent control-c */
	sigaction(SIGINT, &sa, NULL);
	/* killall inq */
	sigaction(SIGTERM, &sa, NULL);
	/* bash -c 'inq | something &'
	 * and the something e.g. netcat6 decides to quit...
	 */
	/* sigaction(SIGPIPE, &sa, NULL); */
	
	run = 1;

	/* connect to the dongle length between 1 and 30
	 *
	 */
	info->num_rsp = 0x00;
	info->lap[0] = 0x33;
	info->lap[1] = 0x8b;
	info->lap[2] = 0x9e;

	info->length = length;
	info->min_period = info->length + 1;
	info->max_period = info->min_period + 1;

	inqmode->mode = 0x01;		//TODO DARIO

	i = 1;
	//setsetsockopt(int s, int level, int optname, const void *optval, int optlen);

	if (setsockopt(dd, SOL_HCI, HCI_DATA_DIR, &i, sizeof(i)) < 0) {		//Option HCI_DATA_DIR
		perror(gettext("Can't request data direction\n"));
		return EXIT_FAILURE;
	}
		
	i = 1;
	if (setsockopt(dd, SOL_HCI, HCI_TIME_STAMP, &i, sizeof(i)) < 0) { //Option HCI_TIME_STAMP
		perror(gettext("Can't request data timestamps\n"));
		return EXIT_FAILURE;
	}
		
	/* filtering tells BlueZ which events we want on our socket... the dongle's
	 * own event mask isnt necessarily just this, it includes what other programs want too..?
	 * I.e. we want command complete, and all inquisition messages.
	 */
	hci_filter_clear(&flt);							//limpar a estrutura do filtro
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);		//Definir o tipo de filtro  -> Para receber apenas pacotes de eventos
	/* hci_filter_all_ptypes(&flt); */
	hci_filter_all_events(&flt);
	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {		//aplicar o filtro ao socket
		perror(gettext("Cannot set filter"));
		return EXIT_FAILURE;
	}



	//int hci_send_cmd(SOCKET , Opcode Group ,  Opcode Command (Comando em si),  length of the command param,  command param);
	//	hcitool cmd 03 0045 01				//Inquiry Result format with RSSI  (pag. 632 of specification)
	if(hci_send_cmd(dd,OGF_HOST_CTL,OCF_WRITE_INQUIRY_MODE,WRITE_INQUIRY_MODE_CP_SIZE,inqmode)){	//TODO DARIO
		perror(gettext("Cannot send command Inquiry Result format with RSSI!\n"));
		return EXIT_FAILURE;
	}

	sleep(1);

	//Enviar o comando PERIODIC_INQUIRY: PART E 7.1.1 -
	//hcitool cmd 01 0003 0A 00 09 00 33 8b 9e 08 00
	if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_PERIODIC_INQUIRY, PERIODIC_INQUIRY_CP_SIZE, info) < 0) {
		perror(gettext("Cannot request periodic inquiry!\n"));
		return EXIT_FAILURE;
	}




	memset(&msg, 0, sizeof(msg));

	iov.iov_base = &buf;
	iov.iov_len = sizeof(buf);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &controlbuf;
	msg.msg_controllen = sizeof(controlbuf);

	inquiry_complete.tv_sec = 0;
	inquiry_complete.tv_usec = 0;

	/* select <-> recvmsg (like read) */
	while (run || p_inquiry) {

	    FD_ZERO(&readSet);
	    FD_ZERO(&writeSet);
	    FD_ZERO(&exceptSet);

		FD_SET(dd, &readSet);
		FD_SET(fileno(stdin), &readSet);

		if (done){
			if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL) < 0) {	//se terminou, envia o comando para terminar a inquisição.
					hci_send_cmd(dd, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL);	//tenta mais 2 vezes terminar a inquisição.
					hci_send_cmd(dd, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL);
					fprintf(stderr, gettext("I cannot stop the inquisition!\n"));
					return EXIT_FAILURE;
			}else
				return EXIT_SUCCESS;
		}


		select(maxi(dd, fileno(stdin))+1, &readSet, &writeSet, &exceptSet, NULL);

		if (FD_ISSET(dd, &readSet)) {		//se o dd (socket) está activo (se o FileDescriptor está activo)
			len = recvmsg(dd, &msg, 0);		//recebe a mensagem do bluetooth
			if (len < 0) {
				/* this is the preferred method of exiting this program ... SIGINT */
				if (errno == EINTR) {
					if (run == 0) {
						perror("Second SIGINT received, giving up waiting for exit periodic!");
						return EXIT_FAILURE;
					}
					run = 0;
					if (p_inquiry) {
					    if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL) < 0) {
					                fprintf(stderr, gettext("I cannot stop the inquisition!\n"));
				        	        return EXIT_FAILURE;
						}
				    }
					perror(gettext("Exiting"));
				} else {
					perror(gettext("Could not receive\n"));
					return EXIT_FAILURE;
				}


				//len tem o comprimento da mensagem guardada em &msg
			} else {
				cmsg = CMSG_FIRSTHDR(&msg);		//aceder ao cabecalho
				while (cmsg) {
					switch (cmsg->cmsg_type) {
						case HCI_CMSG_DIR:
							dir = *((uint8_t *) CMSG_DATA(cmsg));			//se for dados
							break;
						case HCI_CMSG_TSTAMP:
							tstamp = *((struct timeval *) CMSG_DATA(cmsg));		//se for o timestamp
							break;
					}
					cmsg = CMSG_NXTHDR(&msg, cmsg);	//next header
				}
				/* -------------------->        decode packets <-------------- */
				//printf("dump of receved msg: %02X %02X %02X %02X %02X %02X\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

				//agora vamos descodificar a mensagem recebida!!!!!!
				if ((i=decode(len, tstamp)) != EXIT_SUCCESS) { 			//<-------- DECODE
					return i;
				}
			}
		}


		// Depois de descodificado....
		if (FD_ISSET(fileno(stdin), &readSet)) {
			len = read(0, inbuf, 50);
			if (len < 1) {
				/* this is the preferred method of exiting this program ... SIGINT */
				if (errno == EINTR) {
					if (run == 0) {
						perror("Second SIGINT received, giving up waiting for exit periodic!");
						return EXIT_FAILURE;
					}
					run = 0;
					if (p_inquiry) {
					        if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL) < 0) {
					                fprintf(stderr, gettext("I cannot stop the inquisition!\n"));
				        	        return EXIT_FAILURE;
						}
				    }
					perror(gettext("Exiting"));
				} else {
					perror(gettext("Could not receive\n"));
					return EXIT_FAILURE;
				}
			}
		}

		inbuf[len] = 0;
		j = 0; k = 0; l = 0;
		i = sscanf(inbuf, "getname %17s %02x %02x %04x", ieeeaddr, &j, &k, &l);			//LER DO FICHEIRO

		getname->pscan_rep_mode = j;
		getname->pscan_mode = k;
		getname->clock_offset = l;

		ieeeaddr[17] = 0;
		if (i > 0) {
			str2ba(ieeeaddr, &getname->bdaddr);
			ba2str(&getname->bdaddr, ieeeaddr);
			getname->clock_offset = 0x8000 | getname->clock_offset;
			if (i < 4) {
				getname->clock_offset = 0;
			}

			debug && printf("getname request received %d [ %s %02x %02x %04x ]\n", i, ieeeaddr,
				getname->pscan_rep_mode,
				getname->pscan_mode,
				getname->clock_offset);

			if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_REMOTE_NAME_REQ, REMOTE_NAME_REQ_CP_SIZE, getname) < 0) {
				perror(gettext("Cannot request remote name!\n"));
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

//2 - Sensor Loader
void * sense(){
	int length = 0x08,dongle_n = -1;

	dongle_n = hci_get_route(NULL);

	if (dongle_n < 0) {
		fprintf(stderr, gettext("You don't seem to have a bluetooth dongle!\n"));
	}

	debug && fprintf(stderr, gettext("Welcome MIT to the inquisition on device %d\n"), dongle_n);

	dd = hci_open_dev(dongle_n);	//Abrir o dispositivo!

	if (dd < 0) {
		fprintf(stderr, gettext("I cannot connect to your dongle!\n"));
		return NULL;
	}

	done = 0;
	

	//Do inquistion to dongle dd when we are connected
	inquisition(dd, length);

	hci_close_dev(dd);
    
	time_t now;
    	time(&now);

	printf("Done: %s\n",ctime(&now) );
    	system("hciconfig hci0 reset");
    	printf("Reseted\n");
	return NULL;
}



// 1 - Init point - this method is invoked by spotter.c
void start_sense(){
	sense();
//	pthread_create(&sense_thread,NULL,sense,NULL);
}

void stop_cb(){
	sensor_result = NULL;
	
	if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL) < 0) 
	       fprintf(stderr, gettext("I cannot stop the inquisition!\n"));

	if (dd>0)
		hci_close_dev(dd);
}

static void fingerprint(){
	char * finger_file = FINGER_FILE;
	FILE * fp;
	int i = 0;

	fp = fopen(finger_file,"rb");
	char line[30];

	if (fp != NULL)		//se conseguiu abrir o ficheiro, guardar registos
		do_finger = 1;
	else
		return;

	while(fgets(line,30,fp) != NULL && i<20){
		finger_min[i] = atoi(strtok(line, ","));
		finger_max[i++] = atoi(strtok(NULL, ","));
	}

	fclose(finger_file);

	num_fingers = i;
}

void start_cb(void (* sensor_result_cb)(SensorData *)){
	fingerprint();
	sensor_result = sensor_result_cb;
}
