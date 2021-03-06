#define IMAGE_FRAME_MENU_MAIN_FILE
#define DVDSETUP_MAIN_FILE

#include "winav.h"
#include "disp.h"
#include "hal.h"
#include "chips.h"
#include "utl.h"
#include "navigate.h"
#include "cc.h"
#include "comutl.h"
#include "gdi.h"
#include "osd.h"
#include "osddsply.h"
#include "osdnd.h"
#include "osddlg.h"
#include "input.h"
#include "toolbar.h"
#include "menu.h"
#include "mainmenu.h"
#ifdef SUPPORT_STB
#include "STB\\usrif\\landlg.h"
#include "STB\\usrif\\pindlg.h"
#endif //SUPPORT_STB
#include "OSDString\\strextrn.h"
#include "dvdsetup_op.h"
#include "osddivx.h"
#include "infofilter.h"
#include "haljpeg.h"
#include "tft.h"
#include "media.h"
#include "fontindex.h"

#ifdef NL_SOLUTION
#include "rtcdrv.h"
#endif //NL_SOLUTION

#define MENU_MAIN_FILE
#include "dvdsetup.h"

#define IDENTIFY_STRING         "C"

BYTE             _bSetupTemp;
WORD             _wSetupTemp;
WORD             _wSetupString[8];
PARM_DRAW_STRING _DVDSETUP_StrAttr;
PARM_RECT        _DVDSETUP_RectAttr;
#ifdef SUPPORT_STB
BYTE    __bInputPassword;
#endif

#ifndef SUPPORT_TEXT_TOOLBAR
DWORD _aDVDSETUP_PhotoSetting[]=
{
#include "bmp/Menu_PhotoSetting.txt"
};

DWORD _aDVDSETUP_AutoPlay[]=
{
#include "bmp/Menu_AutoPlay.txt"
};

DWORD _aDVDSETUP_Display[]=
{
#include "bmp/Menu_Display.txt"
};

DWORD _aDVDSETUP_CustomSetup[]=
{
#include "bmp/Menu_Custom.txt"
};

DWORD _aDVDSETUP_Exit[]=
{
#include "bmp/Menu_Exit.txt"
};

#ifndef SUPPORT_STB
DWORD *_aDVDSETUPBMPArray[5] = {
    _aDVDSETUP_PhotoSetting,        // 0
    _aDVDSETUP_AutoPlay,            // 1
    _aDVDSETUP_Display,             // 2
    _aDVDSETUP_CustomSetup,         // 3
    _aDVDSETUP_Exit,                // 4
};
#else
DWORD *_aDVDSETUPBMPArray[3] = {
    _aDVDSETUP_PhotoSetting,        // 0
    _aDVDSETUP_AutoPlay,            // 1
    _aDVDSETUP_Exit,                // 2
};
#endif //SUPPORT_STB

DWORD _aDVDSETUP_Palette[]=    // Palette 2 data, for button bitmap
{
#include "bmp/palMenu.txt"                  // bit 8/24 indicate Mix_Enable
};

ICON _DVDSETUPIcon[] = {
    {ICON_DVDSETUP_MAIN_PHOTO_SETTING, DVDSETUP_ICON_PALETTE_ENTRY_PHOTO_SETTING_ICON, ICON_TYPE_POPMENU, _DVDSETUP_IconPhotoSettingInitialFunc, _DVDSETUP_IconExitFunc},
    {ICON_DVDSETUP_MAIN_AUTO_PLAY, DVDSETUP_ICON_PALETTE_ENTRY_AUTO_PLAY_ICON, ICON_TYPE_POPMENU, _DVDSETUP_IconAutoPlaySetupInitialFunc, _DVDSETUP_IconExitFunc},
#ifndef SUPPORT_STB
    {ICON_DVDSETUP_MAIN_DISPLAY, DVDSETUP_ICON_PALETTE_ENTRY_DISPLAY_SETUP_ICON, ICON_TYPE_POPMENU, _DVDSETUP_IconDisplaySetupInitialFunc, _DVDSETUP_IconExitFunc},
    {ICON_DVDSETUP_MAIN_CUSTOM, DVDSETUP_ICON_PALETTE_ENTRY_CUSTOM_SETUP_ICON, ICON_TYPE_POPMENU, _DVDSETUP_IconCustomSetupInitialFunc, _DVDSETUP_IconExitFunc},
#endif //SUPPORT_STB
    {ICON_DVDSETUP_MAIN_EXIT, DVDSETUP_ICON_PALETTE_ENTRY_EXIT_ICON, ICON_TYPE_BUTTON, _DVDSETUP_IconExitInitialFunc, _DVDSETUP_IconExitFunc}
};

#if (DVDSETUP_TOOLBAR == TOOLBAR_STYLE_VERTICAL)
TOOLBAR _DVDSETUPToolBar = {DVDSETUP_TOTAL_ICONS, TOOLBAR_STYLE_VERTICAL, DVDSETUP_TOOLBAR_START_H, DVDSETUP_TOOLBAR_START_V, DVDSETUP_ICON_WIDTH, DVDSETUP_ICON_HEIGHT, _DVDSETUPIcon, _aDVDSETUPBMPArray, _aDVDSETUP_Palette};
BYTE _bDVDSETUPMenuExitKey[] = {KEY_LEFT};
#elif (DVDSETUP_TOOLBAR == TOOLBAR_STYLE_HORIZONTAL)
TOOLBAR _DVDSETUPToolBar = {DVDSETUP_TOTAL_ICONS, TOOLBAR_STYLE_HORIZONTAL, DVDSETUP_TOOLBAR_START_H, DVDSETUP_TOOLBAR_START_V, DVDSETUP_ICON_WIDTH, DVDSETUP_ICON_HEIGHT, _DVDSETUPIcon, _aDVDSETUPBMPArray, _aDVDSETUP_Palette};
BYTE _bDVDSETUPMenuExitKey[] = {KEY_LEFT, KEY_UP};
#endif //DVDSETUP_TOOLBAR

#else //SUPPORT_TEXT_TOOLBAR

//The following strings are the text for the icon.
//If needs to do lanaguage translation, please move it to the normal Excel file.
WORD aDVDSETUPPhoto[] = {5, CHAR_P, CHAR_h, CHAR_o, CHAR_t, CHAR_o};
WORD aDVDSETUPAutoPlay[] = {9, CHAR_A, CHAR_u, CHAR_t, CHAR_o, CHAR_SPACE, CHAR_P, CHAR_l, CHAR_a, CHAR_y};
WORD aDVDSETUPDisplay[] = {7, CHAR_D, CHAR_i, CHAR_s, CHAR_p, CHAR_l, CHAR_a, CHAR_y};
WORD aDVDSETUPCustom[] = {6, CHAR_C, CHAR_u, CHAR_s, CHAR_t, CHAR_o, CHAR_m};
WORD aDVDSETUPExit[] = {4, CHAR_E, CHAR_x, CHAR_i, CHAR_t};

ICON _DVDSETUPIcon[] = {
    {ICON_DVDSETUP_MAIN_PHOTO_SETTING, ICON_TYPE_POPMENU, aDVDSETUPPhoto, _DVDSETUP_IconPhotoSettingInitialFunc, _DVDSETUP_IconExitFunc},
    {ICON_DVDSETUP_MAIN_AUTO_PLAY, ICON_TYPE_POPMENU, aDVDSETUPAutoPlay, _DVDSETUP_IconAutoPlaySetupInitialFunc, _DVDSETUP_IconExitFunc},
#ifndef SUPPORT_STB
    {ICON_DVDSETUP_MAIN_DISPLAY, ICON_TYPE_POPMENU, aDVDSETUPDisplay, _DVDSETUP_IconDisplaySetupInitialFunc, _DVDSETUP_IconExitFunc},
    {ICON_DVDSETUP_MAIN_CUSTOM, ICON_TYPE_POPMENU, aDVDSETUPCustom, _DVDSETUP_IconCustomSetupInitialFunc, _DVDSETUP_IconExitFunc},
#endif //SUPPORT_STB
    {ICON_DVDSETUP_MAIN_EXIT, ICON_TYPE_BUTTON, aDVDSETUPExit, _DVDSETUP_IconExitInitialFunc, _DVDSETUP_IconExitFunc}
};

#if (DVDSETUP_TOOLBAR == TOOLBAR_STYLE_VERTICAL)
TOOLBAR _DVDSETUPToolBar = {DVDSETUP_TOTAL_ICONS, TOOLBAR_STYLE_VERTICAL, DVDSETUP_TOOLBAR_START_H, DVDSETUP_TOOLBAR_START_V, DVDSETUP_ICON_WIDTH, DVDSETUP_ICON_HEIGHT, DVDSETUP_PALETTE_ENTRY_ICON_NORMAL_TEXT, DVDSETUP_PALETTE_ENTRY_ICON_HIGHLIGHT_TEXT, DVDSETUP_PALETTE_ENTRY_ICON_PRESSED_TEXT, _DVDSETUPIcon};
BYTE _bDVDSETUPMenuExitKey[] = {KEY_LEFT};
#elif (DVDSETUP_TOOLBAR == TOOLBAR_STYLE_HORIZONTAL)
TOOLBAR _DVDSETUPToolBar = {DVDSETUP_TOTAL_ICONS, TOOLBAR_STYLE_HORIZONTAL, DVDSETUP_TOOLBAR_START_H, DVDSETUP_TOOLBAR_START_V, DVDSETUP_ICON_WIDTH, DVDSETUP_ICON_HEIGHT, DVDSETUP_PALETTE_ENTRY_ICON_NORMAL_TEXT, DVDSETUP_PALETTE_ENTRY_ICON_HIGHLIGHT_TEXT, DVDSETUP_PALETTE_ENTRY_ICON_PRESSED_TEXT, _DVDSETUPIcon};
BYTE _bDVDSETUPMenuExitKey[] = {KEY_LEFT, KEY_UP};
#endif //DVDSETUP_TOOLBAR

