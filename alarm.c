#define ALARM_MAIN_FILE

#include "winav.h"
#include "ctkav.h"
#include "comdef.h"
#include "utl.h"
#include "chips.h"
#include "romld.h"
#include "hal.h"
#include "comutl.h"
#include "cc.h"
#include "RTCdrv.h"
#include "alarm.h"
#include "input.h"
#include "disp.h"
#include "gdi.h"
#include "osd.h"
#include "setup.h"
#include "poweronmenu.h"
#include "mainmenu.h"
#include "menu.h"
#include "settime.h"
#include "osdss.h"

#ifdef SUPPORT_ALARM_WITH_FM_TUNER
#include "radio.h"
#endif

#define MAXSNDFILESIZE 16384

ALARM_TM __Alarm;
BOOL __bEnableAlarm;
BOOL __bTriggerAlarm;
BYTE __bAlarmState;
BYTE _bALARMRealStopKey = KEY_NO_KEY;

//For UI parts
HMENU _hALARMMenu = MENU_HANDLE_INVALID;
BYTE _bALARMMode;
SETTIME_TIME _AlarmSetTime;
HDIALOG _hALARMDlg = DIALOG_HANDLE_INVALID;
HSETTIME _hAlarmSetTime = SETTIME_HANDLE_INVALID;
BYTE _bALARMExitKey[] = {KEY_LEFT};
extern IMAGE_FRAME_SETUPINFO __ImageFrameSetupInfo;
#ifdef SUPPORT_JPEG_AUDIO_PLAYING
extern void MM_ExitJPEGAudioMode(void);
#endif //SUPPORT_JPEG_AUDIO_PLAYING
#ifdef SUPPORT_ALARM_AUTO_EXIT
DWORD    __dwTimerAlarm=0xffffffff;
BYTE        AlarmTimes=0;
#define    AlarmPlayMaxTimes      2
#endif
/*************************************************************************
*  Function     :   ALARM_InitialValue
*  Description :   Initialize the variables of ALARM module   
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
**************************************************************************/
void ALARM_Initial ( void )
{
    __bTriggerAlarm = FALSE;
    __bAlarmState = ALARM_NONE;
    _bALARMRealStopKey = KEY_NO_KEY;

    if (__ImageFrameSetupInfo.bAlarmEnable == ALARM_ENABLE_ON)
    {
        ALARM_Enable();
    }
    else
    {
        ALARM_Disable();
    }

    __Alarm.bHr = HIBYTE(__ImageFrameSetupInfo.wAlarmTime);
    __Alarm.bMin = LOBYTE(__ImageFrameSetupInfo.wAlarmTime);
}

