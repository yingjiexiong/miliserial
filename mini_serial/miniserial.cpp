
//#define DLL_PROTOCO_API __declspec(dllexport)

#include "miniserial.h"
#include "miniserialBase.h"



extern "C" __declspec(dllexport) MiniSerial* Export(){
	
	return new MiniSerial;
	
}


#define READ_TIMEOUT 10      // milliseconds


	void MiniSerial::Delay(unsigned long ms){

#if defined (_WIN32) || defined( _WIN64)
        Sleep(ms);
#endif


    }
    
    MiniSerial::MiniSerial(){
            hComm = INVALID_HANDLE_VALUE;
            port = COM1;
            
            SetBaudRate(9600);
            SetDataSize(8);
            SetParity('N');
            SetStopBits(1);
        
    }
    
     MiniSerial::MiniSerial(string Device, long BaudRate,long DataSize,char ParityType,float NStopBits){
            hComm = INVALID_HANDLE_VALUE;
         
            port = Device;
         
            SetBaudRate(BaudRate);
            SetDataSize(DataSize);
            SetParity(ParityType);
            SetStopBits(NStopBits);
         
     }
    
     void MiniSerial::SetPort(string Device){
            port = Device;
    }

     string MiniSerial::GetPort(){
        return port;
    }

     void MiniSerial::SetDataSize(long nbits){
        if ((nbits < 5) || (nbits > 8)) nbits = 8;
        dsize=nbits;
    }

     long MiniSerial::GetDataSize(){
        return dsize;
    }

     void MiniSerial::SetParity(char p){
        if ((p != 'N') && (p != 'E') && (p != 'O')){
            if ((p != 'M') && (p != 'S'))
                    p = 'N';
        }
        parity = p;
    }

     char MiniSerial::GetParity(){
        return parity;
    }

     void MiniSerial::SetStopBits(float nbits){
        if (nbits >= 2) stopbits = 2;
        else if(nbits >= 1.5) stopbits = 1.5;

    }

     float MiniSerial::GetStopBits(){
        return stopbits;
    }
    
     void MiniSerial::SetBaudRate(long baudrate){
        
                 if (baudrate < 300)    baud = CBR_110;
            else if (baudrate < 600)    baud = CBR_300;
            else if (baudrate < 1200)   baud = CBR_600;
            else if (baudrate < 2400)   baud = CBR_1200;
            else if (baudrate < 4800)   baud = CBR_2400;
            else if (baudrate < 9600)   baud = CBR_4800;
            else if (baudrate < 14400)  baud = CBR_9600;
            else if (baudrate < 19200)  baud = CBR_14400;
            else if (baudrate < 38400)  baud = CBR_19200;
            else if (baudrate < 57600)  baud = CBR_38400;
            else if (baudrate < 115200) baud = CBR_57600;
            else if (baudrate < 128000) baud = CBR_115200;
            else if (baudrate < 256000) baud = CBR_128000;
            else baud = CBR_256000;
    }
    
     long MiniSerial::GetBaudRate(){
            return baud;
    }
    
     long MiniSerial::Open(){
        
        if (IsOpened()) return 0;
        
        #ifdef UNICODE
            wstring wtext(port.begin(),port.end());
        #else
            string wtext = port;
        #endif
        
        hComm = CreateFile(
            
            /*device name */
            wtext.c_str(),
            
            /*access mode: read and write*/
            GENERIC_READ | GENERIC_WRITE,
            
            /*share mode : no share*/
            0,
            
            /*security*/
            NULL,
            
            /* device must be exist or the creating will be fault*/
            OPEN_EXISTING,
            
            /*overlapping mode*/
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            
            NULL);
        
        /*if create fault then release resource and return*/
        if (hComm == INVALID_HANDLE_VALUE) {return 1;}
        
        if (PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == 0) {return 2;}//purge

        //get initial state
        DCB dcbOri;
        bool fSuccess;
        fSuccess = GetCommState(hComm, &dcbOri);
        if (!fSuccess) {return 3;}

        DCB dcb1 = dcbOri;

        dcb1.BaudRate = baud;

             if (parity == 'E') dcb1.Parity = EVENPARITY;
        else if (parity == 'O') dcb1.Parity = ODDPARITY;
        else if (parity == 'M') dcb1.Parity = MARKPARITY;
        else if (parity == 'S') dcb1.Parity = SPACEPARITY;
        else dcb1.Parity = NOPARITY;

        dcb1.ByteSize = (BYTE)dsize;

        if(stopbits==2) dcb1.StopBits = TWOSTOPBITS;
        else if (stopbits == 1.5) dcb1.StopBits = ONE5STOPBITS;
        else dcb1.StopBits = ONESTOPBIT;

        dcb1.fOutxCtsFlow = false;
        dcb1.fOutxDsrFlow = false;
        dcb1.fOutX = false;
        dcb1.fDtrControl = DTR_CONTROL_DISABLE;
        dcb1.fRtsControl = RTS_CONTROL_DISABLE;
        fSuccess = SetCommState(hComm, &dcb1);
        this->Delay(60);
        if (!fSuccess) {return 4;}
        
        fSuccess = GetCommState(hComm, &dcb1);
        if (!fSuccess) {return 5;}
        
        SetupComm(hComm,2048,2048);
        PurgeComm(hComm,PURGE_RXCLEAR|PURGE_TXCLEAR);
        
        osReader = { 0 };// Create the overlapped event.
        // Must be closed before exiting to avoid a handle leak.
        osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (osReader.hEvent == NULL) {return 6;}// Error creating overlapped event; abort.
        fWaitingOnRead = FALSE;

        osWrite = { 0 };
        osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (osWrite.hEvent == NULL) {return 7;}

        if (!GetCommTimeouts(hComm, &timeouts_ori)) { return 8; } // Error getting time-outs.
        COMMTIMEOUTS timeouts;
        timeouts.ReadIntervalTimeout = 20;
        timeouts.ReadTotalTimeoutMultiplier = 15;
        timeouts.ReadTotalTimeoutConstant = 100;
        timeouts.WriteTotalTimeoutMultiplier = 15;
        timeouts.WriteTotalTimeoutConstant = 100;
        if (!SetCommTimeouts(hComm, &timeouts)) { return 9;} // Error setting time-outs.
        return 10;
    }
    
     void MiniSerial::Close(){
        if (IsOpened())
        {
            SetCommTimeouts(hComm, &timeouts_ori);
            CloseHandle(osReader.hEvent);
            CloseHandle(osWrite.hEvent);
            CloseHandle(hComm);//close comm port
            hComm = INVALID_HANDLE_VALUE;
        }
    }
    
     bool MiniSerial::IsOpened(){
        
        if(hComm == INVALID_HANDLE_VALUE) return false;
        else return true;
    }
    
     bool MiniSerial::Write(char *data,long n){
        
        if (!IsOpened()){
            return false;
        }
        
        PurgeComm(hComm,PURGE_TXCLEAR);
        
        BOOL fRes;
        DWORD dwWritten;
        if (n < 0) n = 0;
        else if(n > 1024) n = 1024;
        
        // Issue write.
        if (!WriteFile(hComm, data, n, &dwWritten, &osWrite)){
            // WriteFile failed, but it isn't delayed. Report error and abort.
            if (GetLastError() != ERROR_IO_PENDING) {fRes = FALSE;}
            else {// Write is pending.
                if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, TRUE)) fRes = FALSE;
                else fRes = TRUE;// Write operation completed successfully.
            }
        }
        else fRes = TRUE;// WriteFile completed immediately.
        return fRes;
    }
    
     bool MiniSerial::WriteChar(char ch){
        
        char s[2];
        s[0]=ch;
        s[1]=0;//null terminated
        return Write(s,2);
        
    }
    
     int MiniSerial::Read(char data[],int len){
		
        int success = false;
        if (!IsOpened()) {return 0;}

        DWORD dwRead;
        DWORD length=len;
        //the creation of the overlapped read operation
        if (!fWaitingOnRead){
            // Issue read operation.
            if (!ReadFile(hComm, data, length, &dwRead, &osReader)){
                if (GetLastError() != ERROR_IO_PENDING) { /*Error*/}
                else { fWaitingOnRead = TRUE; /*Waiting*/}
            }
            else {if(dwRead==length)success = true;}//success
        }


        //detection of the completion of an overlapped read operation
        DWORD dwRes;
        if (fWaitingOnRead){
            dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
            switch (dwRes)
            {
            // Read completed.
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(hComm, &osReader, &dwRead, FALSE)) {/*Error*/ }
                else {
                    if (dwRead == length)success = true;
                    fWaitingOnRead = FALSE;
                // Reset flag so that another opertion can be issued.
                }// Read completed successfully.
                break;

            case WAIT_TIMEOUT:
                // Operation isn't complete yet.
                break;

            default:
                // Error in the WaitForSingleObject;
                break;
            }
        }
        return success;		
		
    }
    
     char MiniSerial::ReadChar(bool& success){
        
        success = false;
        if (!IsOpened()) {return 0;}

        DWORD dwRead;
        DWORD length=1;
        BYTE* data = (BYTE*)(&rxchar);
        //the creation of the overlapped read operation
        if (!fWaitingOnRead) {
            // Issue read operation.
            if (!ReadFile(hComm, data, length, &dwRead, &osReader)){
                if (GetLastError() != ERROR_IO_PENDING) { /*Error*/}
                else { fWaitingOnRead = TRUE; /*Waiting*/}
            }
            else {if(dwRead==length) success = true;}//success
        }


        //detection of the completion of an overlapped read operation
        DWORD dwRes;
        if (fWaitingOnRead) {
            dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
            switch (dwRes)
            {
            // Read completed.
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(hComm, &osReader, &dwRead, FALSE)) {/*Error*/ }
                else {
                    if (dwRead == length) success = true;
                    fWaitingOnRead = FALSE;
                // Reset flag so that another opertion can be issued.
                }// Read completed successfully.
                break;

            case WAIT_TIMEOUT:
                // Operation isn't complete yet.
                break;

            default:
                // Error in the WaitForSingleObject;
                break;
            }
        }
        return rxchar;
    }
    
	 int MiniSerial::getNbrOfBytes(void){
		struct _COMSTAT status;
		int             n;
		unsigned long   etat;
		n = 0;
		if (hComm!=INVALID_HANDLE_VALUE){
			ClearCommError(hComm, &etat, &status);
			n = status.cbInQue;
		}
		return(n);
	}
	
     bool MiniSerial::SetRTS(bool value){
        
        bool r = false;
        if (IsOpened()) {
            if (value) {
                if (EscapeCommFunction(hComm, SETRTS)) r = true;
            }
            else {
                if (EscapeCommFunction(hComm, CLRRTS)) r = true;
            }
        }
        return r;
    }
    
     bool MiniSerial::SetDTR(bool value){
        
        bool r = false;
        if (IsOpened()) {
            if (value) {
                if (EscapeCommFunction(hComm, SETDTR)) r = true;
            }
            else {
                if (EscapeCommFunction(hComm, CLRDTR)) r = true;
            }
        }
        return r;
    }
    
     bool MiniSerial::GetCTS(bool& success){
        
        success = false;
        bool r = false;
        if (IsOpened()) {
            DWORD dwModemStatus;
            if (GetCommModemStatus(hComm, &dwModemStatus)){
                r = MS_CTS_ON & dwModemStatus;
                success = true;
            }
        }
        return r;
    }
    
     bool MiniSerial::GetDSR(bool& success){
        
        success = false;
        bool r = false;
        if (IsOpened()) {
            DWORD dwModemStatus;
            if (GetCommModemStatus(hComm, &dwModemStatus)) {
                r = MS_DSR_ON & dwModemStatus;
                success = true;
            }
        }
        return r;
    }
    
     bool MiniSerial::GetRI(bool& success){
        
        success = false;
        bool r = false;
        if (IsOpened()) {
            DWORD dwModemStatus;
            if (GetCommModemStatus(hComm, &dwModemStatus)) {
                r = MS_RING_ON & dwModemStatus;
                success = true;
            }
        }
        return r;
    }

     bool MiniSerial::GetCD(bool& success){
        
        
        success = false;
        bool r = false;
        if (IsOpened()) {
            DWORD dwModemStatus;
            if (GetCommModemStatus(hComm, &dwModemStatus)) {
                r = MS_RLSD_ON & dwModemStatus;
                success = true;
            }
        }
        return r;
    }
    
string MiniSerial::getComPort(void){
		
	HKEY hKey;
	DWORD result;
	
  temp = "";
	result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm", 0, KEY_READ, &hKey); //read register table
	
	if(ERROR_SUCCESS == result){
		
		TCHAR valueName[255];
		TCHAR portname[255];
		DWORD valueNameSize,dataSize;
		DWORD i = 0;


		while(true){
			
			valueNameSize = dataSize = sizeof(valueName) / sizeof(TCHAR);
			
			result = ::RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, NULL, (PUCHAR)portname, &dataSize);

			if(ERROR_NO_MORE_ITEMS == result ){
				temp.append("s");
				break;
				
			}
			else{
				
				string str(portname);
        temp.append(str+"n");
				i++;
				
			}
			
		}
		
		RegCloseKey(hKey);  

	}
	

	return temp;
}
    