#endif //SUPPORT_TEXT_TOOLBAR


BYTE     __bSetupMode;
BYTE     _bDVDSETUPActiveMode;
BYTE     _bDVDSETUPCursorIconIndex;
HTOOLBAR _hDVDSETUPToolBar;

HMENU    _hDVDSETUPMenu = MENU_HANDLE_INVALID;
BYTE     _bCurMenuID = 0;
BYTE	 __bTVSystemChange = FALSE;

#ifdef SUPPORT_STB
SYSTEMINFO* __pSystemInfo;
WORD     _wInputCode;
BYTE     _bLanguageCodeDlgCursorPos;
HDIALOG  _hLanguageCodeDlg = DIALOG_HANDLE_INVALID;
HEDITBOX _hLanguageCodeDlgEdit = EDITBOX_HANDLE_INVALID;
extern DLG_FLOW _DlgFlowTable[];
#endif //SUPPORT_STB

// Internal functions
void _DVDSETUP_ProcessMenuItem(BYTE bID);
void _DVDSETUP_ShowMenu(HMENU hMenu, BYTE bIconCursorType);
void _DVDSETUP_InitializeSetting(void);
void _DVDSETUP_FillRect(WORD wLeft, WORD wTop, WORD wRight, WORD wBottom);
void _DVDSETUP_DrawDynamicString(WORD *pwString);
void _DVDSETUP_CheckMenuEnable(void);
void _SETUP_PasswordDlg(void);
WORD _SETUP_GetPasswordNumber(void);

void _LanguageCode_InitializeGDI(void);
void _LanguageCode_ShowEditText(void);
WORD _GetLanguageNumber(void);

extern IMAGE_FRAME_SETUPINFO __ImageFrameSetupInfo;
extern BYTE _bOSDSSScreenSaverMode;

#ifdef SUPPORT_TFT_DIGITAL_MODE
extern BYTE    __bTFTOutput;
#endif //SUPPORT_TFT_DIGITAL_MODE

#ifdef IMAGE_FRAME_SETUP
extern DWORD __dwFMJPGShowTime;
extern BYTE __bMMEffectIdx;

#ifdef SUPPORT_JPEG_AUDIO_PLAYING
extern BYTE __bMMJPEGAudioMode;
#endif //SUPPORT_JPEG_AUDIO_PLAYING

#ifdef NL_SOLUTION
extern RTC_TM _CLOCKCurrnetRTCTime; 
#endif //NL_SOLUTION

HSETTIME _hSETUPCLOCKSetTime = SETTIME_HANDLE_INVALID;

#endif

//***************************************************************************
//  Function    :   DVDSETUP_Initial
//  Abstract    :   When the menu is pop-up at 1st time, this initial function will be called. 
//                  Each UI can do its control initialization and draw the UI here.
//  Arguments   :   bRecover: TRUE==>Recover the UI. FALSE==>Initialize the UI.
//  Return      :   nonw.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
#ifdef SUPPORT_STB
void DVDSETUP_Initial(void)
{
    _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_ICON;
    _bDVDSETUPCursorIconIndex = 0;    
    _hDVDSETUPToolBar = TOOLBAR_HANDLE_INVALID;
    _hDVDSETUPMenu = MENU_HANDLE_INVALID;

	_DVDSETUP_InitializeGDI();

    _DVDSETUP_InitializeSetting();

    if (_hDVDSETUPToolBar == TOOLBAR_HANDLE_INVALID)
    {
        _hDVDSETUPToolBar = TOOLBAR_CreateToolBar(&_DVDSETUPToolBar);
    }

    _DVDSETUP_DrawAllUI();

    DVDSETUP_InitStatus();
}
#else
void DVDSETUP_Initial(BYTE bRecover)
{
	_DVDSETUP_InitializeGDI();

    if (!bRecover)
    {
#ifndef NO_TOOLBAR_SETUP_MENU    
        _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_ICON;
        _bDVDSETUPCursorIconIndex = 0;    
        _hDVDSETUPToolBar = TOOLBAR_HANDLE_INVALID;
        _hDVDSETUPMenu = MENU_HANDLE_INVALID;
#else //NO_TOOLBAR_SETUP_MENU
        _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_MENU;          
        _hDVDSETUPMenu = MENU_HANDLE_INVALID;
#endif //NO_TOOLBAR_SETUP_MENU
        
        _DVDSETUP_InitializeSetting();
    }

#ifndef NO_TOOLBAR_SETUP_MENU   
    if (_hDVDSETUPToolBar == TOOLBAR_HANDLE_INVALID)
    {
        _hDVDSETUPToolBar = TOOLBAR_CreateToolBar(&_DVDSETUPToolBar);
    }
#endif //NO_TOOLBAR_SETUP_MENU

    _DVDSETUP_DrawAllUI();

    DVDSETUP_InitStatus();
}

#endif //#ifdef SUPPORT_STB