/*************************************************************************
*  Function     :   ALARM_Enable
*  Description :   Enable the System Alarm   
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Enable(void)
{
    __bEnableAlarm = TRUE;
}

/*************************************************************************
*  Function     :   ALARM_Disable
*  Description :   Disable the System Alarm   
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Disable(void)
{
    __bEnableAlarm = FALSE;
}

/*************************************************************************
*  Function     :   ALARM_Set
*  Description :   Set Alarm Timer
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Set ( PALARM_TM ptm)
{
    //__Alarm.bYear  =  ptm->bYear;
    //__Alarm.bMon   =  ptm->bMon;
    //__Alarm.bDate  =  ptm->bDate;
    __Alarm.bHr     =  ptm->bHr;
    __Alarm.bMin    =  ptm->bMin;
    //__Alarm.bSec    =  ptm->bSec;    
}

/*************************************************************************
*  Function     :   ALARM_Get
*  Description :   Get current Alarm Timer
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Get ( PALARM_TM ptm )
{
    //ptm->bYear =    __Alarm.bYear;
    //ptm->bMon  =    __Alarm.bMon;
    //ptm->bDate =    __Alarm.bDate;
    ptm->bHr     =    __Alarm.bHr;
    ptm->bMin   =    __Alarm.bMin;
    //ptm->bSec   =    __Alarm.bSec;  
}

/*************************************************************************
*  Function     :   ALARM_Stop
*  Description :   Stop the Alarm
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Stop ( void )
{
    if ( __bAlarmState == ALARM_GOING )
    {
        __bTriggerAlarm = FALSE;
        __bAlarmState = ALARM_NONE;
        _ALARM_StopPlaySound();
        
        if (_hALARMDlg != DIALOG_HANDLE_INVALID)
        {
            DIALOG_DeleteDialog(_hALARMDlg, TRUE);
            _hALARMDlg = DIALOG_HANDLE_INVALID;
        }

#ifdef SUUPORT_ALARM_AGAIN
        if (__bKey == KEY_STOP)
        {
            ALARM_RealStop();
        }
        else
        {        
            __Alarm.bMin += ALARM_AGAIN_TIME;
            
            if (__Alarm.bMin > 59)
            {
                __Alarm.bMin -= 60;
                __Alarm.bHr += 1;
            }
            
            if (__Alarm.bHr > 23)
            {
                __Alarm.bHr = 0;
            }
        }
#endif //SUUPORT_ALARM_AGAIN
#ifdef SUPPORT_ALARM_AUTO_EXIT
	 ALARM_RealStop();
	AlarmTimes=0;
#endif
        //__bKey = KEY_NO_KEY;
        //Go to Power on Menu
        //POWERONMENU_Initial();
         __bKey = KEY_FUNCTION;
    }    
}
#ifdef SUPPORT_ALARM_AUTO_EXIT
void ALARM_TIME_Stop ( void )
{
    if ( __bAlarmState == ALARM_GOING )
    {
    		if( OS_GetSysTimer()-__dwTimerAlarm>COUNT_10_SEC*3)
    		{
        		__bTriggerAlarm = FALSE;
        		__bAlarmState = ALARM_NONE;
        		_ALARM_StopPlaySound();
        		if (_hALARMDlg != DIALOG_HANDLE_INVALID)
        		{
            			DIALOG_DeleteDialog(_hALARMDlg, TRUE);
            			_hALARMDlg = DIALOG_HANDLE_INVALID;
        		}
			POWERONMENU_Initial();
			AlarmTimes=AlarmTimes+1;
			if(AlarmTimes<AlarmPlayMaxTimes)
			{
				 __Alarm.bMin += ALARM_AGAIN_TIME;
           		 	if (__Alarm.bMin > 59)
            			{
                			__Alarm.bMin -= 60;
                			__Alarm.bHr += 1;
            			}
            
           		 	if (__Alarm.bHr > 23)
            			{
                			__Alarm.bHr = 0;
            			}
				printf("\n alarm tims pause , AlarmTimes=%d",AlarmTimes);
			}	
			else
			{
			  	 ALARM_RealStop();
			   	printf("\n alarm tims pause , AlarmTimes=%d",AlarmTimes);
			   	AlarmTimes=0;
			   	printf("\n alarm tims stop");
			}
    		 }
		
    }
}    
#endif

/*************************************************************************
*  Function     :   ALARM_Trigger
*  Description :   Check the Alarm should be triggered or not
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Trigger(void)
{
    if(__bEnableAlarm && !__btPowerDown)
    {
        _ALARM_Check();
        _ALARM_Monitor();
	#ifdef SUPPORT_ALARM_AUTO_EXIT
       ALARM_TIME_Stop();
	#endif
		
    }
}

/*************************************************************************
*  Function     :   _ALARM_PlaySound
*  Description :   Playback the Alarm sound
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void _ALARM_PlaySound(void)
{
    DWORD dwToneBitSize, dwToneBitStart, dwCnt;
    PWORD pwPtr;
    PSECTION_ENTRY pWavTbl;
    pWavTbl = ROMLD_GetSectionByName(0, "SND1");  
    pwPtr = (PWORD) pWavTbl->dwRMA;    

    HAL_IOMute(TRUE);
    //Aron, must do it, otherwise it will play no sound.
    HAL_ResetAIUPCM();

    __bAudioType = HAL_AUDIO_MP3;

    if ( pWavTbl->dwRSize > MAXSNDFILESIZE )
    {
        pWavTbl->dwRSize = MAXSNDFILESIZE;
    }
    
    dwToneBitSize = (pWavTbl->dwRSize)/sizeof(WORD);

    //see HAL_SetBuffer for categorization
    if(__bAttrPlay == ATTR_AVI) //AVI/DivX
    {
     dwToneBitStart = DS_AD0BUF_ST_AVI;//DS_AD0BUF_ST_DIVX;
    }
    else if(__bAttrPlay & TYPE_CDROM_AUDIO || __bAttrPlay==ATTR_CDDA) //.MP3/.MP2/.WMA/CDDA/CD-DTS
    {
     dwToneBitStart = DS_AD0BUF_ST_MM;
    }
    //CDROM/ DVDROM motion case except AVI
    else if(__bAttrPlay & TYPE_CDROM_AV)
    {
        dwToneBitStart = DS_AD0BUF_ST_MM_MOTION;
    }
    else  //DVD/VCD
    {
     dwToneBitStart = DS_AD0BUF_ST;
    }

    //HAL_WriteAM(HAL_AM_CHANNEL_MODE, bCh);
    HAL_SetAudioType(HAL_AUDIO_MP3);
    //HAL_SetAudioType(HAL_AUDIO_MPG);
    HAL_WriteAM(HAL_AM_PCM_SCALE, VOLUME_MAX);

    MACRO_MCU_SET_AIU_UNPACKER0_READ_BUFFER(dwToneBitStart, dwToneBitStart+pWavTbl->dwRSize, dwToneBitStart);
    MACRO_MCU_SET_AIU_UNPACKER1_READ_BUFFER(dwToneBitStart, dwToneBitStart+pWavTbl->dwRSize, dwToneBitStart);    

    for(dwCnt=0; dwCnt<dwToneBitSize; dwCnt++)
    {
        *( (volatile WORD *)dwToneBitStart + dwCnt ) = *(pwPtr+dwCnt);
    }
    HAL_SetAudioDAC ( AUDIO_FREQ_44K );
#ifdef  CT909P_IC_SYSTEM    
    *(volatile DWORD *)(0x80000F10) |= 0x10FFFFF;    // Set a0 buffer Remainder
    *(volatile DWORD *)(0x80000F20) |= 0x10FFFFF;    // Set a1 buffer Remainder
    HAL_WriteAM(HAL_AM_PLAY_COMMAND, 1);
    *(volatile DWORD *)(0x80000F00) &= 0xFFFFFFFC; // Set a0 & a1 buffer remainder disable
#else
    *(volatile DWORD *)(0x80000F10) |= 0x10000F0;    // Set buffer Remainder
    *(volatile DWORD *)(0x80000F00) &= 0xFFFFFFFE; // Set buffer remainder disable
    HAL_WriteAM(HAL_AM_PLAY_COMMAND, 1);
#endif

    HAL_IOMute(FALSE); 
}

/*************************************************************************
*  Function     :   _ALARM_StopPlaySound
*  Description :   Stop playback the Alarm sound
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void _ALARM_StopPlaySound(void)
{
	HAL_Reset(HAL_RESET_AUDIO);
    HAL_WriteAM(HAL_AM_PLAY_COMMAND, 0);                         
#ifdef  CT909P_IC_SYSTEM     
    *(volatile DWORD *)(0x80000F00) |= 0x3; // Set a0 a1 buffer remainder enable
    *(volatile DWORD *)(0x80000F10) &= 0x1000000;// Set a0 buffer remainder 0    
    *(volatile DWORD *)(0x80000F20) &= 0x1000000;// Set a1 buffer remainder 0    
#else    
    *(volatile DWORD *)(0x80000F00) |= 0x1; // Set buffer remainder enable
    *(volatile DWORD *)(0x80000F10) &= 0x1000000;// Set buffer remainder 0    
#endif    
}

/*************************************************************************
*  Function     :   _ALARM_Check
*  Description :   Check the timing to trigger Alarm
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void _ALARM_Check ( void )
{
    ALARM_TM alarm_tm;
    RTC_TM   rtc_tm;
    
    __bTriggerAlarm = FALSE;
    
    ALARM_Get ( &alarm_tm );
    RTC_GetTime ( &rtc_tm );
    
    if ( 
        ( 0 == rtc_tm.bSec) &&
        (alarm_tm.bHr     == rtc_tm.bHr) &&
        ( alarm_tm.bMin   == rtc_tm.bMin)
        )
    {
        __bTriggerAlarm = TRUE;
    }
    
}

/*************************************************************************
*  Function     :   _ALARM_Monitor
*  Description :   State machine of ALARM
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void _ALARM_Monitor ( void )
{
    switch ( __bAlarmState )
    {
    case ALARM_NONE :
#ifdef SUPPORT_ALARM_WITH_FM_TUNER
        if (__bPlayRadio == TRUE)
            break;			
#endif
        if(__bTriggerAlarm)
        {
            __bAlarmState = ALARM_TRIGGERED;
        }
        break;
        
    case ALARM_TRIGGERED : 
        //Show the UI to notify the user
#ifdef SUPPORT_ALARM_WITH_FM_TUNER  
        POWERONMENU_ClearScreen();
        OSD_RemoveAllUI();        
        _ALARM_StopPlayMode();
        __bCountStop = 2; // force real stop                
        __bTriggerAlarm = FALSE;
        __bAlarmState = ALARM_NONE;
#ifdef SUUPORT_ALARM_AGAIN
		ALARM_RealStop();		
#endif //SUUPORT_ALARM_AGAIN       
        
        //Show FM Tuner
        RADIO_Entry(RADIO_MODE_FAVORITE);
#else //SUPPORT_ALARM_WITH_FM_TUNER
        _ALARM_ShowNotifyUserDlg();                
        _ALARM_PlaySound();
        __bCountStop = 2; // force real stop                
        __bAlarmState = ALARM_GOING;
#endif //SUPPORT_ALARM_WITH_FM_TUNER        
        #ifdef SUPPORT_ALARM_AUTO_EXIT
	 __dwTimerAlarm=OS_GetSysTimer();
	#endif
        break;
        
    case ALARM_GOING :
        OSDSS_ResetTime();
        break;
        
    default:
        break;
    }
}
/*************************************************************************
*  Function     :   ALARM_Test
*  Description :   Unit test
*  Arguments  :   None
*  Return        :   None
*  Side Effect   :
* *************************************************************************/
void ALARM_Test ( void )
{
    extern void RTC_Test_Set(void);
    ALARM_TM tm;
    tm.bHr = 0;
    tm.bMin = 1;    
    //tm.bSec = 10;     
    __bAlarmState = ALARM_NONE;
    __bTriggerAlarm = FALSE;
    ALARM_Enable();
    RTC_Test_Set();
    ALARM_Set(&tm);
}


