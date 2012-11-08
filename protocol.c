#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "protocol.h"

#define FILEBUF_LENGTH  65536

#define TEST_LOOP_ACK 0

#define B_DOUBLE_SPEED  0x01
#define B_HALF_GAIN     0x10
#define B_P1_DOWN       0x04

#define NULL_MSG  0xFF
#define START_MSG 0x7F
#define INFO_MSG  0x01
#define DATA_MSG  0x00
#define ACK_MSG   0x80
#define ERR_MSG   0x81
#define END_MSG   0x82
#define RDY_MSG   0x84
#define CHGB_MSG  0x8E
#define CHKB_MSG  0x8F
#define XXX_MSG   0x52


#define NULL_STEP    0x00
#define INFO_STEP    0x01
#define CHKB_STEP    0x02
#define CHGB_STEP    0x03
#define UNKNOW_STEP  0x4
#define RDY_STEP     0x05
#define LSTDATA_STEP 0x06
#define FSTDATA_STEP 0x07
#define END_STEP     0x08

unsigned char f_double_speed = 0;
unsigned char f_half_gain = 0;
unsigned char f_p1_down = 0;


static unsigned char UNKNOW_STEP_COUNT = 0;


unsigned char target_info[] = {0x68, 0x00, 0x3B, 0x00, 0x16, 0xBA, 0x16, 0xBA, \
                               0x16, 0xBA, 0x16, 0xBA, 0x16, 0xBA, 0x16, 0xBA, \
                               0X16, 0xBA, 0x16, 0xBA, 0x42, 0x43, 0xFD, 0xF0, \
                               0x02, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, \
                               0x02, 0x07, 0x16 };

unsigned char target_info_head[] = {0x68, 0x00, 0x3B, 0x00 };


unsigned char package_head[] = {0x46, 0xB9 };
unsigned char package_end = 0x16;
unsigned char package_recflag = 0x68;
unsigned char package_senflag = 0x6A;

unsigned char package_data_baudrate_chk[] = {0xFE, 0xC8, 0x01, 0x70, 0x28, 0x81 };
unsigned char package_data_baudrate_chg[] = {0xFE, 0xC8, 0x01, 0x70, 0x26 };

unsigned char package_data_rdy[] = {0x02 };


static unsigned char baudrate_chk_24M_1200[] = {0x02, 0xE2, 0x28 };
static unsigned char baudrate_chg_24M_1200[] = {0x02, 0xE2, 0x26 };
static unsigned char baudrate_chk_24M_2400[] = {0x01, 0x72, 0x28 };
static unsigned char baudrate_chg_24M_2400[] = {0x01, 0x72, 0x23 };
static unsigned char baudrate_chk_24M_4800[] = {0x00, 0xFE, 0x28 };
static unsigned char baudrate_chg_24M_4800[] = {0x00, 0xFF, 0x1E };
static unsigned char baudrate_chk_24M_9600[] = {0x00, 0x9C, 0x28 };
static unsigned char baudrate_chg_24M_9600[] = {0x00, 0x9C, 0x17 };
static unsigned char baudrate_chk_24M_19200[] = {0x00, 0x4E, 0x28 };
static unsigned char baudrate_chg_24M_19200[] = {0x00, 0x4E, 0x17 };
static unsigned char baudrate_chk_24M_38400[] = {0x00, 0x28, 0x28 };
static unsigned char baudrate_chg_24M_38400[] = {0x00, 0x28, 0x17 };
static unsigned char baudrate_chk_24M_57600[] = {0x00, 0x1A, 0x28 };
static unsigned char baudrate_chg_24M_57600[] = {0x00, 0x1A, 0x17 };
static unsigned char baudrate_chk_24M_115200[] = { };
static unsigned char baudrate_chg_24M_115200[] = { };

static unsigned char baudrate_chk_22M_1200[] = {0x02, 0x80, 0x28 };
static unsigned char baudrate_chg_22M_1200[] = {0x02, 0x80, 0x26 };
static unsigned char baudrate_chk_22M_2400[] = {0x01, 0x40, 0x28 };
static unsigned char baudrate_chg_22M_2400[] = {0x01, 0x40, 0x23 };
static unsigned char baudrate_chk_22M_4800[] = {0x00, 0xEE, 0x28 };
static unsigned char baudrate_chg_22M_4800[] = {0x00, 0xFF, 0x1E };
static unsigned char baudrate_chk_22M_9600[] = {0x00, 0x90, 0x28 };
static unsigned char baudrate_chg_22M_9600[] = {0x00, 0x90, 0x17 };
static unsigned char baudrate_chk_22M_19200[] = {0x00, 0x48, 0x28 };
static unsigned char baudrate_chg_22M_19200[] = {0x00, 0x48, 0x17 };
static unsigned char baudrate_chk_22M_38400[] = {0x00, 0x24, 0x28 };
static unsigned char baudrate_chg_22M_38400[] = {0x00, 0x24, 0x17 };
static unsigned char baudrate_chk_22M_57600[] = {0x00, 0x18, 0x28 };
static unsigned char baudrate_chg_22M_57600[] = {0x00, 0x18, 0x17 };
static unsigned char baudrate_chk_22M_115200[] = {0x00, 0x0C, 0x28 };
static unsigned char baudrate_chg_22M_115200[] = {0x00, 0x0C, 0x17 };

