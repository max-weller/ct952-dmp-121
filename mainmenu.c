#define MAINMENU_MAIN_FILE
#define MAINMENU_MAIN_FILE_STRING

#include "winav.h"
#ifdef SUPPORT_STB
#include "dvbcustm.h"
#endif //#ifdef SUPPORT_STB
#include "cell.h"
#include "utl.h"
#include "comutl.h"
#include "gdi.h"
#include "osd.h"
#include "fontindex.h"
#include "infofilter.h"
#include "osddsply.h"
#include "osdnd.h"
#include "input.h"
#ifdef SUPPORT_STB
#include "dvb_func.h"
#include "sysmenu.h"
#include "dvbmenu.h"
#include "ap_flow.h"
#include "ui.h"
#endif //SUPPORT_STB
#include "toolbar.h"
#include "menu.h"
#include "dvdsetup.h"
#include "mainmenu.h"
#include "cc.h"
#include "media.h"
#include "navigate.h"
#include "dvdsetup_op.h"
#include "hdecoder.h"
#include "hal.h"
#include "mmanager.h"
#include "osd.h"
#include "disp.h"
#include "chips.h"
#include "thumb.h"
#ifdef SUPPORT_STB
#include "..\\..\\OSDString\\strextrn.h"
#else
#include "OSDString\\strextrn.h"
#endif //SUPPORT_STB

#ifdef SUPPORT_POWERON_MENU
#include "poweronmenu.h"
#endif //SUPPORT_POWERON_MENU

#define PLAY_AFTER_EXIT_SETUP
BYTE _bMAINMENUMode = FALSE;

#ifdef SUPPORT_STB
BYTE __bMenuState;
BYTE __bMAINMENUDlgIndex;
BYTE _bDMAINMENUGameMenu = FALSE;

BYTE _bMAINMENUCursorIndex;

//For Game menu usage
BYTE _bMAINMENUGameCursorIndex;
#endif //SUPPORT_STB

WORD aMENUTitleSeparation[] = {1, CHAR_HYPHEN_MINUS};

WORD aMENUHelpIconLeft[] = {1, CHAR_LEFT};
WORD aMENUHelpIconRight[] = {1, CHAR_RIGHT};
WORD aMENUHelpIconUp[] = {1, CHAR_UP};
WORD aMENUHelpIconDown[] = {1, CHAR_DOWN};
#ifdef SUPPORT_N_PLAY_AS_ENTER_FLAG
WORD aMENUHelpIconConfirm[] = {7,CHAR_LESS_THAN_SIGN, CHAR_E,CHAR_n,CHAR_t,CHAR_e,CHAR_r,CHAR_GREATER_THAN_SIGN};
#else
WORD aMENUHelpIconConfirm[] = {1, CHAR_N_PLAY};
#endif

#ifdef SUPPORT_STB
extern BYTE __bEPGScheduleMode;
#endif //SUPPORT_STB

extern BYTE __bOSDPromptMode;

extern BYTE BackDoor_ProcessKey(BYTE bKey);
extern void BACKDOOR_ExitBackDoorControl(void);

//*********************************************************************************************************
//  Function    :   MAIN_MENU_ProcessKey
//  Abstract    :   Call this function to process the keys.
//  Arguments   :   none.
//  Return		:   KEY_NO_KEY: The input key is processed.
//              :   INVALID_KEY: The input key is not allowed.
//              :   KEY_xxx: The main program need do more process for the key.
//  Side Effect :   none.
//  Notes       :   
//*********************************************************************************************************
BYTE MAIN_MENU_ProcessKey(void)
{    
    if ((_bMAINMENUMode == FALSE) && (__bKey != KEY_SETUP)) //for DVD
    {
        return KEY_BYPASS;
    }
    
#ifndef SUPPORT_STB
    __bKey = BackDoor_ProcessKey(__bKey);   
#endif //SUPPORT_STB
    
    if ((__bKey == KEY_POWER) || (__bKey == KEY_OPEN_CLOSE))
    {
        _bMAINMENUMode = FALSE;

        MAINMENU_ExitAll(TRUE, FALSE);

#ifdef SUPPORT_STB
        DVB_FUNC_Schedule_term();
#endif //SUPPORT_STB

        return KEY_BYPASS;
    }


#ifdef SUPPORT_STB
    if ((!_bMAINMENUMode) && (__bKey == KEY_SETUP))
    {
#ifdef SUPPORT_POWERON_MENU
        if (_bDMAINMENUGameMenu)
        {
            return KEY_NO_KEY;
        }
#endif //SUPPORT_POWERON_MENU

        __bSetupMode = TRUE; //Let DVD flow think it's in SETUP mode.
        MAINMENU_Initial();
    }
    else
    {
        MAINMENU_ProcessKey(__bKey);
    }
#else
    if ((!_bMAINMENUMode) && (__bKey == KEY_SETUP))
    {
#ifdef SUPPORT_COPY_FILE_IN_SLIDE_SHOW
        if (__bMMSlideShowCopyDeleteMode != MM_SLIDE_SHOW_NORMAL_MODE)
        {
            return KEY_NO_KEY;
        }
#endif //SUPPORT_COPY_FILE_IN_SLIDE_SHOW

        __bSetupMode = TRUE; //Let DVD flow think it's in SETUP mode.
        _bMAINMENUMode = TRUE;
        MAINMENU_Initial();
        DVDSETUP_Initial(FALSE);
    }
    else
    {
          #ifdef SUPPORT_MENU_KEY_FUNCTION_EXIT
	   if (__bKey == KEY_SETUP||__bKey==KEY_FUNCTION)
	   #else
          if (__bKey == KEY_SETUP)
	   #endif
        {            
            MAINMENU_ExitAll(TRUE, FALSE);
        }

        if (__bKey == KEY_PLAY_PAUSE)
        {
            __bKey = KEY_ENTER;
        }

        DVDSETUP_ProcessKey(__bKey);
    }

#endif //#ifdef SUPPORT_STB

    return KEY_NO_KEY;
}

//***************************************************************************
//  Function    :   MAINMENU_Initial
//  Abstract    :   When the menu is pop-up at 1st time, this initial function will be called. 
//                  Each UI can do its control initialization and draw the UI here.
//  Arguments   :   none.
//  Return      :   nonw.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void MAINMENU_Initial(void)
{
#ifdef SUPPORT_STB
    //Initialize the variables.
    _bMAINMENUMode = TRUE;
    __bMenuState = MS_MAIN_MENU;
    
    if (__bChooseMedia == MEDIA_SELECT_STB)
    {
        _bMAINMENUCursorIndex = MAINMENU_BUTTON_DVB_MENU;
    }
    else
    {   
        MM_CompleteJPGEffect(TRUE); //Complete JPEG effect to prevent slide show effect error.
        
        OSD_ChangeUI(OSD_UI_MAIN_MENU, OSD_ENTER_UI);
        _bMAINMENUCursorIndex = MAINMENU_BUTTON_DVD_SETUP;
        _MAINMENU_InitDVDMode();
    }
#else
    MM_CompleteJPGEffect(TRUE); //Complete JPEG effect to prevent slide show effect error.
    
#ifdef PRESTOP_BEFORE_SETUP_AND_PROGRAM    
    CC_ForceStop(CC_FORCE_STOP_FROM_SETUP);
#endif
    
    //OSD_ChangeUI(OSD_UI_MAIN_MENU, OSD_ENTER_UI);
    OSD_ChangeUI(OSD_UI_SETUP, OSD_ENTER_UI); //CoCo2.38
    _MAINMENU_InitDVDMode();
    
#endif //SUPPORT_STB
    
    _MAINMENU_DrawAllUI();
}

