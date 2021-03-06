/*********************************************************
*文件名称：AT24C04.c
*摘    要：EEPROM AT24C04驱动程序
*          
*当前版本：1.0 全部函数经过测试可以正常使用。修改写入等待时间为5ms。
*作    者：赵阳
*开始日期：2012年5月17日
*完成日期：2012年5月17日
*
*注意：使用连续写入函数Write_NByte时，不能超过页边界。
***********************************************************/

#include "AT24C04.h"
#include "Public.h"     /*函数WriteSInt32、ReadSInt32、WriteUInt16、ReadUInt16使用*/


/*******************************************
函数名称：start
功    能：完成IIC的起始条件操作
参    数：无
返回值  ：无
********************************************/
void start(void)
{
  IniRAM;       //初始化I/O
  SCL_H;
  SDA_H;
  AT24C04delayus(25);
  SDA_L;
  AT24C04delayus(25);
  SCL_L;
  AT24C04delayus(25);
}
/*******************************************
函数名称：stop
功    能：完成IIC的终止条件操作
参    数：无
返回值  ：无
********************************************/
void stop(void)
{
  SDA_L;
  AT24C04delayus(25);
  SCL_H;
  AT24C04delayus(25);
  SDA_H;
  AT24C04delayus(25);   

}
/*******************************************
函数名称：mack
功    能：完成IIC的主机应答操作
参    数：无
返回值  ：无
********************************************/
void mack(void)
{
  SDA_L;
  _NOP(); _NOP();
  SCL_H;
  AT24C04delayus(25);
  SCL_L;
  _NOP();_NOP();
  SDA_H;     
  AT24C04delayus(25);
}
/*******************************************
函数名称：mnack
功    能：完成IIC的主机无应答操作
参    数：无
返回值  ：无
********************************************/
void mnack(void)
{
  SDA_H;
  _NOP(); _NOP();
  SCL_H;
  AT24C04delayus(25);
  SCL_L;
  _NOP(); _NOP();
  SDA_L;   
  AT24C04delayus(25);       
}

/**********检查应答信号函数******************/
/*如果返回值为1则证明有应答信号，反之没有*/
/*******************************************
函数名称：check
功    能：检查从机的应答操作
参    数：无
返回值  ：从机是否有应答：1--有，0--无
********************************************/
unsigned char check(void)
{
  unsigned char slaveack;
    
  SDA_H;
  _NOP(); _NOP();
  SCL_H;
  _NOP(); _NOP();
  SDA_in;
  _NOP(); _NOP();
  slaveack = SDA_val;   //读入SDA数值
  SCL_L;
  AT24C04delayus(25);
  SDA_out;
  if(slaveack)    return 0;
  else            return 1;
}
/*******************************************
函数名称：write1
功    能：向IIC总线发送一个1
参    数：无
返回值  ：无
********************************************/
void write1(void)
{
  SDA_H;
  AT24C04delayus(25);
  SCL_H;
  AT24C04delayus(25);
  SCL_L;				
  AT24C04delayus(25);
}
/*******************************************
函数名称：write0
功    能：向IIC总线发送一个0
参    数：无
返回值  ：无
********************************************/
void write0(void)
{
  SDA_L;
  AT24C04delayus(25);
  SCL_H;
  AT24C04delayus(25);			 
  SCL_L;				
  AT24C04delayus(25);
}
/*******************************************
函数名称：write1byte
功    能：向IIC总线发送一个字节的数据
参    数：wdata--发送的数据
返回值  ：无
********************************************/
void write1byte(unsigned char wdata)
{
  unsigned char i;    
  for(i = 8;i > 0;i--)
  {
    if(wdata & 0x80)	write1();
    else 		write0();
    wdata <<= 1;
  }                  	
  SDA_H;
  _NOP();   
}
/*******************************************
函数名称：writeNbyte
功    能：向IIC总线发送N个字节的数据
参    数：outbuffer--指向发送数据存放首地址
          的指针
          n--数据的个数
返回值  ：发送是否成功的标志：1--成功，0--失败
********************************************/
unsigned char writeNbyte(unsigned char * outbuffer,unsigned char n)
{
  unsigned char i;

  for(i = 0;i < n;i++)
  {
    write1byte(* outbuffer);
    if(check())      
    {
      outbuffer++;			
    }
    else 
    {
      stop();
      return 0;
    }
  }

  stop();
  return 1;			
}
/*******************************************
函数名称：read1byte
功    能：从IIC总线读取一个字节
参    数：无
返回值  ：读取的数据
********************************************/
unsigned char read1byte(void)
{
  unsigned char  rdata = 0x00,i;
  unsigned char flag;

  for(i = 0;i < 8;i++)
  {
    SDA_H;
    AT24C04delayus(25);
    SCL_H;
    SDA_in;
    AT24C04delayus(25);
    flag = SDA_val;
    rdata <<= 1;
    
    if(flag)	rdata |= 0x01;
    
    SDA_out;
    SCL_L;
    AT24C04delayus(25);
  }
  return rdata;
}
/*******************************************
函数名称：readNbyte
功    能：从IIC总线读取N个字节的数据
参    数：inbuffer--读取后数据存放的首地址
          n--数据的个数
返回值  ：无
********************************************/
void readNbyte(unsigned char * inbuffer,unsigned char n)
{
  unsigned char i;
    
  for(i = 0;i < n;i++)
  {
    inbuffer[i] = read1byte();
    
    if(i < (n-1))	mack();
    else            mnack();
  }
  stop();
}

