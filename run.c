#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

int main(void) {
	FILE *file;
        char id[3];
        char line[80];
        char str[256];
	FILE *logFile;

        /* Our process ID and Session ID */
        pid_t pid, sid;
        
        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);
                
                
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
        

        
        /* Change the current working directory */
        if ((chdir("/home/pi/raspblue-sensor/src/spotter")) < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
        
        /* Close out the standard file descriptors */
//        close(STDIN_FILENO);
  //      close(STDOUT_FILENO);
   //     close(STDERR_FILENO);

	logFile = fopen("log.log","a+");
	fprintf(logFile,"%s","Reading sensor id");

        /*Read sensor id*/
        file = fopen("sensorid.cfg","rt");

        while(fscanf(file,"%s",id) != EOF){

        }

        fclose(file);
	fprintf(logFile,"%s","Sensor id: ");
	fprintf(logFile,"%s",id);
	fclose(logFile);
        /* The Big Loop */
        while (1) {
           /* Do some task here ... */
	   sprintf("Restart again");
           sprintf(str,"./spotter localhost %s",id);
           system(str);
        }
   exit(EXIT_SUCCESS);
}