#ifdef SUPPORT_STB
BYTE _MAINMENU_MoveCursorUp(void)
{
    BYTE bMAINMENUCursorIndex;

    bMAINMENUCursorIndex = _bMAINMENUCursorIndex;

    bMAINMENUCursorIndex--;
    if (((__bChooseMedia != MEDIA_SELECT_STB) && (bMAINMENUCursorIndex == MAINMENU_BUTTON_DVB_MENU)) ||
        ((__bChooseMedia == MEDIA_SELECT_STB) && (bMAINMENUCursorIndex == MAINMENU_BUTTON_DVD_SETUP)))
    {
        if (bMAINMENUCursorIndex != 0)
        {
            bMAINMENUCursorIndex--;
        }
        else
        {
            bMAINMENUCursorIndex = _bMAINMENUCursorIndex;
        }
    }

    return bMAINMENUCursorIndex;
}

BYTE _MAINMENU_MoveCursorDown(void)
{
    BYTE bMAINMENUCursorIndex;

    bMAINMENUCursorIndex = _bMAINMENUCursorIndex;

    bMAINMENUCursorIndex++;

    if (((__bChooseMedia != MEDIA_SELECT_STB) && (bMAINMENUCursorIndex == MAINMENU_BUTTON_DVB_MENU)) ||
        ((__bChooseMedia == MEDIA_SELECT_STB) && (bMAINMENUCursorIndex == MAINMENU_BUTTON_DVD_SETUP)))
    {
        if (bMAINMENUCursorIndex != (MAINMENU_BUTTON_NUM-1))
        {
            bMAINMENUCursorIndex++;
        }
        else
        {
            bMAINMENUCursorIndex = _bMAINMENUCursorIndex;
        }
    }

    return bMAINMENUCursorIndex;
}

void _MAINMENU_UpdateButton(BYTE bNewCursor)
{
    if (bNewCursor != _bMAINMENUCursorIndex)
    {
        _MAINMENU_DrawButton(_bMAINMENUCursorIndex, MAINMENU_BUTTON_NORMAL, _bMAINMENUButton);
        _bMAINMENUCursorIndex = bNewCursor;
        _MAINMENU_DrawButton(_bMAINMENUCursorIndex, MAINMENU_BUTTON_HIGHLIGHT, _bMAINMENUButton);
    }
}

//***************************************************************************
//  Function    :   MAINMENU_ProcessKey
//  Abstract    :   When the menu is active, IR key will be sent to this menu's processing key function.
//  Arguments   :   none.
//  Return      :   MENU_ACTION_DONE: The key has been processed OK. If the key is invalid, it will be skipped by this function.
//                  MENU_ACTION_EXIT: The menu (sub-UI) exists.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
BYTE MAINMENU_ProcessKey(BYTE bKey)
{
    BYTE bReturn;
    BYTE bMenuFlowTableEntry;
    BYTE bCursorIndex;

    if (bKey == KEY_NO_KEY)
    {
        return MENU_ACTION_DONE;
    }

    if (!_bMAINMENUMode)
    {
        if (bKey == KEY_SETUP)
        {
            _bMAINMENUMode = TRUE;
            MAINMENU_Initial();            
        }
        return MENU_ACTION_DONE;
    }

    //CoCo, don't let sub-UIs enter the back door.
    if (__bMenuState == MS_MAIN_MENU)
    {
        bKey = BackDoor_ProcessKey(bKey);   
    }

    //This judgement need to put after "back door"
    if ((bKey == KEY_SETUP) || ((__bChooseMedia == MEDIA_SELECT_STB) && (bKey == KEY_MENU))) //DVD can't use KEY_MENU to enter main menu.
    {
        MAINMENU_ExitAll(TRUE, FALSE);
        return MENU_ACTION_DONE;
    }

    if (__bMenuState == MS_MAIN_MENU)
    {
        if (bKey == KEY_EXIT)
        {
            MAINMENU_ExitAll(TRUE, FALSE);
            return MENU_ACTION_DONE;
        }

        switch (bKey)
        {
        case KEY_UP:
            if (_bMAINMENUCursorIndex != 0)
            {
                bCursorIndex = _MAINMENU_MoveCursorUp();
                _MAINMENU_UpdateButton(bCursorIndex);
            }
            break;
        case KEY_DOWN:
            if (_bMAINMENUCursorIndex != (MAINMENU_BUTTON_NUM-1))
            {
                bCursorIndex = _MAINMENU_MoveCursorDown();
                _MAINMENU_UpdateButton(bCursorIndex);
            }
            break;
        case KEY_PLAY:
        case KEY_ENTER:
            _MAINMENU_ProcessButton();
            break;
        }
    }
    else if (__bMenuState == MS_DLG)
    {
        if (_DlgFlowTable[__bMAINMENUDlgIndex].Dlg_Proc_Func_Ptr != NULL)
        {
            bReturn = _DlgFlowTable[__bMAINMENUDlgIndex].Dlg_Proc_Func_Ptr(bKey);
            
            if (bReturn == DLG_ACTION_EXIT_RECOVER_NENU)
            {
                __bMenuState = _DlgFlowTable[__bMAINMENUDlgIndex].bRecoverMenuState;

                bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
                if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
                {                    
                    if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr != NULL)
                    {
                        _MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr();
                    }
                }
            }
            else if (bReturn == DLG_ACTION_EXIT_NEW_NENU)
            {
                if (_DlgFlowTable[__bMAINMENUDlgIndex].bNewMenuState == MAINMENU_INVALID_MENU_STATE)
                {
                    __bMenuState = _DlgFlowTable[__bMAINMENUDlgIndex].bRecoverMenuState;
                    bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
                    if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
                    {                    
                        if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr != NULL)
                        {
                            _MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr();
                        }
                    }
                }
                else
                {
                    __bMenuState = _DlgFlowTable[__bMAINMENUDlgIndex].bNewMenuState;
                    MAINMENU_SubUIInitial();
                }
            }
            else if (bReturn == DLG_ACTION_CHANGE_DLG)
            {
                __bMAINMENUDlgIndex = _DlgFlowTable[__bMAINMENUDlgIndex].bChangeDlgState;

                MAINMENU_DlgInitial();
            }
        }       
    }
    else
    {
        bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
        if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
        {
            if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Proc_Func_Ptr != NULL)
            {
                bReturn = _MenuFlowTable[bMenuFlowTableEntry].Menu_Proc_Func_Ptr(bKey);
                
                if (bReturn == MENU_ACTION_EXIT)
                {
                    __bMenuState = _MAINMENU_GetNextMenuState();
                    if (__bMenuState != MAINMENU_INVALID_MENU_STATE)
                    {                    
                        bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
                        if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
                        {                    
                            if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr != NULL)
                            {
                                _MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr();
                            }
                        }
                    }
                }
                else if (bReturn == MENU_ACTION_EXIT_ALL)
                {
                    MAINMENU_ExitAll(TRUE, FALSE);    
                }
            }
        }
    }

    return MENU_ACTION_DONE;
}

//***************************************************************************
//  Function    :   MAINMENU_Trigger
//  Abstract    :   Each UI needs to offer the trigger function. The trigger function can process 
//              :   some function that will be updated continuously or need to cancel some action when time-out.
//  Arguments   :   none.
//  Return      :   none.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void MAINMENU_Trigger(void)
{
    BYTE bMenuFlowTableEntry;

    bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
    if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
    {        
        if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Trigger_Func_Ptr != NULL)
        {
            _MenuFlowTable[bMenuFlowTableEntry].Menu_Trigger_Func_Ptr();
        }
    }
}

#endif //SUPPORT_STB