/*******************************************
函数名称：Write_1Byte
功    能：向EEPROM中写入1个字节的数据
参    数：Wdata--写入的数据
          dataaddress--数据的写入地址
返回值  ：写入结果：1--成功，0--失败
********************************************/
unsigned char Write_1Byte(unsigned char wdata,unsigned char dataaddress)
{
  start();
  write1byte(deviceaddress);
  if(check()) 
  write1byte(dataaddress);
  else 
  return 0;
  if(check()) 
  { write1byte(wdata);AT24C04delayus(25);}
   else 
  return 0;
  if(check())	    stop();
  else            return 0;
    
  AT24C04delayms(5);                                   //等待EEPROM完成内部写入
  return 1; 
}

/*******************************************
函数名称：Write_NByte
功    能：向EEPROM中写入N个字节的数据
参    数：outbuf--指向写入数据存放首地址的指针
          n--数据个数，最大不能超过8，由页地址
             决定其最大长度
          dataaddress--数据写入的首地址
返回值  ：写入结果：1--成功，0--失败
********************************************/
unsigned char Write_NByte(unsigned char *outbuf,unsigned char n,unsigned char dataaddress)
{
	unsigned char  flag; 
	start();
	write1byte(deviceaddress);                                //写入器件地址
	if(check() == 1) 
        write1byte(dataaddress);                                //写入数据字地址
	else 
        return 0;
	if(check()) 
        flag=writeNbyte(outbuf,n);
	else 
        return 0;
        AT24C04delayms(5);                                   //等待EEPROM完成内部写入
	if(flag)    return 1;
	else        return 0;
}

/*******************************************
函数名称：Read_1Byte_currentaddress
功    能：从EEPROM的当前地址读取1个字节的数据
参    数：无
返回值  ：读取的数据
********************************************/
/*unsigned char Read_1Byte_currentaddress(void)
{
	unsigned char temp;
    
	start();
	write1byte((deviceaddress|0x01));
	if(check())	
        temp = read1byte();
	else 
        return 0;
	mnack();
	stop();
	return temp;
}*/
/*******************************************
函数名称：Read_NByte_currentaddress
功    能：从EEPROM的当前地址读取N个字节的数据
参    数：readbuf--指向保存数据地址的指针
          n--读取数据的个数
返回值  ：读取结果：1--成功，0--失败
********************************************/
/*unsigned char Read_NByte_currentaddress(unsigned char * readbuf,unsigned char n)
{   
      start();
      write1byte((deviceaddress|0x01));
      if(check()) 
        readNbyte(readbuf,n);
	else 
        return 0;    
	return  1;
}*/

/*******************************************
函数名称：Read_1Byte_Randomaddress
功    能：从EEPROM的指定地址读取1个字节的数据
参    数：dataaddress--数据读取的地址
返回值  ：读取的数据
********************************************/
unsigned char Read_1Byte_Randomaddress(unsigned char dataaddress)
{
	unsigned char temp;
	start();
	write1byte(deviceaddress);
	if(check())	
        write1byte(dataaddress);
	else 
        return 0;
	if(check()) 
	{
	    start();
	    write1byte((deviceaddress|0x01));
            AT24C04delayus(25);
	}
	else    
        return 0;
	if(check())	
        temp = read1byte();
	else 
        return 0;

	mnack();
	stop();
	return temp;
}
/*******************************************
函数名称：Read_NByte_Randomaddress
功    能：从EEPROM的指定地址读取N个字节的数据
参    数：readbuf--指向保存数据地址的指针
          n--读取数据的个数
          dataaddress--数据读取的首地址
返回值  ：读取结果：1--成功，0--失败
********************************************/
unsigned char Read_NByte_Randomaddress(unsigned char * readbuf,unsigned char n,unsigned char dataaddress)
{
	start();
	write1byte(deviceaddress);
	if(check())	
        write1byte(dataaddress);
	else 
        return 0;
	if(check()) 
	{
	    start();
	    write1byte(deviceaddress|0x01);
	}
	else 
        return 0;
	if(check()) 
        readNbyte(readbuf,n);
	else 
        return 0;	
	return 1;	
}

