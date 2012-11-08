#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"


/*
***********************************************************
* printf_uart
* print out the bytes sent to(received from) uart
* buf:    the bytes to be print out
* len:    how many bytes do you want to see?
* return: NULL
***********************************************************
*/
void printf_uart(unsigned char *buf, unsigned int len)
{
    int i = 0;
    for (i = 0; i < len; i ++) {
        if (! (i % 16)) {
            if (i) printf("\n");
            printf("%04X: ", i);
        }
        printf("%02X ", buf[i]);
    }
    printf("\n\n");
}

/*
***********************************************************
* open_uart
* open the uart 
* prot:    the uart file name, like "/dev/ttyS0"
* oldtio:  the struct termios to save the old setting 
*          with uart
* baud:    the baud rate to set to the uart, 
*          default is 9600
* databit: the num of the data bits
* stopbit: the num of the stop bits , can be 1 or 2
* parity:  parity of the uart, 0 None, 1 Odd, 2 Even,
*                              3 Make, 4 Space
* flow:    flow of the uart, 0 None, 1 Hardware, 
*                            2 Software
* return:  uart file description if success, 
*          or -1 if error
***********************************************************
*/
int open_uart(unsigned char *port, struct termios *oldtio, int baud, int databit, int stopbit, int parity, int flow)
{
    int fd;
    struct termios termios_p;

	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY );

    if ( fd  < 0 ) {
        perror("Open com error:\n");
        //char buff[512];
        //snprintf(buff, 512, "<b>Open com error!</b>\n\n"
        //"<b>Please: chmod 666 %s or make sure it's ok!</b>", port);
        //create_xgcom_msg(maindata->gmain, buff);
        return -1;
    }
	printf("Open %s \n", port);

    if (oldtio != 0)
        tcgetattr(fd, oldtio);

	switch (baud) {
		case 300:
			termios_p.c_cflag =  B300;
			break;
		case 600:
			termios_p.c_cflag =  B600;
			break;
		case 1200:
			termios_p.c_cflag =  B1200;
			break;
		case 2400:
			termios_p.c_cflag =  B2400;
			break;
		case 4800:
			termios_p.c_cflag =  B4800;
			break;
		case 9600:
			termios_p.c_cflag =  B9600;
			//cfsetispeed(&termios_p, B9600);  
			//cfsetospeed(&termios_p, B9600); 
			break; 
		case 19200:
			termios_p.c_cflag =  B19200;
			break;
		case 38400:
			termios_p.c_cflag =  B38400;
			break;
		case 57600:
			termios_p.c_cflag =  B57600;
			break;
		case 115200:
			termios_p.c_cflag =  B115200;
			break;
		default:
			termios_p.c_cflag =  B9600;
			break;
	}
	
	switch (databit) {
		case 5:
			termios_p.c_cflag |= CS5;
			break;
		case 6:
			termios_p.c_cflag |= CS6;
			break;
		case 7:
			termios_p.c_cflag |= CS7;
			break;
		case 8:
			termios_p.c_cflag |= CS8;
			break;
		default:
			termios_p.c_cflag |= CS8;
			break;
	}
	switch (parity) {
		case 0:
			termios_p.c_cflag &= ~PARENB; /* Clear parity enable */
			termios_p.c_iflag &= ~INPCK; /* Enable parity checking */ 
			break;
		case 1:
			termios_p.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
			//termios_p.c_iflag |= INPCK; /* Disnable parity checking */ 
			break;
		case 2:
			termios_p.c_cflag |= PARENB; /* Enable parity */
			//termios_p.c_cflag &= ~PARODD; /* 转换为偶效验*/
			//termios_p.c_iflag |= INPCK; /* Disnable parity checking */
			break;
		default:
			//termios_p.c_cflag &= ~PARENB; /* Clear parity enable */
			//termios_p.c_iflag &= ~INPCK; /* Enable parity checking */ 
			break;
	}
	switch (stopbit) {
		case 1:
			termios_p.c_cflag &= ~CSTOPB; 
			break;
		case 2:
			termios_p.c_cflag |= CSTOPB; 
			break;
		default:
			termios_p.c_cflag &= ~CSTOPB; 
			break;
	}
	termios_p.c_cflag |= CREAD;
	termios_p.c_iflag = IGNPAR | IGNBRK;
	switch (flow) {
		case 1:
			termios_p.c_iflag |= (IXON | IXOFF | IXANY); //Enable Software Flow Control
			break;
		case 2:
			termios_p.c_cflag |= CRTSCTS;  //Enable hardware flow control
			break;
		default:
			//termios_p.c_cflag = 0;
			termios_p.c_cflag |= CLOCAL;
			break;
	}
	
	termios_p.c_oflag = 0;
	termios_p.c_lflag = 0;
	termios_p.c_cc[VTIME] = 0;
	termios_p.c_cc[VMIN] = 1;
	
	tcsetattr(fd, TCSANOW, &termios_p);
	tcflush(fd, TCOFLUSH);  
	tcflush(fd, TCIFLUSH);
	
    printf("Baudrate is %d\n\n", baud);

    return fd;	
}