//***************************************************************************
//  Function    :   MAINMENU_Recover
//  Abstract    :   Each menu needs to offer the recovery function. When it is overwritten by screen saver, it will be recovered.
//  Arguments   :   none.
//  Return      :   none.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
#ifdef SUPPORT_STB
void MAINMENU_Recover(void)
{
    BYTE bMenuFlowTableEntry;

#ifndef CT950_STYLE
    _MAINMENU_DrawAllUI();
#else
    if (__bChooseMedia == MEDIA_SELECT_STB)
    {
        _MAINMENU_DrawAllUI();
    }
    else
    {
        MM_CompleteJPGEffect(TRUE); //Complete JPEG effect to prevent slide show effect error.

        if (__bMenuState == MS_MAIN_MENU)
        {
            _MAINMENU_DrawAllUI();
        }
        else
        {
            if (__bMenuState != MAINMENU_INVALID_MENU_STATE)
            {                    
                bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
                if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
                {                    
                    if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr != NULL)
                    {
                        _MenuFlowTable[bMenuFlowTableEntry].Menu_Recover_Func_Ptr();
                    }
                }
            }
        }        
    }
#endif //CT950_STYLE
}
#else
BYTE MAINMENU_Recover(BYTE bRecoverRegion)
{
    MM_CompleteJPGEffect(TRUE); //Complete JPEG effect to prevent slide show effect error.
    
#ifdef PRESTOP_BEFORE_SETUP_AND_PROGRAM     
    CC_ForceStop(CC_FORCE_STOP_FROM_SETUP);
#endif

    _MAINMENU_InitDVDMode();
    
    BACKDOOR_ExitBackDoorControl();

    _MAINMENU_DrawAllUI();
    DVDSETUP_Initial(TRUE);

    return TRUE;
}
#endif //SUPPORT_STB

#ifdef SUPPORT_STB
void _MAINMENU_DrawButton(BYTE bButtonIndex, BYTE bType, BUTTON *pButton)
{
    PARM_RECT ButtonRect;
        
    ButtonRect.bColor = MAINMENU_BUTTON_HEIGHT;
    if (bType == MAINMENU_BUTTON_NORMAL)
    {
        switch (__SetupInfo.bUIStyle)
        {
        case MENU_SYS_PREFERENCE_UI_STYLE_2:
            ButtonRect.bShadePtr = _bMAINMENUButtonNormalPalette2;
            break;
        case MENU_SYS_PREFERENCE_UI_STYLE_3:
            ButtonRect.bShadePtr = _bMAINMENUButtonNormalPalette3;
            break;
        case MENU_SYS_PREFERENCE_UI_STYLE_4:
            ButtonRect.bShadePtr = _bMAINMENUButtonNormalPalette4;
            break;
        case MENU_SYS_PREFERENCE_UI_STYLE_1:
        default:
            ButtonRect.bShadePtr = _bMAINMENUButtonNormalPalette1;
            break;
        }
    }
    else if (bType == MAINMENU_BUTTON_HIGHLIGHT)
    {
        switch (__SetupInfo.bUIStyle)
        {
        case MENU_SYS_PREFERENCE_UI_STYLE_2:
            ButtonRect.bShadePtr = _bMAINMENUButtonHighlightPalette2;
            break;
        case MENU_SYS_PREFERENCE_UI_STYLE_3:
            ButtonRect.bShadePtr = _bMAINMENUButtonHighlightPalette3;
            break;
        case MENU_SYS_PREFERENCE_UI_STYLE_4:
            ButtonRect.bShadePtr = _bMAINMENUButtonHighlightPalette4;
            break;
        case MENU_SYS_PREFERENCE_UI_STYLE_1:
        default:
            ButtonRect.bShadePtr = _bMAINMENUButtonHighlightPalette1;
            break;
        }
    }
    else //disable
    {
        ButtonRect.bShadePtr = _bMAINMENUButtonDisablePalette;
    }

    ButtonRect.rect.wLeft = _MAINMENU_GetButtonStartX(pButton);
    ButtonRect.rect.wTop = _MAINMENU_GetButtonStartY(bButtonIndex);
    ButtonRect.rect.wRight = ButtonRect.rect.wLeft+_MAINMENU_GetMaxButtonStringWidth(pButton)+MAINMENU_BUTTON_STRING_DISTANCE_H*2-1;
    ButtonRect.rect.wBottom = ButtonRect.rect.wTop+MAINMENU_BUTTON_HEIGHT-1;

    GDI_FillRoundRect(0|GDI_SHADING_OPERATION, MAINMENU_BUTTON_ARC_VALUE, &ButtonRect);

    _MAINMENU_DrawButtonString(bButtonIndex, pButton);
}

WORD _MAINMENU_GetMaxButtonStringWidth(BUTTON* pButton)
{
    BYTE bButtonIndex;
    WORD wStringWidth = 0;
    WORD wMaxStringWidth = 0;

    for (bButtonIndex = 0; bButtonIndex < MAINMENU_BUTTON_NUM; bButtonIndex++)
    {
        wStringWidth = GDI_GetStringWidth_909(pButton[bButtonIndex].pwButtonString);

        if (wMaxStringWidth < wStringWidth)
        {
            wMaxStringWidth = wStringWidth;
        }        
    }

    return wMaxStringWidth;
}

void _MAINMENU_DrawButtonString(BYTE bButtonIndex, BUTTON *pButton)
{
    PARM_DRAW_STRING ButtonStringAttr;
    
    MAINMENU_UseColorKey(&ButtonStringAttr);
    ButtonStringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_BUTTON_STRING;
    
    ButtonStringAttr.wX = _MAINMENU_GetButtonStartX(pButton)+((_MAINMENU_GetButtonWidth(pButton)-GDI_GetStringWidth_909(pButton[bButtonIndex].pwButtonString)) >> 1);
    ButtonStringAttr.wY = _MAINMENU_GetButtonStartY(bButtonIndex)+((MAINMENU_BUTTON_HEIGHT-GDI_GetStringHeight(pButton[bButtonIndex].pwButtonString)) >> 1);
        
    GDI_DrawString_909(0, &ButtonStringAttr, pButton[bButtonIndex].pwButtonString);
}

WORD _MAINMENU_GetButtonStartX(BUTTON *pButton)
{
    return (MAINMENU_CONTENT_REGION_START_X+((MAINMENU_CONTENT_REGION_WIDTH-_MAINMENU_GetButtonWidth(pButton)) >> 1));
}

WORD _MAINMENU_GetButtonStartY(BYTE bButtonIndex)
{
    WORD wButtonDistance;
    
    wButtonDistance = (MAINMENU_CONTENT_REGION_HEIGHT-MAINMENU_BUTTON_BACKGROUND_DISTANCE_V*2-MAINMENU_BUTTON_HEIGHT*MAINMENU_BUTTON_NUM)/(MAINMENU_BUTTON_NUM-1);

    return (MAINMENU_CONTENT_REGION_START_Y+MAINMENU_BUTTON_BACKGROUND_DISTANCE_V+bButtonIndex*MAINMENU_BUTTON_HEIGHT+bButtonIndex*wButtonDistance);
}

WORD _MAINMENU_GetButtonWidth(BUTTON *pButton)
{
    return (_MAINMENU_GetMaxButtonStringWidth(pButton)+MAINMENU_BUTTON_STRING_DISTANCE_H*2);
}

void _MAINMENU_ProcessButton(void)
{
    switch (_bMAINMENUCursorIndex)
    {
    case MAINMENU_BUTTON_DVD_SETUP:
        __bMenuState = MS_DVD_SETUP;
        break;
    case MAINMENU_BUTTON_DVB_MENU:
        __bMenuState = MS_DVB_MENU;
        break;
    case MAINMENU_BUTTON_SYSTEM_SETTING:
        __bMenuState = MS_SYSTEM_SETTING;
        break;
    case MAINMENU_BUTTON_GAME:
        __bMenuState = MS_GAME;
        break;
    case MAINMENU_BUTTON_EXIT:
        MAINMENU_ExitAll(TRUE, FALSE);
        return;
    default:
        return;
    }

    MAINMENU_SubUIInitial();
}

