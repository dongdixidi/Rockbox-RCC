/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 *
 * Copyright (C) 2006 Antoine Cellerier <dionoea @t videolan d.t org>
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

/* *
 * Button Code Definitions for archos recorder target
 *
 * \TODO handle F3
 */

#include "config.h"
#include "action.h"
#include "button.h"
#include "settings.h"

/* CONTEXT_CUSTOM's used in this file...

CONTEXT_CUSTOM|1 = the standard list/tree defines (without directions)


*/

static const struct button_mapping button_context_standard[]  = {
    { ACTION_STD_PREV,        BUTTON_UP,                  BUTTON_NONE },
    { ACTION_STD_PREVREPEAT,  BUTTON_UP|BUTTON_REPEAT,    BUTTON_NONE },
    { ACTION_STD_NEXT,        BUTTON_DOWN,                BUTTON_NONE },
    { ACTION_STD_NEXTREPEAT,  BUTTON_DOWN|BUTTON_REPEAT,  BUTTON_NONE },

    { ACTION_STD_OK,          BUTTON_ON,                  BUTTON_NONE },
    { ACTION_STD_OK,          BUTTON_RIGHT,               BUTTON_NONE },
    { ACTION_STD_OK,          BUTTON_RIGHT|BUTTON_REPEAT, BUTTON_RIGHT },

    { ACTION_STD_OK,          BUTTON_PLAY|BUTTON_REL,     BUTTON_PLAY },
    { ACTION_STD_MENU,        BUTTON_F1,                  BUTTON_NONE },
    { ACTION_STD_QUICKSCREEN, BUTTON_F2,                  BUTTON_NONE },
    { ACTION_STD_CONTEXT,     BUTTON_PLAY|BUTTON_REPEAT,  BUTTON_PLAY },
    { ACTION_STD_CANCEL,      BUTTON_OFF,                 BUTTON_NONE },
    { ACTION_STD_CANCEL,      BUTTON_LEFT,                BUTTON_NONE },
    { ACTION_STD_CANCEL,      BUTTON_LEFT|BUTTON_REPEAT,  BUTTON_NONE },

    LAST_ITEM_IN_LIST
};

static const struct button_mapping button_context_wps[]  = {
    { ACTION_WPS_PLAY,        BUTTON_PLAY,                BUTTON_NONE },
    { ACTION_WPS_SKIPNEXT,    BUTTON_RIGHT|BUTTON_REL,    BUTTON_RIGHT },
    { ACTION_WPS_SKIPPREV,    BUTTON_LEFT|BUTTON_REL,     BUTTON_LEFT },
    { ACTION_WPS_SEEKBACK,    BUTTON_LEFT|BUTTON_REPEAT,  BUTTON_NONE },
    { ACTION_WPS_SEEKFWD,     BUTTON_RIGHT|BUTTON_REPEAT, BUTTON_NONE },
    { ACTION_WPS_STOPSEEK,    BUTTON_LEFT|BUTTON_REL,     BUTTON_LEFT|BUTTON_REPEAT },
    { ACTION_WPS_STOPSEEK,    BUTTON_RIGHT|BUTTON_REL,    BUTTON_RIGHT|BUTTON_REPEAT },
    { ACTION_WPS_STOP,        BUTTON_OFF,                 BUTTON_NONE },
    { ACTION_WPS_VOLDOWN,     BUTTON_DOWN,                BUTTON_NONE },
    { ACTION_WPS_VOLDOWN,     BUTTON_DOWN|BUTTON_REPEAT,  BUTTON_NONE },
    { ACTION_WPS_VOLUP,       BUTTON_UP,                  BUTTON_NONE },
    { ACTION_WPS_VOLUP,       BUTTON_UP|BUTTON_REPEAT,    BUTTON_NONE },
    { ACTION_WPS_MENU,        BUTTON_F1,                  BUTTON_NONE },
    { ACTION_WPS_CONTEXT,     BUTTON_PLAY|BUTTON_REPEAT,  BUTTON_PLAY },
    { ACTION_WPS_QUICKSCREEN, BUTTON_F2,                  BUTTON_NONE },
    { ACTION_WPS_BROWSE,      BUTTON_ON|BUTTON_REL,       BUTTON_ON   },
    { ACTION_WPS_ID3SCREEN,   BUTTON_F1|BUTTON_ON,        BUTTON_F1 },
    { ACTION_WPS_PITCHSCREEN, BUTTON_ON|BUTTON_UP,        BUTTON_ON },
    { ACTION_WPS_PITCHSCREEN, BUTTON_ON|BUTTON_DOWN,      BUTTON_ON },
    { ACTION_STD_KEYLOCK,     BUTTON_F1|BUTTON_DOWN,      BUTTON_NONE },
    

    LAST_ITEM_IN_LIST
};

