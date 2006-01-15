/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2005 Jonas H�ggqvist
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 **************************************************************************/
#include "plugin.h"
#include "playergfx.h"

PLUGIN_HEADER

#ifdef HAVE_LCD_BITMAP
#define DISPLAY_WIDTH LCD_WIDTH
#define DISPLAY_HEIGHT LCD_HEIGHT
#define RAND_SCALE 5

#ifdef HAVE_REMOTE_LCD
#define REMOTE_WIDTH LCD_REMOTE_WIDTH
#define REMOTE_HEIGHT LCD_REMOTE_HEIGHT
#define REMOTE_LOGO_WIDTH 91
#define REMOTE_LOGO_HEIGHT 32
#define REMOTE_LOGO rockbox91x32
/* Smaller logo for the remote */
const unsigned char rockbox91x32[] = {
    0x00, 0x02, 0x7f, 0x02, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xf8, 0xf8, 0xf0,
    0xe0, 0x80, 0x00, 0x00, 0x80, 0xe0, 0xf0, 0xf0, 0xf8, 0xfa, 0xfa, 0xf4, 0xf4,
    0xe8, 0x90, 0x60, 0x80, 0x60, 0x90, 0xe8, 0xf4, 0xf4, 0xfa, 0xfa, 0xfa, 0xfa,
    0xf4, 0xe8, 0x04, 0xf4, 0xf4, 0xf4, 0x04, 0xfe, 0x04, 0xc0, 0xf0, 0xf4, 0xf4,
    0x34, 0x1e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x70, 0x88, 0xf0, 0x00, 0xff, 0xff, 0xff, 0xff, 0x02, 0xc2, 0x3c, 0xc3, 0xff,
    0xff, 0xff, 0x00, 0xfe, 0xff, 0xff, 0x07, 0x01, 0x00, 0xfe, 0x02, 0xfd, 0x07,
    0xff, 0xff, 0xfe, 0x01, 0xfe, 0xff, 0xff, 0x0f, 0x01, 0x00, 0x80, 0x00, 0x01,
    0x03, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xc0, 0xf9, 0xfe, 0xff, 0xff, 0x07, 0x04,
    0x04, 0x04, 0xfc, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc0, 0x40, 0x40,
    0x40, 0x40, 0x80, 0x00, 0x80, 0x40, 0x80, 0x40, 0x40, 0x40, 0x40, 0xc0, 0x00,

    0x00, 0x7c, 0xc3, 0x9e, 0x67, 0x37, 0x8f, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xe3, 0x80, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0x03, 0xc1, 0x3f, 0xc1,
    0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xc1, 0x01, 0x01, 0xff, 0x01, 0x00,
    0x00, 0xc0, 0x00, 0xff, 0xff, 0xff, 0xcf, 0x3f, 0xff, 0xf8, 0xff, 0x00, 0x00,
    0x00, 0x00, 0xe0, 0x10, 0x08, 0x18, 0xe8, 0x10, 0xe0, 0x00, 0x01, 0x02, 0x04,
    0x02, 0x01, 0xe0, 0x10, 0x08, 0x08, 0x18, 0xe8, 0x10, 0xe0, 0x00, 0x01, 0x00,
    0x00, 0xe0, 0x40, 0x01, 0x02, 0x01, 0x40, 0xe0, 0x18, 0xe4, 0x1b, 0x04, 0x03,

    0x00, 0x00, 0x00, 0x00, 0x77, 0x6e, 0x61, 0x7f, 0x40, 0xff, 0x00, 0x07, 0x1f,
    0x3f, 0x7f, 0x7c, 0x71, 0xcf, 0x5f, 0x3f, 0x7e, 0x7c, 0x7d, 0x7d, 0x7e, 0x3f,
    0x1f, 0x0f, 0x01, 0x00, 0x41, 0x47, 0x5f, 0x7f, 0x7e, 0x7c, 0x7f, 0x7c, 0x7e,
    0x3f, 0x1f, 0x00, 0x7f, 0x7f, 0x7f, 0x01, 0x4e, 0x71, 0xcf, 0x7f, 0x40, 0x40,
    0x40, 0x40, 0x60, 0x21, 0x42, 0x43, 0x42, 0x41, 0x40, 0x20, 0x10, 0x08, 0x04,
    0x08, 0x10, 0x20, 0x41, 0x42, 0x42, 0x43, 0x42, 0x41, 0x40, 0x60, 0x50, 0x40,
    0x40, 0x40, 0x20, 0x50, 0x28, 0x10, 0x20, 0x40, 0x43, 0x44, 0x5b, 0x64, 0x18,
};
#endif /* HAVE_REMOTE_LCD */