BYTE _MAINMENU_GetMenuFlowTableEntry(void)
{
    BYTE bEntryIndex;

    for (bEntryIndex = 0; bEntryIndex < (sizeof(_MenuFlowTable)/sizeof(MENU_FLOW)); bEntryIndex++)
    {
        if (_MenuFlowTable[bEntryIndex].MenuState == __bMenuState)
        {
            return bEntryIndex;
        }
    }

    return MAINMENU_INVALID_MENU_FLOW_ENTRY;
}


BYTE _MAINMENU_GetNextMenuState(void)
{
    BYTE bEntryIndex;

    for (bEntryIndex = 0; bEntryIndex < (sizeof(_MenuStateTable)/sizeof(MENU_STATE)); bEntryIndex++)
    {
        if (_MenuStateTable[bEntryIndex].MenuCurrentState == __bMenuState)
        {
            return _MenuStateTable[bEntryIndex].MenuChangeToStateAfterExit;
        }
    }

    return MAINMENU_INVALID_MENU_STATE;
}

void MAINMENU_SubUIInitial(void)
{
    BYTE bMenuFlowTableEntry;

    bMenuFlowTableEntry = _MAINMENU_GetMenuFlowTableEntry();
    if (bMenuFlowTableEntry != MAINMENU_INVALID_MENU_FLOW_ENTRY)
    {
        if (_MenuFlowTable[bMenuFlowTableEntry].Menu_Init_Func_Ptr != NULL)
        {
            _MenuFlowTable[bMenuFlowTableEntry].Menu_Init_Func_Ptr();
        }
    }
}

void MAINMENU_DlgInitial(void)
{    
    if (_DlgFlowTable[__bMAINMENUDlgIndex].Dlg_Init_Func_Ptr != NULL)
    {
        _DlgFlowTable[__bMAINMENUDlgIndex].Dlg_Init_Func_Ptr(MAINMENU_PALETTE_ENTRY_CONTENT_REGION);
    }

    __bMenuState = MS_DLG;
}

//***************************************************************************
//  Function    :   MAINMENU_Game_Menu_Initial
//  Abstract    :   When the menu is pop-up at 1st time, this initial function will be called. 
//                  Each UI can do its control initialization and draw the UI here.
//  Arguments   :   none.
//  Return      :   nonw.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void MAINMENU_Game_Menu_Initial(void)
{
    _bDMAINMENUGameMenu = TRUE;
    _bMAINMENUGameCursorIndex = 0;

    _MAINMENU_Game_Menu_DrawAllUI();
}

//***************************************************************************
//  Function    :   MAINMENU_Game_Menu_Recover
//  Abstract    :   Each menu needs to offer the recovery function. When it is overwritten by screen saver, it will be recovered.
//  Arguments   :   none.
//  Return      :   none.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void MAINMENU_Game_Menu_Recover(void)
{
    _MAINMENU_Game_Menu_DrawAllUI();
}


void _MAINMENU_Game_Menu_DrawAllUI(void)
{
    BYTE bButtonIndex;

    MAINMENU_InitializeGDI(__SetupInfo.bUIStyle - MENU_SYS_PREFERENCE_UI_STYLE_BEGIN);
    
    //Draw background
	MAINMENU_DrawMainBackground();

    //Draw the title
    MAINMENU_DrawTitleString(aMENUMainMenu, aMENUGame, NULL);
    
    //Draw the buttons.
    for (bButtonIndex = 0; bButtonIndex < MAINMENU_GAME_BUTTON_NUM; bButtonIndex++)
    {
        if (bButtonIndex == _bMAINMENUGameCursorIndex)
        {
            _MAINMENU_DrawButton(bButtonIndex, MAINMENU_BUTTON_HIGHLIGHT, _bMAINMENUGameButton);
        }
        else
        {
            _MAINMENU_DrawButton(bButtonIndex, MAINMENU_BUTTON_NORMAL, _bMAINMENUGameButton);
        }
    }

    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
}

//***************************************************************************
//  Function    :   MAINMENU_Game_Menu_Exit
//  Abstract    :   Each UI needs to offer the exit function. When user presses KEY_OPEN_CLOSE or KEY_POWER, 
//              :   the main menu will call the UI's exit function to exit the control.
//              :   Each sub-menu can reset your control or clear some flags here.
//  Arguments   :   none.
//  Return      :   none.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
void MAINMENU_Game_Menu_Exit(BYTE bClearBackground)
{
    //Clear UI background.
#ifdef SUPPORT_POWERON_MENU
    GDI_ClearRegion(0);
#else
    if (bClearBackground)
    {
        MAINMENU_ClearMainBackground();
    }
#endif //SUPPORT_POWERON_MENU

    _bDMAINMENUGameMenu = FALSE;
}

//***************************************************************************
//  Function    :   MAINMENU_Game_Menu_ProcessKey
//  Abstract    :   When the menu is active, IR key will be sent to this menu's processing key function.
//  Arguments   :   none.
//  Return      :   MENU_ACTION_DONE: The key has been processed OK. If the key is invalid, it will be skipped by this function.
//                  MENU_ACTION_EXIT: The menu (sub-UI) exists.
//  Side Effect :   none.
//  Notes       :   
//***************************************************************************
BYTE MAINMENU_Game_Menu_ProcessKey(BYTE bKey)
{
    if (!_bDMAINMENUGameMenu)
    {
        return MENU_ACTION_DONE;
    }
    
#ifdef SUPPORT_POWERON_MENU
    switch (__bMenuState)
    {
#ifdef SUPPORT_GAME_MINER    
    case MS_GAME_MINE:
        return GAME_MINE_ProcessKey(__bKey);
#endif //SUPPORT_GAME_MINER       
#ifdef SUPPORT_GAME_TETRIS
    case MS_GAME_TETRIS:
        return GAME_TETRIS_ProcessKey(__bKey);
#endif //SUPPORT_GAME_TETRIS    
#ifdef SUPPORT_GAME_GOMOKU
    case MS_GAME_GOMOKU:
        return GAME_GOMOKU_ProcessKey(__bKey);
#endif
#ifdef SUPPORT_GAME_SOKOBAN
    case MS_GAME_SOKOBAN:
        return GAME_SOKOBAN_ProcessKey(__bKey);
#endif
#ifdef SUPPORT_GAME_RUSHHOUR
    case MS_GAME_RUSHHOUR:
        return GAME_RUSHHOUR_ProcessKey(__bKey);
#endif
    default:
        break;
    }

#endif //SUPPORT_POWERON_MENU

    switch (bKey)
    {
    case KEY_UP:
        if (_bMAINMENUGameCursorIndex != 0)
        {
            _MAINMENU_DrawButton(_bMAINMENUGameCursorIndex, MAINMENU_BUTTON_NORMAL, _bMAINMENUGameButton);
            _bMAINMENUGameCursorIndex--;
            _MAINMENU_DrawButton(_bMAINMENUGameCursorIndex, MAINMENU_BUTTON_HIGHLIGHT, _bMAINMENUGameButton);
        }
        break;
    case KEY_DOWN:
        if (_bMAINMENUGameCursorIndex != (MAINMENU_GAME_BUTTON_NUM-1))
        {
            _MAINMENU_DrawButton(_bMAINMENUGameCursorIndex, MAINMENU_BUTTON_NORMAL, _bMAINMENUGameButton);
            _bMAINMENUGameCursorIndex++;
            _MAINMENU_DrawButton(_bMAINMENUGameCursorIndex, MAINMENU_BUTTON_HIGHLIGHT, _bMAINMENUGameButton);
        }
        break;
    case KEY_PLAY:
    case KEY_ENTER:
        _MAINMENU_Game_Menu_ProcessButton();
        if(_bDMAINMENUGameMenu == FALSE) //xuli.osdgame
            return MENU_ACTION_EXIT;
    default:
        break;
    }

    return MENU_ACTION_DONE;
}