/**************************************************************
函数名称：unsigned char WriteInt(unsigned char dataaddr,unsigned int num)
函数介绍：向存储器写入一个16bit整型数
输入参数：unsigned char dataaddr--->存储器起始写入地址
          unsigned int num--------->需要写入的16bit数据
输出参数：无
返回值  ：写入成功返回1，失败返回0.
说明：存储时高位在前，低位在后。
例：存入数据：0X1234   存储起始地址为 0X02
    存储器地址      存储内容
    0X02            0X12
    0X03            0X34
*************************************************************/
unsigned char WriteUInt16(unsigned int num,unsigned char dataaddr)
{
  Uint16 u16b={0};                       //16bit整数结构
  
  u16b.num = num;
 
  if(Write_1Byte(u16b.bit16.nHigh,dataaddr)==0)   //写入高8bit
    return 0;
    
  if(Write_1Byte(u16b.bit16.nLow,dataaddr+1)==0) //写入低8bit
    return 0;
  
  return 1;     //返回写入成功
}

/**************************************************************
函数名称：unsigned int ReadInt(unsigned char addr)
函数介绍：从存储器读出一个16bit整型数
输入参数：unsigned char dataaddr--->数据保存地址
          
输出参数：无
返回值  ：读出的数据
*************************************************************/
unsigned int ReadUInt16(unsigned char addr)
{
  unsigned char data8[2]={0,0};
  Uint16 u16b={0};                       //16bit整数结构

  Read_NByte_Randomaddress(data8,2,addr);//连续读出2byte数据
        
  u16b.bit16.nLow = data8[1];           //低8bit
  u16b.bit16.nHigh = data8[0];          //高8bit
  
  return u16b.num;
}


/**************************************************************
函数名称：unsigned char WriteSInt32(unsigned char dataaddr,long num)
函数介绍：向存储器写入一个32bit有符号整型数
输入参数：unsigned char dataaddr--->存储器起始写入地址
          long num--------->需要写入的32bit数据
输出参数：无
返回值  ：写入成功返回1，失败返回0.
说明：存储时高位在前，低位在后。
例：存入数据：0X12345678   存储起始地址为 0X02
    存储器地址      存储内容
    0X02            0X12
    0X03            0X34
    0X04            0X56 
    0X05            0X78
注意：写入数据时注意存储器页边界。
*************************************************************/
unsigned char WriteSInt32(long num,unsigned char dataaddr)
{
  Slong32 s32={0};                       //32bit整数结构
  unsigned char temp[4]={0};
  
  s32.num = num;
    
  temp[0] = s32.bit32.nHigh.bit16.nHigh;
  temp[1] = s32.bit32.nHigh.bit16.nLow;
  temp[2] = s32.bit32.nLow.bit16.nHigh;
  temp[3] = s32.bit32.nLow.bit16.nLow;
  
  return Write_NByte(temp,4,dataaddr);
 /*为了提高写入效率使用的是连续写函数，写入数据时注意不要越界，
  否则无法完整写入4byte数据。*/
}

/**************************************************************
函数名称：unsigned char WriteSInt32(unsigned char dataaddr,long num)
函数介绍：从存储器读出一个32bit有符号整型数
输入参数：unsigned char dataaddr--->数据保存地址
输出参数：无
返回值  ：读出的32bit有符号整型数
说明：存储时高位在前，低位在后。
例：存入数据：0X12345678   存储起始地址为 0X02
    存储器地址      存储内容
    0X02            0X12
    0X03            0X34
    0X04            0X56 
    0X05            0X78
*************************************************************/
signed long ReadSInt32(unsigned char addr)
{
  unsigned char data8[4]={0,0};
  Slong32 s32={0};                       //32bit整数结构

  Read_NByte_Randomaddress(data8,4,addr);//连续读出4byte数据
        
  s32.bit32.nLow.bit16.nLow = data8[3];           //低16bit中的低8bit
  s32.bit32.nLow.bit16.nHigh = data8[2];          //低16bit中的高8bit
  s32.bit32.nHigh.bit16.nLow = data8[1];           //高16bit中的低8bit
  s32.bit32.nHigh.bit16.nHigh = data8[0];          //高16bit中的高8bit
   
  return s32.num;
}

unsigned char AT24C04TEST(unsigned char testadd)
{
  unsigned int x=0XAAAA;
  unsigned int y=0;
  
  WriteUInt16(x,testadd);
  y = ReadUInt16(testadd);
  
  if(x==y)
    return 1;
  else 
    return 0;
}

