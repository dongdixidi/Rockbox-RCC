/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2008 by Maurus Cuelenaere
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
 
#include "config.h"
#include "jz4740.h"
#include "mips.h"
#include "mipsregs.h"
#include "mmu-mips.h"
#include "panic.h"
#include "system.h"
#include "string.h"
#include "kernel.h"

#define NUM_DMA  6
#define NUM_GPIO 128
#define IRQ_MAX  (IRQ_GPIO_0 + NUM_GPIO)

static int irq;
static void UIRQ(void)
{
    panicf("Unhandled interrupt occurred: %d\n", irq);
}

#define default_interrupt(name) \
  extern __attribute__((weak,alias("UIRQ"))) void name (void)

default_interrupt(I2C);
default_interrupt(EMC);
default_interrupt(UHC);
default_interrupt(UART0);
default_interrupt(SADC);
default_interrupt(MSC);
default_interrupt(RTC);
default_interrupt(SSI);
default_interrupt(CIM);
default_interrupt(AIC);
default_interrupt(ETH);
default_interrupt(TCU2);
default_interrupt(TCU1);
default_interrupt(TCU0);
default_interrupt(UDC);
default_interrupt(IPU);
default_interrupt(LCD);

default_interrupt(DMA0);
default_interrupt(DMA1);
default_interrupt(DMA2);
default_interrupt(DMA3);
default_interrupt(DMA4);
default_interrupt(DMA5);

default_interrupt(GPIO0);
default_interrupt(GPIO1);
default_interrupt(GPIO2);
default_interrupt(GPIO3);
default_interrupt(GPIO4);
default_interrupt(GPIO5);
default_interrupt(GPIO6);
default_interrupt(GPIO7);
default_interrupt(GPIO8);
default_interrupt(GPIO9);
default_interrupt(GPIO10);
default_interrupt(GPIO11);
default_interrupt(GPIO12);
default_interrupt(GPIO13);
default_interrupt(GPIO14);
default_interrupt(GPIO15);
default_interrupt(GPIO16);
default_interrupt(GPIO17);
default_interrupt(GPIO18);
default_interrupt(GPIO19);
default_interrupt(GPIO20);
default_interrupt(GPIO21);
default_interrupt(GPIO22);
default_interrupt(GPIO23);
default_interrupt(GPIO24);
default_interrupt(GPIO25);
default_interrupt(GPIO26);
default_interrupt(GPIO27);
default_interrupt(GPIO28);
default_interrupt(GPIO29);
default_interrupt(GPIO30);
default_interrupt(GPIO31);
default_interrupt(GPIO32);
default_interrupt(GPIO33);
default_interrupt(GPIO34);
default_interrupt(GPIO35);
default_interrupt(GPIO36);
default_interrupt(GPIO37);
default_interrupt(GPIO38);
default_interrupt(GPIO39);
default_interrupt(GPIO40);
default_interrupt(GPIO41);
default_interrupt(GPIO42);
default_interrupt(GPIO43);
default_interrupt(GPIO44);
default_interrupt(GPIO45);
default_interrupt(GPIO46);
default_interrupt(GPIO47);
default_interrupt(GPIO48);
default_interrupt(GPIO49);
default_interrupt(GPIO50);
default_interrupt(GPIO51);
default_interrupt(GPIO52);
default_interrupt(GPIO53);
default_interrupt(GPIO54);
default_interrupt(GPIO55);
default_interrupt(GPIO56);
default_interrupt(GPIO57);
default_interrupt(GPIO58);
default_interrupt(GPIO59);
default_interrupt(GPIO60);
default_interrupt(GPIO61);
default_interrupt(GPIO62);
default_interrupt(GPIO63);
default_interrupt(GPIO64);
default_interrupt(GPIO65);
default_interrupt(GPIO66);
default_interrupt(GPIO67);
default_interrupt(GPIO68);
default_interrupt(GPIO69);
default_interrupt(GPIO70);
default_interrupt(GPIO71);
default_interrupt(GPIO72);
default_interrupt(GPIO73);
default_interrupt(GPIO74);
default_interrupt(GPIO75);
default_interrupt(GPIO76);
default_interrupt(GPIO77);
default_interrupt(GPIO78);
default_interrupt(GPIO79);
default_interrupt(GPIO80);
default_interrupt(GPIO81);
default_interrupt(GPIO82);
default_interrupt(GPIO83);
default_interrupt(GPIO84);
default_interrupt(GPIO85);
default_interrupt(GPIO86);
default_interrupt(GPIO87);
default_interrupt(GPIO88);
default_interrupt(GPIO89);
default_interrupt(GPIO90);
default_interrupt(GPIO91);
default_interrupt(GPIO92);
default_interrupt(GPIO93);
default_interrupt(GPIO94);
default_interrupt(GPIO95);
default_interrupt(GPIO96);
default_interrupt(GPIO97);
default_interrupt(GPIO98);
default_interrupt(GPIO99);
default_interrupt(GPIO100);
default_interrupt(GPIO101);
default_interrupt(GPIO102);
default_interrupt(GPIO103);
default_interrupt(GPIO104);
default_interrupt(GPIO105);
default_interrupt(GPIO106);
default_interrupt(GPIO107);
default_interrupt(GPIO108);
default_interrupt(GPIO109);
default_interrupt(GPIO110);
default_interrupt(GPIO111);
default_interrupt(GPIO112);
default_interrupt(GPIO113);
default_interrupt(GPIO114);
default_interrupt(GPIO115);
default_interrupt(GPIO116);
default_interrupt(GPIO117);
default_interrupt(GPIO118);
default_interrupt(GPIO119);
default_interrupt(GPIO120);
default_interrupt(GPIO121);
default_interrupt(GPIO122);
default_interrupt(GPIO123);
default_interrupt(GPIO124);
default_interrupt(GPIO125);
default_interrupt(GPIO126);
default_interrupt(GPIO127);