void MAINMENU_Game_Menu_Trigger(void)
{
#ifdef SUPPORT_POWERON_MENU
    switch (__bMenuState)
    {
#ifdef SUPPORT_GAME_MINER    
    case MS_GAME_MINE:
        GAME_MINE_Trigger();
        break;
#endif //SUPPORT_GAME_MINER       
#ifdef SUPPORT_GAME_TETRIS
    case MS_GAME_TETRIS:
        GAME_TETRIS_Trigger();
        break;
#endif //SUPPORT_GAME_TETRIS    
#ifdef SUPPORT_GAME_GOMOKU
    case MS_GAME_GOMOKU:
        break;
#endif
#ifdef SUPPORT_GAME_SOKOBAN
    case MS_GAME_SOKOBAN:
        GAME_SOKOBAN_Trigger();
        break;
#endif
#ifdef SUPPORT_GAME_RUSHHOUR
    case MS_GAME_RUSHHOUR:
        break;
#endif
    default:
        break;
    }
#endif //SUPPORT_POWERON_MENU
}

void _MAINMENU_Game_Menu_ProcessButton(void)
{
    switch (_bMAINMENUGameCursorIndex)
    {
#ifdef SUPPORT_GAME_MINER    
    case MAINMENU_GAME_BUTTON_MINE:
        __bMenuState = MS_GAME_MINE; //xuli.osdgame
        break;
#endif        
#ifdef SUPPORT_GAME_TETRIS
    case MAINMENU_GAME_BUTTON_TETRIS:
        __bMenuState = MS_GAME_TETRIS; //xuli.osdgame
        break;
#endif    
#ifdef SUPPORT_GAME_GOMOKU
    case MAINMENU_GAME_BUTTON_GOMOKU:
        __bMenuState = MS_GAME_GOMOKU; //xuli.osdgame
        break;
#endif
#ifdef SUPPORT_GAME_SOKOBAN
    case MAINMENU_GAME_BUTTON_SOKOBAN:
        __bMenuState = MS_GAME_SOKOBAN; //xuli.osdgame
        break;
#endif
#ifdef SUPPORT_GAME_RUSHHOUR
    case MAINMENU_GAME_BUTTON_RUSHHOUR:
        __bMenuState = MS_GAME_RUSHHOUR; //xuli.osdgame
        break;
#endif
#ifndef SUPPORT_POWERON_MENU
#ifdef SUPPORT_CALENDAR
    case MAINMENU_GAME_BUTTON_CALENDAR:
        __bMenuState = MS_CALENDAR; //xuli.osdgame    
        break;
#endif //SUPPORT_CALENDAR
#endif //SUPPORT_POWERON_MENU
    case MAINMENU_GAME_BUTTON_EXIT:
        MAINMENU_Game_Menu_Exit(TRUE);
#ifdef SUPPORT_POWERON_MENU
        //__bMenuState = MS_GAME;
        POWERONMENU_BackToMainMenu();
#endif //SUPPORT_POWERON_MENU
        return;
    default:
        break;
    }

    MAINMENU_SubUIInitial();
}

void MAINMENU_Game_Menu_BackToPowerOnMenu(void)
{
#ifdef SUPPORT_POWERON_MENU
    MAINMENU_Game_Menu_Exit(TRUE);    
    __bMenuState = MS_GAME;
    POWERONMENU_BackToMainMenu();    
#endif //SUPPORT_POWERON_MENU
}


void MAINMENU_JumpToState(BYTE bState)
{
    //Initialize the variables.
    _bMAINMENUMode = TRUE;
    __bMenuState = bState;
    _bMAINMENUCursorIndex = 0;

    MAINMENU_InitializeGDI(__SetupInfo.bUIStyle - MENU_SYS_PREFERENCE_UI_STYLE_BEGIN);

    MAINMENU_SubUIInitial();
}

#endif //SUPPORT_STB

void MAINMENU_LoadGroupPalette(BYTE bPaletteStartIndex, DWORD *dwPalette, BYTE bMixEnable)
{
    BYTE i;

    for (i = 0; i < dwPalette[0]; i++)
    {
		GDI_ChangePALEntry(bPaletteStartIndex+i, dwPalette[i+1], bMixEnable);
    }
}

void MAINMENU_InitializeGDI(BYTE bUIStyle)
{
    switch (bUIStyle)
    {
    case 1:
        MAINMENU_LoadGroupPalette(MAINMENU_TITLE_BACKGROUND_PALETTE_ENTRY_BERIN, _dwMAINMENUTitleRegionPalette2, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_HELP_BACKGROUND_PALETTE_ENTRY_BEGIN, _dwMAINMENUHelpRegionPalette2, FALSE);
#ifdef SUPPORT_STB
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_NORMAL_BUTTON_PALETTE_BEGIN, _dwMAINMENUNormalButtonPalette2, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_HIGHLIGHT_BUTTON_PALETTE_BEGIN, _dwMAINMENUHighlightButtonPalette2, FALSE);
#endif //#ifdef SUPPORT_STB
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_CONTENT_REGION, MAINMENU_PALETTE_COLOR_CONTENT_REGION_2, TRUE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_1ST, MAINMENU_PALETTE_COLOR_TITLE_STRING_1ST_2, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_2ND, MAINMENU_PALETTE_COLOR_TITLE_STRING_2ND_2, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_3RD, MAINMENU_PALETTE_COLOR_TITLE_STRING_3RD_2, FALSE);
        break;
    case 2:
        MAINMENU_LoadGroupPalette(MAINMENU_TITLE_BACKGROUND_PALETTE_ENTRY_BERIN,_dwMAINMENUTitleRegionPalette3, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_HELP_BACKGROUND_PALETTE_ENTRY_BEGIN, _dwMAINMENUHelpRegionPalette3, FALSE);
#ifdef SUPPORT_STB
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_NORMAL_BUTTON_PALETTE_BEGIN, _dwMAINMENUNormalButtonPalette3, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_HIGHLIGHT_BUTTON_PALETTE_BEGIN, _dwMAINMENUHighlightButtonPalette3, FALSE);
#endif //#ifdef SUPPORT_STB
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_CONTENT_REGION, MAINMENU_PALETTE_COLOR_CONTENT_REGION_3, TRUE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_1ST, MAINMENU_PALETTE_COLOR_TITLE_STRING_1ST_3, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_2ND, MAINMENU_PALETTE_COLOR_TITLE_STRING_2ND_3, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_3RD, MAINMENU_PALETTE_COLOR_TITLE_STRING_3RD_3, FALSE);
        break;
    case 3:
        MAINMENU_LoadGroupPalette(MAINMENU_TITLE_BACKGROUND_PALETTE_ENTRY_BERIN,_dwMAINMENUTitleRegionPalette4, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_HELP_BACKGROUND_PALETTE_ENTRY_BEGIN, _dwMAINMENUHelpRegionPalette4, FALSE);
#ifdef SUPPORT_STB
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_NORMAL_BUTTON_PALETTE_BEGIN, _dwMAINMENUNormalButtonPalette4, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_HIGHLIGHT_BUTTON_PALETTE_BEGIN, _dwMAINMENUHighlightButtonPalette4, FALSE);
#endif //#ifdef SUPPORT_STB
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_CONTENT_REGION, MAINMENU_PALETTE_COLOR_CONTENT_REGION_4, TRUE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_1ST, MAINMENU_PALETTE_COLOR_TITLE_STRING_1ST_4, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_2ND, MAINMENU_PALETTE_COLOR_TITLE_STRING_2ND_4, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_3RD, MAINMENU_PALETTE_COLOR_TITLE_STRING_3RD_4, FALSE);
        break;
    case 0:
    default:
        MAINMENU_LoadGroupPalette(MAINMENU_TITLE_BACKGROUND_PALETTE_ENTRY_BERIN,_dwMAINMENUTitleRegionPalette1, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_HELP_BACKGROUND_PALETTE_ENTRY_BEGIN, _dwMAINMENUHelpRegionPalette1, FALSE);
