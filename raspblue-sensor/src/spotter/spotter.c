#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stddef.h>
#include <pthread.h>
#include <listType.h>
#include <unistd.h>
#include <netdb.h>
#include <syslog.h>
#include <dlfcn.h>
#include "listType.h"
#include "spotter.h"
#include "location.h"
#include "location_json.h"
#include "gsd_api.h"
#include "spotter_services.h"
#include <signal.h>

#include <curl/curl.h>

__tp(handler)* handler = NULL;
unsigned short SENSE_FREQUENCY = 2; //delay at sensor_loop between inquiry
unsigned short MIN_UPDATE_FREQUENCY = 10;
unsigned short UPDATE_FREQUENCY = 10;
char* server_WS;
int sensor_id;
static int MAC_SIZE = 18;   //with \n char
Location self;
LList plugins;
uint16_t MY_ADDRESS, MAP;
bool manager_available = false;
char buffSensorId[100];
char buffRss[100];
char buffMac[100];
CURL *curl;
CURLcode res;


void sendDataToWebPrepare(){
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    
    curl_global_init(CURL_GLOBAL_ALL);
	char sensorId[100];
	sprintf(sensorId, "%d", sensor_id);
    
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME,"sensorId",
                 CURLFORM_COPYCONTENTS,sensorId,
                 CURLFORM_END);
    curl = curl_easy_init();
    
    //Perform sensor data Cleanup
    if(curl){
        char url[150];
        strcpy(url,server_WS);
        strcat(url,"/clean.php");
        curl_easy_setopt(curl,CURLOPT_URL,url);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        
        /**Perform the request, res will get the return code*/
        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        /**Cleanup**/
        curl_easy_cleanup(curl);
        
        /**clean form**/
        curl_formfree(formpost);
        
    }
    printf("Success");
}






















void sendDataToWebServer(char* sensorId,char* rss,char* mac){
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME,"sensorId",
                 CURLFORM_COPYCONTENTS,sensorId,
                 CURLFORM_END);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME,"rss",
                 CURLFORM_COPYCONTENTS,rss,
                 CURLFORM_END);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME,"macAddress",
                 CURLFORM_COPYCONTENTS,mac,
                 CURLFORM_END);
    
    curl = curl_easy_init();
    
    if(curl){
	char url[150];
        strcpy(url,server_WS);
        strcat(url,"/new_data.php");
        curl_easy_setopt(curl,CURLOPT_URL,url);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        
        /**Perform the request, res will get the return code*/
        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        /**Cleanup**/
        curl_easy_cleanup(curl);
        
        /**clean form**/
        curl_formfree(formpost);
        
    }
    printf("Success");
}



/** Recebe o dado obtido recentemente pelo bluetooth e trata-o*/
void SensorResult(SensorData * data){
    unsigned short i;
    if(data->type == RSS){
        sendDataToWebPrepare();
        printf("--- NUM_NODES: %hu \n", data->RSS.node_number);
        for (i=0; i < data->RSS.node_number; i++){
            //  printf("- ID: %d ,MAC: %s , Distance: %f \n",sensor_id, data->RSS.nodes + (i*MAC_SIZE), data->RSS.rss[i]);
            memset(buffSensorId,0,100);
            memset(buffRss,0,100);
            memset(buffMac,0,100);
            sprintf(buffSensorId,"%d",sensor_id);
            sprintf(buffRss,"%f",data->RSS.rss[i]);
            sprintf(buffMac,"%s",data->RSS.nodes + (i*MAC_SIZE));
            
            sendDataToWebServer(buffSensorId,buffRss,buffMac);
        }
        printf("\n\n\n\n");
    }
}


void sig_handler(int signo)
{
    if (signo == SIGINT){
        printf("received SIGINT\n");
        free_elements();
        exit(0);
    }
}






//Loop to adquire data
void * sensor_loop(){
    LElement * item;
    Plugin * plugin;
    void (* start_sense)();
    int index = 1;
    char * error;
    
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    // A long long wait so that we can easily issue a signal to this process    
    while(true){
        index = 1;
        FOR_EACH(item, plugins){
            index++;
            plugin = (Plugin *) item->data;
            if (plugin->type == SYNC ){
                start_sense = dlsym(plugin->handle, "start_sense");     //inq.c start_sense() //create new thread for this attempt
                if ((error = dlerror()) != NULL) {
                    fprintf(stderr, "%s\n", error);
                }
                start_sense();
            }
        }
        sleep(SENSE_FREQUENCY); //5 secounds
    }
    return NULL;
}