void _DVDSETUP_InitializeSetting(void)
{
	__bTVSystemChange = FALSE;

#ifdef IMAGE_FRAME_SETUP
    _MENU_SETUP_Photo_Setting_Interval_Time.bCheckMenuItemPos = __ImageFrameSetupInfo.bPhotoIntervalTime-SETUP_PHOTO_SETTING_INTERVAL_TIME_BEGIN;
    _MENU_SETUP_Photo_Setting_Interval_Time.bDefaultCursorPos = _MENU_SETUP_Photo_Setting_Interval_Time.bCheckMenuItemPos;

    _MENU_SETUP_Photo_Setting_Transition_Effect.bCheckMenuItemPos = __ImageFrameSetupInfo.bTransitionEffect-SETUP_PHOTO_SETTING_TRANSITION_EFFECT_BEGIN;
    _MENU_SETUP_Photo_Setting_Transition_Effect.bDefaultCursorPos = _MENU_SETUP_Photo_Setting_Transition_Effect.bCheckMenuItemPos;

    _MENU_SETUP_Photo_Setting_Browse.bCheckMenuItemPos = __ImageFrameSetupInfo.bBrowse-SETUP_PHOTO_SETTING_BROWSE_BEGIN;
    _MENU_SETUP_Photo_Setting_Browse.bDefaultCursorPos = _MENU_SETUP_Photo_Setting_Browse.bCheckMenuItemPos;

    _MENU_SETUP_Auto_Play_Photo.bCheckMenuItemPos = __ImageFrameSetupInfo.bAutoPlayPhoto-SETUP_AUTO_PLAY_PHOTO_BEGIN;
    _MENU_SETUP_Auto_Play_Photo.bDefaultCursorPos = _MENU_SETUP_Auto_Play_Photo.bCheckMenuItemPos;

    _MENU_SETUP_Auto_Play_Music.bCheckMenuItemPos = __ImageFrameSetupInfo.bAutoPlayMusic-SETUP_AUTO_PLAY_MUSIC_BEGIN;
    _MENU_SETUP_Auto_Play_Music.bDefaultCursorPos = _MENU_SETUP_Auto_Play_Music.bCheckMenuItemPos;

    _MENU_SETUP_Auto_Play_Photo_Plus_Music.bCheckMenuItemPos = __ImageFrameSetupInfo.bAutoPlayPhotoPlusMusic-SETUP_AUTO_PLAY_PHOTO_PLUS_MUSIC_BEGIN;
    _MENU_SETUP_Auto_Play_Photo_Plus_Music.bDefaultCursorPos = _MENU_SETUP_Auto_Play_Photo_Plus_Music.bCheckMenuItemPos;

    _MENU_SETUP_Auto_Play_Movie.bCheckMenuItemPos = __ImageFrameSetupInfo.bAutoPlayMovie-SETUP_AUTO_PLAY_MOVIE_BEGIN;
    _MENU_SETUP_Auto_Play_Movie.bDefaultCursorPos = _MENU_SETUP_Auto_Play_Movie.bCheckMenuItemPos;

    _MENU_SETUP_Display_TV_Standard.bCheckMenuItemPos = __SetupInfo.bTVSystem-SETUP_DISPLAY_TV_STANDARD_BEGIN;
    _MENU_SETUP_Display_TV_Standard.bDefaultCursorPos = _MENU_SETUP_Display_TV_Standard.bCheckMenuItemPos;

#ifdef SUPPORT_TFT_DIGITAL_MODE
    if (__bTFTOutput)
    {
        _MENU_SETUP_Display_AV_Output.bCheckMenuItemPos = SETUP_DISPLAY_AV_OUTPUT_PANEL-SETUP_DISPLAY_AV_OUTPUT_BEGIN;
    }
    else
    {
        _MENU_SETUP_Display_AV_Output.bCheckMenuItemPos = SETUP_DISPLAY_AV_OUTPUT_TV-SETUP_DISPLAY_AV_OUTPUT_BEGIN;
    }
    _MENU_SETUP_Display_AV_Output.bDefaultCursorPos = _MENU_SETUP_Display_AV_Output.bCheckMenuItemPos;
#endif //SUPPORT_TFT_DIGITAL_MODE

#ifdef SUPPORT_DSPLAY_16_9_AND_AUTO
    _MENU_SETUP_Display_Type.bCheckMenuItemPos = __ImageFrameSetupInfo.bDisplayType-SETUP_DISPLAY_TYPE_BEGIN;
    _MENU_SETUP_Display_Type.bDefaultCursorPos = _MENU_SETUP_Display_Type.bCheckMenuItemPos;
     printf("\n init position %d",_MENU_SETUP_Display_Type.bCheckMenuItemPos);
#endif

    
    _MENU_SETUP_Brightness.bCheckMenuItemPos = __SetupInfo.bBrightness-SETUP_DISPLAY_BRIGHTNESS_BEGIN;
    _MENU_SETUP_Brightness.bDefaultCursorPos = _MENU_SETUP_Brightness.bCheckMenuItemPos;
    
    _MENU_SETUP_Contrast.bCheckMenuItemPos = __SetupInfo.bContrast-SETUP_DISPLAY_CONTRAST_BEGIN;
    _MENU_SETUP_Contrast.bDefaultCursorPos = _MENU_SETUP_Contrast.bCheckMenuItemPos;
    
    //For TFT Brightness
    _MENU_SETUP_TFT_Brightness.bCheckMenuItemPos = __TFTPicAttr.bBrightness-SETUP_DISPLAY_TFT_BRIGHTNESS_BEGIN;
    _MENU_SETUP_TFT_Brightness.bDefaultCursorPos = _MENU_SETUP_TFT_Brightness.bCheckMenuItemPos;

    //For TFT Contrast
    _MENU_SETUP_TFT_Contrast.bCheckMenuItemPos = __TFTPicAttr.bContrast-SETUP_DISPLAY_TFT_CONTRAST_BEGIN;
    _MENU_SETUP_TFT_Contrast.bDefaultCursorPos = _MENU_SETUP_TFT_Contrast.bCheckMenuItemPos;

    //For TFT Contrast
    _MENU_SETUP_TFT_Saturation.bCheckMenuItemPos = __TFTPicAttr.bSaturation-SETUP_DISPLAY_TFT_SATURATION_BEGIN;
    _MENU_SETUP_TFT_Saturation.bDefaultCursorPos = _MENU_SETUP_TFT_Saturation.bCheckMenuItemPos;

    _MENU_SETUP_Custom_OSD.bCheckMenuItemPos = __SetupInfo.bOSDLanguage-SETUP_CUSTOM_OSD_BEGIN;
    _MENU_SETUP_Custom_OSD.bDefaultCursorPos = _MENU_SETUP_Custom_OSD.bCheckMenuItemPos;

//#ifdef SUPPORT_CHAR_ENCODING_SWITCH
    _MENU_SETUP_Custom_Encoding.bCheckMenuItemPos = __SetupInfo.bExSPSTLanguage-SETUP_LANGUAGE_ENCODING_BEGIN-1;
    _MENU_SETUP_Custom_Encoding.bDefaultCursorPos = _MENU_SETUP_Custom_Encoding.bCheckMenuItemPos;
//#endif

    _MENU_SETUP_Custom_UI_Style.bCheckMenuItemPos = __SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN;
    _MENU_SETUP_Custom_UI_Style.bDefaultCursorPos = _MENU_SETUP_Custom_UI_Style.bCheckMenuItemPos;

    DVDSETUP_GetRepeatMode();
    _MENU_SETUP_Custom_Repeat_Mode.bCheckMenuItemPos = __ImageFrameSetupInfo.bRepeatMode-SETUP_CUSTOM_REPEAT_MODE_BEGIN;
    _MENU_SETUP_Custom_Repeat_Mode.bDefaultCursorPos = _MENU_SETUP_Custom_Repeat_Mode.bCheckMenuItemPos;

#ifdef SUPPORT_CHOOSE_MEDIA
    _DVDSETUP_GetMediaStatus();
#endif //SUPPORT_CHOOSE_MEDIA

#else
    _MENU_DVDSETUP_Language_Menu.bCheckMenuItemPos = __SetupInfo.bMenuLanguage - SETUP_LANGUAGE_MENU_BEGIN;
    _MENU_DVDSETUP_Language_Menu.bDefaultCursorPos = _MENU_DVDSETUP_Language_Menu.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Language_Audio.bCheckMenuItemPos = __SetupInfo.bAudioLanguage - SETUP_LANGUAGE_AUDIO_BEGIN;
    _MENU_DVDSETUP_Language_Audio.bDefaultCursorPos = _MENU_DVDSETUP_Language_Audio.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Language_Subtitle.bCheckMenuItemPos = __SetupInfo.bSPSTLanguage - SETUP_LANGUAGE_SP_BEGIN;
    _MENU_DVDSETUP_Language_Subtitle.bDefaultCursorPos = _MENU_DVDSETUP_Language_Subtitle.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Audio_DigitalOut.bCheckMenuItemPos = __SetupInfo.bAudioDigitalOut - SETUP_AUDIO_OUTPUT_BEGIN;
    _MENU_DVDSETUP_Audio_DigitalOut.bDefaultCursorPos = _MENU_DVDSETUP_Audio_DigitalOut.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Audio_AnalogOut.bCheckMenuItemPos = __SetupInfo.bAudioAnalogOut - SETUP_AUDIO_ANALOG_OUT_BEGIN;
    _MENU_DVDSETUP_Audio_AnalogOut.bDefaultCursorPos = _MENU_DVDSETUP_Audio_AnalogOut.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Audio_Downmix.bCheckMenuItemPos = __SetupInfo.bAudioDownMix - SETUP_AUDIO_DOWNMIX_BEGIN;
    _MENU_DVDSETUP_Audio_Downmix.bDefaultCursorPos = _MENU_DVDSETUP_Audio_Downmix.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Audio_Center.bCheckMenuItemPos = __SetupInfo.bAudioCenter - SETUP_AUDIO_CENTER_BEGIN;
    _MENU_DVDSETUP_Audio_Center.bDefaultCursorPos = _MENU_DVDSETUP_Audio_Center.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Audio_Rear.bCheckMenuItemPos = __SetupInfo.bAudioRear - SETUP_AUDIO_REAR_BEGIN;
    _MENU_DVDSETUP_Audio_Rear.bDefaultCursorPos = _MENU_DVDSETUP_Audio_Rear.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Audio_Subwoofer.bCheckMenuItemPos = __SetupInfo.bAudioSubwoofer - SETUP_AUDIO_SUBWOOFER_BEGIN;
    _MENU_DVDSETUP_Audio_Subwoofer.bDefaultCursorPos = _MENU_DVDSETUP_Audio_Subwoofer.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Custom_ParentalControlLevel.bCheckMenuItemPos = __SetupInfo.bParentalCtl - SETUP_CUSTOM_PC_LEVEL_BEGIN;
    _MENU_DVDSETUP_Custom_ParentalControlLevel.bDefaultCursorPos = _MENU_DVDSETUP_Custom_ParentalControlLevel.bCheckMenuItemPos; 

    _MENU_DVDSETUP_Custom_AngleMark.bCheckMenuItemPos = __SetupInfo.bAngleMarkDisplay - SETUP_CUSTOM_ANGLE_MARK_BEGIN;
    _MENU_DVDSETUP_Custom_AngleMark.bDefaultCursorPos = _MENU_DVDSETUP_Custom_AngleMark.bCheckMenuItemPos; 
#endif
}

void _DVDSETUP_DrawAllUI(void)
{
	//Draw the background
	MAINMENU_DrawMainBackground();
    MAINMENU_DrawSelectionHelp(FALSE, FALSE);

#ifndef NO_TOOLBAR_SETUP_MENU
    if (_hDVDSETUPToolBar != TOOLBAR_HANDLE_INVALID)
    {
        TOOLBAR_ShowToolBar(_hDVDSETUPToolBar, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);
    }
#else //NO_TOOLBAR_SETUP_MENU
    _DVDSETUP_NoToolBarEntry();
#endif //NO_TOOLBAR_SETUP_MENU
   
}

