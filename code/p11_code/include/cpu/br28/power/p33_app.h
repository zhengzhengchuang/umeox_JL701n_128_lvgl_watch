#ifndef __P33_APP_H__
#define __P33_APP_H__
/*********************************************************************************************
    *   Filename        : p11_app.h

    *   Description     : 本文用于基于p33.h功能的封装

    *   Author          : MoZhiYe

    *   Email           : mozhiye@zh-jieli.com

    *   Last modifiled  : 2021-04-17 09:00

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/
#include "p11_app.h"
#include "p33.h"

//=============================================================//
//                       P33 SFR操作                           //
//=============================================================//
// void p33_xor_1byte(u16 addr, u8 data0);
#define p33_xor_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x300*4)  = data0)
//#define p33_xor_1byte(addr, data0)      addr ^= (data0)

// void p33_or_1byte(u16 addr, u8 data0);
#define p33_or_1byte(addr, data0)       (*((volatile u8 *)&addr + 0x200*4)  = data0)
//#define p33_or_1byte(addr, data0)       addr |= (data0)

// void p33_and_1byte(u16 addr, u8 data0);
#define p33_and_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x100*4)  = data0)
//#define p33_and_1byte(addr, data0)      addr &= (data0)

// void p33_tx_1byte(u16 addr, u8 data0);
#define p33_tx_1byte(addr, data0)       addr = data0

#define p33_rx_1byte(addr)              addr

#define P33_CON_SET(sfr, start, len, data)  (sfr = (sfr & ~((~(0xff << len))) << (start))) | (((data) & (~(0xff << (len)))) << (start)))

#define P33_CON_GET(sfr)    sfr

//-----special operation----------
#define P33_WKUP_ENABLE()     \
    P3_VLD_KEEP |= BIT(2)

#define P33_WKUP_DISABLE()     \
    P3_VLD_KEEP &= ~BIT(2)

#define P33_IE_ENABLE()     \
    bit_set_ie(IRQ_P33_IDX)

#define P33_IE_DISABLE()     \
    bit_clr_ie(IRQ_P33_IDX)

#define IS_CHARGE_EN_NOW()          (P3_CHG_CON0 & BIT(0))

/*******************************************************************/

#define DVDDLS_PU        \
        P3_LS_IO_DLY      = 0x4; \
        P3_LS_IO_ROM      = 0x4; \
        P3_LS_ADC         = 0x4; \
		P3_LS_AUDIO       = 0x4; \
        P3_LS_RF          = 0x4; \
        P3_LS_PLL         = 0x4; \
        P3_LS_IO_DLY      = 0x6; \
        P3_LS_IO_ROM      = 0x6; \
        P3_LS_ADC         = 0x6; \
		P3_LS_AUDIO       = 0x6; \
        P3_LS_RF          = 0x6; \
        P3_LS_PLL         = 0x6; \

#define DVDDLS_EN       \
        P3_LS_IO_DLY      = 0x6; \
        P3_LS_IO_ROM      = 0x6; \
        P3_LS_ADC         = 0x6; \
		P3_LS_AUDIO       = 0x6; \
        P3_LS_RF          = 0x6; \
        P3_LS_PLL         = 0x6; \

#define DVDDLS_OFF       \
        P3_LS_IO_DLY      = 0x7; \
        P3_LS_IO_ROM      = 0x7; \
        P3_LS_ADC         = 0x7; \
		P3_LS_AUDIO       = 0x7; \
        P3_LS_RF          = 0x7; \
        P3_LS_PLL         = 0x7; \
        P3_LS_IO_DLY      = 0x3; \
        P3_LS_IO_ROM      = 0x3; \
        P3_LS_ADC         = 0x3; \
		P3_LS_AUDIO       = 0x3; \
        P3_LS_RF          = 0x3; \
        P3_LS_PLL         = 0x3; \

#define PVDDLS_EN(a)       \
	if (a) {    \
        P3_LS_P11     = 0x0; \
    } else { \
        P3_LS_P11     = 0x2; \
    }

#define PVDDOK_SOFF 		P3_LS_P11 = 0x1;

/*
 *-------------------P3_ANA_CON0
 */

#define GET_DCVD_STA() 	   (P3_ANA_CON0 & (BIT(2) | BIT(3)))

#define VDD13TO12_SYS_EN(a)       \
    if (a) {    \
        p33_or_1byte(P3_ANA_CON0, BIT(0)); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~BIT(0)); \
}

