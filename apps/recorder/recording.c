/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 by Linus Nielsen Feltzing
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "config.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "system.h"
#include "lcd.h"
#include "mpeg.h"
#include "mp3_playback.h"
#include "mas.h"
#include "button.h"
#include "kernel.h"
#include "settings.h"
#include "lang.h"
#include "font.h"
#include "icons.h"
#include "screens.h"
#include "peakmeter.h"
#include "status.h"
#include "menu.h"
#include "sound_menu.h"
#include "timefuncs.h"
#include "debug.h"
#include "misc.h"
#include "tree.h"
#include "string.h"
#include "dir.h"
#include "errno.h"

bool f2_rec_screen(void);
bool f3_rec_screen(void);

#define SOURCE_MIC 0
#define SOURCE_LINE 1
#define SOURCE_SPDIF 2

char *freq_str[6] =
{
    "44.1kHz",
    "48kHz",
    "32kHz",
    "22.05kHz",
    "24kHz",
    "16kHz"
};

static void set_gain(void)
{
    if(global_settings.rec_source == SOURCE_MIC)
    {
        mpeg_set_recording_gain(global_settings.rec_mic_gain, 0, true);
    }
    else
    {
        mpeg_set_recording_gain(global_settings.rec_left_gain,
                                global_settings.rec_right_gain, false);
    }
}

static char *fmtstr[] =
{
    "",                 /* no decimals */
    "%d.%d %s  ",       /* 1 decimal */
    "%d.%02d %s  "      /* 2 decimals */
};

char *fmt_gain(int snd, int val, char *str, int len)
{
    int tmp, i, d, numdec;
    char *unit;
    
    tmp = mpeg_val2phys(snd, val);
    numdec = mpeg_sound_numdecimals(snd);
    unit = mpeg_sound_unit(snd);
    
    i = tmp / (10*numdec);
    d = abs(tmp % (10*numdec));
    
    snprintf(str, len, fmtstr[numdec], i, d, unit);
    return str;
}

static int cursor;

void adjust_cursor(void)
{
    if(global_settings.rec_source == SOURCE_LINE)
    {
        if(cursor < 0)
            cursor = 0;
    
        if(cursor > 2)
            cursor = 2;
    }
    else
    {
        cursor = 0;
    }
}