//***************************************************************************
//  Function    :   DVDSETUP_ProcessKey
//  Abstract    :   When the menu is active, IR key will be sent to this menu's processing key function.
//  Arguments   :   none.
//  Return      :   MENU_ACTION_DONE: The key has been processed OK. If the key is invalid, it will be skipped by this function.
//                  MENU_ACTION_EXIT: The menu (sub-UI) exists.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
BYTE DVDSETUP_ProcessKey(BYTE bKey)
{
    WORD wReturn;
    BYTE bAction;
    DWORD dwMenuReturn;
    WORD wMenuAction;

    if (!__bSetupMode)
    {
        return MENU_ACTION_DONE;
    }

    if (_bDVDSETUPActiveMode == DVDSETUP_ACTIVE_MODE_MENU)
    {
        dwMenuReturn = MENU_ProcessKey(_hDVDSETUPMenu, bKey);
        wMenuAction = HIWORD(dwMenuReturn);
         printf("\n DVDSETUP_ACTIVE_MODE_MENU   ");
#ifndef NO_TOOLBAR_SETUP_MENU
        if (wMenuAction == MENU_ACTION_EXIT)
        {
            TOOLBAR_UpdateCursor(_hDVDSETUPToolBar, ICON_CURSOR_TYPE_HIGHLIGHT);
            _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_ICON;
		  printf("\n MENU_ACTION_EXIT   ");	
        }
        else if (wMenuAction == MENU_ACTION_PROCESS_MENU_ITEM)
        {
            _DVDSETUP_ProcessMenuItem(LOWORD(dwMenuReturn));
	       printf("\n MENU_ACTION_PROCESS_MENU_ITEM   ");	
        }
#else
if (wMenuAction == MENU_ACTION_PROCESS_MENU_ITEM)
        {
            _DVDSETUP_ProcessMenuItem(LOWORD(dwMenuReturn));
        }
#endif //NO_TOOLBAR_SETUP_MENU
    }
    else if (_bDVDSETUPActiveMode == DVDSETUP_ACTIVE_MODE_ICON)
    {
        wReturn = TOOLBAR_ProcessKey(_hDVDSETUPToolBar, bKey);
        bAction = HIBYTE(wReturn);

        if (bAction == ICON_ACTION_ACTIVATE_MENU)
        {
            if (MENU_AllDisableItems(_hDVDSETUPMenu))
            {
                TOOLBAR_UpdateCursor(_hDVDSETUPToolBar, ICON_CURSOR_TYPE_HIGHLIGHT);
                return MENU_ACTION_DONE;
            }

            MENU_SetMenuState(_hDVDSETUPMenu, TRUE);
            _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_MENU;
        }
        else if (bAction == ICON_ACTION_PROCESS_ICON)
        {
            if (LOBYTE(wReturn) == ICON_DVDSETUP_MAIN_EXIT)
            {
#ifdef SUPPORT_STB
                DVDSETUP_Exit(TRUE);
#else
                MAINMENU_ExitAll(TRUE, FALSE);
#endif //SUPPORT_STB
                return MENU_ACTION_EXIT;
            }
        }
    }
    else if (_bDVDSETUPActiveMode == DVDSETUP_ACTIVE_MODE_DIALOG)
    {
        SETTIME_ProcessKey(_hSETUPCLOCKSetTime, __bKey);
    }
    
    return MENU_ACTION_DONE;
}

//***************************************************************************
//  Function    :   DVDSETUP_Exit
//  Abstract    :   Each UI needs to offer the exit function. When user presses KEY_OPEN_CLOSE or KEY_POWER, 
//              :   the main menu will call the UI's exit function to exit the control.
//              :   Each sub-menu can reset your control or clear some flags here.
//  Arguments   :   none.
//  Return      :   none.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void DVDSETUP_Exit(BYTE bClearBackground)
{
    //Clear UI background.
    if (bClearBackground)
    {
        GDI_ClearRegion(0);
    }
    
#ifndef NO_TOOLBAR_SETUP_MENU
    //free the tool bar
    if (_hDVDSETUPToolBar != TOOLBAR_HANDLE_INVALID)
    {
        TOOLBAR_DeleteToolBar(_hDVDSETUPToolBar, FALSE);
        _hDVDSETUPToolBar = TOOLBAR_HANDLE_INVALID;
    }
#endif //NO_TOOLBAR_SETUP_MENU	
    
    //free the menu
    if (_hDVDSETUPMenu != MENU_HANDLE_INVALID)
    {
        MENU_DeleteMenu(_hDVDSETUPMenu, FALSE);
        _hDVDSETUPMenu = MENU_HANDLE_INVALID;
    }
    
    DVDSETUP_Exit_AdjustSetting();
}

//***************************************************************************
//  Function    :   DVDSETUP_Recover
//  Abstract    :   Each menu needs to offer the recovery function.  
//				:	When it is overwritten by screen saver, it will be recovered.
//  Arguments   :   none.
//  Return      :   none.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void DVDSETUP_Recover(void)
{
    _DVDSETUP_DrawAllUI();
}

void _DVDSETUP_IconPhotoSettingInitialFunc(BYTE bIconCursorType)
{
    //Draw the title
    MAINMENU_DrawTitleString(aMENUSetupMenu, aMENUPhotoSetting, NULL);
    
    if (_hDVDSETUPMenu == MENU_HANDLE_INVALID)
    {
        _hDVDSETUPMenu = MENU_CreateMenu(&_MENU_SETUP_Photo_Setting, DVDSETUP_MAX_VISIBLE_MENU_ITEM_NUM, _bDVDSETUPMenuExitKey);
    }
    
    _DVDSETUP_ShowMenu(_hDVDSETUPMenu, bIconCursorType);
}

void _DVDSETUP_IconAutoPlaySetupInitialFunc(BYTE bIconCursorType)
{
    //Draw the title
    MAINMENU_DrawTitleString(aMENUSetupMenu, aMENUAutoPlay, NULL);
    
    if (_hDVDSETUPMenu == MENU_HANDLE_INVALID)
    {
        _hDVDSETUPMenu = MENU_CreateMenu(&_MENU_SETUP_Auto_Play, DVDSETUP_MAX_VISIBLE_MENU_ITEM_NUM, _bDVDSETUPMenuExitKey);
    }
    
    _DVDSETUP_ShowMenu(_hDVDSETUPMenu, bIconCursorType);
}


void _DVDSETUP_IconDisplaySetupInitialFunc(BYTE bIconCursorType)
{
    //Draw the title
    MAINMENU_DrawTitleString(aMENUSetupMenu, aMENUDisplaySetup, NULL);
    
    if (_hDVDSETUPMenu == MENU_HANDLE_INVALID)
    {
        _hDVDSETUPMenu = MENU_CreateMenu(&_MENU_SETUP_Display, DVDSETUP_MAX_VISIBLE_MENU_ITEM_NUM, _bDVDSETUPMenuExitKey);
    }
    
    _DVDSETUP_ShowMenu(_hDVDSETUPMenu, bIconCursorType);
}

void _DVDSETUP_IconCustomSetupInitialFunc(BYTE bIconCursorType)
{
    //Draw the title
    MAINMENU_DrawTitleString(aMENUSetupMenu, aMENUCustomSetup, NULL);
    
    if (_hDVDSETUPMenu == MENU_HANDLE_INVALID)
    {
        _hDVDSETUPMenu = MENU_CreateMenu(&_MENU_SETUP_Custom, DVDSETUP_MAX_VISIBLE_MENU_ITEM_NUM, _bDVDSETUPMenuExitKey);
    }
    
#ifdef SUPPORT_JPEG_AUDIO_PLAYING
    if (__bMMJPEGAudioMode)
    {
        MENU_SetMenuItemState(_hDVDSETUPMenu, SETUP_CUSTOM_REPEAT_MODE-SETUP_CUSTOM_BEGIN, MENU_ITEM_STATE_DISABLE);        
    }
    else
    {
        MENU_SetMenuItemState(_hDVDSETUPMenu, SETUP_CUSTOM_REPEAT_MODE-SETUP_CUSTOM_BEGIN, MENU_ITEM_STATE_ENABLE);
    }
#endif //SUPPORT_JPEG_AUDIO_PLAYING
    
    _DVDSETUP_ShowMenu(_hDVDSETUPMenu, bIconCursorType);
    
    if (_bDVDSETUPActiveMode == DVDSETUP_ACTIVE_MODE_DIALOG)
    {
        SETTIME_Recover(_hSETUPCLOCKSetTime);
        
#ifdef NL_SOLUTION
        MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        MAINMENU_DrawSelectionHelp(TRUE, FALSE);
#endif //NL_SOLUTION
    }
}

void _DVDSETUP_IconExitFunc(void)
{
    //Clear the title background
    MAINMENU_DrawMainTitleBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    
    if (_hDVDSETUPMenu != MENU_HANDLE_INVALID)
    {
        MENU_DeleteMenu(_hDVDSETUPMenu, TRUE);
        _hDVDSETUPMenu = MENU_HANDLE_INVALID;
    }
}

void _DVDSETUP_IconExitInitialFunc(BYTE bIconCursorType)
{
    //Draw the title
    MAINMENU_DrawTitleString(aMENUSetupMenu, aMENUIconExit, NULL);
}

void _DVDSETUP_ShowMenu(HMENU hMenu, BYTE bIconCursorType)
{
    if (bIconCursorType == ICON_CURSOR_TYPE_PRESSED)
    {
        MENU_ShowMenu(hMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);
    }
    else
    {
        MENU_ShowMenu(hMenu, FALSE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);
    }
}