static unsigned char baudrate_chk_16M_1200[] = {0x01, 0xFE, 0x28 };
static unsigned char baudrate_chg_16M_1200[] = {0x01, 0xFF, 0x26 };
static unsigned char baudrate_chk_16M_2400[] = {0x00, 0xFE, 0x28 };
static unsigned char baudrate_chg_16M_2400[] = {0x00, 0xFF, 0x23 };
static unsigned char baudrate_chk_16M_4800[] = {0x00, 0xD0, 0x28 };
static unsigned char baudrate_chg_16M_4800[] = {0x00, 0xD0, 0x1E };
static unsigned char baudrate_chk_16M_9600[] = {0x00, 0x68, 0x28 };
static unsigned char baudrate_chg_16M_9600[] = {0x00, 0x68, 0x17 };
static unsigned char baudrate_chk_16M_19200[] = {0x00, 0x34, 0x28 };
static unsigned char baudrate_chg_16M_19200[] = {0x00, 0x34, 0x17 };
static unsigned char baudrate_chk_16M_38400[] = {0x00, 0x1A, 0x28 };
static unsigned char baudrate_chg_16M_38400[] = {0x00, 0x1A, 0x17 };
static unsigned char baudrate_chk_16M_57600[] = { };
static unsigned char baudrate_chg_16M_57600[] = { };
static unsigned char baudrate_chk_16M_115200[] = { };
static unsigned char baudrate_chg_16M_115200[] = { };

static unsigned char baudrate_chk_12M_1200[] = {0x01, 0x70, 0x28 };
static unsigned char baudrate_chg_12M_1200[] = {0x01, 0x70, 0x26 };
static unsigned char baudrate_chk_12M_2400[] = {0x00, 0xFE, 0x28 };
static unsigned char baudrate_chg_12M_2400[] = {0x00, 0xFF, 0x23 };
static unsigned char baudrate_chk_12M_4800[] = {0x00, 0x9C, 0x28 };
static unsigned char baudrate_chg_12M_4800[] = {0x00, 0x9C, 0x1E };
static unsigned char baudrate_chk_12M_9600[] = {0x00, 0x4E, 0x28 };
static unsigned char baudrate_chg_12M_9600[] = {0x00, 0x4E, 0x17 };
static unsigned char baudrate_chk_12M_19200[] = {0x00, 0x28, 0x28 };
static unsigned char baudrate_chg_12M_19200[] = {0x00, 0x28, 0x17 };
static unsigned char baudrate_chk_12M_38400[] = {0x00, 0x14, 0x28 };
static unsigned char baudrate_chg_12M_38400[] = {0x00, 0x14, 0x17 };
static unsigned char baudrate_chk_12M_57600[] = { };
static unsigned char baudrate_chg_12M_57600[] = { };
static unsigned char baudrate_chk_12M_115200[] = { };
static unsigned char baudrate_chg_12M_115200[] = { };

static unsigned char baudrate_chk_11M_1200[] = {0x01, 0x40, 0x28 };
static unsigned char baudrate_chg_11M_1200[] = {0x01, 0x40, 0x26 };
static unsigned char baudrate_chk_11M_2400[] = {0x00, 0xFE, 0x28 };
static unsigned char baudrate_chg_11M_2400[] = {0x00, 0xFF, 0x23 };
static unsigned char baudrate_chk_11M_4800[] = {0x00, 0x90, 0x28 };
static unsigned char baudrate_chg_11M_4800[] = {0x00, 0x90, 0x1E };
static unsigned char baudrate_chk_11M_9600[] = {0x00, 0x48, 0x28 };
static unsigned char baudrate_chg_11M_9600[] = {0x00, 0x48, 0x17 };
static unsigned char baudrate_chk_11M_19200[] = {0x00, 0x24, 0x28 };
static unsigned char baudrate_chg_11M_19200[] = {0x00, 0x24, 0x17 };
static unsigned char baudrate_chk_11M_38400[] = {0x00, 0x12, 0x28 };
static unsigned char baudrate_chg_11M_38400[] = {0x00, 0x12, 0x17 };
static unsigned char baudrate_chk_11M_57600[] = {0x00, 0x0C, 0x28 };
static unsigned char baudrate_chg_11M_57600[] = {0x00, 0x0C, 0x17 };
static unsigned char baudrate_chk_11M_115200[] = {0x00, 0x06, 0x28 };
static unsigned char baudrate_chg_11M_115200[] = {0x00, 0x06, 0x17 };

