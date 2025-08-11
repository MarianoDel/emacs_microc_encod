//--------------------------------------------------
// Tests for serial port UART with termios.h lib
//--------------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "crc_enc.h"

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */
#include <pthread.h>
#include <string.h>


// Module Functions for testing ------------------------------------------------
int OpenSerialPort (int * file_descp);

void * SerialInput (void * arg);
void * KeyboardInput (void * arg);

void Print_Keyboard_Options (void);
void Print_Hex_Buffer (int len, unsigned char *buff, int grouped);
static unsigned char CharsHex2Int (char ch);
int Check_Chars_Qtty (int len, unsigned char *buff);
void Process_Encoder_Buffer(int bytes_read, unsigned char * buff_rx);


// Globals ---------------------------------------------------------------------
int main_quit = 0;

// -- envios de usart1 al puerto serie
int usart_data_len = 0;
int usart_data_send = 0;
unsigned short usart_addr = 0;
unsigned short usart_bytes = 0;

char usart_data_buff_rx [256] = { '\0' };
char usart_data_buff_tx [256] = { '\0' };
int usart_data_receiv = 0;


// Module Functions ------------------------------------------------------------
void main(void)
{
    int fd;/*File Descriptor*/
    pthread_t p1, p2;

    if (OpenSerialPort (&fd))
    {
        printf("\nOpening serial port: OK\n");

        int rc1;
        rc1 = pthread_create(&p1, NULL, SerialInput, (void *)&fd);
        if (rc1){
            printf("ERROR; return code from pthread_create() is %d\n", rc1);
            _exit(-1);
        }

        int rc2;
        rc2 = pthread_create(&p2, NULL, KeyboardInput, (void *)&rc2);
        if (rc2){
            printf("ERROR; return code from pthread_create() is %d\n", rc2);
            _exit(-1);
        }
        
    }
    else
    {
        printf("\nError in Opening serial port!!\n");
        return;
    }

    while (!main_quit)
    {
        if (usart_data_send == 1)
        {
	    char text [255];
	    strcpy(text, "Hello from serial\r\n");
	    int len = strlen(text);
	    len = write(fd, text, len);
	    printf("wrote %d bytes over UART\n", len);
            usart_data_send = 0;
        }
        else if (usart_data_send == 2)
        {
	    // crc ok
	    unsigned char text [10] = {0x5a, 0xa5, 0x07, 0x03, 0x20, 0x50, 0x00, 0x02, 0xea, 0x10};
	    // crc nok
	    // unsigned char text [10] = {0x5a, 0xa5, 0x07, 0x03, 0x20, 0x50, 0x00, 0x02, 0xea, 0x11};
	    int len = write(fd, text, 10);
	    printf("wrote %d bytes over UART\n", len);
            usart_data_send = 0;
        }
        else if (usart_data_send == 3)
        {
	    // calc crc
	    // unsigned char text [5] = {0x03, 0x20, 0x50, 0x00, 0x02};
	    unsigned char text [5] = {0x03, 0x02, 0x20, 0x00, 0x02};	    
	    unsigned short crc;
	    unsigned short crc_swap;	    
	    crc = CRC16(text, 5);
	    crc_swap = CRC16_Swap (crc);
	    printf("crc calc: %04x crc_swap: %04x\n", crc, crc_swap);
            usart_data_send = 0;
        }
        else if (usart_data_send == 4)
        {
	    unsigned char a;
	    unsigned short crc;
	    unsigned short crc_swap;	    
	    unsigned char buff_snd [100];

	    buff_snd[0] = 0x5a;
	    buff_snd[1] = 0xa5;
	    buff_snd[2] = 7;
	    buff_snd[3] = 0x03;
	    a = (usart_addr >> 8);
	    buff_snd[4] = a;
	    a = usart_addr & 0x00ff;
	    buff_snd[5] = a;
	    a = (usart_bytes >> 8);
	    buff_snd[6] = a;
	    a = usart_bytes & 0x00ff;
	    buff_snd[7] = a;

	    crc = CRC16(buff_snd + 3, 5);
	    crc_swap = CRC16_Swap (crc);

	    a = (crc_swap >> 8);
	    buff_snd[8] = a;
	    a = crc_swap & 0x00ff;
	    buff_snd[9] = a;
	    
	    printf("crc calc: %04x crc_swap: %04x\n", crc, crc_swap);
	    int len = write(fd, buff_snd, 10);
	    printf("wrote %d bytes over UART\n", len);
            usart_data_send = 0;
        }
        else if (usart_data_send == 5)
        {
	    unsigned char a;
	    unsigned short crc;
	    unsigned short crc_swap;	    
	    unsigned char buff_snd [100];

	    buff_snd[0] = 0x5a;
	    buff_snd[1] = 0xa5;
	    buff_snd[2] = 7;
	    buff_snd[3] = 0x10;
	    a = (usart_addr >> 8);
	    buff_snd[4] = a;
	    a = usart_addr & 0x00ff;
	    buff_snd[5] = a;
	    a = (usart_bytes >> 8);
	    buff_snd[6] = a;
	    a = usart_bytes & 0x00ff;
	    buff_snd[7] = a;

	    crc = CRC16(buff_snd + 3, 5);
	    crc_swap = CRC16_Swap (crc);

	    a = (crc_swap >> 8);
	    buff_snd[8] = a;
	    a = crc_swap & 0x00ff;
	    buff_snd[9] = a;
	    
	    printf("crc calc: %04x crc_swap: %04x\n", crc, crc_swap);
	    int len = write(fd, buff_snd, 10);
	    printf("wrote %d bytes over UART\n", len);
            usart_data_send = 0;
        }

        
        // give back control to OS
        usleep(1000);    //sleep 1ms

    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    
}


// Module Auxialiary Functions -------------------------------------------------
int OpenSerialPort (int * file_descp)
{
    int fd;    //local fd
    
    printf("\n +-------------------------------------+");
    printf("\n |        Serial Port Open Write       |");
    printf("\n +-------------------------------------+");

    /*------------------------------- Opening the Serial Port -------------------------------*/

    /* Change /dev/ttyUSB0 to the one corresponding to your system */

    // ttyUSB0 is the FT232 based USB2SERIAL Converter
    fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY);
    // fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
    
    /* O_RDWR Read/Write access to serial port           */
    /* O_NOCTTY - No terminal will control the process   */
    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
    /* -the status of DCD line,Open() returns immediatly */                                        
									
    if(fd == -1)						/* Error Checking */
    {
        printf("\n  Error! in Opening ttyUSB0  ");
        return 0;
    }
    else
        printf("\n  ttyUSB0 Opened Successfully ");

	
    /*---------- Setting the Attributes of the serial port using termios structure --------- */
    struct termios options;    // for serial port settings
    // Setup serial port
    // int baud_print = 115200;
    int baud_print = 57600;
    
    // options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_cflag = B57600 | CS8 | CLOCAL | CREAD;    
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    // Apply new settings, flush the buffer
    tcflush(fd, TCIFLUSH);
    
    if(tcsetattr(fd, TCSANOW, &options) != 0) /* Set the attributes to the termios structure*/
    {
        printf("\n  ERROR ! in Setting attributes");
        return 0;
    }
    else
        printf("\n  BaudRate = %d StopBits = 1 Parity = none\n", baud_print);

    
    *file_descp = fd;
    return 1;
}


