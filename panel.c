//  *****************************************************************************
//
//              PANEL Module provides display panel output function.
//              1.      NOW, it supports WIBBOND NEC, PANASONIC, ROHM and NJU VFD controller.
//              2.      ALL controlling procedures about these controllers are put
//                      in "VFD_CTRL.H" including :
//                              PANEL_Initial(), _Hardware_CTRL(), PANEL_KeyScan(),
//                              _InByteVFD(), _OutByteVFD, and _LightVFD_LED().
//              3.      If the VFD controller is same as any one above, users need
//                      to revise PANEL.H only. If users want to develop a new VFD
//                      controller PANEL.H & VFD_CTRL.H both need to be revised.
//  -----------------------------------------------------------------------------
//              Version 2.00  For Panel ( gerenal version )
//  -----------------------------------------------------------------------------
//              Copyright (c) 2000, Cheertek Corp. All rights reserved.
//
//  *****************************************************************************

#include "winav.h"
#include "ctkav.h"
#include "ctkav_platform.h"
// wyc0.50-909, make infofilter.h before Mmanager.h because of Mmanager.h need to reference some information defined in infofilter.h
#include "infofilter.h"
#include "Mmanager.h"
#include "cc.h"
#include "osd.h"
#include "input.h"
#include "hal.h"
#include "navigate.h"
#include "digest.h"
#include "utl.h"
#include "srcfilter.h"

#ifdef CT950_STYLE
#include "VFD_CTRL.h" //CoCo2.38
#endif //

#ifndef NO_PANEL

#include "hio.h"
#include "panel.h"
#include "VFD_CTRL.h"

#ifdef USE_VFD_CTRL 

BYTE _bPanelTemp1, _bPanelTemp2;

extern BIT __btPlaying;
extern BYTE __bSetupMode;
extern BYTE    __bMediaSelectUI;
extern BYTE __bThumbnailMode; //Jeff 20050908 add for thubmnail mode show ---
// LLY.160, some important thing for modification:
// [1] Rename _bOSDMode to _bOSDAppMode, and OSD_MODE_XXX to OSD_APPMODE_XXX

// Micky2.78+one_cpu
// Declare variables only when PANEL ias defined.
BYTE    _bDisplayRam[VFD_GRID_ARRAY];
BYTE    _bLettTimeMessage[VFD_LETTNUM];
BYTE    _bLettArray_O[VFD_LETTNUM];
BYTE    *_bLettArray= 0;
BYTE    _bSetupMenu = 0;
BYTE    i=0,j=0,shift_letter=0,Message_length=0,COL_Flash=0;
BYTE    __bMessage_Now;
DWORD   __dwTimerCount_PANEL_SHIFT;
BYTE    PREV_MSG;
BYTE    _bPKEY_REPEAT=0;


#ifdef  LIGHT_LED
BYTE    IDATA _bLedData = 0;
#endif

extern PTREE_INFO  __pMMVolumeDirTree;
extern BYTE __bAngle;                   // For ANGLE showing flag
extern BIT __btInputProgram;    // For program showing flag
extern BYTE __bACHN;                    // For 5.1 CHANNEL showing flag
extern STATUS gcShowStatus;

char    code aNumMap[]
= { NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9};

void    _OutByteVFD(BYTE bByte);
void    _Hardware_CTRL(void);
void    _ShowTime ( WORD wTime);        // ** TCH0.50;
void    _LETT_To_DisplayRam(void);
void    _Updata_System_Status(void);
void    _Show_Program_Message(void);
void    _Show_GotoTime_Message(void);
void    _Show_Search_Message(void);

#elif defined(SW_EXPAND_GPIO)
static DWORD _dwPrevTimePanelKey = 0;
#endif // #ifdef USE_VFD_CTRL
#endif // #ifndef NO_PANEL


void PANEL_Initial(void)
{
#ifndef NO_PANEL

#ifndef USE_VFD_CTRL
    // LLY2.13, nothing need to do
    // Since, initial system will call API to initial expand GPIO
#if 0
    REG_PLAT_MPLL_CONTROL |= 0x01E00000; // set 21 ~ 24th bits as 1 to enable panel key detect and output
    REG_PLAT_SYSTEM_CONFIGURATION1 |= 0x00008000;   // set 15th bit as 1 to enable panel scan 

    REG_PLAT_PANEL_CLK = 0x00FF0000;
    REG_PLAT_PANEL_CFG = 0x00000102;  // 0000 0001 0000 0110

    //    REG_PLAT_PROC1_2ND_INT_MASK_DISABLE = INT_PROC1_2ND_PANEL_IR;   // This is for Panel intrrupt
    //    REG_PLAT_PANEL_INTCTRL = 0xFFFF0000;  // Edge trigger
    //    REG_PLAT_PANEL_INTCTRL = 0x00000000;    // Level trigger

    REG_PLAT_PANEL_INTCTRL = 0x0000FFFF;    // Mask panel interrupt
    REG_PLAT_PANEL_INT = 0x0;

    REG_PLAT_PANEL_DISP0 = 0;
#endif  // #if 0

#else

    __icVFDnow=0x01; 

    HAL_I2CSrc_Ctrl(MUTEX_ACTION_LOCK);

    _VFD_CS_HI;
    Message_length=0;
    for(_bPanelTemp1=0;_bPanelTemp1<VFD_GRID_ARRAY;_bPanelTemp1++)
        _bDisplayRam[_bPanelTemp1]=0;

    for(_bPanelTemp1=0;_bPanelTemp1<VFD_LETTNUM;_bPanelTemp1++)
        _bLettTimeMessage[_bPanelTemp1]=_bLettArray[_bPanelTemp1];

#ifndef POWERON_STANDBY
    //_LETT_To_DisplayRam();
    __btPowerDown=0;    
#endif

    _VFD_CS_LO;
    _OutByteVFD(0x87);                                    // Command 4
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(SETCOMMAND_WRITEDISPLAY);                 // Command 2
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(SET_ADDRESS);                             // Command 3
    for(_bPanelTemp2=0;_bPanelTemp2<VFD_GRID_ARRAY;_bPanelTemp2++)
        _OutByteVFD(_bDisplayRam[_bPanelTemp2]);
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(0x02);                                    // Command 1
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(0x87);                                    // Command 4
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(0x03);                                    // Command 1
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(0x8f);                                    // Command 4
    _VFD_CS_HI;
    for(_bPanelTemp1=0;_bPanelTemp1<4;_bPanelTemp1++){}

    HAL_I2CSrc_Ctrl(MUTEX_ACTION_UNLOCK);

    __icVFDnow=0x00;

    /*
#ifndef POWERON_STANDBY
HAL_WriteGPIO(GPIO_GREEN_LED_HI);
HAL_WriteGPIO(GPIO_RED_LED_LO);
#endif
*/

#endif // #define USE_VFD_CTRL
#endif  // NO_PANEL 
}