static unsigned char baudrate_chk_8M_1200[] = {0x00, 0xFE, 0x28 };
static unsigned char baudrate_chg_8M_1200[] = {0x00, 0xFF, 0x26 };
static unsigned char baudrate_chk_8M_2400[] = {0x00, 0xD0, 0x28 };
static unsigned char baudrate_chg_8M_2400[] = {0x00, 0xD0, 0x23 };
static unsigned char baudrate_chk_8M_4800[] = {0x00, 0x68, 0x28 };
static unsigned char baudrate_chg_8M_4800[] = {0x00, 0x68, 0x1E };
static unsigned char baudrate_chk_8M_9600[] = {0x00, 0x34, 0x28 };
static unsigned char baudrate_chg_8M_9600[] = {0x00, 0x34, 0x17 };
static unsigned char baudrate_chk_8M_19200[] = {0x00, 0x1A, 0x28 };
static unsigned char baudrate_chg_8M_19200[] = {0x00, 0x1A, 0x17 };
static unsigned char baudrate_chk_8M_38400[] = { };
static unsigned char baudrate_chg_8M_38400[] = { };
static unsigned char baudrate_chk_8M_57600[] = { };
static unsigned char baudrate_chg_8M_57600[] = { };
static unsigned char baudrate_chk_8M_115200[] = { };
static unsigned char baudrate_chg_8M_115200[] = { };

static unsigned char baudrate_chk_6M_1200[] = {0x00, 0xFE, 0x28 };
static unsigned char baudrate_chg_6M_1200[] = {0x00, 0xFF, 0x26 };
static unsigned char baudrate_chk_6M_2400[] = {0x00, 0x9C, 0x28 };
static unsigned char baudrate_chg_6M_2400[] = {0x00, 0x9C, 0x23 };
static unsigned char baudrate_chk_6M_4800[] = {0x00, 0x4E, 0x28 };
static unsigned char baudrate_chg_6M_4800[] = {0x00, 0x4E, 0x1E };
static unsigned char baudrate_chk_6M_9600[] = {0x00, 0x28, 0x28 };
static unsigned char baudrate_chg_6M_9600[] = {0x00, 0x28, 0x17 };
static unsigned char baudrate_chk_6M_19200[] = {0x00, 0x14, 0x28 };
static unsigned char baudrate_chg_6M_19200[] = {0x00, 0x14, 0x17 };
static unsigned char baudrate_chk_6M_38400[] = {0x00, 0x0A, 0x28 };
static unsigned char baudrate_chg_6M_38400[] = {0x00, 0x0A, 0x17 };
static unsigned char baudrate_chk_6M_57600[] = { };
static unsigned char baudrate_chg_6M_57600[] = { };
static unsigned char baudrate_chk_6M_115200[] = { };
static unsigned char baudrate_chg_6M_115200[] = { };

static unsigned char baudrate_chk_4M_1200[] = {0x00, 0xD0, 0x28 };
static unsigned char baudrate_chg_4M_1200[] = {0x00, 0xD0, 0x26 };
static unsigned char baudrate_chk_4M_2400[] = {0x00, 0x68, 0x28 };
static unsigned char baudrate_chg_4M_2400[] = {0x00, 0x68, 0x23 };
static unsigned char baudrate_chk_4M_4800[] = {0x00, 0x34, 0x28 };
static unsigned char baudrate_chg_4M_4800[] = {0x00, 0x34, 0x1E };
static unsigned char baudrate_chk_4M_9600[] = {0x00, 0x1A, 0x28 };
static unsigned char baudrate_chg_4M_9600[] = {0x00, 0x1A, 0x17 };
static unsigned char baudrate_chk_4M_19200[] = { };
static unsigned char baudrate_chg_4M_19200[] = { };
static unsigned char baudrate_chk_4M_38400[] = { };
static unsigned char baudrate_chg_4M_38400[] = { };
static unsigned char baudrate_chk_4M_57600[] = { };
static unsigned char baudrate_chg_4M_57600[] = { };
static unsigned char baudrate_chk_4M_115200[] = { };
static unsigned char baudrate_chg_4M_115200[] = { };