#if LCD_WIDTH > 112
#define LOGO_WIDTH 112
#define LOGO_HEIGHT 37
#define LOGO rockbox112x37
const unsigned char rockbox112x37[]={
        0x00, 0x00, 0x02, 0xff, 0x02, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa,
        0xf8, 0xf8, 0xf0, 0xe0, 0x80, 0x00, 0x00, 0x80, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc,
        0x7c, 0x7d, 0xfd, 0xfa, 0xfa, 0xf4, 0xe8, 0x90, 0x60, 0x80, 0xe0, 0x10, 0xc8,
        0xe4, 0xf2, 0xfa, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfa, 0xfa, 0xf4, 0x02, 0xfa,
        0xfa, 0xfa, 0xfa, 0x02, 0xff, 0x02, 0x00, 0x80, 0xe2, 0xfa, 0xfa, 0xfa, 0xfa,
        0x3a, 0x0e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x60, 0x90, 0x20, 0xc0, 0x00, 0xff, 0xff, 0xff, 0xff, 0x05, 0x05, 0x05, 0xf9,
        0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0xfc, 0xff, 0xff, 0xff, 0x0f, 0x01, 0x00,
        0xff, 0x01, 0x0e, 0xf1, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x03, 0xf8, 0xff, 0xff,
        0xff, 0x0f, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x0f, 0x00, 0xff,
        0xff, 0xff, 0xff, 0x80, 0xff, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0x07, 0x07, 0x04,
        0x04, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0xc0, 0x38, 0x07, 0x9d, 0x60, 0xbf, 0xbf, 0xff, 0xff, 0xfc, 0xff, 0xfd, 0xfe,
        0xff, 0xff, 0x9f, 0x0f, 0x03, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0x08,
        0x3f, 0x08, 0x08, 0xff, 0x08, 0xff, 0xff, 0xff, 0xff, 0x08, 0xff, 0xff, 0xff,
        0xff, 0x08, 0x08, 0x08, 0x08, 0xfe, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0xff,
        0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xf3, 0xc0, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x82, 0x41, 0x41, 0xa1, 0xa1, 0x41, 0x41, 0x81, 0x02, 0x02, 0x04,
        0x08, 0x30, 0x08, 0x04, 0x02, 0x02, 0x81, 0x41, 0x41, 0xa1, 0xa1, 0x41, 0x41,
        0x81, 0x01, 0x03, 0x05, 0x01, 0x01, 0x01, 0x01, 0x02, 0x0c, 0x12, 0x0d, 0x02,
        0x01, 0x01, 0xc1, 0x31, 0xc9, 0x35, 0x0b, 0x04,

        0x01, 0x07, 0x0c, 0x09, 0x18, 0xe3, 0x1b, 0xfc, 0xff, 0x00, 0xff, 0x03, 0x1f,
        0x7f, 0xff, 0xff, 0xfc, 0xf0, 0x80, 0x0f, 0x7f, 0xff, 0xff, 0xfc, 0xe0, 0xc0,
        0xa0, 0xa0, 0xdc, 0xe3, 0xfc, 0xff, 0xff, 0x7f, 0x0f, 0x00, 0x07, 0x3f, 0xff,
        0xff, 0xfc, 0xf0, 0xe0, 0xc0, 0xff, 0xc0, 0xc0, 0xe0, 0xf0, 0xfc, 0x00, 0xff,
        0xff, 0xff, 0xff, 0x06, 0x19, 0x67, 0x9f, 0x7f, 0xff, 0xff, 0xfc, 0xf0, 0xc0,
        0x00, 0x06, 0x19, 0x20, 0x20, 0x50, 0x50, 0x29, 0x26, 0x19, 0x06, 0x00, 0x00,
        0x00, 0xc0, 0x00, 0x00, 0x00, 0x06, 0x19, 0x20, 0x20, 0x50, 0x50, 0x29, 0x26,
        0x19, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x06, 0x00, 0x00, 0x80, 0x00, 0x00,
        0x06, 0x09, 0x36, 0xc9, 0x30, 0xc0, 0x00, 0x00,

        0x20, 0xa0, 0x00, 0x40, 0x83, 0xec, 0x0c, 0x0f, 0x0f, 0xe8, 0xff, 0xa8, 0x08,
        0x00, 0x01, 0x0f, 0x0f, 0x0f, 0x0f, 0x0e, 0x58, 0xc9, 0x03, 0x47, 0x07, 0xef,
        0xef, 0xaf, 0x0f, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x08, 0x48, 0xe8, 0xe8,
        0xa9, 0x4b, 0xef, 0xef, 0xaf, 0xaf, 0xaf, 0x07, 0x47, 0x27, 0xc3, 0x00, 0x4f,
        0x8f, 0xef, 0x0f, 0x00, 0x00, 0xe8, 0xe9, 0xae, 0x19, 0x0f, 0x0f, 0x0f, 0x0f,
        0x0f, 0x0c, 0x04, 0x48, 0xc8, 0x08, 0x48, 0x68, 0x48, 0x08, 0x04, 0x24, 0xe2,
        0xe1, 0xa0, 0x61, 0x42, 0x04, 0x04, 0x88, 0x28, 0x08, 0x08, 0x28, 0xe8, 0xe8,
        0xa8, 0xc8, 0xec, 0xea, 0xa8, 0x28, 0xa8, 0x08, 0x44, 0xeb, 0x24, 0x03, 0x04,
        0x08, 0xe8, 0xe8, 0xa8, 0x09, 0x0a, 0x0d, 0x02,
};