static const struct button_mapping button_context_settings[] = {
    { ACTION_SETTINGS_INC,          BUTTON_UP,                  BUTTON_NONE },
    { ACTION_SETTINGS_INCREPEAT,    BUTTON_UP|BUTTON_REPEAT,    BUTTON_NONE },
    { ACTION_SETTINGS_DEC,          BUTTON_DOWN,                BUTTON_NONE },
    { ACTION_SETTINGS_DECREPEAT,    BUTTON_DOWN|BUTTON_REPEAT,  BUTTON_NONE },

    LAST_ITEM_IN_LIST
};

static const struct button_mapping button_context_tree[]  = {
    { ACTION_TREE_WPS,     BUTTON_ON,      BUTTON_NONE },
    { ACTION_TREE_STOP,    BUTTON_OFF,     BUTTON_NONE },
    
    LAST_ITEM_IN_LIST
}; /* button_context_listtree */

static const struct button_mapping button_context_tree_scroll_lr[]  = {
    { ACTION_NONE,              BUTTON_LEFT,                BUTTON_NONE },
    { ACTION_STD_CANCEL,        BUTTON_LEFT|BUTTON_REL,     BUTTON_LEFT },
    { ACTION_TREE_PGLEFT,       BUTTON_LEFT|BUTTON_REPEAT,  BUTTON_NONE },
    { ACTION_TREE_PGLEFT,       BUTTON_LEFT|BUTTON_REL,     BUTTON_LEFT|BUTTON_REPEAT },
    { ACTION_NONE,              BUTTON_RIGHT,               BUTTON_NONE },
    { ACTION_STD_OK,            BUTTON_RIGHT|BUTTON_REL,    BUTTON_RIGHT },
    { ACTION_TREE_PGRIGHT,      BUTTON_RIGHT|BUTTON_REPEAT, BUTTON_NONE },
    { ACTION_TREE_PGRIGHT,      BUTTON_RIGHT|BUTTON_REL,    BUTTON_RIGHT|BUTTON_REPEAT },    
    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_CUSTOM|1),
};

static const struct button_mapping button_context_yesno[] = {
    { ACTION_YESNO_ACCEPT,     BUTTON_PLAY,    BUTTON_NONE },
    
    LAST_ITEM_IN_LIST
}; 
static const struct button_mapping button_context_quickscreen[]  = {
    { ACTION_QS_DOWNINV,    BUTTON_UP,                      BUTTON_NONE },
    { ACTION_QS_DOWNINV,    BUTTON_UP|BUTTON_REPEAT,        BUTTON_NONE },
    { ACTION_QS_DOWN,       BUTTON_DOWN,                    BUTTON_NONE },
    { ACTION_QS_DOWN,       BUTTON_DOWN|BUTTON_REPEAT,      BUTTON_NONE },
    { ACTION_QS_LEFT,       BUTTON_LEFT,                    BUTTON_NONE },
    { ACTION_QS_LEFT,       BUTTON_LEFT|BUTTON_REPEAT,      BUTTON_NONE },
    { ACTION_QS_RIGHT,      BUTTON_RIGHT,                   BUTTON_NONE },
    { ACTION_QS_RIGHT,      BUTTON_RIGHT|BUTTON_REPEAT,     BUTTON_NONE },
    { ACTION_STD_CANCEL,    BUTTON_PLAY,                    BUTTON_NONE },
    
    LAST_ITEM_IN_LIST
}; /* button_context_quickscreen */