static unsigned char databuf_msg[512];
static unsigned int datalen_msg = 0;

static unsigned char filebuf[FILEBUF_LENGTH];
static unsigned int filelen = 0;
static unsigned int fileoffset = 0;

unsigned long target_hz = 12000000;
unsigned int target_baudrate = 1200;
unsigned int down_baudrate = 1200;

static unsigned char step_msg = 0;

static int rec_count_msg = 0;
static unsigned char buf_msg[1024];


/*
***********************************************************
* printf_msg
* print the message receive or send
* buf_name : you can give some note of the message print out
* buf      : point to the buf in which the message will be
*            print out
* len      : the length of the message to be pint out
* return   : NULL
***********************************************************
*/
void printf_msg(unsigned char *buf_name, unsigned char *buf, unsigned int len)
{
    int i = 0;
    printf("%s", buf_name);
    for (i = 0; i < len; i ++) {
        if (! (i % 16)) {
            printf("\n%04X: ", i);
        }
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

/*
***********************************************************
* init_msg
* init the message buf
***********************************************************
*/
void init_msg()
{
    rec_count_msg = 0;
    step_msg = 0;
    fileoffset = 0;
    datalen_msg = 0;
}

/*
***********************************************************
* treat_receive_target_info_msg
* treat the target information message received from uart
* the message contain these information:
*     1.crystal frequency of the target system
*     2.the type of the target MPU
*     3.the version of the ISP
*     4.current settings of the MPU
* buf   : point to the buf used to save the message 
*         received from the uart 
* len   : the length of the message in buf
* return: the type of the message, INFO_MSG if received 
*         complete target infomation message, or else
*         return NULL_MSG
***********************************************************
*/
unsigned char treat_receive_target_info_msg(unsigned char *buf, int len)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned long k = 0;
    double l = 0;

    unsigned char ret = NULL_MSG;

    while (i < len) {
        buf_msg [rec_count_msg ++] = buf [i ++];
        
        switch (rec_count_msg) {
            case 4 : {
                if ((buf_msg[0] != target_info_head[0]) || (buf_msg[1] != target_info_head[1]) || \
                    (buf_msg[2] != target_info_head[2]) || (buf_msg[3] != target_info_head[3]) ) {
                    memset(buf_msg, 0, rec_count_msg);
                    rec_count_msg = 0; 
                }

                break;
            }

            case 20 : {
                k = 0;
                for (j = 4; j < 20; j += 2) {
                    k += ((unsigned long)buf_msg[j] << 8);
                    k += (unsigned long)buf_msg[j + 1]; 
                }
                k = k >> 3;
                k = k * target_baudrate;
                k = k / 5816;
                if ((k%100) >= 50) {
                    k = (k / 100) + 1;  
                } else {
                    k = k / 100;
                }

                if (k == 24) {
                    target_hz = 24000000;
                }
                else if (k == 22) {
                    target_hz = 22118400;
                }
                else if (k == 16) {
                    target_hz = 16000000;
                }
                else if (k == 12) {
                    target_hz = 12000000;
                }
                else if (k == 11) {
                    target_hz = 11059200;
                }
                else if (k == 8) {
                    target_hz = 8000000;
                }
                else if (k == 6) {
                    target_hz = 6000000;
                } 
                else if (k == 4) {
                    target_hz = 4000000;
                }
 
                break;
            }
            case 23 : {
                if (!(buf_msg[rec_count_msg - 1] & B_HALF_GAIN)) {
                    f_half_gain = 1; // 1/2 gain
                    printf("target is 1/2 gain \n");
                }
                else {
                    f_half_gain = 0; // full gain
                    printf("target is full gain \n");
                }
                if (!(buf_msg[rec_count_msg - 1] & B_DOUBLE_SPEED)) {
                    f_double_speed = 1; // 6T
                    printf("target is double speed(6T) \n");
                }
                else {
                    f_double_speed = 0; // 12T
                    printf("target is normal speed(12T) \n");
                }
                if (!(buf_msg[rec_count_msg - 1] & B_P1_DOWN)) {
                    f_p1_down = 1; // P1.1 P1.0 pull down
                    printf("target's P1.1&P1.0 pull down \n");
                }
                else {
                    f_p1_down = 0; // P1.1 P1.0 normal
                    printf("target's P1.1&P1.0 normal \n");
                }
                printf("\n");
                break;
            }
            case 58 : {
                k = 0;
                for (j = 0; j < (rec_count_msg -1); j ++) {
                    k += buf_msg[j];
                }
  
                if ((unsigned char)k != buf_msg[rec_count_msg - 1]) {
                    printf("check target infomation error\n");
                    memset(buf_msg, 0, rec_count_msg);
                    rec_count_msg = 0; 
                }
                break;  
            }
            case 59 : {
                if (buf_msg[rec_count_msg - 1] != 0x16) {
                    printf("target information has not end flag\n");
                    memset(buf_msg, 0, rec_count_msg);
                    rec_count_msg = 0;
                } else {

                    //if (f_double_speed) {
                      //  target_hz <<= 1;
                    //}

                    if (target_hz == 24000000) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_19200, 3);
                                break;
                            case 38400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_38400, 3);
                                break;
                            case 57600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_57600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_57600, 3);
                                down_baudrate = 57600;
                                break;
                            case 115200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_115200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_115200, 3); 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_57600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_57600, 3);
                                down_baudrate = 57600;
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_24M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_24M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 22118400) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_19200, 3);
                                break; 
                            case 38400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_38400, 3);
                                break;
                            case 57600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_57600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_57600, 3);
                                break;
                            case 115200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_115200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_115200, 3); 
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_22M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_22M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 16000000) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_19200, 3);
                                break;
                            case 38400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_38400, 3);
                                break;
                            case 57600 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_57600, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_57600, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_38400, 3);
                                down_baudrate = 38400;
                                break;
                            case 115200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_115200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_115200, 3); 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_38400, 3);
                                down_baudrate = 38400;
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_16M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_16M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 12000000) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_19200, 3);
                                break;
                            case 38400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_38400, 3);
                                break;
                            case 57600 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_57600, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_57600, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_38400, 3);
                                down_baudrate = 38400;
                                break;
                            case 115200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_115200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_115200, 3); 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_38400, 3);
                                down_baudrate = 38400;
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_12M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_12M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 11059200) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_19200, 3);
                                break; 
                            case 38400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_38400, 3);
                                break;
                            case 57600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_57600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_57600, 3);
                                break;
                            case 115200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_115200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_115200, 3); 
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_11M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_11M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 8000000) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_19200, 3);
                                break; 
                            case 38400 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_38400, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_38400, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_19200, 3);
                                down_baudrate = 19200;
                                break;
                            case 57600 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_57600, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_57600, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_19200, 3);
                                down_baudrate = 19200;
                                break;
                            case 115200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_115200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_115200, 3); 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_19200, 3);
                                down_baudrate = 19200;
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_8M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_8M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 6000000) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_9600, 3);
                                break;
                            case 19200 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_19200, 3);
                                break; 
                            case 38400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_38400, 3);
                                break;
                            case 57600 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_57600, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_57600, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_19200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_19200, 3);
                                down_baudrate = 38400;
                                break;
                            case 115200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_115200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_115200, 3); 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_38400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_38400, 3);
                                down_baudrate = 38400;
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_6M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else if (target_hz == 4000000) {
                        switch (down_baudrate) {
                            case 1200 : 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_1200, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_6M_1200, 3);
                                break;
                            case 2400 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_2400, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_2400, 3);
                                break;
                            case 4800 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_4800, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_4800, 3);
                                break;
                            case 9600 :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_9600, 3);
                                break;
                            case 19200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_19200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_19200, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_9600, 3);
                                down_baudrate = 9600;
                                break; 
                            case 38400 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_38400, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_38400, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_9600, 3);
                                down_baudrate = 9600;
                                break;
                            case 57600 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_57600, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_57600, 3);
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_9600, 3);
                                down_baudrate = 9600;
                                break;
                            case 115200 :
                                //memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_115200, 3);
                                //memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_115200, 3); 
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_9600, 3);
                                down_baudrate = 9600;
                                break;
                            default :
                                memcpy(&package_data_baudrate_chk[2], baudrate_chk_4M_9600, 3);
                                memcpy(&package_data_baudrate_chg[2], baudrate_chg_4M_9600, 3);
                                down_baudrate = 9600; 
                                break;
                        }
                    }
                    else {
                        memset(&package_data_baudrate_chk[2], 0, 3);
                        memset(&package_data_baudrate_chg[2], 0, 3);
                        down_baudrate = 9600;
                    }

                    package_data_baudrate_chk[5] = 0x87;
                    if (target_hz > 0) package_data_baudrate_chk[5] = 0x83;
                    if (target_hz > 5000000) package_data_baudrate_chk[5] = 0x82;
                    if (target_hz > 10000000) package_data_baudrate_chk[5] = 0x81;
                    if (target_hz > 20000000) package_data_baudrate_chk[5] = 0x80;

                    l = 32 * down_baudrate;
                    l = target_hz / l;
                    l += 0.5;
                    k = 65536 - (long)l;
                    //k = 65536 - (target_hz / 32) / down_baudrate;
                    package_data_baudrate_chk[0] = (unsigned char)(k >> 8);
                    package_data_baudrate_chk[1] = (unsigned char)k;

                    package_data_baudrate_chg[0] = (unsigned char)(k >> 8);
                    package_data_baudrate_chg[1] = (unsigned char)k;

                    printf_msg("<<<<< Received target_info:", buf_msg, rec_count_msg);
                    printf("\ntarget_hz is %ld\n\n", target_hz);                    

                    ret = INFO_MSG;
                    memcpy(target_info, buf_msg, rec_count_msg);
                    rec_count_msg = 0;
                }
                break;
            }
            default : {
                
                break;
            }
        
        
        }
    }

    return ret;
}