#else /* LCD_WIDTH <= 112 */
#define LOGO_WIDTH 91
#define LOGO_HEIGHT 32
#define LOGO rockbox91x32
const unsigned char rockbox91x32[] = {
        0x00, 0x02, 0x7f, 0x02, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xf8, 0xf8, 0xf0,
        0xe0, 0x80, 0x00, 0x00, 0x80, 0xe0, 0xf0, 0xf0, 0xf8, 0xfa, 0xfa, 0xf4, 0xf4,
        0xe8, 0x90, 0x60, 0x80, 0x60, 0x90, 0xe8, 0xf4, 0xf4, 0xfa, 0xfa, 0xfa, 0xfa,
        0xf4, 0xe8, 0x04, 0xf4, 0xf4, 0xf4, 0x04, 0xfe, 0x04, 0xc0, 0xf0, 0xf4, 0xf4,
        0x34, 0x1e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x70, 0x88, 0xf0, 0x00, 0xff, 0xff, 0xff, 0xff, 0x02, 0xc2, 0x3c, 0xc3, 0xff,
        0xff, 0xff, 0x00, 0xfe, 0xff, 0xff, 0x07, 0x01, 0x00, 0xfe, 0x02, 0xfd, 0x07,
        0xff, 0xff, 0xfe, 0x01, 0xfe, 0xff, 0xff, 0x0f, 0x01, 0x00, 0x80, 0x00, 0x01,
        0x03, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xc0, 0xf9, 0xfe, 0xff, 0xff, 0x07, 0x04,
        0x04, 0x04, 0xfc, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc0, 0x40, 0x40,
        0x40, 0x40, 0x80, 0x00, 0x80, 0x40, 0x80, 0x40, 0x40, 0x40, 0x40, 0xc0, 0x00,

        0x00, 0x7c, 0xc3, 0x9e, 0x67, 0x37, 0x8f, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff,
        0xe3, 0x80, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0x03, 0xc1, 0x3f, 0xc1,
        0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xc1, 0x01, 0x01, 0xff, 0x01, 0x00,
        0x00, 0xc0, 0x00, 0xff, 0xff, 0xff, 0xcf, 0x3f, 0xff, 0xf8, 0xff, 0x00, 0x00,
        0x00, 0x00, 0xe0, 0x10, 0x08, 0x18, 0xe8, 0x10, 0xe0, 0x00, 0x01, 0x02, 0x04,
        0x02, 0x01, 0xe0, 0x10, 0x08, 0x08, 0x18, 0xe8, 0x10, 0xe0, 0x00, 0x01, 0x00,
        0x00, 0xe0, 0x40, 0x01, 0x02, 0x01, 0x40, 0xe0, 0x18, 0xe4, 0x1b, 0x04, 0x03,

        0x00, 0x00, 0x00, 0x00, 0x77, 0x6e, 0x61, 0x7f, 0x40, 0xff, 0x00, 0x07, 0x1f,
        0x3f, 0x7f, 0x7c, 0x71, 0xcf, 0x5f, 0x3f, 0x7e, 0x7c, 0x7d, 0x7d, 0x7e, 0x3f,
        0x1f, 0x0f, 0x01, 0x00, 0x41, 0x47, 0x5f, 0x7f, 0x7e, 0x7c, 0x7f, 0x7c, 0x7e,
        0x3f, 0x1f, 0x00, 0x7f, 0x7f, 0x7f, 0x01, 0x4e, 0x71, 0xcf, 0x7f, 0x40, 0x40,
        0x40, 0x40, 0x60, 0x21, 0x42, 0x43, 0x42, 0x41, 0x40, 0x20, 0x10, 0x08, 0x04,
        0x08, 0x10, 0x20, 0x41, 0x42, 0x42, 0x43, 0x42, 0x41, 0x40, 0x60, 0x50, 0x40,
        0x40, 0x40, 0x20, 0x50, 0x28, 0x10, 0x20, 0x40, 0x43, 0x44, 0x5b, 0x64, 0x18,
};
#endif /* LCD_WIDTH */

