#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <errno.h>
#include <termios.h>

void signal_handler_IO (int status);   /* definition of signal handler */

int n;
int fd;
int connected;
struct termios termAttr;
struct sigaction saio;
char mensaje[10];


int main(int argc, char *argv[])
{
        fd = open("/dev/tty0", O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1)
        {
           perror("open_port: Unable to open /dev/ttyO1\n");
           exit(1);
        }

        saio.sa_handler = signal_handler_IO;
        saio.sa_flags = 0;
        saio.sa_restorer = NULL; 
        sigaction(SIGIO,&saio,NULL);

        fcntl(fd, F_SETFL, FNDELAY);
        fcntl(fd, F_SETOWN, getpid());
        fcntl(fd, F_SETFL,  O_ASYNC ); /**<<<<<<------This line made it work.**/

        tcgetattr(fd,&termAttr);
        //baudRate = B115200;          /* Not needed */
        cfsetispeed(&termAttr,B115200);
        cfsetospeed(&termAttr,B115200);
        termAttr.c_cflag &= ~PARENB;
        termAttr.c_cflag &= ~CSTOPB;
        termAttr.c_cflag &= ~CSIZE;
        termAttr.c_cflag |= CS8;
        termAttr.c_cflag |= (CLOCAL | CREAD);
        termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        termAttr.c_iflag &= ~(IXON | IXOFF | IXANY);
        termAttr.c_oflag &= ~OPOST;
        //PARA ENVIAR MENSAJES
        termAttr.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
        termAttr.c_lflag &= ~ECHOE; // Disable erasure
        termAttr.c_lflag &= ~ECHONL; // Disable new-line echo
        termAttr.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
        termAttr.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
        termAttr.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
        
        tcsetattr(fd,TCSANOW,&termAttr);
        printf("UART1 CONFIGURANDO CHAT....\n");

        termAttr.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
        termAttr.c_cc[VMIN] = 0;

        // Set in/out baud rate to be 9600(Configuracion de que se pueda escribir un mensaje)
        cfsetispeed(&termAttr, B9600);
        cfsetospeed(&termAttr, B9600);
          // Save tty settings, also checking for error
        if (tcsetattr(fd, TCSANOW, &termAttr) != 0) {
          printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
        }

        connected = 1;
        while(connected == 1){
              // some code
            printf("LUIS_TTY0->:");
            scanf("%s", mensaje);
            write(fd, mensaje, sizeof(mensaje));
        }

        close(fd);
        exit(0);             
}

void signal_handler_IO (int status)
{
    char read_buf [256];       
    memset(&read_buf, '\0', sizeof(read_buf));
    int num_bytes = read(fd, &read_buf, sizeof(read_buf));
    if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
    }
    printf("PEDRO_TTY1->: %s\n", read_buf);

    //    printf("received data from UART.\n");
}