/*
***********************************************************
* close_uart
* close the uart
* fd:     uart file description
* return: NULL
***********************************************************
*/
void close_uart(int fd)
{
	if (fd > 0) {
		close(fd);
        printf("close uart\n\n");
    }
}

/*
***********************************************************
* read_uart
* read from uart ,save to buf
* fd:     uart file description
* buf:    save bytes received from uart
* len:    the max size of the buf
* return: num of the bytes read from uart, or -1 if error
***********************************************************
*/
int read_uart(int fd, unsigned char *buf, unsigned int len)
{
    int ret = 0;

    if (fd < 0) {
        perror("file description is valid");
        return -1;
    }

    if (buf == NULL) {
        perror("read buf is NULL");
        return -1;
    } else {
        memset(buf, 0, len);
    }
    
    ret = read(fd, buf, len);
    if (ret < 0) {
        perror("read error:\n");
        return -1;
    }
    
//    if (uart_stat == 0) return -1;

    return ret;
}

/*
***********************************************************
* write_uart
* send bytes to uart
* fd:     uart file description
* buf:    bytes in which will be sent
* len:    the num of the bytes to be sent
* return: the num sent to the write buf, or -1 if error
***********************************************************
*/
int write_uart(int fd, unsigned char *buf, unsigned int len)
{
	int ret = 0;
    int i;

    for (i = 0; i < len; i ++) {
	    while(tcdrain(fd) == -1);
	    ret += write(fd, &buf[i], 1); 
    }
	
    if (ret < 0) perror("write:");
    printf_uart(buf, len);    

	return ret;
}

/*
***********************************************************
* chgb_uart
* change the opened uart's baudrate
* fd: uart file description
* baud: the baudrate to be changed to
* return: 0 success, -1 error
***********************************************************
*/
int chgb_uart(int fd, unsigned int baud)
{
    int ret = 0;
	struct termios termios_p; 
    unsigned int baudrate = B9600;
    
    if (fd == -1) {
        printf("change_baud error\n");
        return -1;
    }

    ret = tcgetattr(fd, &termios_p);
    
    if (ret == -1){
        printf("tcgetattr error\n");
        return -1;
    }

	switch (baud) {
		case 300:
			baudrate =  B300;
			break;
		case 600:
			baudrate =  B600;
			break;
		case 1200:
			baudrate =  B1200;
			//cfsetispeed(&termios_p, B1200);  
			//cfsetospeed(&termios_p, B1200);
            break;
		case 2400:
			baudrate =  B2400;
			break;
		case 4800:
			baudrate =  B4800;
			break;
		case 9600:
			baudrate =  B9600;
			//cfsetispeed(&termios_p, B9600);  
			//cfsetospeed(&termios_p, B9600); 
			break;
		case 19200:
			baudrate =  B19200;
			break;
		case 38400:
			baudrate =  B38400;
			break;
		case 57600:
			baudrate =  B57600;
			break;
		case 115200:
			baudrate =  B115200;
			break;
		default:
			baudrate =  B9600;
			break;
	}
    cfsetispeed(&termios_p, baudrate);  
    cfsetospeed(&termios_p, baudrate);


    while(tcdrain(fd) == -1);
	
    tcflush(fd, TCOFLUSH);  
	tcflush(fd, TCIFLUSH);


	tcsetattr(fd, TCSANOW, &termios_p);

    printf("Baudrate is %d\n\n", baud);
    
    return 0;

}
