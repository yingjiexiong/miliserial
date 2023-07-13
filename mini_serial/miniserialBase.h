#ifndef  MINISERIALBASE_H
#define  MINISERIALBASE_H

#include <windows.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <string>


class MiniSerialBase{
	
public:
	
		virtual ~MiniSerialBase(){};
        //virtual wxSerialPortBase(std::string Device, long BaudRate, long DataSize, char ParityType, float NStopBits);
        
        
        /*return 0 if success*/
        virtual long Open(void) = 0;
        
        /**/
        virtual void Close() = 0;
        
        virtual std::string getComPort(void) = 0;
        
        /*return success flag*/
        virtual int Read(char data[],int len) = 0;
        
        /*return read char if success*/
        virtual char ReadChar(bool& success) = 0;
        
        /*return success flag*/
        virtual bool WriteChar(char ch) = 0;
        
        /*write null terminated string and return success flag*/
        virtual bool Write(char *data,long n) = 0;
        
        /*return success flag*/
        virtual bool SetRTS(bool value) = 0;
        
        /*return success flag*/
        virtual bool SetDTR(bool value) = 0;
        
        virtual bool GetCTS(bool& success) = 0;
        virtual bool GetDSR(bool& success) = 0;
        virtual bool GetRI(bool& success) = 0;
        virtual bool GetCD(bool& success) = 0;
        virtual bool IsOpened() = 0;
        virtual void SetPort(std::string Port) = 0;
        virtual std::string GetPort() = 0;
        virtual void SetBaudRate(long baudrate) = 0;
        virtual long GetBaudRate() = 0;
        virtual void SetDataSize(long nbits) = 0;
        virtual long GetDataSize() = 0;
        virtual void SetParity(char p) = 0;
        virtual char GetParity() = 0;
        virtual void SetStopBits(float nbits) = 0;
        virtual float GetStopBits() = 0;
		virtual int getNbrOfBytes(void) = 0;	
};



#endif