/*
***********************************************************
* treat_receive_normal_msg
* treat the message(not target infomation) received 
* from uart
* buf   : point to the buf used to save the message 
*         received from the uart 
* len   : the length of the message in buf
* return: the type of the message, such as 
*             DATA_MSG :data message
*             ACK_MSG  :ack message
*             ERR_MSG  :error message
*             END_MSG  :programm is complete
*             RDY_MSG  :ready to down load
*             CHGB_MSG :change the baud rate
*             CHKB_MSG :check the baud rate
***********************************************************
*/
unsigned char treat_receive_normal_msg(unsigned char *buf, int len)
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    static   unsigned char length = 0x06;

    unsigned ret = NULL_MSG;

    while (i < len) {
//printf("----------0x%02X------------\n", buf[i]);
        buf_msg [rec_count_msg ++] = buf [i ++];
        if (rec_count_msg == 2) { // head
            //i = memcmp(buf_msg, package_head, 2);
            if ((buf_msg[0] != package_head[0]) || (buf_msg[1] != package_head[1])) {
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                length = 0x06;
            } 
        } 
        else if (rec_count_msg == 3) { // receive flag
            if (buf_msg[rec_count_msg - 1] !=  package_recflag) {
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                length = 0x06;
            }
        } 
        else if (rec_count_msg == 5) { // length
            if (buf_msg[rec_count_msg - 1] <  0x06) {
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                length = 0x06;
            } else {
                length = buf_msg[rec_count_msg - 1];
            }
        } 
        else if (rec_count_msg == 6) { // type
            if (buf_msg[rec_count_msg - 1] != DATA_MSG && buf_msg[rec_count_msg - 1] != ACK_MSG  && \
                buf_msg[rec_count_msg - 1] != ERR_MSG  && buf_msg[rec_count_msg - 1] != END_MSG  && \
                buf_msg[rec_count_msg - 1] != RDY_MSG  && buf_msg[rec_count_msg - 1] != CHGB_MSG && \
                buf_msg[rec_count_msg - 1] != CHKB_MSG ) {
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                length = 0x06;
            } 
        } 
        else if ((rec_count_msg >= 7) && (rec_count_msg < (length + 1))) { // data
            databuf_msg[rec_count_msg - 7] = buf_msg[rec_count_msg - 1];
            datalen_msg = rec_count_msg - 6;

        } 
        else if ((rec_count_msg - 1)== length) { // check
            k = 0;
            for (j = 0; j < length; j ++) {
                k += buf_msg[j];
            }
            k ++;
            if (buf_msg[rec_count_msg - 1] != (unsigned char)k) {
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                memset(databuf_msg, 0, datalen_msg);
                datalen_msg = 0;
                length = 0x06;
            } 
        }
        else if (rec_count_msg == (length + 2)) { // end
            if (buf_msg[rec_count_msg - 1] !=  package_end) {
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                length = 0x06;
                memset(databuf_msg, 0, datalen_msg);
                datalen_msg = 0;
            } else {
                ret = buf_msg[5];
                
                printf_msg("<<<<< Receive MSG:", buf_msg, rec_count_msg);
                printf("\n");
                 
                memset(buf_msg, 0, rec_count_msg);
                rec_count_msg = 0;
                length = 0x06;
                break;
            }
        }

    }
 
    return ret;
}