#ifdef ADC_KEY
#define ADCGLB (*(volatile int *)(0x8000407C))	
BYTE RDATA0,RDATA1;
DWORD wADCTemp;
#endif
#include "key.h"
BYTE PANEL_KeyScan(void)
{
#ifdef ENABLE_PANEL_KEYSCAN
#ifdef ADC_KEY
    	BYTE  bGetKey=0;
    	extern BYTE __bThumbnailMode;
    	extern BYTE __bSetupMode;
    	extern BYTE _bOSDNDMsg; 
    	RDATA0 = 0xFF;
    	RDATA1 = 0xFF;

    	//wADCTemp =  ADCGLB & 0xFF00FFFF;   // reset [23:16]
    	//ADCGLB = wADCTemp | 0x008e0000;   // write [23:16] 8e
    	ADCGLB = 0x00840000;
    	for (RDATA0=0xF8;RDATA0<0xFE;RDATA0++);
        
    	wADCTemp = ADCGLB;
    	RDATA0 = (BYTE) (wADCTemp >>24);

    	//wADCTemp =  ADCGLB & 0xFF00FFFF;  // reset [23:16]
    	//ADCGLB = wADCTemp | 0x00C40000;  // write [23:16] A4
	#if (DECODER_SYSTEM == DMP952A_EVAL)
   		ADCGLB = 0x00C40000;  //FS
	#endif
	#if (DECODER_SYSTEM == DMP956A_EVAL)
    	ADCGLB = 0x00E40000;  //FE
	#endif
    	for (RDATA1=0xF8;RDATA1<0xFE;RDATA1++);
        
    	//printf("wADCTemp = %lx",(wADCTemp | 0x00C40000));
    	//ADCGLB = 0x00000000;
    	//ADCGLB = 0x00C40000;
    	wADCTemp = ADCGLB;
    	RDATA1 = (BYTE) (wADCTemp >>24);
	if(RDATA0< 0xF0)
	{   
    		if (RDATA0 > 0xD0)
    		{
        		bGetKey=1;
    		}
    		else if(RDATA0 > 0x90)
    		{
    			bGetKey=2;
    		}        
    		else if(RDATA0 > 0x60)
    		{    	
    			bGetKey=3;
    		}        
    		else if(RDATA0 > 0x10)
    		{    	
    			bGetKey=4;
    		}        
    		else
    		{    	
    			bGetKey=5;       	
    		}	
	}   
	if(RDATA1< 0xF0)
	{    
    		if (RDATA1 > 0xD0)
    		{
        		bGetKey=6;
		}
		else if(RDATA1 > 0x90)
		{
		        bGetKey=7;
		}        
		else if(RDATA1 > 0x60)
		{    	
		        bGetKey=8;	
		 }        
		 else if(RDATA1 > 0x10)
		 {    	
		       bGetKey=9;	
		 }        
		 else
		 {    	
		       bGetKey=10;	           	
		 }	
	}    
	if(bGetKey)
	{
		printf("\n bGetKey=%d",bGetKey);
	}
   	return aScanMap[bGetKey];
#endif // KEY_8
#endif   // #ifdef ENABLE_PANEL_KEYSCAN
    return KEY_NO_KEY;
}