#ifdef SUPPORT_STB
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_NORMAL_BUTTON_PALETTE_BEGIN, _dwMAINMENUNormalButtonPalette1, FALSE);
        MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_HIGHLIGHT_BUTTON_PALETTE_BEGIN, _dwMAINMENUHighlightButtonPalette1, FALSE);
#endif //#ifdef SUPPORT_STB
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_CONTENT_REGION, MAINMENU_PALETTE_COLOR_CONTENT_REGION_1, TRUE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_1ST, MAINMENU_PALETTE_COLOR_TITLE_STRING_1ST_1, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_2ND, MAINMENU_PALETTE_COLOR_TITLE_STRING_2ND_1, FALSE);
        GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_TITLE_STRING_3RD, MAINMENU_PALETTE_COLOR_TITLE_STRING_3RD_1, FALSE);
        break;
    }
    
#ifdef SUPPORT_STB
    MAINMENU_LoadGroupPalette(MAINMENU_PALETTE_ENTRY_DISABLE_BUTTON_PALETTE_BEGIN, _dwMAINMENUDisableButtonPalette, FALSE);
    GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_BUTTON_STRING, MAINMENU_PALETTE_COLOR_BUTTON_STRING, FALSE);
#endif //#ifdef SUPPORT_STB
    
    GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_HELP_STRING, MAINMENU_PALETTE_COLOR_HELP_STRING, FALSE);
    GDI_ChangePALEntry(MAINMENU_PALETTE_ENTRY_BACKDOOR_STRING, MAINMENU_PALETTE_COLOR_BACKDOOR_STRING, FALSE);
    
    GDI_WaitPaletteComplete();
}