static void (* const irqvector[])(void) =
{
    I2C,EMC,UHC,UIRQ,UIRQ,UIRQ,UIRQ,UIRQ,
    UART0,UIRQ,UIRQ,SADC,UIRQ,MSC,RTC,SSI,
    CIM,AIC,ETH,UIRQ,TCU2,TCU1,TCU0,UDC,
    UIRQ,UIRQ,UIRQ,UIRQ,IPU,LCD,UIRQ,DMA0,
    DMA1,DMA2,DMA3,DMA4,DMA5,UIRQ,UIRQ,UIRQ,
    UIRQ,UIRQ,UIRQ,UIRQ,UIRQ,UIRQ,UIRQ,
    GPIO0,GPIO1,GPIO2,GPIO3,GPIO4,GPIO5,GPIO6,GPIO7,
    GPIO8,GPIO9,GPIO10,GPIO11,GPIO12,GPIO13,GPIO14,GPIO15,
    GPIO16,GPIO17,GPIO18,GPIO19,GPIO20,GPIO21,GPIO22,GPIO23,
    GPIO24,GPIO25,GPIO26,GPIO27,GPIO28,GPIO29,GPIO30,GPIO31,
    GPIO32,GPIO33,GPIO34,GPIO35,GPIO36,GPIO37,GPIO38,GPIO39,
    GPIO40,GPIO41,GPIO42,GPIO43,GPIO44,GPIO45,GPIO46,GPIO47,
    GPIO48,GPIO49,GPIO50,GPIO51,GPIO52,GPIO53,GPIO54,GPIO55,
    GPIO56,GPIO57,GPIO58,GPIO59,GPIO60,GPIO61,GPIO62,GPIO63,
    GPIO64,GPIO65,GPIO66,GPIO67,GPIO68,GPIO69,GPIO70,GPIO71,
    GPIO72,GPIO73,GPIO74,GPIO75,GPIO76,GPIO77,GPIO78,GPIO79,
    GPIO80,GPIO81,GPIO82,GPIO83,GPIO84,GPIO85,GPIO86,GPIO87,
    GPIO88,GPIO89,GPIO90,GPIO91,GPIO92,GPIO93,GPIO94,GPIO95,
    GPIO96,GPIO97,GPIO98,GPIO99,GPIO100,GPIO101,GPIO102,GPIO103,
    GPIO104,GPIO105,GPIO106,GPIO107,GPIO108,GPIO109,GPIO110,GPIO111,
    GPIO112,GPIO113,GPIO114,GPIO115,GPIO116,GPIO117,GPIO118,GPIO119,
    GPIO120,GPIO121,GPIO122,GPIO123,GPIO124,GPIO125,GPIO126,GPIO127
};