void _DVDSETUP_ProcessMenuItem(BYTE bID)
{
#ifdef IMAGE_FRAME_SETUP
    if ((SETUP_PHOTO_SETTING_INTERVAL_TIME_BEGIN <= bID) && (SETUP_PHOTO_SETTING_INTERVAL_TIME_END >= bID))
    {
        __ImageFrameSetupInfo.bPhotoIntervalTime = bID;
        DVDSETUP_SetSlideShowIntervalTime();
        DVDSETUP_SaveDataToSetupInfo(SETUP_PHOTO_SETTING_INTERVAL_TIME);
    }
    else if ((SETUP_PHOTO_SETTING_TRANSITION_EFFECT_BEGIN <= bID) && (SETUP_PHOTO_SETTING_TRANSITION_EFFECT_END >= bID))
    {
        __ImageFrameSetupInfo.bTransitionEffect = bID;
        DVDSETUP_SetSlideShowEffect();
        DVDSETUP_SaveDataToSetupInfo(SETUP_PHOTO_SETTING_TRANSITION_EFFECT);
    }
    else if ((SETUP_PHOTO_SETTING_BROWSE_BEGIN <= bID) && (SETUP_PHOTO_SETTING_BROWSE_END >= bID))
    {
        __ImageFrameSetupInfo.bBrowse = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_PHOTO_SETTING_BROWSE);
    }
    else if ((SETUP_AUTO_PLAY_PHOTO_BEGIN <= bID) && (SETUP_AUTO_PLAY_PHOTO_END >= bID))
    {
        __ImageFrameSetupInfo.bAutoPlayPhoto = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUTO_PLAY_PHOTO);
    }
    else if ((SETUP_AUTO_PLAY_MUSIC_BEGIN <= bID) && (SETUP_AUTO_PLAY_MUSIC_END >= bID))
    {
        __ImageFrameSetupInfo.bAutoPlayMusic = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUTO_PLAY_MUSIC);
    }
    else if ((SETUP_AUTO_PLAY_PHOTO_PLUS_MUSIC_BEGIN <= bID) && (SETUP_AUTO_PLAY_PHOTO_PLUS_MUSIC_END >= bID))
    {
        __ImageFrameSetupInfo.bAutoPlayPhotoPlusMusic = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUTO_PLAY_PHOTO_PLUS_MUSIC);
    }
    else if ((SETUP_AUTO_PLAY_MOVIE_BEGIN <= bID) && (SETUP_AUTO_PLAY_MOVIE_END >= bID))
    {
        __ImageFrameSetupInfo.bAutoPlayMovie = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUTO_PLAY_MOVIE);
    }
 #ifdef SUPPORT_DSPLAY_16_9_AND_AUTO
    else if ((SETUP_DISPLAY_TYPE_BEGIN <= bID) && (SETUP_DISPLAY_TYPE_END >= bID))
    {
        __ImageFrameSetupInfo.bDisplayType = bID;
	 printf("\n set   __ImageFrameSetupInfo.bDisplayType  position %d",bID);	
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_DISPLAY_TYPE);
	  _SetDisplayToIC();
    }

  #endif
    else if ((SETUP_DISPLAY_TV_STANDARD_BEGIN <= bID) && (SETUP_DISPLAY_TV_STANDARD_END >= bID))
    {
		__bTVSystemChange = TRUE;
        __SetupInfo.bTVSystem = bID;
		 printf("\n set   __SetupInfo.bTVSystem  position %d",bID);
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_TV_STANDARD);
    }
#ifdef SUPPORT_TFT_DIGITAL_MODE
    else if ((SETUP_DISPLAY_AV_OUTPUT_BEGIN <= bID) && (SETUP_DISPLAY_AV_OUTPUT_END >= bID))
    {
        _DVDSETUP_SetAVOutput(bID);
    }
#endif //SUPPORT_TFT_DIGITAL_MODE
    else if ((SETUP_DISPLAY_BRIGHTNESS_BEGIN <= bID) && (SETUP_DISPLAY_BRIGHTNESS_END >= bID))
    {
        __SetupInfo.bBrightness = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_BRIGHTNESS);
    }
    else if ((SETUP_DISPLAY_CONTRAST_BEGIN <= bID) && (SETUP_DISPLAY_CONTRAST_END >= bID))
    {
        __SetupInfo.bContrast = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_CONTRAST);
    }
    else if ((SETUP_DISPLAY_TFT_BRIGHTNESS_BEGIN <= bID) && (SETUP_DISPLAY_TFT_BRIGHTNESS_END >= bID))
    {
        __TFTPicAttr.bBrightness = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_TFT_BRIGHTNESS);
    }
    else if ((SETUP_DISPLAY_TFT_CONTRAST_BEGIN <= bID) && (SETUP_DISPLAY_TFT_CONTRAST_END >= bID))
    {
        __TFTPicAttr.bContrast = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_TFT_CONTRAST);
    }
    else if ((SETUP_DISPLAY_TFT_SATURATION_BEGIN <= bID) && (SETUP_DISPLAY_TFT_SATURATION_END >= bID))
    {
        __TFTPicAttr.bSaturation = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_DISPLAY_TFT_SATURATION);
    }
    else if ((SETUP_CUSTOM_OSD_BEGIN <= bID) && (SETUP_CUSTOM_OSD_END >= bID))
    {
        __SetupInfo.bOSDLanguage = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_CUSTOM_OSD);
        
        _DVDSETUP_ChangeOSDLanguage();            
    }
//#ifdef SUPPORT_CHAR_ENCODING_SWITCH
    else if ((SETUP_LANGUAGE_ENCODING_BEGIN <= bID) && (SETUP_LANGUAGE_ENCODING_END-1 >= bID))
    {
        __SetupInfo.bExSPSTLanguage = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_LANGUAGE_ENCODING);
    }
//#endif
    else if ((SETUP_CUSTOM_UI_STYLE_BEGIN <= bID) && (SETUP_CUSTOM_UI_STYLE_END >= bID))
    {
        GDI_ClearRegion(0);
        
        __SetupInfo.bUIStyle = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_CUSTOM_UI_STYLE);
        
        //re-draw all UI.
        MAINMENU_InitializeGDI(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        _DVDSETUP_DrawAllUI();
    }
    else if (bID == SETUP_CUSTOM_DEFAULT_READ)
    {
        DVDSETUP_SetDefaultState(); 
        MAINMENU_InitializeGDI(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        _DVDSETUP_InitializeSetting();
        _DVDSETUP_ChangeOSDLanguage();   
    }
    else if ((SETUP_CUSTOM_REPEAT_MODE_BEGIN <= bID) && (SETUP_CUSTOM_REPEAT_MODE_END >= bID))
    {
        __ImageFrameSetupInfo.bRepeatMode = bID;
		DVDSETUP_SaveDataToSetupInfo(SETUP_CUSTOM_REPEAT_MODE);
        DVDSETUP_SetRepeatMode();
    }
#ifdef SUPPORT_CHOOSE_MEDIA
    else if ((SETUP_CUSTOM_MEDIA_BEGIN <= bID) && (SETUP_CUSTOM_MEDIA_END >= bID))
    {
        _DVDSETUP_SetMediaStatus(bID);
    }
#endif //SUPPORT_CHOOSE_MEDIA
#ifdef NL_SOLUTION
    else if (bID == SETUP_CUSTOM_SETUP_TIME)
    {
        MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        MAINMENU_DrawSelectionHelp(TRUE, FALSE);

        _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_DIALOG;
        RTC_GetTime(&_CLOCKCurrnetRTCTime);
        _SETUP_CLOCK_SET_TIME.bHour = _CLOCKCurrnetRTCTime.bHr;
        _SETUP_CLOCK_SET_TIME.bMinute = _CLOCKCurrnetRTCTime.bMin;
        _SETUP_CLOCK_SET_TIME.bSecond = _CLOCKCurrnetRTCTime.bSec;
        _hSETUPCLOCKSetTime  = SETTIME_Create(&_SETUP_CLOCK_SET_TIME, &_SETUP_CLOCK_SET_TIME_Action);
        if (_hSETUPCLOCKSetTime != SETTIME_HANDLE_INVALID)
        {
            SETTIME_Show(_hSETUPCLOCKSetTime, MAINMENU_PALETTE_ENTRY_CONTENT_REGION, __SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
        }
    }
#endif //NL_SOLUTION

#ifdef NO_TOOLBAR_SETUP_MENU
    _DVDSETUP_NoToolBarExit(bID);
#endif //NO_TOOLBAR_SETUP_MENU

#else
    if (SETUP_LANGUAGE_MENU_BEGIN <= bID && SETUP_LANGUAGE_MENU_END >= bID)
    {
        __SetupInfo.bMenuLanguage = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_LANGUAGE_MENU);
    }
    else if (SETUP_LANGUAGE_AUDIO_BEGIN <= bID && SETUP_LANGUAGE_AUDIO_END >= bID)
    {
        __SetupInfo.bAudioLanguage = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_LANGUAGE_AUDIO);
    }
    else if (SETUP_LANGUAGE_SP_BEGIN <= bID && SETUP_LANGUAGE_SP_END >= bID)
    {
        __SetupInfo.bSPSTLanguage = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_LANGUAGE_SUBTITLE);
    }
    else if (SETUP_LANGUAGE_EX_SP_BEGIN <= bID && SETUP_LANGUAGE_EX_SP_END >= bID)
    {
        __SetupInfo.bExSPSTLanguage = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_LANGUAGE_EX_SUBTITLE);
    }
    else if (SETUP_AUDIO_DIGITAL_OUT_BEGIN <= bID && SETUP_AUDIO_DIGITAL_OUT_END >= bID)
    {
        __SetupInfo.bAudioDigitalOut = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUDIO_DIGITAL_OUT);
    }
    else if (SETUP_AUDIO_ANALOG_OUT_BEGIN <= bID && SETUP_AUDIO_ANALOG_OUT_END >= bID)
    {
        __SetupInfo.bAudioAnalogOut = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUDIO_ANALOG_OUT);
    }
    else if (SETUP_AUDIO_DOWNMIX_BEGIN <= bID && SETUP_AUDIO_DOWNMIX_END >= bID)
    {
        __SetupInfo.bAudioDownMix = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUDIO_DOWNMIX);
    }
    else if (SETUP_AUDIO_CENTER_BEGIN <= bID && SETUP_AUDIO_CENTER_END >= bID)
    {
        __SetupInfo.bAudioCenter = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUDIO_CENTER);
    }
    else if (SETUP_AUDIO_REAR_BEGIN <= bID && SETUP_AUDIO_REAR_END >= bID)
    {
        __SetupInfo.bAudioRear = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUDIO_REAR);
    }
    else if (SETUP_AUDIO_SUBWOOFER_BEGIN <= bID && SETUP_AUDIO_SUBWOOFER_END >= bID)
    {
        __SetupInfo.bAudioSubwoofer = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_AUDIO_SUBWOOFER);
    }
    else if (SETUP_CUSTOM_PC_LEVEL_BEGIN <= bID && SETUP_CUSTOM_PC_LEVEL_END >= bID)
    {
        __bMAINMENUDlgIndex = DS_PIN_DLG_DVD_PARENTAL_LEVEL;
        MAINMENU_DlgInitial();
    }
    else if (SETUP_CUSTOM_ANGLE_MARK_BEGIN <= bID && SETUP_CUSTOM_ANGLE_MARK_END >= bID)
    {
        __SetupInfo.bAngleMarkDisplay = bID;
        DVDSETUP_SaveDataToSetupInfo(SETUP_CUSTOM_ANGLE_MARK);
    }