int pckt_flag = 0;
void * SerialInput (void * arg)
{
    int fd = *(int *) arg;
    int bytes_read = 0;
    char buff_rx [256] = { '\0' };


    printf("SerialInput thread fd: %d\n", fd);
    while (!main_quit)
    {
        memset(buff_rx, '\0', sizeof(buff_rx));
        bytes_read = read(fd, buff_rx, sizeof(buff_rx));        

        if (bytes_read > 0)
        {
	    usleep(100000);    // wait 100ms to get the entire buffer
	    // len += read(fd, *(text + len), 255);
	    bytes_read += read(fd, buff_rx + bytes_read, sizeof(buff_rx));	    
            pckt_flag = 1;

	    printf("received %d binary bytes\n", bytes_read);
	    Print_Hex_Buffer(bytes_read, buff_rx, 2);
	    int clen = Check_Chars_Qtty(bytes_read, buff_rx);
	    if ((clen == (bytes_read - 1)) ||
		(clen == (bytes_read - 2)))
	    {
		printf("chars: %d str: %s\n", clen, buff_rx);
	    }

	    Process_Encoder_Buffer(bytes_read, buff_rx);
	}

	// give back control to OS
        usleep(2000);
    }

    pthread_exit(NULL);
    
}


void * KeyboardInput (void * arg)
{
    int loop = 1;
    char key;

    Print_Keyboard_Options ();    
    while(loop)
    {
        scanf(" %c", &key);
        if (key == 'q')
        {
            loop = 0;
            main_quit = 1;
            printf("ending simulation\n");
        }
        else if (key == '1')
        {
            usart_data_send = 1;
        }
        else if (key == '2')
        {
            usart_data_send = 2;         
        }
        else if (key == '3')
        {
            usart_data_send = 3;         
        }
        else if (key == '4')
        {
	    
	    printf("    address in hex (2 bytes): ");
	    scanf(" %x", &usart_addr);
	    printf("    how many bytes: ");
	    scanf(" %d", &usart_bytes);
	    printf("read from addr: %04x %d bytes\n", usart_addr, usart_bytes);
            usart_data_send = 4;         
        }
        else if (key == '5')
        {
	    
	    printf("    address in hex (2 bytes): ");
	    scanf(" %x", &usart_addr);
	    printf("    value in hex (2 bytes): ");
	    scanf(" %x", &usart_bytes);
	    printf("write to addr: %04x value: %04x\n", usart_addr, usart_bytes);
            usart_data_send = 5;         
        }
        else if (key == 'm')
        {
	    Print_Keyboard_Options ();
        }
        else
        {
            // send_char = key;
            // data_write = 1;
        }
    }
    
    pthread_exit(NULL);
}