#define VDD13TO12_RVD_EN(a)       \
	if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(1)); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~BIT(1)); \
}

#define DCVD_TO_DIG_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, (BIT(1) | BIT(0))); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~(BIT(1) | BIT(0))); \
}

#define DCVDD_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(2)); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~BIT(2)); \
}

#define DCDC13_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(3)); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~BIT(3)); \
}

#define IS_DCDC13_EN() 			((P3_ANA_CON0 & BIT(3)) ? 1: 0 )

#define MVIO_PVDD_MVBG_ONLY()	P3_ANA_CON0 = BIT(4) | BIT(5) | BIT(6) | BIT(7)
#define PVDD_MVBG_ONLY()  		P3_ANA_CON0 = BIT(4) | BIT(7)
#define PVDD_ONLY()  			P3_ANA_CON0 = BIT(4)

#define PVDD_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(4)); \
	} else { \
		p33_and_1byte(P3_ANA_CON0, ~BIT(4)); \
}

#define PW_GATE_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(5) | BIT(6)); \
	} else { \
		p33_and_1byte(P3_ANA_CON0, ~(BIT(5) | BIT(6))); \
}

#define MVIO_VBAT_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(5)); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~BIT(5)); \
}

#define MVIO_VPWR_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(6)); \
	} else { \
        p33_and_1byte(P3_ANA_CON0, ~BIT(6)); \
}

#define MBG_EN_EN(a)       \
   if (a) {    \
		p33_or_1byte(P3_ANA_CON0, BIT(7)); \
	} else { \
		p33_and_1byte(P3_ANA_CON0, ~BIT(7)); \
}

/*******************************************************************/

/*
 *-------------------P3_ANA_CON1
 */
#define RVDD_BYPASS_EN(a)       \
	if (a) {    \
		p33_or_1byte(P3_ANA_CON1, BIT(0)); \
	} else { \
        p33_and_1byte(P3_ANA_CON1, ~BIT(0)); \
}

#define WVDD_SHORT_RVDD(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON1, BIT(1)); \
	} else { \
		p33_and_1byte(P3_ANA_CON1, ~BIT(1)); \
}

#define WVDD_SHORT_DVDD(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON1, BIT(2)); \
	} else { \
        p33_and_1byte(P3_ANA_CON1, ~BIT(2)); \
}

#define WLDO06_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON1, BIT(3)); \
	} else { \
		p33_and_1byte(P3_ANA_CON1, ~BIT(3)); \
	}

#define PVD_SHORT_PB3(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON1, BIT(7)); \
	} else { \
		p33_and_1byte(P3_ANA_CON1, ~BIT(7)); \
	}

#define WVDD_PDOWN_ENTER		P3_ANA_CON1 = (P3_ANA_CON1 & (~0x1f)) | 0x1e

#define WVDD_POFF_ENTER			P3_ANA_CON1 = (P3_ANA_CON1 & (~0x1f)) | 0x1a

#define WVDD_PDOWN_POFF_EXIT 	P3_ANA_CON1 = (P3_ANA_CON1 & (~0x1f)) | 0x4

#define WVDD_LIGHT_PDOWN_ENTER  P3_ANA_CON1 = (P3_ANA_CON1 & (~0x1f)) | 0x1c

/*******************************************************************/

/*
 *-------------------P3_ANA_CON2
 */
#define IS_VCM_DET_EN() 		((P3_ANA_CON2 & BIT(3)) ? 1: 0 )

#define VCM_DET_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON2, BIT(3)); \
	} else { \
		p33_and_1byte(P3_ANA_CON2, ~BIT(3)); \
	}

#define IOVD_DRI_FULL(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON2, (BIT(4) | BIT(5))); \
	} else { \
		p33_and_1byte(P3_ANA_CON2, ~(BIT(4) | BIT(5))); \
	}

#define DCVD_DRI_FULL(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON2, BIT(6)); \
	} else { \
		p33_and_1byte(P3_ANA_CON2, ~BIT(6)); \
	}

/*******************************************************************/

/*
 *-------------------P3_ANA_CON3
 */

/*******************************************************************/

/*
 *-------------------P3_ANA_CON4
 */
#define PMU_DET_EN(a)       \
	if (a) {    \
		p33_or_1byte(P3_ANA_CON4, BIT(0)); \
	} else { \
		p33_and_1byte(P3_ANA_CON4, ~BIT(0)); \
	}