#ifndef NO_DRM
    else if (SETUP_CUSTOM_DIVX_VOD == bID)
    {
        OSDDIVX_DrawDialog(DLG_REG_CODE);
    }
#endif //NO_DRM
    
#endif //#ifdef IMAGE_FRAME_SETUP
}

#ifdef IMAGE_FRAME_SETUP
void DVDSETUP_SetSlideShowIntervalTime(void)
{
#ifndef MUSTEK_SETUP_MENU
    switch (__ImageFrameSetupInfo.bPhotoIntervalTime)
    {
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_5_SEC:
        __dwFMJPGShowTime = 5;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_10_SEC:
        __dwFMJPGShowTime = 10;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_15_SEC:
        __dwFMJPGShowTime = 15;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_30_SEC:
        __dwFMJPGShowTime = 30;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_1_MIN:
        __dwFMJPGShowTime = 60;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_3_MIN:
        __dwFMJPGShowTime = 180;
        break;
    default:
        break;
    }
#else
switch (__ImageFrameSetupInfo.bPhotoIntervalTime)
    {
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_15_SEC:
        __dwFMJPGShowTime = 15;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_30_SEC:
        __dwFMJPGShowTime = 30;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_1_MIN:
        __dwFMJPGShowTime = 60;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_5_MIN:
        __dwFMJPGShowTime = 300;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_30_MIN:
        __dwFMJPGShowTime = 1800;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_1_HOUR:
        __dwFMJPGShowTime = 3600;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_4_HOUR:
        __dwFMJPGShowTime = 14400;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_12_HOUR:
        __dwFMJPGShowTime = 43200;
        break;
    case SETUP_PHOTO_SETTING_INTERVAL_TIME_1_DAY:
        __dwFMJPGShowTime = 86400;
        break;
    default:
        break;
    }
#endif //MUSTEK_SETUP_MENU
}

void DVDSETUP_SetSlideShowEffect(void)
{
#ifndef MUSTEK_SETUP_MENU
    switch (__ImageFrameSetupInfo.bTransitionEffect)
    {
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_COVER_UP:
        __bMMEffectIdx = HALJPEG_EFFECT_COVER_UP;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_COVER_DOWN:
        __bMMEffectIdx = HALJPEG_EFFECT_COVER_DOWN;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_COVER_LEFT:
        __bMMEffectIdx = HALJPEG_EFFECT_COVER_LEFT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_COVER_RIGHT:
        __bMMEffectIdx = HALJPEG_EFFECT_COVER_RIGHT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_UNCOVER_UP:
        __bMMEffectIdx = HALJPEG_EFFECT_UNCOVER_UP;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_UNCOVER_DOWN:
        __bMMEffectIdx = HALJPEG_EFFECT_UNCOVER_DOWN;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_UNCOVER_LEFT:
        __bMMEffectIdx = HALJPEG_EFFECT_UNCOVER_LEFT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_UNCOVER_RIGHT:
        __bMMEffectIdx = HALJPEG_EFFECT_UNCOVER_RIGHT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_WIPE_UP:
        __bMMEffectIdx = HALJPEG_EFFECT_WIPE_UP;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_WIPE_DOWN:
        __bMMEffectIdx = HALJPEG_EFFECT_WIPE_DOWN;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_WIPE_LEFT:
        __bMMEffectIdx = HALJPEG_EFFECT_WIPE_LEFT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_WIPE_RIGHT:
        __bMMEffectIdx = HALJPEG_EFFECT_WIPE_RIGHT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_PUSH_UP:
        __bMMEffectIdx = HALJPEG_EFFECT_PUSH_UP;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_PUSH_DOWN:
        __bMMEffectIdx = HALJPEG_EFFECT_PUSH_DOWN;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_PUSH_LEFT:
        __bMMEffectIdx = HALJPEG_EFFECT_PUSH_LEFT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_PUSH_RIGHT:
        __bMMEffectIdx = HALJPEG_EFFECT_PUSH_RIGHT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_RANDOM:
        __bMMEffectIdx = HALJPEG_EFFECT_RANDOM;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_OFF:
    default:
        __bMMEffectIdx = HALJPEG_EFFECT_NONE;
        break;
    }
#else
    switch (__ImageFrameSetupInfo.bTransitionEffect)
    {
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_COVER_UP:
        __bMMEffectIdx = HALJPEG_EFFECT_COVER_UP;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_COVER_DOWN:
        __bMMEffectIdx = HALJPEG_EFFECT_COVER_DOWN;
        break;       
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_UNCOVER_LEFT:
        __bMMEffectIdx = HALJPEG_EFFECT_UNCOVER_LEFT;
        break;  
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_WIPE_UP:
        __bMMEffectIdx = HALJPEG_EFFECT_WIPE_UP;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_WIPE_DOWN:
        __bMMEffectIdx = HALJPEG_EFFECT_WIPE_DOWN;
        break;       
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_PUSH_RIGHT:
        __bMMEffectIdx = HALJPEG_EFFECT_PUSH_RIGHT;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_RANDOM:
        __bMMEffectIdx = HALJPEG_EFFECT_RANDOM;
        break;
    case SETUP_PHOTO_SETTING_TRANSITION_EFFECT_OFF:
    default:
        __bMMEffectIdx = HALJPEG_EFFECT_NONE;
        break;
    }
#endif //MUSTEK_SETUP_MENU
}

void DVDSETUP_SetRepeatMode(void)
{
    switch (__ImageFrameSetupInfo.bRepeatMode)
    {
    case SETUP_CUSTOM_REPEAT_MODE_OFF:
        __bRepeat = REPEAT_NONE;
        break;
    case SETUP_CUSTOM_REPEAT_MODE_ONE:
        __bRepeat = REPEAT_TRACK;
        break;
    case SETUP_CUSTOM_REPEAT_MODE_FOLDER:
        __bRepeat = REPEAT_FOLDER;
        break;
    case SETUP_CUSTOM_REPEAT_MODE_ALL:
        __bRepeat = REPEAT_DISC;
        break;
    default:
        break;
    }
}

void DVDSETUP_GetRepeatMode(void)
{
    switch (__bRepeat)
    {
    case REPEAT_NONE:
        __ImageFrameSetupInfo.bRepeatMode = SETUP_CUSTOM_REPEAT_MODE_OFF;
        break;
    case REPEAT_TRACK:
        __ImageFrameSetupInfo.bRepeatMode = SETUP_CUSTOM_REPEAT_MODE_ONE;
        break;
    case REPEAT_FOLDER:
        __ImageFrameSetupInfo.bRepeatMode = SETUP_CUSTOM_REPEAT_MODE_FOLDER;
        break;
    case REPEAT_DISC:
        __ImageFrameSetupInfo.bRepeatMode = SETUP_CUSTOM_REPEAT_MODE_ALL;
        break;
    }
}

void _DVDSETUP_ChangeOSDLanguage(void)
{
    MENU_ClearMenuBackground(MENU_MAIN_MENU, _hDVDSETUPMenu);
    MENU_ClearMenuBackground(MENU_SUB_MENU, _hDVDSETUPMenu);
    
    SETUP_LoadLanguage();       
    
    //re-draw all UI.
    MAINMENU_DrawMainTitleBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    MAINMENU_DrawTitleString(aMENUSetupMenu, aMENUCustomSetup, NULL);
    MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
    MENU_ShowMenu(_hDVDSETUPMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);        
}

#ifdef SUPPORT_TFT_DIGITAL_MODE
void _DVDSETUP_SetAVOutput(BYTE bID)
{
    switch (bID)
    {
    case SETUP_DISPLAY_AV_OUTPUT_TV:
        __bTFTOutput = FALSE;
        break;
    case SETUP_DISPLAY_AV_OUTPUT_PANEL:
    default:
        __bTFTOutput = TRUE;
        break;
    }

    HAL_AdjustTFT_DigitalMode(__bTFTOutput);
}
#endif //SUPPORT_TFT_DIGITAL_MODE