//  ***************************************************************************
//  Function    :   PANEL_Trigger
//  Abstract    :   Update the value of ( MIN:SEC ) on panel
//  Arguments   :   wTime
//  Return      :   none
//  ***************************************************************************
// LLY2.53, expand time value unit from WORD to DWORD
// Since, we will use [23:16]:hour, [15:8]: min, [7:0]: sec
//void PANEL_Trigger(WORD wTime)
void PANEL_Trigger(DWORD dwTime)
{
#ifndef NO_PANEL
#ifdef USE_VFD_CTRL
    extern BIT __btPlaying;
    extern BYTE __bSetupMode;

    if(__bSetupMode==1)    //to avoid show LOAD DISC in setup mode; LHsu.104
    {
        for (i=0;i<10;i++) _bLettArray_O[i]=0;  

        Message_length=4;
        _bLettArray = _bLettTimeMessage;
        _bLettArray_O[0] = LETT_S;
        _bLettArray_O[1] = LETT_E;
        _bLettArray_O[2] = LETT_t;
        _bLettArray_O[3] = LETT_U;    
        _bLettArray_O[4] = LETT_P;              	
    }
    else if ((__bMediaSelectUI==1)||(__bThumbnailMode))
    {
        for (i=0;i<10;i++) _bLettArray_O[i]=0;  

        Message_length=2;
        _bLettArray = _bLettTimeMessage;
        _bLettArray_O[0] = LETT_DASH;
        _bLettArray_O[1] = LETT_DASH;
        _bLettArray_O[2] = LETT_DASH;          	
    }
    else if (!(__wDiscType & CDROM_M1)||(__bMMMenuID != MM_MENU_NONE)) //Jeff 20050908 modify for Jpeg show
        PANEL_Output(__bMessage_Now,3);


    // Micky1.20, 4M flash
    /*
#ifndef NO_DIGEST
if(__btDIGEST)
{
_bLettArray = aMessageDigest;
_LETT_To_DisplayRam();
    //        _Hardware_CTRL();
    }
    else 
#endif
*/

if ((__bSetupMode==0)&&(__bMediaSelectUI==0)&& (!__bThumbnailMode))
{
    if ((__btPlaying && __dwMMJPEGPreview==false)||(__btPause))
    {
        // LLY2.53, expand the time value from WORD to DWORD unit
        // Since, we will use [23:16]:hour, [15:8]:min, [7:0]: sec
        //_ShowTime(gcShowStatus.wCurrentTime);
        _ShowTime(gcShowStatus.dwCurrentTime);
        _Updata_System_Status();
        //        _Hardware_CTRL();
    }
}

if ((Message_length>0)&&(!__btPowerDown))
{
    _bLettArray = _bLettTimeMessage;

    for (i=0;i<5;i++)
    {
        _bLettArray[i]=0;
    }

#ifdef Pannel_running
    j=shift_letter;

    if (Message_length<VFD_NUM)
    {
        for (i=0;i<VFD_NUM;i++)	
        {
            _bLettArray[i]=_bLettArray_O[i];
        } 
    }   
    else
    {   
        for (i=(VFD_NUM);i>0;i--)	 
        {
            _bLettArray[i-1]=_bLettArray_O[j];
            if (j==0) break;
            else j--;
        }
    }

    if ((OS_GetSysTimer()-COUNT_500_MSEC)>__dwTimerCount_PANEL_SHIFT)
    {
        __dwTimerCount_PANEL_SHIFT=OS_GetSysTimer();
        shift_letter++;  
        if (shift_letter>Message_length+VFD_NUM) 
        {	
            shift_letter=0;
        } 
    }     
#else
    for (i=0;i<VFD_NUM;i++)	
    {
        _bLettArray[i]=_bLettArray_O[i];
    }  
#endif  // #ifdef Pannel_running

    _LETT_To_DisplayRam();   
}
_Hardware_CTRL();        

#endif // #ifdef USE_VFD_CTRL
#endif // #ifndef NO_PANEL
}


//  ***************************************************************************
//  Function    :   PANEL_Output
//  Abstract    :   Display the information according to bMessage and wParam.
//  Arguments   :   bMessage : Specifies the behavior of panel display.
//                  wParam : some bMessage use it !!
//  Return      :   none
//  Side Effect :