char *rec_create_filename(char *buffer)
{
    int fpos;
    struct tm *tm = get_time();

    if(global_settings.rec_directory)
        getcwd(buffer, MAX_PATH);
    else
        strncpy(buffer, rec_base_directory, MAX_PATH);

    /* Append filename to path: RYYMMDD-HH.MM.SS.mp3 */
    fpos = strlen(buffer);
    snprintf(&buffer[fpos], MAX_PATH-fpos, "/R%02d%02d%02d-%02d%02d%02d.mp3",
             tm->tm_year%100, tm->tm_mon+1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buffer;
}

bool recording_screen(void)
{
    int button;
    bool done = false;
    char buf[32];
    char buf2[32];
    long timeout = current_tick + HZ/10;
    int gain;
    int w, h;
    int update_countdown = 1;
    bool have_recorded = false;
    unsigned int seconds;
    unsigned int last_seconds = 0;
    int hours, minutes;
    char path_buffer[MAX_PATH];
    int rc;
    bool been_in_usb_mode = false;

    cursor = 0;
    mpeg_init_recording();

    mpeg_sound_set(SOUND_VOLUME, global_settings.volume);
    
    status_set_playmode(STATUS_STOP);

    /* Yes, we use the D/A for monitoring */
    peak_meter_playback(true);
    
    peak_meter_enabled = true;

    mpeg_set_recording_options(global_settings.rec_frequency,
                               global_settings.rec_quality,
                               global_settings.rec_source,
                               global_settings.rec_channels,
                               global_settings.rec_editable,
                               global_settings.rec_prerecord_time);

    set_gain();

    lcd_setfont(FONT_SYSFIXED);
    lcd_getstringsize("M", &w, &h);
    lcd_setmargins(global_settings.invert_cursor ? 0 : w, 8);

    /* Try to create the base directory if needed */
    if(global_settings.rec_directory == 0)
    {
        rc = mkdir(rec_base_directory, 0);
        if(rc < 0 && errno != EEXIST)
        {
            splash(HZ * 2, true,
                   "Can't create the %s directory. Error code %d.",
                   rec_base_directory, rc);
        }
        else
        {
            /* If we have created the directory, we want the dir browser to
               be refreshed even if we haven't recorded anything */
            if(errno != EEXIST)
                have_recorded = true;
        }
    }
    
    while(!done)
    {
        button = button_get_w_tmo(HZ / peak_meter_fps);
        switch(button)
        {
            case BUTTON_OFF:
                if(mpeg_status() & MPEG_STATUS_RECORD)
                {
                    mpeg_stop();
                    status_set_playmode(STATUS_STOP);
                }
                else
                {
                    peak_meter_playback(true);
                    peak_meter_enabled = false;
                    done = true;
                }
                update_countdown = 1; /* Update immediately */
                break;

            case BUTTON_PLAY:
                /* Only act if the mpeg is stopped */
                if(!(mpeg_status() & MPEG_STATUS_RECORD))
                {
                    have_recorded = true;
                    mpeg_record(rec_create_filename(path_buffer));
                    status_set_playmode(STATUS_RECORD);
                    update_countdown = 1; /* Update immediately */
                }
                else
                {
                    mpeg_new_file(rec_create_filename(path_buffer));
                    update_countdown = 1; /* Update immediately */
                }
                last_seconds = 0;
                break;

            case BUTTON_UP:
                cursor--;
                adjust_cursor();
                update_countdown = 1; /* Update immediately */
                break;
                
            case BUTTON_DOWN:
                cursor++;
                adjust_cursor();
                update_countdown = 1; /* Update immediately */
                break;

            case BUTTON_RIGHT:
            case BUTTON_RIGHT | BUTTON_REPEAT:
                switch(cursor)
                {
                    case 0:
                        if(global_settings.rec_source == SOURCE_MIC)
                        {
                            global_settings.rec_mic_gain++;
                            if(global_settings.rec_mic_gain >
                               mpeg_sound_max(SOUND_MIC_GAIN))
                                global_settings.rec_mic_gain =
                                    mpeg_sound_max(SOUND_MIC_GAIN);
                        }
                        else
                        {
                            gain = MAX(global_settings.rec_left_gain,
                                       global_settings.rec_right_gain) + 1;
                            if(gain > mpeg_sound_max(SOUND_MIC_GAIN))
                                gain = mpeg_sound_max(SOUND_MIC_GAIN);
                            global_settings.rec_left_gain = gain;
                            global_settings.rec_right_gain = gain;
                        }
                        break;
                    case 1:
                        global_settings.rec_left_gain++;
                        if(global_settings.rec_left_gain >
                           mpeg_sound_max(SOUND_LEFT_GAIN))
                            global_settings.rec_left_gain =
                                mpeg_sound_max(SOUND_LEFT_GAIN);
                        break;
                    case 2:
                        global_settings.rec_right_gain++;
                        if(global_settings.rec_right_gain >
                           mpeg_sound_max(SOUND_RIGHT_GAIN))
                            global_settings.rec_right_gain =
                                mpeg_sound_max(SOUND_RIGHT_GAIN);
                        break;
                }
                set_gain();
                update_countdown = 1; /* Update immediately */
                break;
                
            case BUTTON_LEFT:
            case BUTTON_LEFT | BUTTON_REPEAT:
                switch(cursor)
                {
                    case 0:
                        if(global_settings.rec_source == SOURCE_MIC)
                        {
                            global_settings.rec_mic_gain--;
                            if(global_settings.rec_mic_gain <
                               mpeg_sound_min(SOUND_MIC_GAIN))
                                global_settings.rec_mic_gain =
                                    mpeg_sound_min(SOUND_MIC_GAIN);
                        }
                        else
                        {
                            gain = MAX(global_settings.rec_left_gain,
                                       global_settings.rec_right_gain) - 1;
                            if(gain < mpeg_sound_min(SOUND_LEFT_GAIN))
                                gain = mpeg_sound_min(SOUND_LEFT_GAIN);
                            global_settings.rec_left_gain = gain;
                            global_settings.rec_right_gain = gain;
                        }
                        break;
                    case 1:
                        global_settings.rec_left_gain--;
                        if(global_settings.rec_left_gain <
                           mpeg_sound_min(SOUND_LEFT_GAIN))
                            global_settings.rec_left_gain =
                                mpeg_sound_min(SOUND_LEFT_GAIN);
                        break;
                    case 2:
                        global_settings.rec_right_gain--;
                        if(global_settings.rec_right_gain <
                           mpeg_sound_min(SOUND_MIC_GAIN))
                            global_settings.rec_right_gain =
                                mpeg_sound_min(SOUND_MIC_GAIN);
                        break;
                }
                set_gain();
                update_countdown = 1; /* Update immediately */
                break;
                
            case BUTTON_F1:
                if (recording_menu(false))
                    return SYS_USB_CONNECTED;
                settings_save();

                mpeg_set_recording_options(global_settings.rec_frequency,
                                           global_settings.rec_quality,
                                           global_settings.rec_source,
                                           global_settings.rec_channels,
                                           global_settings.rec_editable,
                                           global_settings.rec_prerecord_time);
                
                set_gain();

                update_countdown = 1; /* Update immediately */

                lcd_setfont(FONT_SYSFIXED);
                lcd_setmargins(global_settings.invert_cursor ? 0 : w, 8);
                break;

            case BUTTON_F2:
                if(mpeg_status() != MPEG_STATUS_RECORD)
                {
                    if (f2_rec_screen())
                    {
                        have_recorded = true;
                        done = true;
                    }
                    else
                        update_countdown = 1; /* Update immediately */
                }
                break;

            case BUTTON_F3:
                if(mpeg_status() != MPEG_STATUS_RECORD)
                {
                    if (f3_rec_screen())
                    {
                        have_recorded = true;
                        done = true;
                    }
                    else
                        update_countdown = 1; /* Update immediately */
                }
                break;

            case SYS_USB_CONNECTED:
                /* Only accept USB connection when not recording */
                if(mpeg_status() != MPEG_STATUS_RECORD)
                {
                    usb_screen();
                    done = true;
                    been_in_usb_mode = true;
                }
                break;
        }

        peak_meter_peek();
        
        if(TIME_AFTER(current_tick, timeout))
        {
            lcd_setfont(FONT_SYSFIXED);

            timeout = current_tick + HZ/10;

            seconds = mpeg_recorded_time() / HZ;
            
            update_countdown--;
            if(update_countdown == 0 || seconds > last_seconds)
            {
                unsigned int dseconds, dhours, dminutes;
                int pos = 0;

                update_countdown = 5;
                last_seconds = seconds;

                lcd_clear_display();

                hours = seconds / 3600;
                minutes = (seconds - (hours * 3600)) / 60;
                snprintf(buf, 32, "%s %02d:%02d:%02d",
                         str(LANG_RECORDING_TIME),
                         hours, minutes, seconds%60);
                lcd_puts(0, 0, buf);

                dseconds = rec_timesplit_seconds(); 

                if(mpeg_status() & MPEG_STATUS_PRERECORD)
                {
                    snprintf(buf, 32, "%s...", str(LANG_RECORD_PRERECORD));
                }
                else
                {
                    /* Display the split interval if the record timesplit
                       is active */
                    if (global_settings.rec_timesplit)
                    {
                        /* Display the record timesplit interval rather than 
                           the file size if the record timer is active */
                        dhours = dseconds / 3600;
                        dminutes = (dseconds - (dhours * 3600)) / 60;
                        snprintf(buf, 32, "%s %02d:%02d",
                                 str(LANG_RECORD_TIMESPLIT_REC),
                                 dhours, dminutes);
                    }
                    else
                        snprintf(buf, 32, "%s %s", str(LANG_RECORDING_SIZE),
                                 num2max5(mpeg_num_recorded_bytes(), buf2));
                }
                lcd_puts(0, 1, buf);

                /* We will do file splitting regardless, since the OFF
                   setting really means 24 hours. This is to make sure
                   that the recorded files don't get too big. */
                if (mpeg_status() && (seconds >= dseconds))
                {
                    mpeg_new_file(rec_create_filename(path_buffer));
                    update_countdown = 1;
                    last_seconds = 0;
                }

                peak_meter_draw(0, 8 + h*2, LCD_WIDTH, h);

                /* Show mic gain if input source is Mic */
                if(global_settings.rec_source == 0)
                {
                    snprintf(buf, 32, "%s: %s", str(LANG_RECORDING_GAIN),
                             fmt_gain(SOUND_MIC_GAIN,
                                      global_settings.rec_mic_gain,
                                      buf2, sizeof(buf2)));
                    if (global_settings.invert_cursor && (pos++ == cursor))
                        lcd_puts_style(0, 3, buf, STYLE_INVERT);
                    else
                        lcd_puts(0, 3, buf);
                }
                else
                {
                    if(global_settings.rec_source == SOURCE_LINE)
                    {
                        gain = MAX(global_settings.rec_left_gain,
                                   global_settings.rec_right_gain);
                        
                        snprintf(buf, 32, "%s: %s", str(LANG_RECORDING_GAIN),
                                 fmt_gain(SOUND_LEFT_GAIN, gain,
                                          buf2, sizeof(buf2)));
                        if (global_settings.invert_cursor && (pos++ == cursor))
                            lcd_puts_style(0, 3, buf, STYLE_INVERT);
                        else
                            lcd_puts(0, 3, buf);
                        
                        snprintf(buf, 32, "%s: %s", str(LANG_RECORDING_LEFT),
                                 fmt_gain(SOUND_LEFT_GAIN,
                                          global_settings.rec_left_gain,
                                          buf2, sizeof(buf2)));
                        if (global_settings.invert_cursor && (pos++ == cursor))
                            lcd_puts_style(0, 4, buf, STYLE_INVERT);
                        else
                            lcd_puts(0, 4, buf);
                        
                        snprintf(buf, 32, "%s: %s", str(LANG_RECORDING_RIGHT),
                                 fmt_gain(SOUND_RIGHT_GAIN,
                                          global_settings.rec_right_gain,
                                          buf2, sizeof(buf2)));
                        if (global_settings.invert_cursor && (pos++ == cursor))
                            lcd_puts_style(0, 5, buf, STYLE_INVERT);
                        else
                            lcd_puts(0, 5, buf);
                    }
                }

                if(global_settings.rec_source != SOURCE_SPDIF)
                    put_cursorxy(0, 3 + cursor, true);

                snprintf(buf, 32, "%s %s [%d]",
                         freq_str[global_settings.rec_frequency],
                         global_settings.rec_channels?
                         str(LANG_CHANNEL_MONO):str(LANG_CHANNEL_STEREO),
                         global_settings.rec_quality);
                lcd_puts(0, 6, buf);

                status_draw(true);

                lcd_update();
            }
            else
            {
                lcd_clearrect(0, 8 + h*2, LCD_WIDTH, h);
                peak_meter_draw(0, 8 + h*2, LCD_WIDTH, h);
                lcd_update_rect(0, 8 + h*2, LCD_WIDTH, h);
            }
        }

        if(mpeg_status() & MPEG_STATUS_ERROR)
        {
            done = true;
        }
    }
    
    if(mpeg_status() & MPEG_STATUS_ERROR)
    {
        status_set_playmode(STATUS_STOP);
        splash(0, true, str(LANG_DISK_FULL));
        status_draw(true);
        lcd_update();
        mpeg_error_clear();

        while(1)
        {
            button = button_get(true);
            if(button == (BUTTON_OFF | BUTTON_REL))
                break;
        }
    }
    
    mpeg_init_playback();

    mpeg_sound_channel_config(global_settings.channel_config);
    mpeg_sound_set(SOUND_BASS, global_settings.bass);
    mpeg_sound_set(SOUND_TREBLE, global_settings.treble);
    mpeg_sound_set(SOUND_BALANCE, global_settings.balance);
    mpeg_sound_set(SOUND_VOLUME, global_settings.volume);
    mpeg_sound_set(SOUND_LOUDNESS, global_settings.loudness);
    mpeg_sound_set(SOUND_SUPERBASS, global_settings.bass_boost);
    mpeg_sound_set(SOUND_AVC, global_settings.avc);

    lcd_setfont(FONT_UI);

    if (have_recorded)
        reload_directory();

    return been_in_usb_mode;
}

bool f2_rec_screen(void)
{
    bool exit = false;
    bool used = false;
    int w, h;
    char buf[32];

    lcd_setfont(FONT_SYSFIXED);
    lcd_getstringsize("A",&w,&h);

    while (!exit) {
        char* ptr=NULL;

        lcd_clear_display();

        /* Recording quality */
        lcd_putsxy(0, LCD_HEIGHT/2 - h*2, str(LANG_RECORDING_QUALITY));
        snprintf(buf, 32, "%d", global_settings.rec_quality);
        lcd_putsxy(0, LCD_HEIGHT/2-h, buf);
        lcd_bitmap(bitmap_icons_7x8[Icon_FastBackward], 
                   LCD_WIDTH/2 - 16, LCD_HEIGHT/2 - 4, 7, 8, true);

        /* Frequency */
        snprintf(buf, sizeof buf, "%s:", str(LANG_RECORDING_FREQUENCY));
        lcd_getstringsize(buf,&w,&h);
        lcd_putsxy((LCD_WIDTH-w)/2, LCD_HEIGHT - h*2, buf);
        ptr = freq_str[global_settings.rec_frequency];
        lcd_getstringsize(ptr, &w, &h);
        lcd_putsxy((LCD_WIDTH-w)/2, LCD_HEIGHT - h, ptr);
        lcd_bitmap(bitmap_icons_7x8[Icon_DownArrow],
                   LCD_WIDTH/2 - 3, LCD_HEIGHT - h*3, 7, 8, true);

        /* Channel mode */
        switch ( global_settings.rec_channels ) {
            case 0:
                ptr = str(LANG_CHANNEL_STEREO);
                break;

            case 1:
                ptr = str(LANG_CHANNEL_MONO);
                break;
        }

        lcd_getstringsize(str(LANG_RECORDING_CHANNELS), &w, &h);
        lcd_putsxy(LCD_WIDTH - w, LCD_HEIGHT/2 - h*2,
                   str(LANG_RECORDING_CHANNELS));
        lcd_getstringsize(str(LANG_F2_MODE), &w, &h);
        lcd_putsxy(LCD_WIDTH - w, LCD_HEIGHT/2 - h, str(LANG_F2_MODE));
        lcd_getstringsize(ptr, &w, &h);
        lcd_putsxy(LCD_WIDTH - w, LCD_HEIGHT/2, ptr);
        lcd_bitmap(bitmap_icons_7x8[Icon_FastForward], 
                   LCD_WIDTH/2 + 8, LCD_HEIGHT/2 - 4, 7, 8, true);

        lcd_update();

        switch (button_get(true)) {
            case BUTTON_LEFT:
            case BUTTON_F2 | BUTTON_LEFT:
                global_settings.rec_quality++;
                if(global_settings.rec_quality > 7)
                    global_settings.rec_quality = 0;
                used = true;
                break;

            case BUTTON_DOWN:
            case BUTTON_F2 | BUTTON_DOWN:
                global_settings.rec_frequency++;
                if(global_settings.rec_frequency > 5)
                    global_settings.rec_frequency = 0;
                used = true;
                break;

            case BUTTON_RIGHT:
            case BUTTON_F2 | BUTTON_RIGHT:
                global_settings.rec_channels++;
                if(global_settings.rec_channels > 1)
                    global_settings.rec_channels = 0;
                used = true;
                break;

            case BUTTON_F2 | BUTTON_REL:
                if ( used )
                    exit = true;
                used = true;
                break;

            case BUTTON_F2 | BUTTON_REPEAT:
                used = true;
                break;

            case SYS_USB_CONNECTED:
                usb_screen();
                return true;
        }
    }

    mpeg_set_recording_options(global_settings.rec_frequency,
                               global_settings.rec_quality,
                               global_settings.rec_source,
                               global_settings.rec_channels,
                               global_settings.rec_editable,
                               global_settings.rec_prerecord_time);

    set_gain();
    
    settings_save();
    lcd_setfont(FONT_UI);

    return false;
}

bool f3_rec_screen(void)
{
    bool exit = false;
    bool used = false;
    int w, h;
    char *src_str[] =
    {
        str(LANG_RECORDING_SRC_MIC),
        str(LANG_RECORDING_SRC_LINE),
        str(LANG_RECORDING_SRC_DIGITAL)
    };

    lcd_setfont(FONT_SYSFIXED);
    lcd_getstringsize("A",&w,&h);

    while (!exit) {
        char* ptr=NULL;

        lcd_clear_display();

        /* Recording source */
        lcd_putsxy(0, LCD_HEIGHT/2 - h*2, str(LANG_RECORDING_SOURCE));
        ptr = src_str[global_settings.rec_source];
        lcd_getstringsize(ptr, &w, &h);
        lcd_putsxy(0, LCD_HEIGHT/2-h, ptr);
        lcd_bitmap(bitmap_icons_7x8[Icon_FastBackward], 
                   LCD_WIDTH/2 - 16, LCD_HEIGHT/2 - 4, 7, 8, true);

        lcd_update();

        switch (button_get(true)) {
            case BUTTON_LEFT:
            case BUTTON_F3 | BUTTON_LEFT:
                global_settings.rec_source++;
                if(global_settings.rec_source > 2)
                    global_settings.rec_source = 0;
                used = true;
                break;

            case BUTTON_F3 | BUTTON_REL:
                if ( used )
                    exit = true;
                used = true;
                break;

            case BUTTON_F3 | BUTTON_REPEAT:
                used = true;
                break;

            case SYS_USB_CONNECTED:
                usb_screen();
                return true;
        }
    }

    mpeg_set_recording_options(global_settings.rec_frequency,
                               global_settings.rec_quality,
                               global_settings.rec_source,
                               global_settings.rec_channels,
                               global_settings.rec_editable,
                               global_settings.rec_prerecord_time);

    set_gain();

    settings_save();
    lcd_setfont(FONT_UI);

    return false;
}