#else /* !LCD_BITMAP */
#define DISPLAY_WIDTH 55
#define DISPLAY_HEIGHT 14
#define RAND_SCALE 2
#define LOGO_WIDTH 16
#define LOGO_HEIGHT 7
#define LOGO rockbox16x7
const unsigned char rockbox16x7[] = {
    0x47, 0x18, 0xa6, 0xd8, 0x66, 0xde, 0xb7, 0x9b,
    0x76, 0xdb, 0x26, 0xdb, 0x66, 0xde,
};
#endif /* !LCD_BITMAP */

/* variable button definitions */
#if CONFIG_KEYPAD == PLAYER_PAD
#define LP_QUIT BUTTON_STOP
#define LP_DEC_X BUTTON_LEFT
#define LP_INC_X BUTTON_RIGHT
#define LP_DEC_Y (BUTTON_ON | BUTTON_LEFT)
#define LP_INC_Y (BUTTON_ON | BUTTON_RIGHT)
#elif (CONFIG_KEYPAD == IPOD_4G_PAD)
#define LP_QUIT BUTTON_MENU
#define LP_DEC_X BUTTON_LEFT
#define LP_INC_X BUTTON_RIGHT
#define LP_DEC_Y BUTTON_SCROLL_BACK
#define LP_INC_Y BUTTON_SCROLL_FWD
#elif CONFIG_KEYPAD == IRIVER_IFP7XX_PAD
#define LP_QUIT BUTTON_PLAY
#define LP_DEC_X BUTTON_LEFT
#define LP_INC_X BUTTON_RIGHT
#define LP_DEC_Y BUTTON_DOWN
#define LP_INC_Y BUTTON_UP
#else
#define LP_QUIT BUTTON_OFF
#define LP_DEC_X BUTTON_LEFT
#define LP_INC_X BUTTON_RIGHT
#define LP_DEC_Y BUTTON_DOWN
#define LP_INC_Y BUTTON_UP
#endif

#ifdef CONFIG_REMOTE_KEYPAD
#if (CONFIG_REMOTE_KEYPAD == H100_REMOTE) || \
    (CONFIG_REMOTE_KEYPAD == H300_REMOTE)
#define LP_R_QUIT BUTTON_RC_STOP
#define LP_R_DEC_X BUTTON_RC_REW
#define LP_R_INC_X BUTTON_RC_FF
#define LP_R_DEC_Y BUTTON_RC_SOURCE
#define LP_R_INC_Y BUTTON_RC_BITRATE
#endif
#endif /* CONFIG_REMOTE_KEYPAD */


