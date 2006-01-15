/***************************************************************************
*             __________               __   ___.
*   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
*   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
*   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
*   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
*                     \/            \/     \/    \/            \/
* $Id$
*
* Plasma demo plugin
*
* My crack at making a 80's style retro plasma effect for the fantastic
* rockbox! 
* Okay, I could've hard-coded the sine wave values, I just wanted the 
* challange of calculating them! silly: maybe, fun: yes!
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
****************************************************************************/

#ifndef SIMULATOR /* not for simulator by now */
#include "plugin.h"

#ifdef HAVE_LCD_BITMAP /* and also not for the Player */
#include "gray.h"

PLUGIN_HEADER

/******************************* Globals ***********************************/

static struct plugin_api* rb; /* global api struct pointer */
static unsigned char *gbuf;
static unsigned int gbuf_size = 0;
static unsigned char wave_array[256];  /* Pre calculated wave array */
static unsigned char colours[256]; /* Smooth transition of shades */
static unsigned char graybuffer[LCD_HEIGHT*LCD_WIDTH]; /* off screen buffer */
static unsigned char sp1, sp2, sp3, sp4; /* Speed of plasma */
static int plasma_frequency;

/* Key assignement, all bitmapped models */
#define PLASMA_QUIT BUTTON_OFF
#define PLASMA_INCREASE_FREQUENCY BUTTON_UP
#define PLASMA_DECREASE_FREQUENCY BUTTON_DOWN

#define WAV_AMP 90


/* Precalculated sine * 16384 (fixed point 18.14) */
static const short sin_table[91] =
{
    0,   285,   571,   857,  1142,  1427,  1712,  1996,  2280,  2563,
    2845,  3126,  3406,  3685,  3963,  4240,  4516,  4790,  5062,  5334,
    5603,  5871,  6137,  6401,  6663,  6924,  7182,  7438,  7691,  7943,
    8191,  8438,  8682,  8923,  9161,  9397,  9630,  9860, 10086, 10310,
    10531, 10748, 10963, 11173, 11381, 11585, 11785, 11982, 12175, 12365,
    12550, 12732, 12910, 13084, 13254, 13420, 13582, 13740, 13894, 14043,
    14188, 14329, 14466, 14598, 14725, 14848, 14967, 15081, 15190, 15295,
    15395, 15491, 15582, 15668, 15749, 15825, 15897, 15964, 16025, 16082,
    16135, 16182, 16224, 16261, 16294, 16321, 16344, 16361, 16374, 16381,
    16384
};

static short sin(int val)
{
    /* value should be between 0 and 360 degree for correct lookup*/
    val%=360;
    if(val<0)
        val+=360;

    /* Speed improvement through successive lookup */
    if (val < 181)
    {
        if (val < 91)
        {
            /* phase 0-90 degree */
            return (short)sin_table[val];
        }
        else
        {
            /* phase 91-180 degree */
            return (short)sin_table[180-val];
        }
    }
    else
    {
        if (val < 271)
        {
            /* phase 181-270 degree */
            return -(short)sin_table[val-180];
        }
        else
        {
            /* phase 270-359 degree */
            return -(short)sin_table[360-val];
        }
    }
    return 0;
}


/*
 * Main wave function so we don't have to re-calc the sine 
 * curve every time. Mess around WAV_AMP and FREQ to make slighlty
 * weirder looking plasmas!
 */

static void wave_table_generate(void)
{
    int i;
    for (i=0;i<256;++i)
    {
        wave_array[i] = (unsigned char)((WAV_AMP
                      * (sin((i * 360 * plasma_frequency) / 256))) / 16384);
    }
}

/*
 * This function is to make a smooth shade from white into
 * black and back into white again. I thought this would be quicker 
 * than calculating on the fly - am I wrong?
 */

static void shades_generate(void)
{
    int i=1;
    int j=0;
    int nAdd=2;

    for(i=0; i < 256; ++i)
    {
        colours[i] = j;
        j += nAdd;

        if(j > 240 || j < 2)
        {
            nAdd = -nAdd;
        }
    }
}

void cleanup(void *parameter)
{
    (void)parameter;
    
    gray_release();
    rb->backlight_set_timeout(rb->global_settings->backlight_timeout);
}

/*
 * Main function that also contain the main plasma
 * algorithm.
 */

int main(void)
{
    plasma_frequency = 1;
    int shades, button, x, y;
    unsigned char p1,p2,p3,p4,t1,t2,t3,t4, z;
    int n=0;

    /*Generate the neccesary pre calced stuff*/
    wave_table_generate();
    shades_generate();

    /* get the remainder of the plugin buffer */
    gbuf = (unsigned char *) rb->plugin_get_buffer(&gbuf_size);

    shades = gray_init(rb, gbuf, gbuf_size, false, LCD_WIDTH, LCD_HEIGHT/8,
                       32, NULL) + 1;

    /* switch on grayscale overlay */
    gray_show(true);
    sp1 = 4;
    sp2 = 2;
    sp3 = 4;
    sp4 = 2;
    p1=p2=p3=p4=0;
    while (true)
    {
        t1=p1;
        t2=p2;
        n=0;
        for(y = 0; y < LCD_HEIGHT; ++y)
        {
            t3=p3;
            t4=p4;
            for(x = 0; x < LCD_WIDTH; ++x)
            {
                z = wave_array[t1] + wave_array[t2] + wave_array[t3]
                  + wave_array[t4];
                graybuffer[n] = colours[z];
                t3+=1;
                t4+=2;
                ++n;
            }
            t1+=2;
            t2+=1;
            rb->yield();
        }
        p1+=sp1;
        p2-=sp2;
        p3+=sp3;
        p4-=sp4;
        gray_ub_gray_bitmap(graybuffer, 0, 0, LCD_WIDTH, LCD_HEIGHT);

        button = rb->button_get(false);

        switch(button)
        {
            case(PLASMA_QUIT):
                cleanup(NULL);
                return PLUGIN_OK;
                break;

            case (PLASMA_INCREASE_FREQUENCY):
                ++plasma_frequency;
                wave_table_generate();
                break;

            case (PLASMA_DECREASE_FREQUENCY):
                if(plasma_frequency>1)
                {
                    --plasma_frequency;
                    wave_table_generate();
                }
                break;
                
            default:
                if (rb->default_event_handler_ex(button, cleanup, NULL)
                    == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
                break;
        }
    }
}

/*************************** Plugin entry point ****************************/

enum plugin_status plugin_start(struct plugin_api* api, void* parameter)
{
    int ret;

    rb = api; // copy to global api pointer
    (void)parameter;
    if (rb->global_settings->backlight_timeout > 0)
        rb->backlight_set_timeout(1);/* keep the light on */

    ret = main();

    return ret;
}

#endif // #ifdef HAVE_LCD_BITMAP
#endif // #ifndef SIMULATOR