#define PMU_DET_BG_BUF_DISABLE() (P3_ANA_CON4 &=  ~(BIT(5)|BIT(6)|BIT(7)))

#define GET_P3_ANA_CON4() 		P3_ANA_CON4

#define SET_P3_ANA_CON4(val) 	P3_ANA_CON4 = val

/*******************************************************************/

/*
 *-------------------P3_ANA_CON5
 */
#define GET_MVIO_SEL()          (P3_ANA_CON5 & 0x7)
#define GET_WVIO_SEL()          ((P3_ANA_CON5 >> 3) & 0x7)

#define MVIO_SEL(cur)           P3_ANA_CON5 = (P3_ANA_CON5 & (~0x7)) | cur
#define WVIO_SEL(cur)           P3_ANA_CON5 = (P3_ANA_CON5 & (~(0x7 << 3))) | (cur << 3)
#define VDDIO_HD_SEL(cur)       P3_ANA_CON5 = (P3_ANA_CON5 & (~(0x3 << 6)) | (cur << 6)

/*
 *-------------------P3_IOV2_CON
 */
#define IOV2_CFG(set)             P3_IOV2_CON = (P3_IOV2_CON & ~0x7) | (set & 0x7)

#define GET_IOV2_EN            (P3_IOV2_CON & BIT(0))
#define GET_IOV2_BYPASS        (P3_IOV2_CON & BIT(1))

#define IOV2_LDO_EN             P3_IOV2_CON = (P3_IOV2_CON & ~BIT(1)) | BIT(0)

#define IOV2_EN(a) \
	if (a) {    \
		p33_or_1byte(P3_IOV2_CON, BIT(0)); \
	} else { \
		p33_and_1byte(P3_IOV2_CON, ~BIT(0)); \
	}


#define IOV2_BYPASS(a) \
	if (a) {    \
		p33_or_1byte(P3_IOV2_CON, BIT(1)); \
	} else { \
		p33_and_1byte(P3_IOV2_CON, ~BIT(1)); \
	}

#define VIO2_DRI_FULL(a)       \
    if (a) {    \
		p33_or_1byte(P3_IOV2_CON, BIT(2)); \
	} else { \
		p33_and_1byte(P3_IOV2_CON, ~BIT(2)); \
	}

/*******************************************************************/

/*
 *-------------------P3_RVD_CON
 */
#define RVDD_CMP_EN(en)				\
	if(en){  \
		p33_or_1byte(P3_RVD_CON, BIT(4));\
	}else{ \
		p33_and_1byte(P3_RVD_CON, BIT(4));\
	}

#define PVDD_DCDC_LEV_SEL(sel)		p33_tx_1byte(P3_RVD_CON, p33_rx_1byte(P3_RVD_CON) & (~0x0f) | sel)

#define GET_PVDD_DCDC_LEV_SEL()		(p33_rx_1byte(P3_RVD_CON) & 0xf)

/*******************************************************************/

/*
 *-------------------P3_ANA_CON6
 */
#define DCVD_SEL(sel)        P3_ANA_CON6 = (P3_ANA_CON6 & (~0xf)) | sel

#define GET_DCVD_SEL()       (P33_CON_GET(P3_ANA_CON6) & 0xf)

enum {
    DCVDD_VOL_SEL_100V = 0,
    DCVDD_VOL_SEL_105V,
    DCVDD_VOL_SEL_1075V,
    DCVDD_VOL_SEL_110V,
    DCVDD_VOL_SEL_1125V,
    DCVDD_VOL_SEL_115V,
    DCVDD_VOL_SEL_1175V,
    DCVDD_VOL_SEL_120V,
    DCVDD_VOL_SEL_1225V,
    DCVDD_VOL_SEL_125V,
    DCVDD_VOL_SEL_1275V,
    DCVDD_VOL_SEL_130V,
    DCVDD_VOL_SEL_1325V,
    DCVDD_VOL_SEL_135V,
    DCVDD_VOL_SEL_1375V,
    DCVDD_VOL_SEL_140V,
};

/*******************************************************************/

/*
 *-------------------P3_ANA_CON7
 */

/*******************************************************************/

/*
 *-------------------P3_ANA_CON8
 */

/*******************************************************************/

/*
 *-------------------P3_ANA_CON9
 */

/*******************************************************************/

/*
 *-------------------P3_ANA_CON10
 */

/*******************************************************************/

/*
 *-------------------P3_ANA_CON10
 */

/*******************************************************************/