//  ***************************************************************************
//#pragma NOAREGS
void PANEL_Output(BYTE bMessage, WORD wParam)
{
#ifndef NO_PANEL
#ifdef USE_VFD_CTRL 
    BYTE File_num_temp;
    WORD File_type_temp;
    __bMessage_Now=bMessage;

    if (PREV_MSG !=	bMessage)
    {
        shift_letter=0;
        PREV_MSG=bMessage;
    }	

    //    extern BYTE  __bTitle;
    //    extern WORD  __wChapter;
    //LJY1.20, support 4M flash, remove digest
    /*
#ifndef NO_DIGEST    
extern BYTE  __bTrackNO;
#endif
*/
    //    extern BYTE __bPageNO;
    //    extern BYTE __bSetupMode;
    //    extern BYTE _bOSDAppMode;

#ifdef  SHOW_LOADING_STATUS
    // don't clear all LED light
    if ( (bMessage==MSG_WAITING) && (wParam == OSD_SERVO_STABLE) )
    {
        for(_bPanelTemp1=0;_bPanelTemp1<VFD_GRID_ARRAY;_bPanelTemp1++)
            _bDisplayRam[_bPanelTemp1] = 0x00;
    }
    else
#endif
    {
        for(_bPanelTemp1=0;_bPanelTemp1<VFD_GRID_ARRAY;_bPanelTemp1++)
            _bDisplayRam[_bPanelTemp1] = 0x00;
#ifdef LIGHT_LED
        _bLedData = 0xFF;
#endif
    }
    if((__wDiscType & BOOK_SVCD)||(__wDiscType & BOOK_CVD)||(__wDiscType & BOOK_VCD))
    {
        if(__wPlayItem > 999)
        {
            for (i=0;i<10;i++)
                _bLettArray_O[i]=0;                    
            Message_length=2;
            _bLettArray = _bLettTimeMessage;
            _bLettArray_O[0] = LETT_P;
            _bLettArray_O[1] = LETT_b;
            _bLettArray_O[2] = LETT_C;                    
        }   
    }

    if((bMessage == MSG_OPEN)||(bMessage == MSG_STOP))
    {
        __bSearchMode = 0;  // System will clear __bSearchMode while do KEY_OPEN or KEY_STOP action
    }
    if(__btInputProgram)                        // Show program message:
    {
        //for DVD program Sunny 1.00b
        _Show_Program_Message();                // like: P 01  008
    }
    if(__bSearchMode)                           // DVD search two mode show
    {                                           // 1. Title Chapter Search
        _Show_Search_Message();                 // 2. GotoTime Search
    }
    //alex1.23,20040218 combine gototime , search,bookmark
    //Remvoe __btGoToTime , use bSearchMode  in Pancel Output
    /**********************************************************************/
    else if(__bSearchMode==SEARCH_MODE_TIME)              // CD/VCD/SVCD goto time show
        /**********************************************************************/
    {
        _Show_GotoTime_Message();
    }
    else
    {
        for (i=0;i<10;i++)
            _bLettArray_O[i]=0;   
        switch(bMessage)                                // Show LETTER message
        {            
            // like StOP,LOAD dISC,PLAY, OPEN
        case MSG_OPEN:                          // NO dISC
            if(wParam)
            {
                Message_length=3;
                _bLettArray = _bLettTimeMessage;
                _bLettArray_O[0] = LETT_O;
                _bLettArray_O[1] = LETT_P;
                _bLettArray_O[2] = LETT_E;
                _bLettArray_O[3] = LETT_N;                    
            }   
            break;
        case MSG_WAITING:
            Message_length=3;
            _bLettArray = _bLettTimeMessage;
            _bLettArray_O[0] = LETT_L;
            _bLettArray_O[1] = LETT_O;
            _bLettArray_O[2] = LETT_A;
            _bLettArray_O[3] = LETT_d;       
            break;
        case MSG_DISCTYPE:
            if(__wDiscType == NO_DISC_IN_DRIVE)
            {
                Message_length=6;
                _bLettArray = _bLettTimeMessage;
                _bLettArray_O[0] = LETT_N;
                _bLettArray_O[1] = LETT_O;
                _bLettArray_O[2] = 0;
                _bLettArray_O[3] = LETT_d;
                _bLettArray_O[4] = LETT_I;                
                _bLettArray_O[5] = LETT_S;                
                _bLettArray_O[6] = LETT_C;               
            }   
            break;
        case MSG_STOP:
            if(__bCountStop == 1 )
            { 
                Message_length=5;
                _bLettArray = _bLettTimeMessage;
                _bLettArray_O[0] = LETT_P;
                _bLettArray_O[1] = LETT_DASH;
                _bLettArray_O[2] = LETT_S;
                _bLettArray_O[3] = LETT_t;                
                _bLettArray_O[4] = LETT_O;                
                _bLettArray_O[5] = LETT_P; 
            } 
            else
            {
                Message_length=3;
                _bLettArray = _bLettTimeMessage;
                _bLettArray_O[0] = LETT_S;
                _bLettArray_O[1] = LETT_t;
                _bLettArray_O[2] = LETT_O;
                _bLettArray_O[3] = LETT_P;
                _bLettArray_O[4] = 0;
                _bLettArray_O[5] = 0;
            }
            break;
        case MSG_PLAYING:
            if( _bLettArray!= _bLettTimeMessage)
            {
                Message_length=3;
                _bLettArray = _bLettTimeMessage;
                _bLettArray_O[0] = LETT_P;
                _bLettArray_O[1] = LETT_L;
                _bLettArray_O[2] = LETT_A;
                _bLettArray_O[3] = LETT_Y;
            }
            break;                 
            //           case MSG_AUTOUPGRADE:
            //               _bLettArray = aMessageUpgrade;
            //               break;
            /*
#ifndef NO_DIGEST                
case MSG_DIGESTNUMBER:
_bLettArray = _bLettTimeMessage;
_bLettArray[0] = 0;
_bLettArray[1] = 0;
if(__bPageNO)
{
_bLettArray[2] = 0;
if(__wDiscType & VCD_2_0)
{
_bLettArray[3] = aNumMap[__bTrackNO-__bPageNO *9 -1];
}
else
{
_bLettArray[3] = aNumMap[__bTrackNO-__bPageNO];
}
}
else
{
if(__wDiscType & VCD_2_0)
{
_bLettArray[2] = aNumMap[(__bTrackNO -1)/10];
_bLettArray[3] = aNumMap[(__bTrackNO-1)%10];
}
else
{
_bLettArray[2] = aNumMap[__bTrackNO/10];
_bLettArray[3] = aNumMap[__bTrackNO%10];
}
}
_bLettArray[4] = 0;
break;
#endif                
*/
            }
}
/*    
      if(_bOSDAppMode == OSD_APPMODE_SETUP)
      {
      if(!_bSetupMenu)
      {
      _bSetupMenu = 1;
      if(_bLettArray == aMessageOpen)
      _bSetupMenu = 2;
      else if(_bLettArray == aMessageNoDisc)
      _bSetupMenu = 3;
      _bLettArray = aMessageSetup;
      if(_bSetupMenu > 1)
      {
      _LETT_To_DisplayRam();
      goto HARDWARE_CTRL;
      }
      }
      } OSD_APPMODE_SETUP
      */

/* 
   if(_bSetupMenu)
   {
   if(__bModeCmd == KEY_STOP)
   {
   switch(_bSetupMenu)
   {
   case 1:
   Message_length=5;
   _bLettArray = _bLettTimeMessage;
   _bLettArray_O[0] = 0;
   _bLettArray_O[1] = 0;
   _bLettArray_O[2] = LETT_S;
   _bLettArray_O[3] = LETT_t;
   _bLettArray_O[4] = LETT_O;
   _bLettArray_O[5] = LETT_P;
   break;
   case 2:
   Message_length=5;
   _bLettArray = _bLettTimeMessage;
   _bLettArray_O[0] = 0;
   _bLettArray_O[1] = 0;
   _bLettArray_O[2] = LETT_O;
   _bLettArray_O[3] = LETT_P;
   _bLettArray_O[4] = LETT_E;
   _bLettArray_O[5] = LETT_N;  
   break;
   case 3:
   Message_length=8;
   _bLettArray = _bLettTimeMessage;
   _bLettArray_O[0] = 0;
   _bLettArray_O[1] = 0;
   _bLettArray_O[2] = LETT_N;
   _bLettArray_O[3] = LETT_O;
   _bLettArray_O[4] = 0;
   _bLettArray_O[5] = LETT_d;
   _bLettArray_O[6] = LETT_I;                
   _bLettArray_O[7] = LETT_S;                
   _bLettArray_O[8] = LETT_C;
   break;
   }
   }
   _bSetupMenu = 0;
   }
   */
// Micky2.79, tune the usage of __bCountStop, don't reference DOUBLE_STOP
// Only DOUBLE_STOP is defined, the value will be 1(pre-stop)
if ((__wDiscType & CDROM_M1)&&(__bMMMenuID != MM_MENU_NONE))
{ 
    if ((__MMSelectCursor.bPosition==POS_FILE_LIST) && (__MMSelectCursor.wIndex>=__pMMVolumeDirTree[__MMSelectCursor.wDirID].bNoOfSubDir))
    {    
        Message_length=2;
        File_num_temp=__MMSelectCursor.wIndex-__pMMVolumeDirTree[__MMSelectCursor.wDirID].bNoOfSubDir+1;
        _bLettArray_O[0] = aNumMap[File_num_temp/100];
        File_num_temp = File_num_temp % 100;
        _bLettArray_O[1] = aNumMap[File_num_temp/10];
        File_num_temp = File_num_temp % 10;
        _bLettArray_O[2] = aNumMap[File_num_temp];
        File_type_temp=__pMMFileRecordStart[MM_ID_RelativeToAbsolute(__MMSelectCursor.wDirID, __MMSelectCursor.wIndex-__pMMVolumeDirTree[__MMSelectCursor.wDirID].bNoOfSubDir)].FileName.bExtID;
        if(File_type_temp == EXTNAME_MP3)
        {
            _bDisplayRam[VFD_MP3_ADDR] |= VFD_MP3;
        }    /*
                else if(File_type_temp == EXTNAME_VOB||File_type_temp == EXTNAME_AVI)
                {
                _bDisplayRam[VFD_JPEG_ADDR] |= VFD_DVD;
                }
                else if(File_type_temp == EXTNAME_MPG||File_type_temp == EXTNAME_DAT)
                {
                _bDisplayRam[VFD_JPEG_ADDR] |= VFD_VCD;
                }*/
        else if(File_type_temp == EXTNAME_JPG)
        {
            _bDisplayRam[VFD_JPEG_ADDR] |= VFD_JPEG;
        }                                           
    }
    else
    {
        if (__SF_SourceGBL[__dwSFStreamID].bSourceIndex == SOURCE_USB0_0)
        {
            for (i=0;i<10;i++)
                _bLettArray_O[i]=0;  
            Message_length=2;
            _bLettArray = _bLettTimeMessage;
            _bLettArray_O[0] = LETT_U;
            _bLettArray_O[1] = LETT_S;
            _bLettArray_O[2] = LETT_b;

        }
        else
        {
            for (i=0;i<10;i++)
                _bLettArray_O[i]=0;  
            Message_length=3;
            _bLettArray = _bLettTimeMessage;
            _bLettArray_O[0] = LETT_F;
            _bLettArray_O[1] = LETT_I;
            _bLettArray_O[2] = LETT_L;
            _bLettArray_O[3] = LETT_E;                
        }    
    }   
}    
// Micky.051 for debugging usage ...
if(bMessage==MSG_VERSION)
{
    _bLettArray = _bLettTimeMessage;
    _bLettArray[4] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[3] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[2] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[1] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[0] = aNumMap[wParam%10];
}
// Sunny add for 1.02A
if(bMessage == MSG_ERRCODE)
{
    _bLettArray = _bLettTimeMessage;
    _bLettArray[4] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[3] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[2] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[1] = aNumMap[wParam%10];
    wParam/=10;
    _bLettArray[0] = aNumMap[wParam%10];
}

//PANEL_Trigger(0);    

#endif  // #ifdef USE_VFD_CTRL 
#endif  // ifndef NO_PANEL
}