void _MAINMENU_DrawAllUI(void)
{
#ifdef SUPPORT_STB
    BYTE bButtonIndex;
#endif //SUPPORT_STB

    MAINMENU_InitializeGDI(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
    
    GDI_ClearRegion(0);

    //Draw background
	MAINMENU_DrawMainBackground();

#ifdef SUPPORT_STB
    //Draw the title
    MAINMENU_DrawTitleString(aMENUMainMenu, NULL, NULL);
    
    //Draw the buttons.
    for (bButtonIndex = 0; bButtonIndex < MAINMENU_BUTTON_NUM; bButtonIndex++)
    {
        if (bButtonIndex == _bMAINMENUCursorIndex)
        {
            _MAINMENU_DrawButton(bButtonIndex, MAINMENU_BUTTON_HIGHLIGHT, _bMAINMENUButton);
        }
        else
        {
            if (((__bChooseMedia != MEDIA_SELECT_STB) && (bButtonIndex == MAINMENU_BUTTON_DVB_MENU)) ||
                ((__bChooseMedia == MEDIA_SELECT_STB) && (bButtonIndex == MAINMENU_BUTTON_DVD_SETUP)))
            {
                _MAINMENU_DrawButton(bButtonIndex, MAINMENU_BUTTON_DISABLE, _bMAINMENUButton);
            }
            else
            {
                _MAINMENU_DrawButton(bButtonIndex, MAINMENU_BUTTON_NORMAL, _bMAINMENUButton);
            }
        }
    }
#endif //SUPPORT_STB

    MAINMENU_DrawSelectionHelp(FALSE, FALSE);
}

void MAINMENU_DrawMainTitleBackground(BYTE bUIStyle)
{
	PARM_RECT TitleRect;

    TitleRect.bColor = MAINMENU_TITLE_REGION_HEIGHT;
    TitleRect.rect.wLeft = MAINMENU_TITLE_REGION_START_X;
    TitleRect.rect.wTop = MAINMENU_TITLE_REGION_START_Y;
    TitleRect.rect.wRight = TitleRect.rect.wLeft+MAINMENU_TITLE_REGION_WIDTH-1;
    TitleRect.rect.wBottom = TitleRect.rect.wTop+MAINMENU_TITLE_REGION_HEIGHT-1;

    switch (bUIStyle)
    {
    case 1:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette2;
        break;
    case 2:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette3;
        break;
    case 3:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette4;
        break;
    case 0:
    default:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette1;
        GDI_FillRect(0|GDI_SHADING_OPERATION, &TitleRect);
        return;
    }

    GDI_FillRoundRectTop(0|GDI_SHADING_OPERATION, MAINMENU_TITLE_REGION_ARC, &TitleRect);
}

void MAINMENU_DrawMainContentBackground(void)
{
	PARM_RECT ContentRect;

    ContentRect.bColor = MAINMENU_PALETTE_ENTRY_CONTENT_REGION;
    ContentRect.rect.wLeft = MAINMENU_CONTENT_REGION_START_X;
    ContentRect.rect.wTop = MAINMENU_CONTENT_REGION_START_Y;
    ContentRect.rect.wRight = ContentRect.rect.wLeft+MAINMENU_CONTENT_REGION_WIDTH-1;
    ContentRect.rect.wBottom = ContentRect.rect.wTop+MAINMENU_CONTENT_REGION_HEIGHT-1;

    GDI_FillRect(0, &ContentRect);
}

void MAINMENU_DrawMainHelpBackground(BYTE bUIStyle)
{
	PARM_RECT HelpRect;

    HelpRect.bColor = MAINMENU_HELP_REGION_HEIGHT;
    HelpRect.rect.wLeft = MAINMENU_HELP_REGION_START_X;
    HelpRect.rect.wTop = MAINMENU_HELP_REGION_START_Y;
    HelpRect.rect.wRight = HelpRect.rect.wLeft+MAINMENU_HELP_REGION_WIDTH-1;
    HelpRect.rect.wBottom = HelpRect.rect.wTop+MAINMENU_HELP_REGION_HEIGHT-1;

    switch (bUIStyle)
    {
    case 1:
        HelpRect.bShadePtr = _bMAINMENUHelpPalette2;
        break;
    case 2:
        HelpRect.bShadePtr = _bMAINMENUHelpPalette3;
        break;
    case 3:
        HelpRect.bShadePtr = _bMAINMENUHelpPalette4;
        break;
    case 0:
    default:
        HelpRect.bShadePtr = _bMAINMENUHelpPalette1;
        GDI_FillRect(0|GDI_SHADING_OPERATION, &HelpRect);
        return;
    }

    GDI_FillRoundRectBottom(0|GDI_SHADING_OPERATION, MAINMENU_HELP_REGION_ARC, &HelpRect);
}

void MAINMENU_DrawMainBackground(void)
{
	MAINMENU_DrawMainTitleBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
	MAINMENU_DrawMainContentBackground();
	MAINMENU_DrawMainHelpBackground(__SetupInfo.bUIStyle-SETUP_CUSTOM_UI_STYLE_BEGIN);
}

void MAINMENU_ClearMainBackground(void)
{
	PARM_RECT MainBackgroundRect;

    MainBackgroundRect.bColor = PAL_ENTRY_COLOR_TRANSPARENT;
    MainBackgroundRect.rect.wLeft = MAINMENU_TITLE_REGION_START_X;
    MainBackgroundRect.rect.wTop = MAINMENU_TITLE_REGION_START_Y;
    MainBackgroundRect.rect.wRight = MainBackgroundRect.rect.wLeft+MAINMENU_TITLE_REGION_WIDTH-1;
    MainBackgroundRect.rect.wBottom = MainBackgroundRect.rect.wTop+MAINMENU_TITLE_REGION_HEIGHT+MAINMENU_CONTENT_REGION_HEIGHT+MAINMENU_HELP_REGION_HEIGHT-1;

    GDI_FillRect(0, &MainBackgroundRect);
}

void MAINMENU_ClearMainTitleBackground(BYTE bUIStyle, PARM_RECT *RectAttr)
{
    PARM_RECT TitleRect;

    TitleRect = *RectAttr;
    TitleRect.bColor = MAINMENU_TITLE_REGION_HEIGHT;    

    switch (bUIStyle)
    {
    case 1:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette2;
        break;
    case 2:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette3;
        break;
    case 3:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette4;
        break;
    case 0:
    default:
        TitleRect.bShadePtr = _bMAINMENUTitlePalette1;        
        break;
    }

    GDI_FillRect(0|GDI_SHADING_OPERATION, &TitleRect);

}

void MAINMENU_DrawTitleString(WORD *pw1stTitle, WORD *pw2ndTitle, WORD *pw3rdTitle)
{
    PARM_DRAW_STRING TitleStringAttr;
    
    MAINMENU_UseColorKey(&TitleStringAttr);
    TitleStringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_TITLE_STRING_1ST;
  
    if (pw1stTitle != NULL)
    {
        TitleStringAttr.wX = MAINMENU_TITLE_STRING_START_X;
        TitleStringAttr.wY = MAINMENU_TITLE_REGION_START_Y+((MAINMENU_TITLE_REGION_HEIGHT-GDI_GetStringHeight(pw1stTitle))>>1);        
        GDI_DrawString_909(0, &TitleStringAttr, pw1stTitle);
    }
    else
        return;

    if (pw2ndTitle != NULL)
    {
        TitleStringAttr.wX += GDI_GetStringWidth_909(pw1stTitle);
        GDI_DrawString_909(0, &TitleStringAttr, aMENUTitleSeparation);

        TitleStringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_TITLE_STRING_2ND;
        TitleStringAttr.wX += GDI_GetStringWidth_909(aMENUTitleSeparation);
        GDI_DrawString_909(0, &TitleStringAttr, pw2ndTitle);
    }
    else
        return;

    if (pw3rdTitle != NULL)
    {
        TitleStringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_TITLE_STRING_1ST;
        TitleStringAttr.wX += GDI_GetStringWidth_909(pw2ndTitle);
        GDI_DrawString_909(0, &TitleStringAttr, aMENUTitleSeparation);
        
        TitleStringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_TITLE_STRING_3RD;
        TitleStringAttr.wX += GDI_GetStringWidth_909(aMENUTitleSeparation);
        GDI_DrawString_909(0, &TitleStringAttr, pw3rdTitle);
    }    
}

void MAINMENU_UseColorKey(PARM_DRAW_STRING *StringAttr)
{
	StringAttr->bBackground = 1;
    StringAttr->bColorKey = 1;
    StringAttr->bShadow1 = 1;
    StringAttr->bShadow2 = 1;
}

void MAINMENU_DrawSelectionHelp(BYTE bUpDownOnly, BYTE bExitHelp)
{
    PARM_DRAW_STRING StringAttr;
  
	MAINMENU_UseColorKey(&StringAttr);
    StringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_HELP_STRING;  
   
    StringAttr.wX = MAINMENU_HELP_STRING_START_X;
    StringAttr.wY = MAINMENU_HELP_REGION_START_Y+((MAINMENU_HELP_REGION_HEIGHT-GDI_GetStringHeight(aMENUHelpIconLeft))>>1);

    if (!bUpDownOnly)
    {
        GDI_DrawString_909(0, &StringAttr, aMENUHelpIconLeft);
        StringAttr.wX += (GDI_GetStringWidth(aMENUHelpIconLeft)+MAINMENU_HELP_ICON_DISTANCE_H);
    }
    
    GDI_DrawString_909(0, &StringAttr, aMENUHelpIconUp);
    StringAttr.wX += (GDI_GetStringWidth(aMENUHelpIconUp)+MAINMENU_HELP_ICON_DISTANCE_H);
    
    if (!bUpDownOnly)
    {
        GDI_DrawString_909(0, &StringAttr, aMENUHelpIconRight);
        StringAttr.wX += (GDI_GetStringWidth(aMENUHelpIconRight)+MAINMENU_HELP_ICON_DISTANCE_H);
    }
    
    GDI_DrawString_909(0, &StringAttr, aMENUHelpIconDown);
    
    StringAttr.wX += GDI_GetStringWidth(aMENUHelpIconDown);
    GDI_DrawString_909(0, &StringAttr, aMENUHelpSelect);
    
    if (bExitHelp)
    {
        //Confirm
        StringAttr.wX += (GDI_GetStringWidth(aMENUHelpSelect)+MAINMENU_HELP_SELECT_EXIT_DISTANCE_H);
        GDI_DrawString_909(0, &StringAttr, aMENUHelpIconConfirm);
        StringAttr.wX += GDI_GetStringWidth(aMENUHelpIconConfirm);
        GDI_DrawString_909(0, &StringAttr, aMENUHelpConfirm);
        
        StringAttr.wX = MAINMENU_HELP_REGION_START_X+MAINMENU_HELP_REGION_WIDTH-MAINMENU_HELP_BACKGROUND_STRING_DISTANCE_H-GDI_GetStringWidth(aMENUHelpExit);
        GDI_DrawString_909(0, &StringAttr, aMENUHelpExit);        
    }
    else
    {
        StringAttr.wX = MAINMENU_HELP_REGION_START_X+MAINMENU_HELP_REGION_WIDTH-MAINMENU_HELP_BACKGROUND_STRING_DISTANCE_H-GDI_GetStringWidth(aMENUHelpConfirm);
        GDI_DrawString_909(0, &StringAttr, aMENUHelpConfirm);
        
        StringAttr.wX -= GDI_GetStringWidth(aMENUHelpIconConfirm);
        GDI_DrawString_909(0, &StringAttr, aMENUHelpIconConfirm);
    }
}

void MAINMENU_DrawExitHelpOnly(void)
{
    PARM_DRAW_STRING StringAttr;
    
    MAINMENU_UseColorKey(&StringAttr);
    StringAttr.bTextColor = MAINMENU_PALETTE_ENTRY_HELP_STRING;  
    
    StringAttr.wX = MAINMENU_HELP_REGION_START_X+MAINMENU_HELP_REGION_WIDTH-MAINMENU_HELP_BACKGROUND_STRING_DISTANCE_H-GDI_GetStringWidth(aMENUHelpExit);
    StringAttr.wY = MAINMENU_HELP_REGION_START_Y+((MAINMENU_HELP_REGION_HEIGHT-GDI_GetStringHeight(aMENUHelpExit))>>1);
    GDI_DrawString_909(0, &StringAttr, aMENUHelpExit);
}

void MAINMENU_ExitAll(BYTE bClearBackground, BYTE bForceExit)
{
#ifdef SUPPORT_STB
    BYTE bEntryIndex;
#endif //SUPPORT_STB

    _bMAINMENUMode = FALSE;

    __bSetupMode = FALSE; //Let DVD flow think SETUP mode exists.

#ifdef SUPPORT_STB
    
    for (bEntryIndex = 0; bEntryIndex < (sizeof(_MenuFlowTable)/sizeof(MENU_FLOW)); bEntryIndex++)
    {
        if (_MenuFlowTable[bEntryIndex].Menu_Exit_Func_Ptr != NULL)
        {
            _MenuFlowTable[bEntryIndex].Menu_Exit_Func_Ptr(FALSE);
        }
    }
    
    if (bClearBackground)
    {
        GDI_ClearRegion(0);
    }

    if ( __bChooseMedia == MEDIA_SELECT_STB)
    {
        if (!bForceExit)
        {
            if (__bEPGScheduleMode)
            {
                ap_ch_reswitch_nowait();
                if (normal_play_get_srv_type() == EN_SERVICE_TYPE_RADIO)
                {
                    FlowCtl_EnterNewState(FS_EPG_LIST_RADIO);
                }
                else
                {
                    FlowCtl_EnterNewState(FS_EPG_LIST_TV);
                }
            }
            else
            {
                FlowCtl_EnterNewState(FS_NORMAL_PLAY);
            }
        }
    }
    else //DVD mode.
    {
        if (!bForceExit)
        {
            OSD_ChangeUI(OSD_UI_MAIN_MENU, OSD_EXIT_UI);
            _MAINMENU_ExitDVDMode();
        }
    }

#else
    DVDSETUP_Exit(bClearBackground);
        
    if (!bForceExit)
    {
        //OSD_ChangeUI(OSD_UI_MAIN_MENU, OSD_EXIT_UI);
        OSD_ChangeUI(OSD_UI_SETUP, OSD_EXIT_UI);
    }
    
    _MAINMENU_ExitDVDMode();
#endif //#ifdef SUPPORT_STB

}

void _MAINMENU_InitDVDMode(void)
{
    if (!(__wDiscType & BOOK_DVD))
    {
        if (__btPlaying) // Brian2.81, thumbnail support KEY_PAUSE
        {
            // brian.171a, change to KEY_PLAY then KEY_PAUSE
            // This is a workaround to make SCF/SCB, FF look like paused
            // (before the real KEY_PAUSE issue is solved)
            // However add this would also make it change to PLAY mode when exit Setup.
            // This is a side effect. When KEY_PAUSE issue is sovled for SCF/SCB, FF,
            // we could remove this part
#ifdef  PLAY_AFTER_EXIT_SETUP
            // this for none-DVD
            if (__bAttrPlay&TYPE_CDROM_AUDIO) // Brian1.25, audio only could directly enter pause mode. Otherwise ==> // Kevin, MP3 SCF/SCB->Setup->Exit Setup=> jump to next file (below (KEY_PLAY) will reset audio s.t. A2-Rem < 720) 
                __bModeCmd = KEY_PLAY;
            
#ifndef PRESTOP_BEFORE_SETUP_AND_PROGRAM
            //xuli2.34, Because MP3 still toggle KEY_PAUSE when enter/exit SETUP, need set __bModeCmd to KEY_PLAY before issue KEY_PAUSE, or else KEY_PAUSE will be INVALID.                  
            // wyc2.00-909S, don't issue play command again when exit setup mode from JPG preview active, because preview mode also is play
            // mode and don't need to issue play again. And issue play again also will call reset video and it will abort JPG decode and make
            // JPG decoding progress aborted and won't continue to decode after exit setup mode.
            if (( __bModeCmd != KEY_PLAY) && (__dwMMJPEGPreview == FALSE) && (!__bThumbnailMode))  // if it is already in play mode, don't need to issue KEY_PLAY again
                CC_DoKeyAction (KEY_PLAY);
#endif  //#ifndef PRESTOP_BEFORE_SETUP_AND_PROGRAM
#endif  //#ifdef  PLAY_AFTER_EXIT_SETUP
            
#ifndef PRESTOP_BEFORE_SETUP_AND_PROGRAM //XuLi.230
            CC_DoKeyAction (KEY_PAUSE);
#endif
        }
        
        // Brian1.10, add kevin's support for MP3 Fast Forward.
#ifdef  PLAY_AFTER_EXIT_SETUP
        //MP3 Fast Forward case, SCF->enter SETUP->exit SETUP => Normal Play
        if (__bAttrPlay==ATTR_MP3)
        {   
            // always set to normal play
            __bScanLevel=0;
            HAL_WriteAM(HAL_AM_FF_SPEED, __bScanLevel); 
        }
#endif
        
#ifndef NO_DIGEST //LJY1.20, support 4M flash, remove digest
        //LJY1.00, fix the noise problem while it entered setup in digest mode.
        if(__btDIGEST)
        {
            CHIPS_OpenAudio(FALSE);
        }
#endif
        
    }
    else if (__btPlaying)
    {
        // brian.171a, chnage to KEY_PLAY then KEY_PAUSE
#ifdef  PLAY_AFTER_EXIT_SETUP
#ifndef PRESTOP_BEFORE_SETUP_AND_PROGRAM //XuLi.230
        // this is for DVD
        if (__bModeCmd != KEY_PLAY) // if it is already in play mode, don't need to issue KEY_PLAY again
            CC_DoKeyAction (KEY_PLAY);
#endif  //#ifndef PRESTOP_BEFORE_SETUP_AND_PROGRAM
#endif  //#ifdef  PLAY_AFTER_EXIT_SETUP
        
#ifndef PRESTOP_BEFORE_SETUP_AND_PROGRAM //XuLi.230
        CC_DoKeyAction (KEY_PAUSE);
#endif
    }
}

void _MAINMENU_ExitDVDMode()
{
    // Brian0.87, disable one field display when exit Setup
    DISP_ForceOneField(FALSE);
    
    // brian.277a Test Tone
    // PCM_SCALE may be set by Test Tone, so we turn it off as it was when we enter Setup
    // if we don't turn PCM_SCALE off, there may be noise (on AMP) when it change Audio Output
    //CHIPS_OpenAudio(FALSE);
    
#ifndef NO_DISC_MODE //CoCo2.38    
    //turn on highlight
    HAL_ControlHighlight(HAL_HLI_ACTION_SELECT, __bNowBTNNumber+__bGroupOffset);
#endif
    
    //__bKey=KEY_NO_KEY;  
    
#ifdef PRESTOP_BEFORE_SETUP_AND_PROGRAM
    if (__wDiscType & BOOK_M1)
    {
        // wyc2.00-909S, only restore main video in MM UI mode because JPG slide show or motion mode we don't disable main video when entering media select UI.
        // Also will happen video scramble when open main video during JPG decoding here, so must only open it when MM UI mode.
        if ((__bMMMenuID != MM_MENU_NONE) || (__bThumbnailMode))
        {
            DISP_DisplayCtrl(DISP_MAINVIDEO, TRUE);
        }
    }

    CC_ForceResumeRePlay(CC_FORCE_STOP_FROM_SETUP);
#else // #ifdef PRESTOP_BEFORE_SETUP_AND_PROGRAM
    if (__btPause)
    {
        // wyc2.00-909S, because the __bModeCmd is KEY_STOP when JPG preview decoding mode and it will make CC_DoKeyAction(KEY_PAUSE) to issue
        // CC_KeyCommand(KEY_STOP) and then JPG preview won't continue when exit setup mode. So we set __bModeCmd to KEY_PLAY to 
        // cheat CC_DoKeyAction(KEY_PAUSE) to issue CC_KeyCommand(KEY_PLAY) to continue to decode preview after exiting setup mode.
        if (__dwMMJPEGPreview == TRUE)
            __bModeCmd = KEY_PLAY;
        CC_DoKeyAction(KEY_PAUSE);
    }
#endif
        
#ifndef NO_DISC_MODE //CoCo2.37p
    _IdentifyRegionCode();  // brian.161 check region code (show or clear the error message)
#endif //NO_DISC_MODE
    
#ifndef NO_DIGEST //LJY1.20, support 4M flash, remove digest
    //LJY0.87, enable audio for digest mode
    if (__btDIGEST)
        CHIPS_OpenAudio( TRUE );
#endif    
}

void MAINMENU_Exit(BYTE bClearBackground)
{
    MAINMENU_ExitAll(bClearBackground, TRUE);
}

#ifdef SUPPORT_STB
void MAINMENU_DVD_Recover(BYTE bRecoverRegion)
{
    MAINMENU_Recover();
}
#endif //SUPPORT_STB



