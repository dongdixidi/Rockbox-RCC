/***************************************************************************
 *             __________               __   ___.                  
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___  
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /  
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <   
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \  
 *                     \/            \/     \/    \/            \/ 
 * $Id$
 *
 * Copyright (C) 2007 by Michael Sevakis
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#ifndef _SYSTEM_SDL_H_
#define _SYSTEM_SDL_H_

#include <stdbool.h>

#define HIGHEST_IRQ_LEVEL 1

int set_irq_level(int level);

#define disable_irq() \
    ((void)set_irq_level(HIGHEST_IRQ_LEVEL))

#define enable_irq()  \
    ((void)set_irq_level(0))

#define disable_irq_save() \
    set_irq_level(HIGHEST_IRQ_LEVEL)

#define restore_irq(level) \
    ((void)set_irq_level(level))

void sim_enter_irq_handler(void);
void sim_exit_irq_handler(void);
bool sim_kernel_init(void);
void sim_kernel_shutdown(void);

extern long start_tick;

#endif /* _SYSTEM_SDL_H_ */