/*
 *-------------------P3_ANA_CON11
 */
#define DVDD_VOL_SEL(sel)      P3_ANA_CON11 = (P3_ANA_CON11 & (~0xf)) | sel

enum {
    SYSVDD_VOL_SEL_081V = 0,
    SYSVDD_VOL_SEL_084V,
    SYSVDD_VOL_SEL_087V,
    SYSVDD_VOL_SEL_090V,
    SYSVDD_VOL_SEL_093V,
    SYSVDD_VOL_SEL_096V,
    SYSVDD_VOL_SEL_099V,
    SYSVDD_VOL_SEL_102V,
    SYSVDD_VOL_SEL_105V,
    SYSVDD_VOL_SEL_108V,
    SYSVDD_VOL_SEL_111V,
    SYSVDD_VOL_SEL_114V,
    SYSVDD_VOL_SEL_117V,
    SYSVDD_VOL_SEL_120V,
    SYSVDD_VOL_SEL_123V,
    SYSVDD_VOL_SEL_126V,
};

#define GET_DVDD_VOL_SEL()     (P3_ANA_CON11 & 0xf)

#define SET_P3_ANA_CON11(val)	(P3_ANA_CON11 = val)

#define GET_P3_ANA_CON11() 		P3_ANA_CON11
/*******************************************************************/

/*
 *-------------------P3_ANA_CON12
 */
#define RVDD_VOL_SEL(sel)       P3_ANA_CON12 = (P3_ANA_CON12 & (~0xf)) | sel

#define GET_RVDD_VOL_SEL()      (P3_ANA_CON12 & 0xf)

#define SET_P3_ANA_CON12(val)	(P3_ANA_CON12 = val)

#define GET_P3_ANA_CON12() 		P3_ANA_CON12

enum {
    RVDD_VOL_SEL_081V = 0,
    RVDD_VOL_SEL_084V,
    RVDD_VOL_SEL_087V,
    RVDD_VOL_SEL_090V,
    RVDD_VOL_SEL_093V,
    RVDD_VOL_SEL_096V,
    RVDD_VOL_SEL_099V,
    RVDD_VOL_SEL_102V,
    RVDD_VOL_SEL_105V,
    RVDD_VOL_SEL_108V,
    RVDD_VOL_SEL_111V,
    RVDD_VOL_SEL_114V,
    RVDD_VOL_SEL_117V,
    RVDD_VOL_SEL_120V,
    RVDD_VOL_SEL_123V,
    RVDD_VOL_SEL_126V,
};

/*******************************************************************/

/*
 *-------------------P3_ANA_CON13
 */
#define WVDD_VOL_SEL(vol) 		P3_ANA_CON13 = (P3_ANA_CON13 & (~0xf)) | vol

#define WVDDIO_SEL_AUTO_EN(a)       \
	if (a) {    \
		p33_or_1byte(P3_ANA_CON13, BIT(5)); \
	} else { \
		p33_and_1byte(P3_ANA_CON13, ~BIT(5)); \
	}

#define WVDDIO_FBRES_EN(a)       \
	if (a) {    \
		p33_or_1byte(P3_ANA_CON13, BIT(7)); \
	} else { \
		p33_and_1byte(P3_ANA_CON13, ~BIT(7)); \
	}

/*******************************************************************/

/*
 *-------------------P3_ANA_CON14
 */
#define PVD_HD_SEL(val) 		P3_ANA_CON14 = (P3_ANA_CON14 & (~0x7)) | val

#define GET_PVD_HD_SEL()		(p33_rx_1byte(P3_ANA_CON14) & 0x7)

#define PVDD_DEUDSHT_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON14, BIT(3)); \
	} else { \
		p33_and_1byte(P3_ANA_CON14, ~BIT(3)); \
	}

#define RVD2PVD_SHORT_EN(a)       \
    if (a) {    \
		p33_or_1byte(P3_ANA_CON14, BIT(4)); \
	} else { \
		p33_and_1byte(P3_ANA_CON14, ~BIT(4)); \
	}

#define PVDD_DRV_SEL(val) \
    if (val) {    			\
        P3_ANA_CON14  = val & 0xf; \
    } else { 				\
        P3_ANA_CON14 = 0xf0; \
    }


/*******************************************************************/

/*
 *-------------------P3_VLVD_CON
 */
#define GET_P33_VLVD_EN  	(P3_VLVD_CON & BIT(0))