//////////////////////////// Private functions /////////////////////////

#ifndef NO_PANEL

#ifdef USE_VFD_CTRL

void _OutByteVFD(BYTE bByte)
{
    for (_bPanelTemp1 = 0; _bPanelTemp1 < 8; _bPanelTemp1++)
    {
        _VFD_SCK_LO;
        if (bByte & 0x01)
        {
            _VFD_SDA_HI;
        }
        else
        {
            _VFD_SDA_LO;
        }
        _VFD_SCK_HI;
        bByte >>= 1;
    }
}


void _Hardware_CTRL(void)
{   
    if (__btPowerDown)
    {
        _bDisplayRam[VFD_LED_ADDR] = VFD_LED0;
    }            
    else
    {
        _bDisplayRam[VFD_LED_ADDR] = VFD_LED1;
    }            

    __icVFDnow = 0x01;

    HAL_I2CSrc_Ctrl(MUTEX_ACTION_LOCK);

    _VFD_CS_LO;
    _OutByteVFD(SETCOMMAND_WRITEDISPLAY);                 // Command 2
    _VFD_CS_HI;
    for (_bPanelTemp1 = 0; _bPanelTemp1 < 4; _bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(SET_ADDRESS);                             // Command 3
    for (_bPanelTemp2 = 0; _bPanelTemp2 < VFD_GRID_ARRAY; _bPanelTemp2++)
        _OutByteVFD(_bDisplayRam[_bPanelTemp2]);
    _VFD_CS_HI;
    for (_bPanelTemp1 = 0; _bPanelTemp1 < 4; _bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(VFD_DISP_MODE);                           // Command 1
    _VFD_CS_HI;
    for (_bPanelTemp1 = 0; _bPanelTemp1 < 4; _bPanelTemp1++){}

    _VFD_CS_LO;
    _OutByteVFD(VFD_CTRL_MODE);                           // Command 4
    _VFD_CS_HI;
    for (_bPanelTemp1 = 0; _bPanelTemp1 < 4; _bPanelTemp1++){}

    HAL_I2CSrc_Ctrl(MUTEX_ACTION_UNLOCK);

    __icVFDnow=0x00;
}


//  ***************************************************************************
//  Function    :   _Updata_System_Status
//  Abstract    :   Display the information according to system status.
//  Arguments   :   noe.
//  Return      :   none
//  Side Effect :
//  ***************************************************************************
//#pragma NOAREG
void _Updata_System_Status(void)
{
    //---- Disctype DVD CD VCD MP3 SVCD ----------------------------------------
    if(__wDiscType & BOOK_DVD )
    {
#ifndef NO_DISC_MODE //CoCo2.37p
        _bDisplayRam[VFD_DVD_ADDR] |= VFD_DVD;
        if(__bSearchMode!=SEARCH_MODE_TIME)
        {
            _bDisplayRam[VFD_TITLE_ADDR] |= VFD_TITLE;
            _bDisplayRam[VFD_CHAPTER_ADDR] |= VFD_CHAPTER;
        }
#endif //#ifndef NO_DISC_MODE //CoCo2.37p
    }
    else if(__wDiscType & BOOK_CDDA)
    {
        _bDisplayRam[VFD_CD_ADDR]  = VFD_CD;
    }
    else if(__wDiscType & CDROM_M1 && (MM_GetFileType(__MMPlayingItem.wDirID, __wPlayItem)== (EXTNAME_MP3)))///||EXTNAME_MP2||EXTNAME_WMA||EXTNAME_DTS||EXTNAME_AC3)))
    {
        _bDisplayRam[VFD_MP3_ADDR] = VFD_MP3;
    }    
    /*   
         else if(__wDiscType & CDROM_M1 && (MM_GetFileType(__wPlayItem)==(EXTNAME_VOB)))///||EXTNAME_AVI)))
         {
         _bDisplayRam[VFD_JPEG_ADDR] = VFD_DVD;
         }
         else if(__wDiscType & CDROM_M1 && (MM_GetFileType(__wPlayItem)==(EXTNAME_MPG||EXTNAME_DAT)))
         {
         _bDisplayRam[VFD_JPEG_ADDR] = VFD_VCD;
         }
         */
    else if(__wDiscType & CDROM_M1 && (MM_GetFileType(__MMPlayingItem.wDirID, __wPlayItem)==EXTNAME_JPG))
    {
        _bDisplayRam[VFD_JPEG_ADDR] = VFD_JPEG;
    }        
    else if((__wDiscType & BOOK_SVCD)||(__wDiscType & BOOK_CVD))
    {
        _bDisplayRam[VFD_SVCD_ADDR]= VFD_SVCD;
        _bDisplayRam[VFD_VCD_ADDR] = VFD_VCD;
    }
    else if(__wDiscType & BOOK_VCD)
        _bDisplayRam[VFD_VCD_ADDR]= VFD_VCD;

    //---- Repeat REPEAT 1 ALL -------------------------------------------------
    if(__bRepeat != REPEAT_NONE)
    {
        _bDisplayRam[VFD_REPEAT_ADDR] |= VFD_REPEAT;
        if(__bRepeat == REPEAT_DISC)
        {
            _bDisplayRam[VFD_REP_ALL_ADDR] |= VFD_REP_ALL;
        }
        else if(__bRepeat == REPEAT_TRACK)
            _bDisplayRam[VFD_REP_1_ADDR]    |= VFD_REP_1;
    }
    //---- RepeatAB A -B -------------------------------------------------------
    if(__bRepeatAB == REPEAT_A)
    {
        _bDisplayRam[VFD_REP_A_ADDR] |= VFD_REP_A;
    }
    else if(__bRepeatAB == REPEAT_AB)
    {
        _bDisplayRam[VFD_REP_A_ADDR] |= VFD_REP_A;
        _bDisplayRam[VFD_REP_B_ADDR] |= VFD_REP_B;
    }
    //---- PBC------------------------------------------------------------------
#ifndef NO_DISC_MODE //++CoCo2.37p
    if(__btPBC)
        _bDisplayRam[VFD_PBC_ADDR] |= VFD_PBC;
#endif //#ifndef NO_DISC_MODE //++CoCo2.37p
    //---- Random RAN ----------------------------------------------------------
    if(__btRandom)
        _bDisplayRam[VFD_RAN_ADDR] |= VFD_RAN;
    //---- Program PROG ----------------------------------------------------------
    if(__btProgram)
        _bDisplayRam[VFD_PGM_ADDR] |= VFD_PGM;
    //---- Play Pause |> ||-----------------------------------------------------
    if(gcShowStatus.bPlayingMode == MSG_PAUSE)
        _bDisplayRam[VFD_PAUSE_ADDR] |= VFD_PAUSE;
    else if(gcShowStatus.bPlayingMode == MSG_PLAYING)
        _bDisplayRam[VFD_PLAY_ADDR] |= VFD_PLAY;
    //---- Audio  1L/2R --------------------------------------------------------
    //if(gcShowStatus.bAudioChannel==AUDIO_STEREO)
    if(__bModeChannel==AUDIO_STEREO) //CoCo1.21, remove gcShowStatus.bAudioChannel. use __bModeChannel to judge it
    {
        _bDisplayRam[VFD_L_ADDR] |= VFD_L;
        _bDisplayRam[VFD_R_ADDR] |= VFD_R;
    }
    //else if(gcShowStatus.bAudioChannel== AUDIO_LL)
    else if(__bModeChannel== AUDIO_LL)
        _bDisplayRam[VFD_L_ADDR] |= VFD_L;
    //else if(gcShowStatus.bAudioChannel== AUDIO_RR)
    else if(__bModeChannel== AUDIO_RR)
        _bDisplayRam[VFD_R_ADDR] |= VFD_R;
    //---- DVD information DOLBY 5.1 DTS ANGLE ---------------------------------
#ifndef NO_DISC_MODE //CoCo2.37p
    if(__wDiscType & BOOK_DVD)
    {
        if(__bACHN==6)
            _bDisplayRam[VFD_51CH_ADDR] |= VFD_51CH;
        if(__bAudioType == HAL_AUDIO_AC3)
            _bDisplayRam[VFD_DOLBY_ADDR] |= VFD_DOLBY;
        else if(__bAudioType== HAL_AUDIO_DTS)
            _bDisplayRam[VFD_DTS_ADDR] |= VFD_DTS;
        if(__bAGL_Ns)
            _bDisplayRam[VFD_ANGLE_ADDR] |= VFD_ANGLE;
    }
#endif //#ifndef NO_DISC_MODE //CoCo2.37p
}

//  ***************************************************************************
//  Function    :   _LETT_To_DisplayRam
//  Abstract    :   Built _bLettArray Letter to _bDisplayRam
//  Arguments   :   none
//  Return      :   none
//  Side Effect :       none
//  ***************************************************************************
//#pragma NOAREGS
void _LETT_To_DisplayRam(void)
{
    for (i=0;i<VFD_NUM;i++)
    {
        _bDisplayRam[i*2] = _bLettArray[i];
    }

    //if((gcShowStatus.bSetProgram)||(__bSearchMode==SEARCH_MODE_TITLE_CHAPTER))
#ifndef NO_DISC_MODE //CoCo2.37p    
    if((__bSearchMode==SEARCH_MODE_TITLE_CHAPTER))
        return;
#endif //#ifndef NO_DISC_MODE //CoCo2.37p    

    if (__bMediaSelectUI)
        return;
    if( _bLettArray == _bLettTimeMessage)
    {
        if(!(((__wDiscType & BOOK_SVCD)||(__wDiscType & BOOK_CVD)||(__wDiscType & BOOK_VCD))&&(__wPlayItem >= 999)))
        {
            if(!(__wDiscType & CDROM_M1 && (MM_GetFileType(__MMPlayingItem.wDirID, __wPlayItem)==EXTNAME_JPG)))
            {
                //if (LOBYTE(gcShowStatus.wCurrentTime)%2)
#ifdef Col_Flash            	
                if (((OS_GetSysTimer()- __dwTimerCount_PANEL_SHIFT)>COUNT_500_MSEC))
                {
                    __dwTimerCount_PANEL_SHIFT=OS_GetSysTimer();
                    if (COL_Flash==0)
                        COL_Flash=1;
                    else
                        COL_Flash=0;    
                }
#else
                COL_Flash=1;
#endif

                if (COL_Flash==1) 
                {
                    _bDisplayRam[VFD_HOUR_COL_ADDR] |= VFD_HOUR_COL;
                    _bDisplayRam[VFD_MIN_COL_ADDR]  |= VFD_MIN_COL;                
                }
                else
                {
                    _bDisplayRam[VFD_HOUR_COL_ADDR] &= ~VFD_HOUR_COL;
                    _bDisplayRam[VFD_MIN_COL_ADDR]  &= ~VFD_MIN_COL;     
                }              	
            }
        }
    }
    if (Message_length>0)
    {
        _bDisplayRam[VFD_HOUR_COL_ADDR] &= ~VFD_HOUR_COL;
        _bDisplayRam[VFD_MIN_COL_ADDR]  &= ~VFD_MIN_COL; 
    }                
}

//  ***************************************************************************
//  Function    :   _ShowTime
//  Abstract    :   Built Time Message Letter (_bLettTimeMessage)
//                                      to _bDisplayRam
//  Arguments   :   wTime
//  Return      :   none
//  Side Effect :       none
//  ***************************************************************************
//#pragma NOAREGS
// LLY2.53, expand the time value from WORD to DWORD unit
// Since, we will use [23:16]:hour, [15:8]:min, [7:0]: sec
//void _ShowTime( WORD wTime) // ** TCH0.50;
void _ShowTime(DWORD dwTime) // ** TCH0.50;
{
    //    BYTE bHour, bMin, bSec,bData;
    //    extern BYTE  __bTitle;
    //    extern WORD  __wChapter;
    extern WORD __wPlayItem;
    Message_length = 0;
    if((__wDiscType & BOOK_SVCD)||(__wDiscType & BOOK_CVD)||(__wDiscType & BOOK_VCD))
    {
        if(__wPlayItem > 999)
        {
            for (i=0;i<10;i++)
                _bLettArray_O[i]=0;                    
            Message_length=4;
            _bLettArray = _bLettTimeMessage;
            _bLettArray_O[0] = 0;
            _bLettArray_O[1] = 0;
            _bLettArray_O[2] = LETT_P;
            _bLettArray_O[3] = LETT_b;
            _bLettArray_O[4] = LETT_C; 
            goto  DIRECT_FILL_PBC_MESSAGE;
        }
    }

    _bLettArray = _bLettTimeMessage;

    if(__wDiscType & BOOK_DVD )
    {

    }
    else if(__wDiscType & CDROM_M1)
    {
    }
    else if(__wDiscType & BOOK_CDDA)
    {
    }
    else if((__wDiscType & BOOK_SVCD)||(__wDiscType & BOOK_CVD)||(__wDiscType & BOOK_VCD))
    {
        if(__wPlayItem < 999)
        {
        }
    }
    if(((__wDiscType & BOOK_SVCD)||(__wDiscType & BOOK_CVD)||(__wDiscType & BOOK_VCD))&&(__wPlayItem >= 999))
    {
    }
    else
    {

        if(__wDiscType & CDROM_M1 && (MM_GetFileType(__MMPlayingItem.wDirID, __wPlayItem)==EXTNAME_JPG))
        {
            _bLettArray[0] = aNumMap[gcShowStatus.wCurrentTrack/100];
            _bPanelTemp1 = gcShowStatus.wCurrentTrack % 100;
            _bLettArray[1] = aNumMap[_bPanelTemp1/10];
            _bLettArray[2] = aNumMap[_bPanelTemp1%10];
        }
        else
        {
            // LLY2.53, modify the procedure for time calculation
            // Since, current we will use DWORD to keep time value
            // ie. [23:16]:hour, [15:8]: min, [7:0]: sec
            BYTE bIdx=0
#if 0
                _bPanelTemp2=HIBYTE(wTime);
            _bPanelTemp1=_bPanelTemp2/60;
            _bPanelTemp2=_bPanelTemp2%60;

            _bLettArray[0] = aNumMap[_bPanelTemp1];
            _bLettArray[1] = aNumMap[_bPanelTemp2/10];
            _bLettArray[2] = aNumMap[_bPanelTemp2%10];
            _bPanelTemp1=LOBYTE(wTime);
            _bLettArray[3] = aNumMap[_bPanelTemp1/10];
            _bLettArray[4] = aNumMap[_bPanelTemp1%10];
#endif  // #if 0

            // Hour part
            _bPanelTemp1 = (BYTE)(HIWORD(dwTime));
            if(_bPanelTemp1 >= 10)
            {
                _bLettArray[bIdx++] = aNumMap[_bPanelTemp1/10];
            }
            _bLettArray[bIdx++] = aNumMap[_bPanelTemp1%10];

            // Min part
            _bPanelTemp1 = HIBYTE(LOWORD(dwTime));
            _bLettArray[bIdx++] = aNumMap[_bPanelTemp1/10];
            _bLettArray[bIdx++] = aNumMap[_bPanelTemp1%10];

            // Sec part
            _bPanelTemp1 = LOBYTE(LOWORD(dwTime));
            _bLettArray[bIdx++] = aNumMap[_bPanelTemp1/10];
            _bLettArray[bIdx++] = aNumMap[_bPanelTemp1%10];

        }
    }

DIRECT_FILL_PBC_MESSAGE:
    _LETT_To_DisplayRam();

    if(__wDiscType & BOOK_DVD)                      // Add TITLE CHAPTER
    {
#ifndef NO_DISC_MODE //CoCo2.37p
        if(gcShowStatus.bPlayingMode == MSG_STOP)
        {
            _bDisplayRam[VFD_TITLE_ADDR] &= ~VFD_TITLE;
            _bDisplayRam[VFD_CHAPTER_ADDR] &= ~VFD_CHAPTER;
        }
        else
        {
            _bDisplayRam[VFD_TITLE_ADDR] |= VFD_TITLE;
            _bDisplayRam[VFD_CHAPTER_ADDR] |= VFD_CHAPTER;
        }
#endif //#ifndef NO_DISC_MODE //CoCo2.37p
    }
    else if(__wPlayItem < 999)
    {
        _bDisplayRam[VFD_TRACK_ADDR] |= VFD_TRACK;
    }
}

//  ***************************************************************************
//  Function    :   _Show_Program_Message
//  Abstract    :   Show _bPrgramData to panel
//  Arguments   :   none
//  Return      :   none
//  Side Effect :       none
//  ***************************************************************************
void    _Show_Program_Message(void)
{
    _bLettArray = _bLettTimeMessage;
    _bLettArray[0]=LETT_P;
    _bLettArray[1]=LETT_r;
    _bLettArray[2]=LETT_g;
}
//  ***************************************************************************
//  Function    :   _Show_GotoTime_Message
//  Abstract    :   Show gototime message to panel
//  Arguments   :   none
//  Return      :   none
//  Side Effect :       none
//  ***************************************************************************
void    _Show_GotoTime_Message(void)
{
    extern  BYTE __bTimeHour, __bTimeMin, __bTimeSec;
    //extern  BYTE _bTimeIndex; //--CoCo1.21, may remove _bTimeIndex, it is fixed value 0xff for now

    _bLettArray = _bLettTimeMessage;
    _bLettArray[0] = aNumMap[__bTimeHour];
    _bLettArray[1] = aNumMap[__bTimeMin/10];
    _bLettArray[2] = aNumMap[__bTimeMin%10];
    _bLettArray[3] = aNumMap[__bTimeSec/10];
    _bLettArray[4] = aNumMap[__bTimeSec%10];

    // --CoCo1.21, the following case never exist ==> for _bTimeIndex is 0xff
#ifdef GOTOTIME_LEFTTORIGHT
    switch(_bTimeIndex)
    {
    case 0:
        _bLettArray[0] = LETT_DASH;
    case 1:
        _bLettArray[1] = LETT_DASH;
    case 2:
        _bLettArray[2] = LETT_DASH;
    case 3:
        _bLettArray[3] = LETT_DASH;
    case 4:
        _bLettArray[4] = LETT_DASH;
        break;
    }

#endif
}
//  ***************************************************************************
//  Function    :   _Show_Search_Message
//  Abstract    :   Show DVD search message to panel
//  Arguments   :   none
//  Return      :   none
//  Side Effect :       none
//  ***************************************************************************
void    _Show_Search_Message(void)
{
    extern BYTE __bTitle;
    extern WORD __wChapter;

    _bLettArray = _bLettTimeMessage;
#ifndef NO_DISC_MODE //CoCo2.37p    
    if(__bSearchMode==SEARCH_MODE_TITLE_CHAPTER)
    {

        _bLettArray[0] = aNumMap[__bTitle/10];
        _bLettArray[1] = aNumMap[__bTitle%10];
        _bLettArray[2] = 0;
        _bLettArray[3] = aNumMap[LOBYTE(__wChapter)/10];
        _bLettArray[4] = aNumMap[LOBYTE(__wChapter)%10];

    }
#endif //#ifndef NO_DISC_MODE //CoCo2.37p    
    if(__bSearchMode==SEARCH_MODE_TIME)
    {
        _Show_GotoTime_Message();
    }
}

#endif  // #ifdef USE_VFD_CTRL
#endif  // #ifndef NO_PANEL
