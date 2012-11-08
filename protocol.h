extern unsigned long target_hz;
extern unsigned int target_baudrate;
extern unsigned int down_baudrate;

extern unsigned char f_double_speed;
extern unsigned char f_half_gain;
extern unsigned char f_p1_down;

extern void init_msg();
extern int input_file_msg(unsigned char *filename);
extern unsigned char treat_msg(unsigned char *outbuf, int outlen, unsigned char *inbuf, int inlen, int *plen);