#ifdef SUPPORT_CHOOSE_MEDIA
void _DVDSETUP_GetMediaStatus(void)
{
    BYTE bMenuItemIndex;
    DWORD dwMediaDevice;

    dwMediaDevice = MEDIA_QueryDevice();

    for (bMenuItemIndex = 0; bMenuItemIndex < (SETUP_CUSTOM_MEDIA_END-SETUP_CUSTOM_MEDIA_BEGIN+1); bMenuItemIndex++)
    {
        _MENU_ITEM_SETUP_Custom_Media[bMenuItemIndex].bAttribute = MENU_ITEM_STATE_DISABLE;
    }

    //Enalbe or disable the items according to Media's status.
    if (dwMediaDevice & MEDIA_DEVICE_USB)
    {
        _MENU_ITEM_SETUP_Custom_Media[SETUP_CUSTOM_MEDIA_USB-SETUP_CUSTOM_MEDIA_BEGIN].bAttribute = MENU_ITEM_STATE_ENABLE;
    }

    if (dwMediaDevice & MEDIA_DEVICE_SD)
    {
        _MENU_ITEM_SETUP_Custom_Media[SETUP_CUSTOM_MEDIA_SD-SETUP_CUSTOM_MEDIA_BEGIN].bAttribute = MENU_ITEM_STATE_ENABLE;
    }

    if (dwMediaDevice & MEDIA_DEVICE_MS)
    {
        _MENU_ITEM_SETUP_Custom_Media[SETUP_CUSTOM_MEDIA_MS-SETUP_CUSTOM_MEDIA_BEGIN].bAttribute = MENU_ITEM_STATE_ENABLE;
    }

    if (dwMediaDevice & MEDIA_DEVICE_MMC)
    {
        _MENU_ITEM_SETUP_Custom_Media[SETUP_CUSTOM_MEDIA_MMC-SETUP_CUSTOM_MEDIA_BEGIN].bAttribute = MENU_ITEM_STATE_ENABLE;
    }

    if (dwMediaDevice & MEDIA_DEVICE_CF)
    {
        _MENU_ITEM_SETUP_Custom_Media[SETUP_CUSTOM_MEDIA_CF-SETUP_CUSTOM_MEDIA_BEGIN].bAttribute = MENU_ITEM_STATE_ENABLE;
    }

    if (dwMediaDevice & MEDIA_DEVICE_XD)
    {
        _MENU_ITEM_SETUP_Custom_Media[SETUP_CUSTOM_MEDIA_XD-SETUP_CUSTOM_MEDIA_BEGIN].bAttribute = MENU_ITEM_STATE_ENABLE;
    }

    dwMediaDevice = MEDIA_QueryDefaultDevice();

    if (dwMediaDevice == 0)
    {
        _MENU_SETUP_Custom_Media.bCheckMenuItemPos = MENU_ITEM_NO_CHECKED_ITEM;
    }
    else
    {        
        if (dwMediaDevice & MEDIA_DEVICE_USB)
        {
            _MENU_SETUP_Custom_Media.bCheckMenuItemPos = SETUP_CUSTOM_MEDIA_USB-SETUP_CUSTOM_MEDIA_BEGIN;
        }
        else if (dwMediaDevice & MEDIA_DEVICE_SD)
        {
            _MENU_SETUP_Custom_Media.bCheckMenuItemPos = SETUP_CUSTOM_MEDIA_SD-SETUP_CUSTOM_MEDIA_BEGIN;
        }
        else if (dwMediaDevice & MEDIA_DEVICE_MS)
        {
            _MENU_SETUP_Custom_Media.bCheckMenuItemPos = SETUP_CUSTOM_MEDIA_MS-SETUP_CUSTOM_MEDIA_BEGIN;
        }
        else if (dwMediaDevice & MEDIA_DEVICE_MMC)
        {
            _MENU_SETUP_Custom_Media.bCheckMenuItemPos = SETUP_CUSTOM_MEDIA_MMC-SETUP_CUSTOM_MEDIA_BEGIN;
        }
        else if (dwMediaDevice & MEDIA_DEVICE_CF)
        {
            _MENU_SETUP_Custom_Media.bCheckMenuItemPos = SETUP_CUSTOM_MEDIA_CF-SETUP_CUSTOM_MEDIA_BEGIN;
        }
        else if (dwMediaDevice & MEDIA_DEVICE_XD)
        {
            _MENU_SETUP_Custom_Media.bCheckMenuItemPos = SETUP_CUSTOM_MEDIA_XD-SETUP_CUSTOM_MEDIA_BEGIN;
        }

        _MENU_SETUP_Custom_Media.bDefaultCursorPos = _MENU_SETUP_Custom_Media.bCheckMenuItemPos;
    }
}

void _DVDSETUP_SetMediaStatus(BYTE bID)
{
    switch (bID)
    {
    case SETUP_CUSTOM_MEDIA_SD:
        MEIDA_SetDefaultDevice(MEDIA_DEVICE_SD);
        break;
    case SETUP_CUSTOM_MEDIA_MS:
        MEIDA_SetDefaultDevice(MEDIA_DEVICE_MS);
        break;
    case SETUP_CUSTOM_MEDIA_MMC:
        MEIDA_SetDefaultDevice(MEDIA_DEVICE_MMC);
        break;
    case SETUP_CUSTOM_MEDIA_CF:
        MEIDA_SetDefaultDevice(MEDIA_DEVICE_CF);
        break;
    case SETUP_CUSTOM_MEDIA_XD:
        MEIDA_SetDefaultDevice(MEDIA_DEVICE_XD);
        break;
    case SETUP_CUSTOM_MEDIA_USB:
        MEIDA_SetDefaultDevice(MEDIA_DEVICE_USB);
    default:
        break;
    }
}

void DVDSETUP_UpdateMediaStatus(void)
{
    BYTE bMenuID;

    _DVDSETUP_GetMediaStatus();

    if ((_hDVDSETUPMenu != MENU_HANDLE_INVALID) && (!_bOSDSSScreenSaverMode))
    {
        bMenuID = MENU_GetCurrentItemID(_hDVDSETUPMenu);
        
        if ((bMenuID == SETUP_CUSTOM_MEDIA) || ((bMenuID >= SETUP_CUSTOM_MEDIA_BEGIN) && (bMenuID <= SETUP_CUSTOM_MEDIA_END)))
        {
            //update the menu.
            MENU_ClearMenuBackground(MENU_SUB_MENU, _hDVDSETUPMenu);
            MENU_ShowMenu(_hDVDSETUPMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);
        }
    }
}

#endif //SUPPORT_CHOOSE_MEDIA

#endif //IMAGE_FRAME_SETUP


/////////////////////  Public function //////////////////////////////////////////////////

// The following functions are copied from setup.c
void _InitRect(PURECT pRect, WORD wLeft, WORD wTop, WORD wRight, WORD wBottom)
{
    pRect->wLeft   = wLeft;
    pRect->wRight  = wRight;
    pRect->wTop    = wTop;
    pRect->wBottom = wBottom;
}


#ifdef SUPPORT_STB

void LanguageCode_Initial(BYTE bBackgroundColor)
{
#ifndef NO_DISC_MODE
    BYTE _bPINDLGBackgroundColor;
    BYTE _bPINMode = FALSE;


    _bPINMode = TRUE;

    _bLanguageCodeDlgCursorPos = LANGUAGE_CODE_DIALOG_CURSOR_POS_EDIT;

    _PIN_InitializeGDI();

    _hLanguageCodeDlg = DIALOG_CreateDialog(&_LanguageCodeDlg);
    if (_hLanguageCodeDlg != DIALOG_HANDLE_INVALID)
    {
        DIALOG_ShowDialog(_hLanguageCodeDlg, bBackgroundColor, FALSE);

        _LanguageCode_ShowEditText();

        _hLanguageCodeDlgEdit = EDITBOX_CreateEditBox(&_LanguageCodeEdit);
        if (_hLanguageCodeDlgEdit != EDITBOX_HANDLE_INVALID)
        {
            EDITBOX_ShowEditBox(_hLanguageCodeDlgEdit, TRUE);
        }

        _bPINDLGBackgroundColor = bBackgroundColor;
    }
#endif
}