void free_elements(){
    LElement * elem;
    char * error;
    void (* stop_cb)();
    FOR_EACH(elem, plugins){
        Plugin * p = (Plugin *)elem->data;
        stop_cb = dlsym(p->handle, "stop_cb");
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            continue;
        }
        stop_cb();
        dlclose(p->handle);
    }
    FreeList(&plugins);
    system("hciconfig hci0 reset");
}

void print_plugins(){
    LElement * item;
    char * type;
    Plugin * plugin;
    FOR_EACH(item, plugins){
        plugin = (Plugin *) item->data;
        switch(plugin->type){
            case SYNC : type = "SYNC";
                break;
            case ASYNC : type = "ASYNC";
                break;
        }
        printf("LOCATION: %s ; TYPE: %s\n", plugin->location, type);
    }
}


int main(int argc, char ** argv) {
    
    //LER CONFIGS E VER QUAIS OS PLUGINS QUE EXISTEM
    char line[80];
    char * var_value;
    char * plugin_location;
    FILE * config_file;
    Plugin * plugin;
    void * handle;
    void (*start_cb)();
    char * error;
    int op = 0;
    LElement * item;
    char * spotter_file;
    
    if (argc != 3) {
        printf("USAGE: spotter <server_WSWebPage> <sensorId>\n");
        return 0;
    }else{
        server_WS = argv[1];
        sensor_id = atoi(argv[2]);
        spotter_file = "config/spotter.cfg";
    }
    
    CreateList(&plugins);
    
    logger("Main - Reading Config file\n");
    
    //LER CONFIGS DO FICHEIRO
    if (!(config_file = fopen(spotter_file, "rt"))) {
        printf("Invalid spotter config file\n");
        return 0;
    }
    
    while (fgets(line, 80, config_file) != NULL) {
        if (line[0] == '#')
            continue;
        
        var_value = strtok(line, "=");
        if (!memcmp(var_value, "PLUGIN", strlen("PLUGIN"))){
            plugin = (Plugin *) malloc(sizeof(Plugin));
            plugin_location = strtok(NULL, " \n");
            plugin->location = (char *) malloc(strlen(plugin_location) + 1);
            memcpy(plugin->location,plugin_location,strlen(plugin_location) + 1);
            char * plugin_type = strtok(NULL, " \n");
            if (!memcmp(plugin_type, "SYNC", strlen("SYNC")))
                plugin->type = SYNC;
            else if (!memcmp(plugin_type, "ASYNC", strlen("ASYNC")))
                plugin->type = ASYNC;
            else{
                debugger("INVALID PLUGIN TYPE\n");
                exit(1);
            }
            AddToList(plugin, &plugins);
        }else if (!memcmp(var_value, "MY_ADDRESS",strlen("MY_ADDRESS")))
            MY_ADDRESS = atoi(strtok(NULL, "=\n"));
        else if (!memcmp(var_value, "MAP",strlen("MAP")))
            MAP = atoi(strtok(NULL, "=\n"));
        else if(!memcmp(var_value, "LOCATION", strlen("LOCATION"))){
            self.x = atof(strtok(NULL, ";"));
            self.y = atof(strtok(NULL, "\n"));
        }else if (!memcmp(var_value, "SENSE_FREQUENCY",strlen("SENSE_FREQUENCY")))
            SENSE_FREQUENCY = atoi(strtok(NULL, "=\n"));
        else if (!memcmp(var_value, "MIN_UPDATE_FREQUENCY", strlen("MIN_UPDATE_FREQUENCY")))
            MIN_UPDATE_FREQUENCY = atoi(strtok(NULL, "=\n"));
        
    }
    
    fclose(config_file);
    logger("Main - Configuration concluded\n");
    
    system("hciconfig hci0 reset");
    
    
    
    //START GATHERING SENSOR DATA
    int index = 1;
    FOR_EACH(item, plugins) {
        plugin = (Plugin *) item->data;
        handle = dlopen(plugin->location, RTLD_NOW);
        if (!handle) {
            fprintf(stderr, "%s\n", dlerror());
            free_elements();
            exit(1);
        }else
            plugin->handle = handle;
        dlerror(); /* Clear any existing error */
        start_cb = dlsym(handle, "start_cb");
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            free_elements();
            exit(1);
        }else{
            start_cb(SensorResult);
        }
        index++;
    }
    
    // printf("If some problem, try: hciconfig hci0 reset\n");
    printf("MENU Bluetooth Controler: 1-EXIT\n\n");
    sensor_loop();

    return 0;
}