/*
***********************************************************
* treat_receive_msg
* treat the msg received
* buf    : message in which
* len    : the message's length
* return : the type of the message should be sent followed
***********************************************************
*/
unsigned char treat_receive_msg(unsigned char *buf, int len)
{
    unsigned int i = 0;
    unsigned char rec_msg_type = NULL_MSG;
    unsigned char ret = NULL_MSG;    

    if (step_msg == NULL_STEP) {
        rec_msg_type = treat_receive_target_info_msg(buf, len); 
    } else {
        rec_msg_type = treat_receive_normal_msg(buf, len);
    }
    
    switch  (rec_msg_type) {
    case NULL_MSG : {
//        ret = START_MSG;
        break;
    }
    case INFO_MSG : { // receive targeg infomation package
        if (step_msg == NULL_STEP) {
            step_msg = INFO_STEP;
            // send check burdrate package
            ret = CHKB_MSG;  
        }
        break;
    }
    case DATA_MSG : { // receive data package
        ret = NULL_MSG;
        break;                
    }

    case ACK_MSG : {
        if (step_msg == CHGB_STEP) {
            step_msg = UNKNOW_STEP;
            UNKNOW_STEP_COUNT = 0;
            // send ack package
            ret = ACK_MSG;
        } else if (step_msg == UNKNOW_STEP) {
            if (UNKNOW_STEP_COUNT >= (5 - 2)) {
                UNKNOW_STEP_COUNT = 0;
                if (TEST_LOOP_ACK) {
                    // send ack package to loop (can not program the IC)
                    ret = ACK_MSG;
                } else {
                    step_msg = RDY_STEP;
                    // send ready package
                    ret = RDY_MSG;
                }

            } else {
               UNKNOW_STEP_COUNT ++;
               // send ack package
               ret = ACK_MSG;
            }
        } else if (step_msg == RDY_STEP) {
            step_msg = LSTDATA_STEP;
            // send last bank of data package
            ret = DATA_MSG;
        } else if (step_msg == LSTDATA_STEP) {
            step_msg = FSTDATA_STEP;
            // send first bank of data package
            ret = DATA_MSG;
            if ((fileoffset + 0x80) >= filelen) { // send banks of data package is end
                step_msg = END_STEP;
                fileoffset = 0;
                // send end package
                ret = END_MSG;
            }
        } else if (step_msg == FSTDATA_STEP) {
            // send follow bank of data package
            ret = DATA_MSG;
            if ((fileoffset + 0x80) >= filelen) { // send banks of data package is end
                step_msg = END_STEP;
                fileoffset = 0;
                // send end package
                ret = END_MSG;
            }
        } else if (step_msg == END_STEP) {
            // what else can be send ?
            ret = NULL_MSG;
        } 
        break;               
    }
    case ERR_MSG : {
        ret = NULL_MSG;
        break;
    }
    case END_MSG : {
        ret = NULL_MSG;
        break;          
    }
    case RDY_MSG : {
        ret = NULL_MSG;
        break;               
    }
    case CHGB_MSG : {
        if (step_msg == CHKB_STEP) {
            step_msg = CHGB_STEP;
            // send ack package
            ret = ACK_MSG;
        }
        break;
    }
    case CHKB_MSG : {
        if (step_msg == INFO_STEP) {
            step_msg = CHKB_STEP;
            // send change burdrate package
            ret = CHGB_MSG;
        } 
        break;
    }
    default : 
        break;    
    }   

    return ret;
}