enum plugin_status plugin_start(struct plugin_api* api, void* parameter) {
    int button;
    int timer = 10;
    int x = (DISPLAY_WIDTH / 2) - (LOGO_WIDTH / 2);
    int y = (DISPLAY_HEIGHT / 2) - (LOGO_HEIGHT / 2);
    struct plugin_api* rb = api;
    int dx;
    int dy;
#ifdef HAVE_LCD_CHARCELLS
    int cpos = -1;
    int old_cpos = -1;
#endif

    (void)parameter;

#ifdef HAVE_LCD_CHARCELLS
    if (!pgfx_init(rb, 4, 2)) {
        rb->splash(HZ*2, true, "Old LCD :(");
        return PLUGIN_OK;
    }
#endif
    rb->srand(*rb->current_tick);
    dx = rb->rand()%(2*RAND_SCALE+1) - RAND_SCALE;
    dy = rb->rand()%(2*RAND_SCALE+1) - RAND_SCALE;

    while (1) {
#ifdef HAVE_LCD_BITMAP
        rb->lcd_clear_display();
        rb->lcd_mono_bitmap(LOGO, x, y, LOGO_WIDTH, LOGO_HEIGHT);
#ifdef REMOTE_LOGO
        rb->lcd_remote_clear_display();
        rb->lcd_remote_mono_bitmap(REMOTE_LOGO,
                (x * (REMOTE_WIDTH - REMOTE_LOGO_WIDTH)) / (DISPLAY_WIDTH - LOGO_WIDTH),
                (y * (REMOTE_HEIGHT - REMOTE_LOGO_HEIGHT)) / (DISPLAY_HEIGHT - LOGO_HEIGHT),
                REMOTE_LOGO_WIDTH, REMOTE_LOGO_HEIGHT);
#endif
#else
        pgfx_clear_display();
        pgfx_mono_bitmap(LOGO, x % 5, y, LOGO_WIDTH, LOGO_HEIGHT);
        cpos = x / 5;
#endif

        x += dx;
        if (x < 0) {
            dx = -dx;
            x = 0;
        }
        if (x > DISPLAY_WIDTH - LOGO_WIDTH) {
            dx = -dx;
            x = DISPLAY_WIDTH - LOGO_WIDTH;
        }

        y += dy;
        if (y < 0) {
            dy = -dy;
            y = 0;
        }
        if (y > DISPLAY_HEIGHT - LOGO_HEIGHT) {
            dy = -dy;
            y = DISPLAY_HEIGHT - LOGO_HEIGHT;
        }

#ifdef HAVE_LCD_BITMAP
        rb->lcd_update();
#ifdef REMOTE_LOGO
        rb->lcd_remote_update();
#endif
#else
        if (cpos != old_cpos) {
            rb->lcd_clear_display();
            pgfx_update();
            pgfx_display(cpos, 0);
            old_cpos = cpos;
        }
        else
            pgfx_update();
#endif
        rb->sleep(HZ/timer);
        
        button = rb->button_get(false);
        switch (button) {
            case LP_QUIT:
#ifdef CONFIG_REMOTE_KEYPAD
            case LP_R_QUIT:
#endif
#ifdef HAVE_LCD_CHARCELLS
                pgfx_release();
#endif
                return PLUGIN_OK;
            case LP_DEC_X:
#ifdef CONFIG_REMOTE_KEYPAD
            case LP_R_DEC_X:
#endif
                if (dx)
                    dx += (dx < 0) ? 1 : -1;
                break;
            case LP_INC_X:
#ifdef CONFIG_REMOTE_KEYPAD
            case LP_R_INC_X:
#endif
                dx += (dx < 0) ? -1 : 1;
                break;
            case LP_DEC_Y:
#ifdef CONFIG_REMOTE_KEYPAD
            case LP_R_DEC_Y:
#endif
                if (dy)
                    dy += (dy < 0) ? 1 : -1;
                break;
            case LP_INC_Y:
#ifdef CONFIG_REMOTE_KEYPAD
            case LP_R_INC_Y:
#endif
                dy += (dy < 0) ? -1 : 1;
                break;
                
            default:
                if (rb->default_event_handler(button) == SYS_USB_CONNECTED) {
#ifdef HAVE_LCD_CHARCELLS
                    pgfx_release();
#endif
                    return PLUGIN_USB_CONNECTED;
                }
                break;
        }
    }
}