//*********************************************************************************************************
//  Function    :   ALARM_Entry
//  Abstract    :   This is the entry function of the alarm.
//  Arguments   :   none.
//  Return		:   
//  Side Effect :   none.
//  Notes       :
//*********************************************************************************************************
void ALARM_Entry(void)
{
    _hALARMMenu = MENU_HANDLE_INVALID;
    _bALARMRealStopKey = KEY_NO_KEY;
    
    //Config the OSD to 8 bit mode.
    _ALARM_ConfigOSDRegion();

    _MENU_ALARM_Enable.bDefaultCursorPos = __ImageFrameSetupInfo.bAlarmEnable-ALARM_ENABLE_BEGIN;
    _MENU_ALARM_Enable.bCheckMenuItemPos = _MENU_ALARM_Enable.bDefaultCursorPos;

    //Show Alarm menu.
    _ALARM_ShowMenu();
}

//  ***************************************************************************
//  Function    :   _ALARM_ConfigOSDRegion
//  Description :   
//  Arguments   :   None.
//  Return      :   None.
//  Side Effect :
//  ***************************************************************************
void _ALARM_ConfigOSDRegion(void)
{
    GDI_REGION_INFO RegionInfo;
#ifdef  BORROW_FRAME_BUFFER
    DWORD dwBufferAddr;
#endif //

    GDI_ClearRegion(0);

#ifdef  BORROW_FRAME_BUFFER
   
    if ((POWERPNMENU_WIDTH == __RegionList[0].wWidth) && (POWERPNMENU_HEIGHT <= __RegionList[0].wHeight) &&
        (GDI_OSD_8B_MODE == __RegionList[0].bColorMode)) 
    {
        return;
    }
    else if (__RegionList[0].bColorMode != GDI_OSD_8B_MODE)   // Thumbnail mode & preview mode
    {
        // Use Frame buffer 1 or 2
        dwBufferAddr = DISP_QueryAvailableBuffer();
        if (0 == dwBufferAddr)
        {
            RegionInfo.dwTAddr = DS_OSDFRAME_ST;
        }
        else
        {
            RegionInfo.dwTAddr = dwBufferAddr;
        }
    } 
    else
    {
        // Use OSD buffer and Frame buffer 0
        RegionInfo.dwTAddr = DS_OSDFRAME_ST;   // Use OSD buffer and Frame buffer 0 
    }

#else
    RegionInfo.dwTAddr = DS_OSDFRAME_ST;
#endif // #ifdef  BORROW_FRAME_BUFFER

    RegionInfo.wWidth = POWERPNMENU_WIDTH;
    RegionInfo.wHeight = POWERPNMENU_HEIGHT;
    RegionInfo.bColorMode = GDI_OSD_8B_MODE;

#ifdef  BORROW_FRAME_BUFFER
    OSD_SetRegion(0, TRUE, &RegionInfo);
#else
    OSD_SetRegion(0, FALSE, &RegionInfo);
#endif
}