/*
***********************************************************
* package_send_msg
* package the message which would be sent to uart
* outbuf : point to the buf used to save the packaged message
* outlen : the max length of the outbuf
* inbuf  : point to the buf which saved the data of the 
*          message which should be sent to the uart
* inlen  : the length of the data packaged 
* t_msg  : type of the message which should be sent
* return : the length of the packaged message
***********************************************************
*/
int package_send_msg(unsigned char *outbuf, int outlen, unsigned char *inbuf, int inlen, unsigned char t_msg)
{
    int len = 0;
    int i = 0;
    int j = 0;
    if ((inlen + 0x06 +0x02) > outlen) return 0;
    if (outbuf == NULL) return 0;

    memset(outbuf, 0, outlen);
    memcpy(outbuf, package_head, 2);
    len += 2;
    outbuf[len] = package_senflag;
    len += 2;
    outbuf[len ++] = inlen + 0x06;
    outbuf[len ++] = t_msg;
    if (inlen > 0) {
        memcpy(&outbuf[len], inbuf, inlen);
        len += inlen;
    }
    j = 0;
    for (i = 0; i < len; i ++) {
        j += outbuf[i];
    }
    outbuf[len ++] = j + 1;
    outbuf[len ++] = package_end;


    return len;
}

/*
***********************************************************
* treat_send_msg
* treat the message to be send
* buf    : the buf should be send by uart
* buflen : the max length of the buf
* t_msg  : which message should be sent ?
* return : how many bytes should be sent ?
***********************************************************
*/
int treat_send_msg(unsigned char *buf, int buflen, unsigned char t_msg)
{
    int ret = 0;
    unsigned char tempbuf[512];
    int i = 0;

    if (buf == NULL) return 0;

    switch (t_msg) {
    case NULL_MSG : {
        break;                 
    }
    case START_MSG : {
        buf[0] = 0x7F;
        ret = 1;
        break;                 
    }
    case CHKB_MSG : {
        ret = package_send_msg(buf, buflen, package_data_baudrate_chk, 6, CHKB_MSG); 
        break;        
    }
    case CHGB_MSG : {
        ret = package_send_msg(buf, buflen, package_data_baudrate_chg, 5, CHGB_MSG);
        break;                
    }
    case ACK_MSG : {
        ret = package_send_msg(buf, buflen, 0, 0, ACK_MSG);
        break;               
    }
    case RDY_MSG : {
        ret = package_send_msg(buf, buflen, package_data_rdy, 1, RDY_MSG);               
    }
    case DATA_MSG : {
        if (step_msg == LSTDATA_STEP) {
            i = filelen % 0x80;
            if (!i) {
                i = 0x80;
            }
            memset(tempbuf, 0, 512);
            tempbuf[2] = (unsigned char)((filelen - i) >> 8);
            tempbuf[3] = (unsigned char)(filelen - i);
            tempbuf[5] = 0x80;
            memcpy(&tempbuf[6], &filebuf[filelen - i], i);
            i += 6;   
            ret = package_send_msg(buf, buflen, tempbuf, 0x86, DATA_MSG);
        } else if (step_msg == FSTDATA_STEP) {
            if ((fileoffset + 0x80) < filelen) {
                memset(tempbuf, 0, 512);
                tempbuf[2] = (unsigned char)(fileoffset >> 8);
                tempbuf[3] = (unsigned char)(fileoffset);
                tempbuf[5] = 0x80;
                memcpy(&tempbuf[6], &filebuf[fileoffset], 0x80);
                fileoffset += 0x80;
                ret = package_send_msg(buf, buflen, tempbuf, 0x86, DATA_MSG);
            } 
        }
        break;                
    }
    case END_MSG : {
        ret = package_send_msg(buf, buflen, 0, 0, END_MSG);
        break;               
    }
    default :
        break;
    
    }

    return ret;
}