#define P33_VLVD_EN(a)       \
    if (a) {    \
        p33_or_1byte(P3_VLVD_CON, BIT(0)); \
    } else { \
        p33_and_1byte(P3_VLVD_CON, ~BIT(0)); \
    }

/*******************************************************************/

/*
 *-------------------P3_LRC_CON0
 */
#define LRC_EN(a)       \
    if (a) {    \
        p33_or_1byte(P3_LRC_CON0, BIT(0)); \
    } else { \
        p33_and_1byte(P3_LRC_CON0, ~BIT(0)); \
    }

/*******************************************************************/

/*
 *-------------------P3_PVDD0_AUTO
 */
#define SET_P3_PVDD0_AUTO(val)		p33_tx_1byte(P3_PVDD0_AUTO, val)

enum {
    PVDD_VOL_SEL_050V = 0,
    PVDD_VOL_SEL_055V,
    PVDD_VOL_SEL_060V,
    PVDD_VOL_SEL_065V,
    PVDD_VOL_SEL_070V,
    PVDD_VOL_SEL_075V,
    PVDD_VOL_SEL_080V,
    PVDD_VOL_SEL_085V,
    PVDD_VOL_SEL_090V,
    PVDD_VOL_SEL_095V,
    PVDD_VOL_SEL_100V,
    PVDD_VOL_SEL_105V,
    PVDD_VOL_SEL_110V,
    PVDD_VOL_SEL_115V,
    PVDD_VOL_SEL_120V,
    PVDD_VOL_SEL_125V,
};

#define PVDD_VOL_SEL_MAX PVDD_VOL_SEL_125V

/*******************************************************************/

/*
 *-------------------P3_PVDD1_AUTO
 */
#define PVDD_LEVEL_NOW(val)       	p33_tx_1byte(P3_PVDD1_AUTO, p33_rx_1byte(P3_PVDD1_AUTO) & (~0x0f) | val)

#define PVDD_LEVEL_HIGH_NOW(sel)	p33_tx_1byte(P3_PVDD1_AUTO, ((sel<<4)|sel))

#define PVDD_LEVEL_HIGH(sel)		p33_tx_1byte(P3_PVDD1_AUTO, p33_rx_1byte(P3_PVDD1_AUTO) & (~0xf0) | val)

#define GET_PVDD_LEVEL_NOW()		(p33_rx_1byte(P3_PVDD1_AUTO) & 0x0f)

/*******************************************************************/

/*
 *-------------------P3_PVDD2_AUTO
 */
#define PVDD_DRI_SEL(a)   \
    if (a) {    \
        p33_or_1byte(P3_PVDD2_AUTO, BIT(0)); \
    } else { \
        p33_and_1byte(P3_PVDD2_AUTO, ~BIT(0)); \
    }



/*******************************************************************/

/*
 *-------------------P3_ANA_KEEP
 */
#define SET_P3_ANA_KEEP(val)	p33_tx_1byte(P3_ANA_KEEP, val)

/*******************************************************************/

/*
 *-------------------P3_ANA_KEEP1
 */
#define SET_P3_ANA_KEEP1(val)	p33_tx_1byte(P3_ANA_KEEP1, val)

/*******************************************************************/

/*
 *  *-------------------P3_CLK_CON0
 *   */

#define RC_250K_EN(a)           \
    if (a) {    \
        p33_or_1byte(P3_CLK_CON0, BIT(0)); \
    } else { \
        p33_and_1byte(P3_CLK_CON0, ~BIT(0)); \
    }

/*******************************************************************/

/*
 *  *-------------------P3_RST_CON0
 *   */

#define DPOR_MASK(a)           \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(0)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, ~BIT(0)); \
    }


#define VLVD_RST_EN(a)         \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(2)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, ~BIT(2)); \
    }


#define VLVD_WKUP_EN(a)        \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(3)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, ~BIT(3)); \
    }

#define PPOR_MASK(a)           \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(4)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, ~BIT(4)); \
    }

#define P11_TO_P33_RST_MASK(a)             \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(5)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, ~BIT(5)); \
    }

#define VDDOK_EN(a)       \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(6)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, ~BIT(6)); \
    }

#define PVDDOK_EN(a)       \
    if (a) {    \
        p33_or_1byte(P3_RST_CON0, BIT(7)); \
    } else { \
        p33_and_1byte(P3_RST_CON0, (u8)(~BIT(7))); \
    }


#endif /* #ifndef __P33_APP_H__ */