void _ALARM_ShowMenu(void)
{    
    MAINMENU_InitializeGDI(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    MAINMENU_DrawMainBackground();
    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
    
    //Draw the title
    MAINMENU_DrawTitleString(aALARMAlarmSetting, NULL, NULL);
    
    if (_hALARMMenu == MENU_HANDLE_INVALID)
    {
        _hALARMMenu = MENU_CreateMenu(&_MENU_ALARM, ALARM_MAX_VISIBLE_MENU_ITEM_NUM, _bALARMExitKey);
    }
    
    MENU_ShowMenu(_hALARMMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);      
    
    _bALARMMode = ALARM_MODE_MENU;
}

//*********************************************************************************************************
//  Function    :   ALARM_ProcessKey
//  Abstract    :   Call this function to process the keys.
//  Arguments   :   none.
//  Return		:   KEY_NO_KEY: The input key is processed.
//              :   INVALID_KEY: The input key is not allowed.
//              :   KEY_BYPASS: Will not process this key and pass it to main flow to process it.
//              :   KEY_XXX: The main program need do more process for the key.
//  Side Effect :   none.
//  Notes       :
//*********************************************************************************************************
BYTE ALARM_ProcessKey(void)
{
    WORD wMenuAction;
    DWORD dwMenuReturn;

    if (__bKey == KEY_FUNCTION)
    {
        ALARM_Exit(FALSE);
        POWERONMENU_BackToMainMenu();            
    }

    switch (_bALARMMode)
    {
    case ALARM_MODE_MENU:
        dwMenuReturn = MENU_ProcessKey(_hALARMMenu, __bKey);
        wMenuAction = HIWORD(dwMenuReturn);
        
        if (HIWORD(dwMenuReturn) == MENU_ACTION_EXIT)
        {
            ALARM_Exit(FALSE);
            POWERONMENU_BackToMainMenu();                        
        }
        else if (wMenuAction == MENU_ACTION_PROCESS_MENU_ITEM)
        {
            _ALARM_ProcessMenuItem(LOWORD(dwMenuReturn));
        }
        break;
    case ALARM_MODE_SET_TIME:
        SETTIME_ProcessKey(_hAlarmSetTime, __bKey);
        break;
    default:
        break;
    }

    return KEY_NO_KEY;
}

void _ALARM_ProcessMenuItem(BYTE bID)
{
    switch (bID)
    {
    case ALARM_ENABLE_ON:
        ALARM_Enable();

        //Set the setting to flash.
        __ImageFrameSetupInfo.bAlarmEnable = ALARM_ENABLE_ON;

#ifdef WRITE_EEPROM_ENABLE //SYSTEM_8051
        HAL_WriteStorage(SETUP_ADDR_ALARM_ENABLE, (BYTE *)&(__ImageFrameSetupInfo.bAlarmEnable), 1);
#endif

        break;
    case ALARM_ENABLE_OFF:
        ALARM_Disable();

        //Set the setting to flash.
        __ImageFrameSetupInfo.bAlarmEnable = ALARM_ENABLE_OFF;

#ifdef WRITE_EEPROM_ENABLE //SYSTEM_8051
        HAL_WriteStorage(SETUP_ADDR_ALARM_ENABLE, (BYTE *)&(__ImageFrameSetupInfo.bAlarmEnable), 1);
#endif
        break;
    case ALARM_MAIN_EXIT:
        ALARM_Exit(FALSE);
        POWERONMENU_BackToMainMenu();                        
        break;
    case ALARM_MAIN_SET_TIME:
#ifdef NL_SOLUTION
        MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        MAINMENU_DrawSelectionHelp(TRUE, FALSE);
#endif //NL_SOLUTION

        _bALARMMode = ALARM_MODE_SET_TIME;
        _ALARM_SET_TIME.bHour = HIBYTE(__ImageFrameSetupInfo.wAlarmTime);
        _ALARM_SET_TIME.bMinute = LOBYTE(__ImageFrameSetupInfo.wAlarmTime);   
        _hAlarmSetTime  = SETTIME_Create(&_ALARM_SET_TIME, &_ALARM_SET_TIME_Action);
        if (_hAlarmSetTime != SETTIME_HANDLE_INVALID)
        {
            SETTIME_Show(_hAlarmSetTime, MAINMENU_PALETTE_ENTRY_CONTENT_REGION, __SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        }
        break;
    default:
        break;
    }
}

void ALARM_Exit(BYTE bClearBackground)
{
    if (_hALARMMenu != MENU_HANDLE_INVALID)
    {
        MENU_DeleteMenu(_hALARMMenu, bClearBackground);
        _hALARMMenu = MENU_HANDLE_INVALID;
    }

    if (_hAlarmSetTime != SETTIME_HANDLE_INVALID)
    {
        SETTIME_Exit(_hAlarmSetTime, TRUE);
        _hAlarmSetTime = SETTIME_HANDLE_INVALID;
    }
}

//*********************************************************************************************************
//  Function    :   ALARM_Recover
//  Abstract    :   Call this function to do recovery.
//  Arguments   :   bRecoverRegion: OSD_RECOVER_UPPER_REGION/OSD_RECOVER_BOTTOM_REGION.
//  Return		:   TRUE: The region is recovered.
//              :   FALSE: The region doesn't need to be recovered in THUMB UI.
//  Side Effect :   none.
//  Notes       :
//*********************************************************************************************************
BYTE ALARM_Recover(BYTE bRecoverRegion)
{
    //Config the OSD to 8 bit mode.
    _ALARM_ConfigOSDRegion();

    _ALARM_ShowMenu();

    if (_bALARMMode == ALARM_MODE_SET_TIME)
    {
#ifdef NL_SOLUTION
        MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        MAINMENU_DrawSelectionHelp(TRUE, FALSE);
#endif //NL_SOLUTION

        SETTIME_Recover(_hAlarmSetTime);
    }

    return TRUE;
}

void ALARM_SetTime(BYTE bHour, BYTE bMinute, BYTE bSecond)
{
    __Alarm.bHr = bHour;
    __Alarm.bMin = bMinute;

    //Save the alarm time to flash.
    __ImageFrameSetupInfo.wAlarmTime = (bHour << 8) | bMinute;

#ifdef WRITE_EEPROM_ENABLE //SYSTEM_8051
    HAL_WriteStorage(SETUP_ADDR_ALARM_TIME, (BYTE *)&(__ImageFrameSetupInfo.wAlarmTime), 2);
#endif
    
    SETTIME_Exit(_hAlarmSetTime, TRUE);
    MENU_ShowMenu(_hALARMMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION); 
#ifdef NL_SOLUTION
    MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
#endif //NL_SOLUTION

    _bALARMMode = ALARM_MODE_MENU;
}

void ALARM_CancelSetTime(void)
{
    SETTIME_Exit(_hAlarmSetTime, TRUE);
    MENU_ShowMenu(_hALARMMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION); 

    _bALARMMode = ALARM_MODE_MENU;
}

void _ALARM_StopPlayMode(void)
{
    OSDSS_Reset();

#ifdef PRESTOP_BEFORE_SETUP_AND_PROGRAM    
    CC_ForceStop(CC_FORCE_STOP_FROM_ALARM_MENU);
#endif

#ifdef SUPPORT_JPEG_AUDIO_PLAYING
{
    extern BYTE __bMMJPEGAudioMode;
     if (__bMMJPEGAudioMode)
     {
        MM_ExitJPEGAudioMode();
     }
}
#endif

    CC_KeyCommand(KEY_STOP);
    CC_ResetState( KEY_OPEN_CLOSE );
}

void _ALARM_ShowNotifyUserDlg(void)
{   
	_ALARM_StopPlayMode();
	
    __bLOGO = 0;
    UTL_ShowLogo();

    //Config the OSD to 8 bit mode.
    _ALARM_ConfigOSDRegion();

    if (_hALARMDlg == DIALOG_HANDLE_INVALID)
    {
        _hALARMDlg = DIALOG_CreateDialog(&_ALARMDlg, __SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    }

    if (_hALARMDlg != DIALOG_HANDLE_INVALID)
    {
        DIALOG_InitializeGDI(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);

        DIALOG_ShowDialog(_hALARMDlg, PAL_ENTRY_COLOR_TRANSPARENT, FALSE);                    
        DIALOG_ShowMessage(_hALARMDlg, (ALARM_DIALOG_WIDTH-GDI_GetStringWidth(aALARMDlgContent))/2, ALARM_DIALOG_CONTENT_DISTANCE_V, aALARMDlgContent);
    }
}

void ALARM_RealStop(void)
{
    __Alarm.bHr = HIBYTE(__ImageFrameSetupInfo.wAlarmTime);
    __Alarm.bMin = LOBYTE(__ImageFrameSetupInfo.wAlarmTime);
}