// the main function
/*
***********************************************************
* treat_msg
* treat the message received from target system
* outbuf : point to the buf used to save the message 
*          which will be sent to the target
* outlen : max size of the outbuf
* inbuf  : point to the buf used to save the message 
*          received from the target
* inlen  : the size of the message received from the target
* plen   : point to the "unsigned int" buf used to save 
           the size of the message sent to the target
* return : 0 : nothing to be done for uart
*          1 : there is a message to be sent to the target
*          2 : the baudrate of the uart should be changed
*          3 : data msg will be sent
*          4 : the uart should be closed
***********************************************************
*/
unsigned char treat_msg(unsigned char *outbuf, int outlen, unsigned char *inbuf, int inlen, int *plen)
{
    unsigned char sendmsgtype = NULL_MSG;
    unsigned char ret = 0;
//static int i = 0;
    *plen = 0;   
 
    if (step_msg == END_STEP) {
        step_msg = NULL_STEP;
        return 4;
    }

    sendmsgtype = treat_receive_msg(inbuf, inlen);
    *plen = treat_send_msg(outbuf, (outlen), sendmsgtype);
    if (*plen > 0) ret = 1;
    
    if ((sendmsgtype == CHKB_MSG) || (sendmsgtype == CHGB_MSG)) ret = 2;  
    if (sendmsgtype == DATA_MSG) ret = 3;

    return ret;
}

/*
***********************************************************
* input_file_msg
* get the file to be down load into the MPU
* filename : point to the buf in which saved the filename
* return   : the length of the file
***********************************************************
*/
int input_file_msg(unsigned char *filename)
{    
    int fd = -1;
    int nread = 0;
    int i = 0;
    
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("opne bin file error\n");
        return -1;
    } else {
        nread = read(fd, filebuf, FILEBUF_LENGTH);
        if (nread == 0) {
            printf("file is empty\n");
        } else if (nread < 0) {
            printf("file read error\n");
            return -1;
        } else if (nread == FILEBUF_LENGTH) {
            printf("file is too big\n");
        } else {
            filelen = nread;
        }
        close(fd);
        for (i = 0; i < filelen; i ++) {
            if (!(i % 16)) printf("\n%04X: ", i);
            printf("%02x ", filebuf[i]);    
        }
        printf("\n");
        return filelen;
    }

}