static unsigned int dma_irq_mask = 0;
static unsigned int gpio_irq_mask[4] = {0};

void system_enable_irq(unsigned int irq)
{
    register unsigned int t;
    if ((irq >= IRQ_GPIO_0) && (irq <= IRQ_GPIO_0 + NUM_GPIO))
    {
        __gpio_unmask_irq(irq - IRQ_GPIO_0);
        t = (irq - IRQ_GPIO_0) >> 5;
        gpio_irq_mask[t] |= (1 << ((irq - IRQ_GPIO_0) & 0x1f));
        __intc_unmask_irq(IRQ_GPIO0 - t);
    }
    else if ((irq >= IRQ_DMA_0) && (irq <= IRQ_DMA_0 + NUM_DMA))
    {
        __dmac_channel_enable_irq(irq - IRQ_DMA_0);
        dma_irq_mask |= (1 << (irq - IRQ_DMA_0));
        __intc_unmask_irq(IRQ_DMAC);
    }
    else if (irq < 32)
        __intc_unmask_irq(irq);
}

static void dis_irq(unsigned int irq)
{
    register unsigned int t;

    if ((irq >= IRQ_GPIO_0) && (irq <= IRQ_GPIO_0 + NUM_GPIO))
    {
        __gpio_mask_irq(irq - IRQ_GPIO_0);
        t = (irq - IRQ_GPIO_0) >> 5;
        gpio_irq_mask[t] &= ~(1 << ((irq - IRQ_GPIO_0) & 0x1f));
        if (!gpio_irq_mask[t])
            __intc_mask_irq(IRQ_GPIO0 - t);
    }
    else if ((irq >= IRQ_DMA_0) && (irq <= IRQ_DMA_0 + NUM_DMA))
    {
        __dmac_channel_disable_irq(irq - IRQ_DMA_0);
        dma_irq_mask &= ~(1 << (irq - IRQ_DMA_0));
        if (!dma_irq_mask)
            __intc_mask_irq(IRQ_DMAC);
    }
    else if (irq < 32)
        __intc_mask_irq(irq);
}

static void ack_irq(unsigned int irq)
{
    if ((irq >= IRQ_GPIO_0) && (irq <= IRQ_GPIO_0 + NUM_GPIO))
    {
        __intc_ack_irq(IRQ_GPIO0 - ((irq - IRQ_GPIO_0)>>5));
        __gpio_ack_irq(irq - IRQ_GPIO_0);
    }
    else if ((irq >= IRQ_DMA_0) && (irq <= IRQ_DMA_0 + NUM_DMA))
        __intc_ack_irq(IRQ_DMAC);
    else if (irq < 32)
        __intc_ack_irq(irq);
}

static unsigned long ipl;
static int get_irq_number(void)
{
    register int irq = 0;
    
    ipl |= REG_INTC_IPR;
    
    if (ipl == 0)
        return -1;

    /* find out the real irq defined in irq_xxx.c */
    for (irq = 31; irq >= 0; irq--)
        if (ipl & (1 << irq))
            break;
    
    if (irq < 0)
        return -1;

    ipl &= ~(1 << irq);

    switch (irq)
    {
        case IRQ_GPIO0:
            irq = __gpio_group_irq(0) + IRQ_GPIO_0;
            break;
        case IRQ_GPIO1:
            irq = __gpio_group_irq(1) + IRQ_GPIO_0 + 32;
            break;
        case IRQ_GPIO2:
            irq = __gpio_group_irq(2) + IRQ_GPIO_0 + 64;
            break;
        case IRQ_GPIO3:
            irq = __gpio_group_irq(3) + IRQ_GPIO_0 + 96;
            break;
        case IRQ_DMAC:
            irq = __dmac_get_irq() + IRQ_DMA_0;
            break;
    }

    return irq;
}

void intr_handler(void)
{
    int irq = get_irq_number();
    if(UNLIKELY(irq < 0))
        return;
    
    ack_irq(irq);
    if(LIKELY(irq > 0))
        irqvector[irq-1]();
}