void Print_Keyboard_Options (void)
{
    printf("\n  Menu Options...\n");
    printf("  send test string [1]\n");
    printf("  send test buffer [2]\n");
    printf("  calc crc in buff [3]\n");
    printf("  read address     [4]\n");
    printf("  write address    [5]\n");
    printf("  show this menu [m]\n");
    printf("  quit app [q]\n");    
    
}


void Print_Hex_Buffer (int len, unsigned char *buff, int grouped)
{
    int gb = grouped;

    for (int i = 0; i < len; i++)
    {
	printf("%02x", *(buff + i));
	if (grouped)
	{
	    if (gb - 1)
		gb--;
	    else
	    {
		printf(" ");
		gb = grouped;
	    }
	}
    }
    
    printf("\n");

}


// check how many chars are in the buffer to first non char
int Check_Chars_Qtty (int len, unsigned char *buff)
{
    int qtty = 0;
    
    for (int i = 0; i < len; i++)
    {
	if ((*(buff + i) >= ' ') &&
	    (*(buff + i) <= '~'))
	    qtty++;
    }

    return qtty;

}

// char in hex to int value
static unsigned char CharsHex2Int (char ch)
{
    unsigned char a = 0;
    
    if ((ch >= '0') && (ch <= '9'))
       a = ch - '0';
    if ((ch >= 'A') && (ch <= 'F'))
       a = ch - 'A' + 10;
    if ((ch >= 'a') && (ch <= 'f'))
       a = ch - 'a' + 10;

    return a;
}


#define CMD_READ    1
#define CMD_WRITE    2
void Process_Encoder_Buffer(int bytes_read, unsigned char * buff_rx)
{
    unsigned char cmd = 0;
    // disarm packet
    if ((buff_rx [0] != 0x5a) &&
	(buff_rx [1] != 0xa5))
    {
	printf("header: nok! length: %d ", buff_rx [2]);
	return;
    }

    printf("header: ok length: %d ", buff_rx [2]);

    if (buff_rx [3] == 0x10)
    {
	cmd = CMD_WRITE;
	printf("cmd: write ");
    }
    else if (buff_rx [3] == 0x03)
    {
	cmd = CMD_READ;
	printf("cmd: read ");
    }
    else
    {
	printf("cmd: unknow\n");
	return;
    }
	
    if (cmd == CMD_WRITE)
    {
	if ((buff_rx [4] == 0xff) &&
	    (buff_rx [5] == 0x4c) &&
	    (buff_rx [6] == 0x30))
	{
	    printf("crc ok ");
	}
	else
	{
	    printf("crc: nok! ");
	}

	if ((bytes_read - 3) == buff_rx[2])
	    printf("len: ok read: %d buffr: %d\n", bytes_read, buff_rx[2]);
	else
	    printf("len: nok! read: %d buffr: %d\n", bytes_read, buff_rx[2]);

	return;
    }
    else
    {
	int r = 0;
	// cmd == CMD_READ
	if ((buff_rx [4] == 0xff) &&
	    (buff_rx [5] == 0x41) &&
	    (buff_rx [6] == 0x00))
	{
	    printf("crc ok ");
	}
	else
	{
	    printf("crc: nok! ");
	    r = 1;
	}

	if ((bytes_read - 3) == buff_rx[2])
	{
	    printf("len: ok no further data!\n");
	    r = 1;
	}
	else if ((bytes_read - 3) > buff_rx[2])
	    printf("len: further data read: %d buffr: %d\n", bytes_read, buff_rx[2]);
	else
	{
	    printf("len: nok! read: %d buffr: %d\n", bytes_read, buff_rx[2]);
	    r = 1;
	}

	if (r)
	    return;
	
    }

    if (bytes_read < 15)
    {
	printf("length: nok! len: %d\n", bytes_read);
	return;
    }
	
    if ((buff_rx [7] != 0x5a) &&
	(buff_rx [8] != 0xa5))
    {
	printf("header: nok! length: %d\n", buff_rx [9]);
	return;
    }
    
    if (buff_rx [10] != 0x03)
    {
	printf("cmd: nok! length: %d\n", buff_rx [9]);
	return;	
    }

    printf("addr read: %02x%02x\n", buff_rx[11], buff_rx[12]);
    printf("bytes read: %02x%02x B: %d\n", buff_rx[13], buff_rx[14], buff_rx[14]);

    int base = 0;
    for (int i = 0; i < buff_rx[14]; i++)
    {
	base = i * 2 + 15;
	printf("value%d: %02x%02x\n", i + 1, buff_rx[base], buff_rx[base + 1]);
    }
    
}

//--- end of file ---//