BYTE LanguageCode_ProcessKey(BYTE bKey)
{
#ifndef NO_DISC_MODE
    WORD wReturn;

    switch (_bLanguageCodeDlgCursorPos)
    {
    case LANGUAGE_CODE_DIALOG_CURSOR_POS_EDIT:
        wReturn = EDITBOX_Processkey(_hLanguageCodeDlgEdit, bKey);
        if (EDITBOX_ACTION_EXCEED_RIGHT == wReturn)
        {
            EDITBOX_DrawDigit(_hLanguageCodeDlgEdit, _LanguageCodeEdit.bDigitNum - 1,
                _LanguageCodeEdit.bBackgroundColor);
            DIALOG_SetCursor(_hLanguageCodeDlg, PIN_DIALOG_BUTTON_ID_OK);
            _bLanguageCodeDlgCursorPos = LANGUAGE_CODE_DIALOG_CURSOR_POS_BUTTON;
        }
        else if (EDITBOX_ACTION_EXCEED_DOWN == wReturn)
        {
            _bLanguageCodeDlgCursorPos = LANGUAGE_CODE_DIALOG_CURSOR_POS_BUTTON;
            DIALOG_SetCursor(_hLanguageCodeDlg, PIN_DIALOG_BUTTON_ID_CANCEL);
        }
        break;
    case LANGUAGE_CODE_DIALOG_CURSOR_POS_BUTTON:
        wReturn = DIALOG_Processkey(_hLanguageCodeDlg, bKey);
        if (wReturn == DIALOG_ACTION_NONE)
        {
            if (bKey == KEY_UP)
            {
                DIALOG_ClearCursor(_hLanguageCodeDlg);
                _bLanguageCodeDlgCursorPos = LANGUAGE_CODE_DIALOG_CURSOR_POS_EDIT;
                _LanguageCodeEdit.wDefaultNumber = EDITBOX_INVALID_PASSWORD_NUM;
                EDITBOX_ShowEditBox(_hLanguageCodeDlgEdit, TRUE);
            }
        }
        else if (wReturn & DIALOG_ACTION_BUTTON_PRESSED)
        {
            if (LOBYTE(wReturn) == PIN_DIALOG_BUTTON_ID_OK)
            {
                if (_LanguageCodeEdit.wDefaultNumber == EDITBOX_INVALID_PASSWORD_NUM)
                {
                    //The password is not completed. Show "invalid password" message box.
                    //_PINDLG_ShowMessageBox(aMENUPINInvalidPassword, _bPINDLGBackgroundColor);
                    //return DLG_ACTION_EXIT_RECOVER_NENU;
                }
                else
                {
                    //Check if the password is right?
                    _wInputCode = EDITBOX_GetNumber(_hLanguageCodeDlgEdit);
                    _wInputCode = _GetLanguageNumber();

                    if (_DlgFlowTable[__bMAINMENUDlgIndex].Dlg_Call_Back_Ptr != NULL)
                    {
                        _DlgFlowTable[__bMAINMENUDlgIndex].Dlg_Call_Back_Ptr(TRUE);
                    }

                    LanguageCode_Exit();
                    return DLG_ACTION_EXIT_NEW_NENU;
                }
            }
            else
            {
                LanguageCode_Exit();
                return DLG_ACTION_EXIT_RECOVER_NENU;
            }

        }

        break;
    }

#endif //#ifndef NO_DISC_MODE
    return DLG_ACTION_DONE;
}


void LanguageCode_Exit()
{
#ifndef NO_DISC_MODE
    _LanguageCodeEdit.wDefaultNumber = EDITBOX_INVALID_PASSWORD_NUM;

    if (DIALOG_HANDLE_INVALID != _hLanguageCodeDlg)
    {
        DIALOG_DeleteDialog(_hLanguageCodeDlg, TRUE);
        _hLanguageCodeDlg = DIALOG_HANDLE_INVALID;
    }

    if (EDITBOX_HANDLE_INVALID != _hLanguageCodeDlgEdit)
    {
        EDITBOX_DeleteEditBox(_hLanguageCodeDlgEdit);
        _hLanguageCodeDlgEdit = EDITBOX_HANDLE_INVALID;
    }
#endif
}

void _LanguageCode_ShowEditText(void)
{
#ifndef NO_DISC_MODE
    PARM_DRAW_STRING StringAttr;

    //Draw TV list title string
    StringAttr.bBackground = 1;
    StringAttr.bColorKey = 1;
    StringAttr.bShadow1 = 1;
    StringAttr.bShadow2 = 1;
    StringAttr.bTextColor = PIN_DIALOG_PALETTE_ENTRY_EDIT_TEXT;


    StringAttr.wX = LANGUAGE_CODE_DIALOG_EDIT_START_X - LANGUAGE_CODE_DIALOG_EDIT_TEXT_EDIT_DISTANCE_H -
                    GDI_GetStringWidth_909(aMENUEnterCode);
    StringAttr.wY = LANGUAGE_CODE_DIALOG_EDIT_START_Y +
                    ((LANGUAGE_CODE_DIALOG_EDIT_HEIGHT - GDI_GetStringHeight(aMENUEnterCode)) >> 1);
    GDI_DrawString_909(0, &StringAttr, aMENUEnterCode);
#endif //#ifndef NO_DISC_MODE
}

void LanguageCode_Callback(BYTE bSuccess)
{
#ifndef NO_DISC_MODE
    if (!bSuccess) return;

    switch (_bCurMenuID)
    {
    case SETUP_LANGUAGE_MENU_OTHERS:
        //Save the menu language code to __SetupInfo
        __SetupInfo.wMenuLanguageCode = _wInputCode;
        HAL_WriteStorage(SETUP_ADDR_MENU_LANGUAGE_CODE, (BYTE *)&(__SetupInfo.wMenuLanguageCode), 2);
        break;
    case SETUP_LANGUAGE_AUDIO_OTHERS:
        //Save the menu language code to __SetupInfo
        __SetupInfo.wAudioLanguageCode = _wInputCode;
        HAL_WriteStorage(SETUP_ADDR_AUDIO_LANGUAGE_CODE, (BYTE *)&(__SetupInfo.wAudioLanguageCode), 2);
        break;
    case SETUP_LANGUAGE_SP_OTHERS:
        //Save the menu language code to __SetupInfo
        __SetupInfo.wSPSTLanguageCode = _wInputCode; 
        HAL_WriteStorage(SETUP_ADDR_SPST_LANGUAGE_CODE, (BYTE *)&(__SetupInfo.wSPSTLanguageCode), 2);
        break;
    }
#endif //#ifndef NO_DISC_MODE
}


//  ***************************************************************************
//  Function    :   _GetLanguageNumber
//  Description :   This function will transfer the char password to "number" password.
//  Arguments   :   bID, the ID.
//  Return      :   The password number.
//  Side Effect :
//  ***************************************************************************
WORD    _GetLanguageNumber(void)
{
    WORD    wLanguageNum = 0;

    wLanguageNum =  _wInputCode / 100;
    wLanguageNum <<= 8;
    wLanguageNum += _wInputCode % 100;

    return wLanguageNum;
}

#endif //SUPPORT_STB

#ifdef IMAGE_FRAME_SETUP
#ifdef NL_SOLUTION
void _DVDSETUP_CLOCK_SetTime(BYTE bHour, BYTE bMinute, BYTE bSecond)
{
    _CLOCKCurrnetRTCTime.bHr = bHour;
    _CLOCKCurrnetRTCTime.bMin = bMinute;
    _CLOCKCurrnetRTCTime.bSec = bSecond;

    RTC_SetTime(&_CLOCKCurrnetRTCTime);

    SETTIME_Exit(_hSETUPCLOCKSetTime, TRUE);

    _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_MENU;
    MENU_ShowMenu(_hDVDSETUPMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION); 

#ifdef NL_SOLUTION
    MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
#endif //NL_SOLUTION
}

void _DVDSETUP_CancelSetTime(void)
{
    SETTIME_Exit(_hSETUPCLOCKSetTime, TRUE);

    _bDVDSETUPActiveMode = DVDSETUP_ACTIVE_MODE_MENU;
    MENU_ShowMenu(_hDVDSETUPMenu, TRUE, MAINMENU_PALETTE_ENTRY_CONTENT_REGION);

#ifdef NL_SOLUTION
    MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
#endif //NL_SOLUTION
}

#endif //NL_SOLUTION
#endif //#ifdef IMAGE_FRAME_SETUP

#ifdef IMAGE_FRAME_SETUP
void _DVDSETUP_NoToolBarExit(BYTE bID)
{
#ifdef NO_TOOLBAR_SETUP_MENU
#ifdef MUSTEK_SETUP_MENU
    if (bID == SETUP_EXIT_SETUP)
    {
#ifdef SUPPORT_STB
        DVDSETUP_Exit(TRUE);
#else
        MAINMENU_ExitAll(TRUE, FALSE);
#endif //SUPPORT_STB   
    }
#endif //MUSTEK_SETUP_MENU
#endif //NO_TOOLBAR_SETUP_MENU
}

void _DVDSETUP_NoToolBarEntry(void)
{
#ifdef NO_TOOLBAR_SETUP_MENU
#ifdef MUSTEK_SETUP_MENU
    _DVDSETUP_IconPhotoSettingInitialFunc(ICON_CURSOR_TYPE_PRESSED);
#endif //MUSTEK_SETUP_MENU
#endif //NO_TOOLBAR_SETUP_MENU
}

#endif //#ifdef IMAGE_FRAME_SETUP

void _DVDSETUP_InitializeGDI(void)
{
#ifdef SUPPORT_TEXT_TOOLBAR
    //Initialize the text palettes
    GDI_ChangePALEntry(DVDSETUP_PALETTE_ENTRY_ICON_NORMAL_TEXT, DVDSETUP_PALETTE_COLOR_ICON_NORMAL_TEXT, FALSE);
    GDI_ChangePALEntry(DVDSETUP_PALETTE_ENTRY_ICON_HIGHLIGHT_TEXT, DVDSETUP_PALETTE_COLOR_ICON_HIGHLIGHT_TEXT, FALSE);
    GDI_ChangePALEntry(DVDSETUP_PALETTE_ENTRY_ICON_PRESSED_TEXT, DVDSETUP_PALETTE_COLOR_ICON_PRESSED_TEXT, FALSE);

    GDI_WaitPaletteComplete();
#endif //SUPPORT_TEXT_TOOLBAR
}