#define EXC(x,y) case (x): return (y);
static char* parse_exception(unsigned int cause)
{
    switch(cause & M_CauseExcCode)
    {
        EXC(EXC_INT, "Interrupt");
        EXC(EXC_MOD, "TLB Modified");
        EXC(EXC_TLBL, "TLB Exception (Load or Ifetch)");
        EXC(EXC_ADEL, "Address Error (Load or Ifetch)");
        EXC(EXC_ADES, "Address Error (Store)");
        EXC(EXC_TLBS, "TLB Exception (Store)");
        EXC(EXC_IBE, "Instruction Bus Error");
        EXC(EXC_DBE, "Data Bus Error");
        EXC(EXC_SYS, "Syscall");
        EXC(EXC_BP, "Breakpoint");
        EXC(EXC_RI, "Reserved Instruction");
        EXC(EXC_CPU, "Coprocessor Unusable");
        EXC(EXC_OV, "Overflow");
        EXC(EXC_TR, "Trap Instruction");
        EXC(EXC_FPE, "Floating Point Exception");
        EXC(EXC_C2E, "COP2 Exception");
        EXC(EXC_MDMX, "MDMX Exception");
        EXC(EXC_WATCH, "Watch Exception");
        EXC(EXC_MCHECK, "Machine Check Exception");
        EXC(EXC_CacheErr, "Cache error caused re-entry to Debug Mode");
        default:
            return NULL;
    }
}

void exception_handler(void* stack_ptr, unsigned int cause, unsigned int epc)
{
    panicf("Exception occurred: %s [0x%08x] at 0x%08x (stack at 0x%08x)", parse_exception(cause), cause, epc, (unsigned int)stack_ptr);
}

static unsigned int iclk;
static void detect_clock(void)
{
    iclk = __cpm_get_cclk();
}

void udelay(unsigned int usec)
{
    unsigned int i = usec * (iclk / 2000000);
    __asm__ __volatile__ (
                          ".set noreorder    \n"
                          "1:                \n"
                          "bne  %0, $0, 1b   \n"
                          "addi %0, %0, -1   \n"
                          ".set reorder      \n"
                          : "=r" (i)
                          : "0" (i)
                          );
}

void mdelay(unsigned int msec)
{
    unsigned int i;
    for(i=0; i<msec; i++)
        udelay(1000);
}

/* Core-level interrupt masking */
void clear_interrupts(void)
{
    register unsigned int t;
    t = read_c0_status();
    t &= ~1;
    write_c0_status(t);
}

unsigned int mips_get_sr(void)
{
    return read_c0_status();
}

void store_interrupts(void)
{
    register unsigned int t;
    t = read_c0_status();
    t |= 1;
    t &= ~2;
    write_c0_status(t);
}

#define Index_Invalidate_I      0x00
#define Index_Writeback_Inv_D   0x01
#define Index_Load_Tag_I        0x04
#define Index_Load_Tag_D        0x05
#define Index_Store_Tag_I       0x08
#define Index_Store_Tag_D       0x09
#define Hit_Invalidate_I        0x10
#define Hit_Invalidate_D        0x11
#define Hit_Writeback_Inv_D     0x15
#define Hit_Writeback_I         0x18
#define Hit_Writeback_D         0x19

#define CACHE_SIZE              16*1024
#define CACHE_LINE_SIZE         32
#define KSEG0                   0x80000000

#define SYNC_WB() __asm__ __volatile__ ("sync")

#define __CACHE_OP(op, addr)                 \
    __asm__ __volatile__(                    \
    "    .set    noreorder        \n"        \
    "    .set    mips32\n\t       \n"        \
    "    cache   %0, %1           \n"        \
    "    .set    mips0            \n"        \
    "    .set    reorder          \n"        \
    :                                        \
    : "i" (op), "m" (*(unsigned char *)(addr)))

void __flush_dcache_line(unsigned long addr)
{
    __CACHE_OP(Hit_Writeback_Inv_D, addr);
    SYNC_WB();
}

