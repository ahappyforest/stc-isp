#include <termios.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "uart.h"
#include "protocol.h"

#define MAX_PATH_LENGTH 1024
#define MAX_BUF_LENGTH  1024

#define DEFAULT_SERIAL_DEVICE  "/dev/ttyUSB0"
#define DEFAULT_BAUDE_RATE     "9600"

int main (int argc, char *argv[])
{
#define _BITSET(x) ((1) << (x))
#define OPT_SET_HEX_FILE  _BITSET(0)


	char serial_path[MAX_PATH_LENGTH];
	char hex_path[MAX_PATH_LENGTH];
	char binary_path[MAX_PATH_LENGTH];
	int baudrate;
	int serial_fd;
	int startflag;
	int interval = 100;

	// 设置默认值
	{
		sprintf(serial_path, "%s", DEFAULT_SERIAL_DEVICE);
		baudrate = atoi(DEFAULT_BAUDE_RATE);
	}

	int flag = 0;
	int opt;
	while ((opt = getopt(argc, argv, "s:b:f:h")) != -1) {
		switch(opt) {
			case 's':
				sprintf(serial_path, "%s", optarg);
				break;
			case 'b':
				baudrate = atoi(optarg);
				break;
			case 'f':
				flag |= OPT_SET_HEX_FILE;
				sprintf(hex_path, "%s", optarg);
				break;
			case 'h':
				printf("version: stcisp/1.0.0\n"
					   "Usage: stcisp [-h] [ -s serial ] [ -b baudrate ] [ -f hexfile ]\n"
					   "-h\t\t\t: this help\n"
					   "-s\t\t\t: serial path(default: /dev/ttyUSB0)\n"
					   "-b\t\t\t: serial baudrate(default: 9600)\n"
					   "-f\t\t\t: the .ihx/.hex file by sdcc compiler or other compiler\n"
					   "\n\n");
				return 0;
			case '?':
				printf("invalid option: %c\n", optopt);
				return -1;
		}
	}

	if (!(flag & OPT_SET_HEX_FILE)) {
		fprintf(stderr, "no hex file\n");
		exit(1);
	}

	// check hex_path is a regular file
	{
		struct stat statbuf;
		if (lstat(hex_path, &statbuf) < 0) {
			fprintf(stderr, "%s:%s\n", hex_path, strerror(errno));
			exit(1);
		}
		if (!S_ISREG(statbuf.st_mode)) {
			fprintf(stderr, "%s:Not regular file.\n", hex_path);
			exit(1);
		}
	}


	// 0. convert hex file to binary file
	{
		char cmd[MAX_BUF_LENGTH];
		char prefix[MAX_BUF_LENGTH];
		
		// 使用prefix做临时buf
		sprintf(prefix, "%s", hex_path);

		// 找到prefix中最后一个点, 将它置为\0, 那么就可以得到真正的前缀了
		char *p;
		p = strrchr(prefix, '.');
		assert(p != NULL);
		*p = '\0';

		printf("Hello: %s\n", hex_path);
		if (strstr(hex_path, ".ihx") != NULL) {
			sprintf(cmd, "objcopy -I ihex -O binary %s.ihx %s.bin", prefix, prefix);
		} else if (strstr(hex_path, ".hex") != NULL) {
			sprintf(cmd, "objcopy -I ihex -O binary %s.hex %s.bin", prefix, prefix);
		} else {
			fprintf(stderr, "you must input .ihex or .hex file");
			exit(1);
		}

		sprintf(binary_path, "%s.bin", prefix);
		system(cmd);
	}

	// 1. initial var
/*	{
		sprintf(serial_path, "%s", argv[1]);
		sprintf(binary_path, "%s", argv[3]);
		baudrate = atoi(argv[2]);
		serial_fd = -1;
		startflag = 0;            // 该变量用来标识mcu首次响应, 也就是起始信号
	}
*/
	startflag = 0;
	// 2. init serial device and open binary file
	{
		down_baudrate = baudrate;
		target_baudrate = down_baudrate;
		if (target_baudrate > 9600) 
			target_baudrate = 9600;

		init_msg();
		interval = input_file_msg((unsigned char *)binary_path);
		if (!(interval % 0x80)) {
			interval = interval / 0x80 + 1;
		} else {
			interval = interval / 0x80;
		}

		interval = interval * 0x80 * 8 * 10 / down_baudrate;
		if (interval < 50) interval = 50;
		serial_fd = open_uart(serial_path, 0, baudrate, 8, 1, 0, 0);
		assert(serial_fd != 0);
	}

	// 3. communicate with mcu
	unsigned char rec_buf[MAX_BUF_LENGTH], sen_buf[MAX_BUF_LENGTH];
    int sen_len;
    int rec_len;
    unsigned char do_type = 0;
	int nRead;

	fd_set fd_reads;
	struct timeval timeout;
	int ret;
//	int try_times;

//	try_times = 10000;
	while (1) {
		FD_ZERO(&fd_reads);
		FD_SET(serial_fd, &fd_reads);

		timeout.tv_sec = 0;
		timeout.tv_usec = interval * 1000;

		ret = select(FD_SETSIZE, &fd_reads, NULL, NULL, &timeout);
		if (ret == 0) {
			// 不断的向串口设备写入0x7F, 直到mcu响应， 在这之前我们按兵不动
			if (startflag == 0) {
				unsigned char buf = 0x7F;
				write_uart(serial_fd, &buf, 1);
			} else {
		//		try_times--;
		//		printf("Waiting Mcu...\n");
		//		if (try_times <= 0) {
		//			fprintf(stderr, "Mcu Do not Response...\n");
		//			return -1;
		//		}
			}
			continue;
		} else if (ret == -1) {
			// 发生错误直接退出
			fprintf(stderr, "select error:%s\n", strerror(errno));
			exit(1);
		} else {
			// 设置mcu首次响应标志
			if (startflag == 0) {
				printf("Start Programming>>>\n");
				startflag = 1;
			}

			// 先从串口读取mcu响应的数据
			rec_len = read_uart(serial_fd, rec_buf, 1024);
			if (rec_len < 0) {
				fprintf(stderr, "Read UART Error\n\n");
				exit(-1);
			}
			
			// 再将rec_buf数据交给treat_msg进行解析, 得到可以发给mcu的数据sen_buf
			do_type = treat_msg(sen_buf, 1024, rec_buf, rec_len, &sen_len);

			// 分析treat_msg解析的返回值
			// do_type:
			//		 0 : nothing to be done for uart
			//		 1 : there is a message to be sent to the target
			//		 2 : the baudrate of the uart should be changed
			//		 3 : data msg will be sent
			//		 4 : the uart should be closed

			switch (do_type) {
				case 1:
					printf(">>>>> Send MSG:\n");
					write_uart(serial_fd, sen_buf, sen_len);
					break;
				case 2:
					chgb_uart(serial_fd, target_baudrate);
					printf(">>>>> Send MSG:\n");
					write_uart(serial_fd, sen_buf, sen_len);
					chgb_uart(serial_fd, down_baudrate);
					break;
				case 3:
					printf(">>>> Send MSG:\n");
					write_uart(serial_fd, sen_buf, sen_len);
					break;
				case 4:
					 close_uart(serial_fd);
					 printf("Program OK\n\rHave already encrypt.\n\r");
					 char cmd[MAX_BUF_LENGTH];
					 sprintf(cmd, "rm -rf %s", binary_path);
					 return 0;
			}
		}
	}

	return 0;
}