static const struct button_mapping button_context_pitchscreen[]  = {
    { ACTION_PS_INC_SMALL,      BUTTON_UP,                  BUTTON_NONE },
    { ACTION_PS_INC_BIG,        BUTTON_UP|BUTTON_REPEAT,    BUTTON_NONE },
    { ACTION_PS_DEC_SMALL,      BUTTON_DOWN,                BUTTON_NONE },
    { ACTION_PS_DEC_BIG,        BUTTON_DOWN|BUTTON_REPEAT,  BUTTON_NONE },
    { ACTION_PS_NUDGE_LEFT,     BUTTON_LEFT,                BUTTON_NONE },
    { ACTION_PS_NUDGE_LEFTOFF,  BUTTON_LEFT|BUTTON_REL,     BUTTON_NONE },
    { ACTION_PS_NUDGE_RIGHT,    BUTTON_RIGHT,               BUTTON_NONE },
    { ACTION_PS_NUDGE_RIGHTOFF, BUTTON_RIGHT|BUTTON_REL,    BUTTON_NONE },
    { ACTION_PS_RESET,          BUTTON_ON,                  BUTTON_NONE },
    { ACTION_PS_EXIT,           BUTTON_OFF,                 BUTTON_NONE },
    
    LAST_ITEM_IN_LIST
}; /* button_context_pitchcreen */

const struct button_mapping button_context_recscreen[]  = {
    { ACTION_REC_PAUSE,             BUTTON_PLAY,                BUTTON_NONE },
    { ACTION_REC_F2,                BUTTON_F2,                  BUTTON_NONE },
    { ACTION_REC_F3,                BUTTON_F3,                  BUTTON_NONE },
    { ACTION_SETTINGS_INC,          BUTTON_RIGHT,               BUTTON_NONE },
    { ACTION_SETTINGS_INC,          BUTTON_RIGHT|BUTTON_REPEAT, BUTTON_NONE },
    { ACTION_SETTINGS_DEC,          BUTTON_LEFT,                BUTTON_NONE },
    { ACTION_SETTINGS_DEC,          BUTTON_LEFT|BUTTON_REPEAT,  BUTTON_NONE },
    
    LAST_ITEM_IN_LIST
}; /* button_context_recscreen */

/*****************************************************************************
 *    Remote control mappings
 *****************************************************************************/

static const struct button_mapping remote_button_context_standard[]  = {
    { ACTION_STD_PREV,      BUTTON_RC_LEFT,     BUTTON_NONE },
    { ACTION_STD_NEXT,      BUTTON_RC_RIGHT,    BUTTON_NONE },
    { ACTION_STD_CANCEL,    BUTTON_RC_STOP,     BUTTON_NONE },
    { ACTION_STD_OK,        BUTTON_RC_PLAY,     BUTTON_NONE },

    LAST_ITEM_IN_LIST
};

static const struct button_mapping remote_button_context_wps[]  = {
    { ACTION_WPS_PLAY,      BUTTON_RC_PLAY,     BUTTON_NONE },
    { ACTION_WPS_SKIPNEXT,  BUTTON_RC_RIGHT,    BUTTON_NONE },
    { ACTION_WPS_SKIPPREV,  BUTTON_RC_LEFT,     BUTTON_NONE },
    { ACTION_WPS_STOP,      BUTTON_RC_STOP,     BUTTON_NONE },
    
    { ACTION_WPS_VOLDOWN,   BUTTON_RC_VOL_DOWN, BUTTON_NONE },
    { ACTION_WPS_VOLUP,     BUTTON_RC_VOL_UP,   BUTTON_NONE },

    LAST_ITEM_IN_LIST
};


static const struct button_mapping* get_context_mapping_remote( int context )
{
    context ^= CONTEXT_REMOTE;
    
    switch (context)
    {
        case CONTEXT_WPS:
            return remote_button_context_wps;

        default:
            return remote_button_context_standard;
    }
}

const struct button_mapping* get_context_mapping( int context )
{
    if (context&CONTEXT_REMOTE)
        return get_context_mapping_remote(context);
    
    switch( context )
    {
        case CONTEXT_WPS:
            return button_context_wps;

        case CONTEXT_SETTINGS:
            return button_context_settings;

        case CONTEXT_YESNOSCREEN:
            return button_context_yesno;

        case CONTEXT_PITCHSCREEN:
            return button_context_pitchscreen;

        case CONTEXT_TREE:
            if (global_settings.hold_lr_for_scroll_in_list)
                return button_context_tree_scroll_lr;
            /* else fall through to CUSTOM|1 */
        case CONTEXT_CUSTOM|1:
            return button_context_tree;

        case CONTEXT_QUICKSCREEN:
            return button_context_quickscreen;

        case CONTEXT_RECSCREEN:
            return button_context_recscreen;

        case CONTEXT_STD:
        case CONTEXT_LIST:
        case CONTEXT_MAINMENU:
        default:
            return button_context_standard;
    }
}