void __icache_invalidate_all(void)
{
    unsigned int i;

    asm volatile (".set   noreorder  \n"
                  ".set   mips32     \n"
                  "mtc0   $0, $28    \n" /* TagLo */
                  "mtc0   $0, $29    \n" /* TagHi */
                  ".set   mips0      \n"
                  ".set   reorder    \n"
                  );
    for(i=KSEG0; i<KSEG0+CACHE_SIZE; i+=CACHE_LINE_SIZE)
        __CACHE_OP(Index_Store_Tag_I, i);

    /* invalidate btb */
    asm volatile (
        ".set mips32        \n"
        "mfc0 %0, $16, 7    \n"
        "nop                \n"
        "ori  %0, 2         \n"
        "mtc0 %0, $16, 7    \n"
        ".set mips0         \n"
        :
        : "r" (i));
}

void __dcache_invalidate_all(void)
{
    unsigned int i;

    asm volatile (".set   noreorder  \n"
                  ".set   mips32     \n"
                  "mtc0   $0, $28    \n"
                  "mtc0   $0, $29    \n"
                  ".set   mips0      \n"
                  ".set   reorder    \n"
                  );
    for (i=KSEG0; i<KSEG0+CACHE_SIZE; i+=CACHE_LINE_SIZE)
        __CACHE_OP(Index_Store_Tag_D, i);
}

void __dcache_writeback_all(void)
{
    unsigned int i;
    for(i=KSEG0; i<KSEG0+CACHE_SIZE; i+=CACHE_LINE_SIZE)
        __CACHE_OP(Index_Writeback_Inv_D, i);
    
    SYNC_WB();
}

void dma_cache_wback_inv(unsigned long addr, unsigned long size)
{
    unsigned long end, a;

    if (size >= CACHE_SIZE)
        __dcache_writeback_all();
    else
    {
        unsigned long dc_lsize = CACHE_LINE_SIZE;
        
        a = addr & ~(dc_lsize - 1);
        end = (addr + size - 1) & ~(dc_lsize - 1);
        for(; a < end; a += dc_lsize)
            __flush_dcache_line(a);    /* Hit_Writeback_Inv_D */
    }
}

void tlb_refill_handler(void)
{
    panicf("TLB refill handler at 0x%08lx! [0x%x]", read_c0_epc(), read_c0_badvaddr());
}

static int dma_count = 0;
void dma_enable(void)
{
    if(++dma_count == 1)
    {
        __cpm_start_dmac();
        
        REG_DMAC_DCCSR(0) = 0;
        REG_DMAC_DCCSR(1) = 0;
        REG_DMAC_DCCSR(2) = 0;
        REG_DMAC_DCCSR(3) = 0;
        REG_DMAC_DCCSR(4) = 0;
        REG_DMAC_DCCSR(5) = 0;
        
        REG_DMAC_DMACR = (DMAC_DMACR_PR_012345 | DMAC_DMACR_DMAE);
    }
}

void dma_disable(void)
{
    if(--dma_count == 0)
    {
        REG_DMAC_DMACR &= ~DMAC_DMACR_DMAE;
        __cpm_stop_dmac();
    }
}

/* PLL output clock = EXTAL * NF / (NR * NO)
 *
 * NF = FD + 2, NR = RD + 2
 * NO = 1 (if OD = 0), NO = 2 (if OD = 1 or 2), NO = 4 (if OD = 3)
 */
