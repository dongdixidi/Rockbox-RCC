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
#ifndef _USB_H_
#define _USB_H_

#include "kernel.h"
#include "button.h"

/* Messages from usb_tick and thread states */
enum {
    USB_INSERTED,
    USB_EXTRACTED,
    USB_REENABLE,
    USB_POWERED,
    USB_TRANSFER_COMPLETION,
    USB_REQUEST_DISK,
    USB_REQUEST_REBOOT
};


#ifdef HAVE_USB_POWER
#if CONFIG_KEYPAD == RECORDER_PAD
#define USBPOWER_BUTTON BUTTON_F1
#define USBPOWER_BTN_IGNORE BUTTON_ON
#elif CONFIG_KEYPAD == ONDIO_PAD
#define USBPOWER_BUTTON BUTTON_MENU
#define USBPOWER_BTN_IGNORE BUTTON_OFF
#elif (CONFIG_KEYPAD == IPOD_4G_PAD)
#define USBPOWER_BUTTON BUTTON_MENU
#define USBPOWER_BTN_IGNORE BUTTON_PLAY
#elif CONFIG_KEYPAD == IRIVER_H300_PAD
#define USBPOWER_BUTTON BUTTON_REC
#define USBPOWER_BTN_IGNORE BUTTON_ON
#elif CONFIG_KEYPAD == GIGABEAT_PAD
#define USBPOWER_BUTTON BUTTON_MENU
#define USBPOWER_BTN_IGNORE BUTTON_POWER
#elif CONFIG_KEYPAD == GIGABEAT_S_PAD
#define USBPOWER_BUTTON BUTTON_MENU
#define USBPOWER_BTN_IGNORE BUTTON_BACK
#elif (CONFIG_KEYPAD == IRIVER_H10_PAD) || \
      (CONFIG_KEYPAD == MROBE100_PAD)
#define USBPOWER_BUTTON BUTTON_RIGHT
#define USBPOWER_BTN_IGNORE BUTTON_POWER
#elif (CONFIG_KEYPAD == SANSA_E200_PAD) || \
      (CONFIG_KEYPAD == SANSA_C200_PAD)
#define USBPOWER_BUTTON BUTTON_SELECT
#define USBPOWER_BTN_IGNORE BUTTON_POWER
#endif
#endif /* HAVE_USB_POWER */

#ifdef HAVE_USBSTACK
/* USB class drivers */
enum {
    USB_DRIVER_MASS_STORAGE,
    USB_DRIVER_SERIAL,
    USB_DRIVER_CHARGING_ONLY,
    USB_NUM_DRIVERS
};
#endif
#ifdef HAVE_USBSTACK
struct usb_transfer_completion_event_data
{
    unsigned char endpoint;
    bool in;
    int status;
    int length;
    void* data;
};
#endif


void usb_init(void);
void usb_enable(bool on);
void usb_start_monitoring(void);
void usb_acknowledge(long id);
void usb_wait_for_disconnect(struct event_queue *q);
int usb_wait_for_disconnect_w_tmo(struct event_queue *q, int ticks);
bool usb_inserted(void); /* return the official value, what's been reported to the threads */
int usb_detect(void); /* return the raw hardware value - nothing/pc/charger */
#ifdef HAVE_USB_POWER
bool usb_powered(void);
#ifdef CONFIG_CHARGING
bool usb_charging_enable(bool on);
bool usb_charging_enabled(void);
#endif
#endif
#ifdef HAVE_USBSTACK
void usb_signal_transfer_completion(struct usb_transfer_completion_event_data* event_data);
bool usb_driver_enabled(int driver);
bool usb_exclusive_ata(void); /* ata is available for usb */
void usb_request_exclusive_ata(void);
#endif

#if defined(IPOD_COLOR) || defined(IPOD_4G) \
 || defined(IPOD_MINI)  || defined(IPOD_MINI2G)
bool firewire_detect(void);
#endif

#endif
