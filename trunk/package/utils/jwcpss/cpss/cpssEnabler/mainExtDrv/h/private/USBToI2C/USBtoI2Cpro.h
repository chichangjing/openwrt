/*   ========================================================================*/
/*   =================    USB-to-I2Cpro  DLL interface   ====================*/
/*   =================          www.i2ctools.com      =======================*/
/*   ===========      Copyright (c) 2004-2005 SB Solutions        ===========*/
/*   ========================================================================*/
/*   ===========                                                  ===========*/
/*   ===========            http://www.i2ctools.com               ===========*/
/*   ========================================================================*/

#if defined(WIN32)

#define usb __stdcall
#else
#define usb __pascal far
#endif /* defined(WIN32)*/

/*#ifdef __cplusplus
     {
#endif*/

 short int     __stdcall Enable3VOutputPower(short int);
 short int     __stdcall Enable5VOutputPower(short int);
 unsigned char __stdcall GetFirmwareRevision(void);
 unsigned char __stdcall GetHardwareInfo(unsigned char *HardwareData);
 int           __stdcall GetI2CFrequency(void);
 int           __stdcall GetNumberOfDevices(void);
 int           __stdcall GetSerialNumbers(int *SerialNumbers);
 unsigned char __stdcall I2CRead(unsigned char address, short int nBytes, unsigned char *ReadData, short int SendStop);
 unsigned char __stdcall I2C10ReadArray(short int address, unsigned char subaddress, short int nBytes, unsigned char *ReadData);
 unsigned char __stdcall I2C10WriteArray(short int address, unsigned char subaddress, short int nBytes, unsigned char *ReadData);
 unsigned char __stdcall I2CReadArray(unsigned char address, unsigned char subaddress, short int nBytes, unsigned char *ReadData);
 unsigned char __stdcall I2CReadByte(unsigned char address, unsigned char subaddress, unsigned char *ReadData);
 unsigned char __stdcall I2CReceiveByte(unsigned char address, unsigned char *ReadData); 
 unsigned char __stdcall I2CSendByte(unsigned char address, unsigned char SendData);
 unsigned char __stdcall I2CWrite(unsigned char address, short int nBytes, unsigned char *WriteData, short int SendStop);
 unsigned char __stdcall I2CWriteArray(unsigned char address, unsigned char subaddress, short int nBytes, unsigned char *WriteData);
 unsigned char __stdcall I2CWriteByte(unsigned char address,unsigned char subaddress,unsigned char dataByte);
 unsigned char __stdcall I2CWriteRepWrite(unsigned char address0,short int nBytes0,unsigned char *WriteData0,unsigned char address1,short int nBytes1,unsigned char *WriteData1);
 int           __stdcall SelectBySerialNumber(int SerialNumber);
 int           __stdcall SetI2CFrequency(int frequency);
 void          __stdcall ShutdownProcedure(void); 
 unsigned char __stdcall SPI_Configure(unsigned char SPI_Mode);
 unsigned char __stdcall SPI_Generic(short int nBytes,unsigned char *WriteData, unsigned char *ReadData); 
 int           __stdcall SPI_SetFrequency(int frequency);
 unsigned char __stdcall SPI_Write(short int nBytes,unsigned char *SPI_Data);
 unsigned char __stdcall SPI_WriteRead(short int nBytesWrite,short int nBytesRead,unsigned char *WriteData,unsigned char *ReadData);
 unsigned char __stdcall SPI_WriteWithOC(short int nBytes,unsigned char *WriteData);
 int           __stdcall Read_IO(void);
 int           __stdcall Write_IO(int OutputState);
 int           __stdcall Disable_SPI(void);
 int           __stdcall Enable_SPI(void);
 int           __stdcall GPIO_SPI_Write(unsigned char GPIOData);
 int           __stdcall GPIO_SPI_Read(void);
 int           __stdcall Enable_I2C(void);
 int           __stdcall Disable_I2C(void);
 int           __stdcall GPIO_I2C_Write(unsigned char GPIOData);
 int           __stdcall GPIO_I2C_Read(void); 

/*#ifdef __cplusplus
	}
#endif*/