static void pll_init(void) ICODE_ATTR;
static void pll_init(void)
{
    register unsigned int cfcr, plcr1;
    int n2FR[33] = {
        0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
        7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
        9
    };
    int div[5] = {1, 3, 3, 3, 3}; /* divisors of I:S:P:L:M */
    int nf, pllout2;

    cfcr = CPM_CPCCR_CLKOEN |
        CPM_CPCCR_PCS |
        (n2FR[div[0]] << CPM_CPCCR_CDIV_BIT) | 
        (n2FR[div[1]] << CPM_CPCCR_HDIV_BIT) | 
        (n2FR[div[2]] << CPM_CPCCR_PDIV_BIT) |
        (n2FR[div[3]] << CPM_CPCCR_MDIV_BIT) |
        (n2FR[div[4]] << CPM_CPCCR_LDIV_BIT) |
        CPM_CPCCR_CE; /* Perform clock divisions immediately */

    pllout2 = (cfcr & CPM_CPCCR_PCS) ? CPU_FREQ : (CPU_FREQ / 2);

    /* Init USB Host clock, pllout2 must be n*48MHz */
    REG_CPM_UHCCDR = pllout2 / 48000000 - 1;

    nf = CPU_FREQ * 2 / CFG_EXTAL;
    plcr1 = ((nf - 2) << CPM_CPPCR_PLLM_BIT) | /* FD */
        (0 << CPM_CPPCR_PLLN_BIT) |    /* RD=0, NR=2 */
        (0 << CPM_CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
        (0x20 << CPM_CPPCR_PLLST_BIT) | /* PLL stable time */
        CPM_CPPCR_PLLEN;                /* enable PLL */          

    /* init PLL */
    REG_CPM_CPCCR = cfcr;
    REG_CPM_CPPCR = plcr1;
}

// SDRAM paramters
#define CFG_SDRAM_BW16      0    /* Data bus width: 0-32bit, 1-16bit */
#define CFG_SDRAM_BANK4     1    /* Banks each chip: 0-2bank, 1-4bank */
#define CFG_SDRAM_ROW       12   /* Row address: 11 to 13 */
#define CFG_SDRAM_COL       8    /* Column address: 8 to 12 */
#define CFG_SDRAM_CASL      2    /* CAS latency: 2 or 3 */

// SDRAM Timings, unit: ns
#define CFG_SDRAM_TRAS      45    /* RAS# Active Time */
#define CFG_SDRAM_RCD       20    /* RAS# to CAS# Delay */
#define CFG_SDRAM_TPC       20    /* RAS# Precharge Time */
#define CFG_SDRAM_TRWL      7     /* Write Latency Time */
#define CFG_SDRAM_TREF      7812  /* Refresh period: 8192 refresh cycles/64ms */

/*
 * Init SDRAM memory.
 */
static void sdram_init(void) ICODE_ATTR;
static void sdram_init(void)
{
    register unsigned int dmcr0, dmcr, sdmode, tmp, cpu_clk, mem_clk, ns;

    unsigned int cas_latency_sdmr[2] = {
        EMC_SDMR_CAS_2,
        EMC_SDMR_CAS_3,
    };

    unsigned int cas_latency_dmcr[2] = {
        1 << EMC_DMCR_TCL_BIT,  /* CAS latency is 2 */
        2 << EMC_DMCR_TCL_BIT   /* CAS latency is 3 */
    };

    int div[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32 };

    cpu_clk = CPU_FREQ;
    mem_clk = cpu_clk * div[__cpm_get_cdiv()] / div[__cpm_get_mdiv()];

    //REG_EMC_BCR = 0;      /* Disable bus release */
    REG_EMC_RTCSR = 0;          /* Disable clock for counting */
    REG_EMC_RTCOR = 0;
    REG_EMC_RTCNT = 0;

    /* Fault DMCR value for mode register setting */
#define SDRAM_ROW0    11
#define SDRAM_COL0     8
#define SDRAM_BANK40   0

    dmcr0 = ((SDRAM_ROW0 - 11) << EMC_DMCR_RA_BIT) |
        ((SDRAM_COL0 - 8) << EMC_DMCR_CA_BIT) |
        (SDRAM_BANK40 << EMC_DMCR_BA_BIT) |
        (CFG_SDRAM_BW16 << EMC_DMCR_BW_BIT) |
        EMC_DMCR_EPIN | cas_latency_dmcr[((CFG_SDRAM_CASL == 3) ? 1 : 0)];

    /* Basic DMCR value */
    dmcr = ((CFG_SDRAM_ROW - 11) << EMC_DMCR_RA_BIT) |
        ((CFG_SDRAM_COL - 8) << EMC_DMCR_CA_BIT) |
        (CFG_SDRAM_BANK4 << EMC_DMCR_BA_BIT) |
        (CFG_SDRAM_BW16 << EMC_DMCR_BW_BIT) |
        EMC_DMCR_EPIN | cas_latency_dmcr[((CFG_SDRAM_CASL == 3) ? 1 : 0)];

    /* SDRAM timimg */
    ns = 1000000000 / mem_clk;
    tmp = CFG_SDRAM_TRAS / ns;
    if (tmp < 4)
        tmp = 4;
    if (tmp > 11)
        tmp = 11;
    dmcr |= ((tmp - 4) << EMC_DMCR_TRAS_BIT);
    tmp = CFG_SDRAM_RCD / ns;
    if (tmp > 3)
        tmp = 3;
    dmcr |= (tmp << EMC_DMCR_RCD_BIT);
    tmp = CFG_SDRAM_TPC / ns;
    if (tmp > 7)
        tmp = 7;
    dmcr |= (tmp << EMC_DMCR_TPC_BIT);
    tmp = CFG_SDRAM_TRWL / ns;
    if (tmp > 3)
        tmp = 3;
    dmcr |= (tmp << EMC_DMCR_TRWL_BIT);
    tmp = (CFG_SDRAM_TRAS + CFG_SDRAM_TPC) / ns;
    if (tmp > 14)
        tmp = 14;
    dmcr |= (((tmp + 1) >> 1) << EMC_DMCR_TRC_BIT);

    /* SDRAM mode value */
    sdmode = EMC_SDMR_BT_SEQ |
        EMC_SDMR_OM_NORMAL |
        EMC_SDMR_BL_4 | cas_latency_sdmr[((CFG_SDRAM_CASL == 3) ? 1 : 0)];

    /* Stage 1. Precharge all banks by writing SDMR with DMCR.MRSET=0 */
    REG_EMC_DMCR = dmcr;
    REG8(EMC_SDMR0 | sdmode) = 0;

    /* Wait for precharge, > 200us */
    tmp = (cpu_clk / 1000000) * 1000;
    while (tmp--);

    /* Stage 2. Enable auto-refresh */
    REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH;

    tmp = CFG_SDRAM_TREF / ns;
    tmp = tmp / 64 + 1;
    if (tmp > 0xff)
        tmp = 0xff;
    REG_EMC_RTCOR = tmp;
    REG_EMC_RTCNT = 0;
    REG_EMC_RTCSR = EMC_RTCSR_CKS_64;   /* Divisor is 64, CKO/64 */

    /* Wait for number of auto-refresh cycles */
    tmp = (cpu_clk / 1000000) * 1000;
    while (tmp--);

    /* Stage 3. Mode Register Set */
    REG_EMC_DMCR = dmcr0 | EMC_DMCR_RFSH | EMC_DMCR_MRSET;
    REG8(EMC_SDMR0 | sdmode) = 0;

    /* Set back to basic DMCR value */
    REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

    /* everything is ok now */
}

extern int main(void);
void system_main(void) ICODE_ATTR;
void system_main(void)
{
    int i;
       
    __dcache_writeback_all();
    __icache_invalidate_all();
    
    write_c0_status(1 << 28 | 1 << 10 ); /* Enable CP | Mask interrupt 2 */
    
    /* Disable all interrupts */
    for(i=0; i<IRQ_MAX; i++)
        dis_irq(i);
    
    tlb_init();
    
    //pll_init();
    //sdram_init();
    
    detect_clock();
    
    /* Disable unneeded clocks, clocks are enabled when needed */
    __cpm_stop_all();
    __cpm_suspend_usbhost();
    
    /* Enable interrupts at core level */
    store_interrupts();
    
    main(); /* Shouldn't return */
    
    while(1)
        core_idle();
}

void system_reboot(void)
{
    REG_WDT_TCSR = WDT_TCSR_PRESCALE4 | WDT_TCSR_EXT_EN;
    REG_WDT_TCNT = 0;
    REG_WDT_TDR = JZ_EXTAL/1000;   /* reset after 4ms */
    REG_TCU_TSCR = TCU_TSSR_WDTSC; /* enable wdt clock */
    REG_WDT_TCER = WDT_TCER_TCEN;  /* wdt start */
    
    while (1);
}

void system_exception_wait(void)
{
    /* check for power button without including any .h file */
    while (1)
    {
        if( (~(*(volatile unsigned int *)(0xB0010300))) & (1 << 29) )
            break;
    }
}

void power_off(void)
{
    /* Put system into hibernate mode */
    __rtc_clear_alarm_flag();
    __rtc_clear_hib_stat_all();
    /* __rtc_set_scratch_pattern(0x12345678); */
    __rtc_enable_alarm_wakeup();
    __rtc_set_hrcr_val(0xfe0);
    __rtc_set_hwfcr_val((0xFFFF << 4));
    __rtc_power_down();
    
    while(1);
}

void system_init(void)
{
}

int system_memory_guard(int newmode)
{
    (void)newmode;
    return 0;
}
