
#ifndef MINISERIAL_H
#define MINISERIAL_H


#if defined(__CYGWIN__)
    // This is Cygwin special case
    #include <sys/time.h>
#endif

// Include for windows
#if defined (_WIN32) || defined (_WIN64)
#if defined(__GNUC__)
    // This is MinGW special case
    #include <sys/time.h>
#else
    // sys/time.h does not exist on "actual" Windows
    #define NO_POSIX_TIME
#endif
    // Accessing to the serial port under Windows
    #include <windows.h>
#endif

// Include for Linux
#if defined (__linux__) || defined(__APPLE__)
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/shm.h>
    #include <termios.h>
    #include <string.h>
    #include <iostream>
    #include <sys/time.h>
    // File control definitions
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif


#include <string>
#include <vector>
#include <stdio.h>

#include "miniserialBase.h"





#define COM1     "COM1"
#define COM2     "COM2"
#define COM3     "COM3"
#define COM4     "COM4"
#define COM5     "COM5"
#define COM6     "COM6"
#define COM7     "COM7"
#define COM8     "COM8"
#define COM9     "COM9"
#define COM10    "COM10"
#define COM11    "COM11"
#define COM12    "COM12"
#define COM13    "COM13"
#define COM14    "COM14"
#define COM15    "COM15"
#define COM16    "COM16"
#define COM17    "COM17"
#define COM18    "COM18"
#define COM19    "COM19"
#define COM20    "COM20"
#define COM21    "COM21"


/**
 * number of serial data bits
 */
enum SerialDataBits {
    SERIAL_DATABITS_5, /**< 5 databits */
    SERIAL_DATABITS_6, /**< 6 databits */
    SERIAL_DATABITS_7, /**< 7 databits */
    SERIAL_DATABITS_8,  /**< 8 databits */
    SERIAL_DATABITS_16,  /**< 16 databits */
};

/**
 * number of serial stop bits
 */
enum SerialStopBits {
    SERIAL_STOPBITS_1, /**< 1 stop bit */
    SERIAL_STOPBITS_1_5, /**< 1.5 stop bits */
    SERIAL_STOPBITS_2, /**< 2 stop bits */
};

/**
 * type of serial parity bits
 */
enum SerialParity {
    SERIAL_PARITY_NONE, /**< no parity bit */
    SERIAL_PARITY_EVEN, /**< even parity bit */
    SERIAL_PARITY_ODD, /**< odd parity bit */
    SERIAL_PARITY_MARK, /**< mark parity */
    SERIAL_PARITY_SPACE /**< space bit */
};

using namespace std;


class MiniSerial : public MiniSerialBase{
	
public:       
        
		 MiniSerial();
		
		  ~MiniSerial(){};
        
		 MiniSerial(string Device, long BaudRate, long DataSize, char ParityType, float NStopBits);
        
      /*return 0 if success*/
        long Open(void);
      
      /**/
        void Close();
      
        string getComPort(void);
      
      /*return success flag*/
        int Read(char data[],int len);
      
      /*return read char if success*/
        char ReadChar(bool& success);
      
      /*return success flag*/
        bool WriteChar(char ch);
      
      /*write null terminated string and return success flag*/
        bool Write(char *data,long n);
        
		  bool IsOpened();
        
		  std::string GetPort();
		
        /*return success flag*/
          bool SetRTS(bool value);
        
        /*return success flag*/
          bool SetDTR(bool value);
        
          bool GetCTS(bool& success);
          bool GetDSR(bool& success);
          bool GetRI(bool& success);
          bool GetCD(bool& success);
		
		  void SetPort(string Port);
          void SetBaudRate(long baudrate);
          void SetDataSize(long nbits);
          void SetParity(char p);
          void SetStopBits(float nbits);
		
		  long GetBaudRate();
		  long GetDataSize();
		  char GetParity();
      float GetStopBits();
		  int getNbrOfBytes(void);
	
	private:
		char rxchar;
        string port;
        long baud;
        long dsize;
        char parity;
        float stopbits;
        string temp;
        /*handle*/

#if defined (_WIN32) || defined( _WIN64)
        HANDLE hComm; 
        OVERLAPPED osReader;
        OVERLAPPED osWrite;
        BOOL fWaitingOnRead;
        COMMTIMEOUTS timeouts_ori;
#endif
#if defined (__linux__) || defined(__APPLE__)
    int             fd;
#endif



        static void Delay(unsigned long ms);
};







#endif