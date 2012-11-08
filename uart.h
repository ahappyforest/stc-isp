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
extern int open_uart(unsigned char *port, struct termios *oldtio, int baud, int databit, int stopbit, int parity, int flow);

/*
***********************************************************
* close_uart
* close the uart
* fd:     uart file description
* return: NULL
***********************************************************
*/
extern void close_uart(int fd);

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
extern int read_uart(int fd, unsigned char *buf, unsigned int len);

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
extern int write_uart(int fd, unsigned char *buf, unsigned int len);

/*
***********************************************************
* chgb_uart
* change the opened uart's baudrate
* fd: uart file description
* baud: the baudrate to be changed to
* return: 0 success, -1 error
***********************************************************
*/
extern int chgb_uart(int fd, unsigned int baud);


