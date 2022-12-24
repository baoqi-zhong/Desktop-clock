//IO输出设置
#define  P00_OUT_EN     P0M0 |= 0x01; P0M1 &= 0xFE;
#define  P01_OUT_EN     P0M0 |= 0x02; P0M1 &= 0xFD;
#define  P02_OUT_EN     P0M0 |= 0x04; P0M1 &= 0xFB;
#define  P03_OUT_EN     P0M0 |= 0x08; P0M1 &= 0xF7;
#define  P04_OUT_EN     P0M0 |= 0x10; P0M1 &= 0xEF;
#define  P05_OUT_EN     P0M0 |= 0x20; P0M1 &= 0xDF;
#define  P06_OUT_EN     P0M0 |= 0x40; P0M1 &= 0xBF;
#define  P07_OUT_EN     P0M0 |= 0x80; P0M1 &= 0x7F;
#define  P0_OUT_EN      P0M0  = 0xFF; P0M1 = 0x00;

#define  P10_OUT_EN     P1M0 |= 0x01; P1M1 &= 0xFE;
#define  P11_OUT_EN     P1M0 |= 0x02; P1M1 &= 0xFD;
#define  P12_OUT_EN     P1M0 |= 0x04; P1M1 &= 0xFB;
#define  P13_OUT_EN     P1M0 |= 0x08; P1M1 &= 0xF7;
#define  P14_OUT_EN     P1M0 |= 0x10; P1M1 &= 0xEF;
#define  P15_OUT_EN     P1M0 |= 0x20; P1M1 &= 0xDF;
#define  P16_OUT_EN     P1M0 |= 0x40; P1M1 &= 0xBF;
#define  P17_OUT_EN     P1M0 |= 0x80; P1M1 &= 0x7F;
#define  P1_OUT_EN      P1M0  = 0xFF; P1M1 = 0x00;

#define  P20_OUT_EN     P2M0 |= 0x01; P2M1 &= 0xFE;
#define  P21_OUT_EN     P2M0 |= 0x02; P2M1 &= 0xFD;
#define  P22_OUT_EN     P2M0 |= 0x04; P2M1 &= 0xFB;
#define  P23_OUT_EN     P2M0 |= 0x08; P2M1 &= 0xF7;
#define  P24_OUT_EN     P2M0 |= 0x10; P2M1 &= 0xEF;
#define  P25_OUT_EN     P2M0 |= 0x20; P2M1 &= 0xDF;
#define  P26_OUT_EN     P2M0 |= 0x40; P2M1 &= 0xBF;
#define  P27_OUT_EN     P2M0 |= 0x80; P2M1 &= 0x7F;
#define  P2_OUT_EN      P2M0  = 0xFF; P2M1 = 0x00;

#define  P30_OUT_EN     P3M0 |= 0x01; P3M1 &= 0xFE;
#define  P31_OUT_EN     P3M0 |= 0x02; P3M1 &= 0xFD;
#define  P32_OUT_EN     P3M0 |= 0x04; P3M1 &= 0xFB;
#define  P33_OUT_EN     P3M0 |= 0x08; P3M1 &= 0xF7;
#define  P34_OUT_EN     P3M0 |= 0x10; P3M1 &= 0xEF;
#define  P35_OUT_EN     P3M0 |= 0x20; P3M1 &= 0xDF;
#define  P36_OUT_EN     P3M0 |= 0x40; P3M1 &= 0xBF;
#define  P37_OUT_EN     P3M0 |= 0x80; P3M1 &= 0x7F;
#define  P3_OUT_EN      P3M0  = 0xFF; P3M1 = 0x00;

#define  P40_OUT_EN     P4M0 |= 0x01; P4M1 &= 0xFE;
#define  P41_OUT_EN     P4M0 |= 0x02; P4M1 &= 0xFD;
#define  P42_OUT_EN     P4M0 |= 0x04; P4M1 &= 0xFB;
#define  P43_OUT_EN     P4M0 |= 0x08; P4M1 &= 0xF7;
#define  P44_OUT_EN     P4M0 |= 0x10; P4M1 &= 0xEF;
#define  P45_OUT_EN     P4M0 |= 0x20; P4M1 &= 0xDF;
#define  P46_OUT_EN     P4M0 |= 0x40; P4M1 &= 0xBF;
#define  P47_OUT_EN     P4M0 |= 0x80; P4M1 &= 0x7F;
#define  P4_OUT_EN      P4M0  = 0xFF; P4M1 = 0x00;

#define  P50_OUT_EN     P5M0 |= 0x01; P5M1 &= 0xFE;
#define  P51_OUT_EN     P5M0 |= 0x02; P5M1 &= 0xFD;
#define  P52_OUT_EN     P5M0 |= 0x04; P5M1 &= 0xFB;
#define  P53_OUT_EN     P5M0 |= 0x08; P5M1 &= 0xF7;
#define  P54_OUT_EN     P5M0 |= 0x10; P5M1 &= 0xEF;
#define  P55_OUT_EN     P5M0 |= 0x20; P5M1 &= 0xDF;
#define  P56_OUT_EN     P5M0 |= 0x40; P5M1 &= 0xBF;
#define  P57_OUT_EN     P5M0 |= 0x80; P5M1 &= 0x7F;
#define  P5_OUT_EN      P5M0  = 0xFF; P5M1 = 0x00;

#define  P60_OUT_EN     P6M0 |= 0x01; P6M1 &= 0xFE;
#define  P61_OUT_EN     P6M0 |= 0x02; P6M1 &= 0xFD;
#define  P62_OUT_EN     P6M0 |= 0x04; P6M1 &= 0xFB;
#define  P63_OUT_EN     P6M0 |= 0x08; P6M1 &= 0xF7;
#define  P64_OUT_EN     P6M0 |= 0x10; P6M1 &= 0xEF;
#define  P65_OUT_EN     P6M0 |= 0x20; P6M1 &= 0xDF;
#define  P66_OUT_EN     P6M0 |= 0x40; P6M1 &= 0xBF;
#define  P67_OUT_EN     P6M0 |= 0x80; P6M1 &= 0x7F;
#define  P6_OUT_EN      P6M0  = 0xFF; P6M1 = 0x00;

#define  P70_OUT_EN     P7M0 |= 0x01; P7M1 &= 0xFE;
#define  P71_OUT_EN     P7M0 |= 0x02; P7M1 &= 0xFD;
#define  P72_OUT_EN     P7M0 |= 0x04; P7M1 &= 0xFB;
#define  P73_OUT_EN     P7M0 |= 0x08; P7M1 &= 0xF7;
#define  P74_OUT_EN     P7M0 |= 0x10; P7M1 &= 0xEF;
#define  P75_OUT_EN     P7M0 |= 0x20; P7M1 &= 0xDF;
#define  P76_OUT_EN     P7M0 |= 0x40; P7M1 &= 0xBF;
#define  P77_OUT_EN     P7M0 |= 0x80; P7M1 &= 0x7F;
#define  P7_OUT_EN      P7M0  = 0xFF; P7M1 = 0x00;

//IO输入设置
#define  P00_IN_EN     P0M0 &= 0xFE; P0M1 |= 0x01;
#define  P01_IN_EN     P0M0 &= 0xFD; P0M1 |= 0x02;
#define  P02_IN_EN     P0M0 &= 0xFB; P0M1 |= 0x04;
#define  P03_IN_EN     P0M0 &= 0xF7; P0M1 |= 0x08;
#define  P04_IN_EN     P0M0 &= 0xEF; P0M1 |= 0x10;
#define  P05_IN_EN     P0M0 &= 0xDF; P0M1 |= 0x20;
#define  P06_IN_EN     P0M0 &= 0xBF; P0M1 |= 0x40;
#define  P07_IN_EN     P0M0 &= 0xEF; P0M1 |= 0x80;
#define  P0_IN_EN      P0M0  = 0x00; P0M1 = 0xFF;

#define  P10_IN_EN     P1M0 &= 0xFE; P1M1 |= 0x01;
#define  P11_IN_EN     P1M0 &= 0xFD; P1M1 |= 0x02;
#define  P12_IN_EN     P1M0 &= 0xFB; P1M1 |= 0x04;
#define  P13_IN_EN     P1M0 &= 0xF7; P1M1 |= 0x08;
#define  P14_IN_EN     P1M0 &= 0xEF; P1M1 |= 0x10;
#define  P15_IN_EN     P1M0 &= 0xDF; P1M1 |= 0x20;
#define  P16_IN_EN     P1M0 &= 0xBF; P1M1 |= 0x40;
#define  P17_IN_EN     P1M0 &= 0xEF; P1M1 |= 0x80;
#define  P1_IN_EN      P1M0  = 0x00; P1M1 = 0xFF;

#define  P20_IN_EN     P2M0 &= 0xFE; P2M1 |= 0x01;
#define  P21_IN_EN     P2M0 &= 0xFD; P2M1 |= 0x02;
#define  P22_IN_EN     P2M0 &= 0xFB; P2M1 |= 0x04;
#define  P23_IN_EN     P2M0 &= 0xF7; P2M1 |= 0x08;
#define  P24_IN_EN     P2M0 &= 0xEF; P2M1 |= 0x10;
#define  P25_IN_EN     P2M0 &= 0xDF; P2M1 |= 0x20;
#define  P26_IN_EN     P2M0 &= 0xBF; P2M1 |= 0x40;
#define  P27_IN_EN     P2M0 &= 0xEF; P2M1 |= 0x80;
#define  P2_IN_EN      P2M0  = 0x00; P2M1 = 0xFF;

#define  P30_IN_EN     P3M0 &= 0xFE; P3M1 |= 0x01;
#define  P31_IN_EN     P3M0 &= 0xFD; P3M1 |= 0x02;
#define  P32_IN_EN     P3M0 &= 0xFB; P3M1 |= 0x04;
#define  P33_IN_EN     P3M0 &= 0xF7; P3M1 |= 0x08;
#define  P34_IN_EN     P3M0 &= 0xEF; P3M1 |= 0x10;
#define  P35_IN_EN     P3M0 &= 0xDF; P3M1 |= 0x20;
#define  P36_IN_EN     P3M0 &= 0xBF; P3M1 |= 0x40;
#define  P37_IN_EN     P3M0 &= 0xEF; P3M1 |= 0x80;
#define  P3_IN_EN      P3M0  = 0x00; P3M1 = 0xFF;

#define  P40_IN_EN     P4M0 &= 0xFE; P4M1 |= 0x01;
#define  P41_IN_EN     P4M0 &= 0xFD; P4M1 |= 0x02;
#define  P42_IN_EN     P4M0 &= 0xFB; P4M1 |= 0x04;
#define  P43_IN_EN     P4M0 &= 0xF7; P4M1 |= 0x08;
#define  P44_IN_EN     P4M0 &= 0xEF; P4M1 |= 0x10;
#define  P45_IN_EN     P4M0 &= 0xDF; P4M1 |= 0x20;
#define  P46_IN_EN     P4M0 &= 0xBF; P4M1 |= 0x40;
#define  P47_IN_EN     P4M0 &= 0xEF; P4M1 |= 0x80;
#define  P4_IN_EN      P4M0  = 0x00; P4M1 = 0xFF;

#define  P50_IN_EN     P5M0 &= 0xFE; P5M1 |= 0x01;
#define  P51_IN_EN     P5M0 &= 0xFD; P5M1 |= 0x02;
#define  P52_IN_EN     P5M0 &= 0xFB; P5M1 |= 0x04;
#define  P53_IN_EN     P5M0 &= 0xF7; P5M1 |= 0x08;
#define  P54_IN_EN     P5M0 &= 0xEF; P5M1 |= 0x10;
#define  P55_IN_EN     P5M0 &= 0xDF; P5M1 |= 0x20;
#define  P56_IN_EN     P5M0 &= 0xBF; P5M1 |= 0x40;
#define  P57_IN_EN     P5M0 &= 0xEF; P5M1 |= 0x80;
#define  P5_IN_EN      P5M0  = 0x00; P5M1 = 0xFF;

#define  P60_IN_EN     P6M0 &= 0xFE; P6M1 |= 0x01;
#define  P61_IN_EN     P6M0 &= 0xFD; P6M1 |= 0x02;
#define  P62_IN_EN     P6M0 &= 0xFB; P6M1 |= 0x04;
#define  P63_IN_EN     P6M0 &= 0xF7; P6M1 |= 0x08;
#define  P64_IN_EN     P6M0 &= 0xEF; P6M1 |= 0x10;
#define  P65_IN_EN     P6M0 &= 0xDF; P6M1 |= 0x20;
#define  P66_IN_EN     P6M0 &= 0xBF; P6M1 |= 0x40;
#define  P67_IN_EN     P6M0 &= 0xEF; P6M1 |= 0x80;
#define  P6_IN_EN      P6M0  = 0x00; P6M1 = 0xFF;

#define  P70_IN_EN     P7M0 &= 0xFE; P7M1 |= 0x01;
#define  P71_IN_EN     P7M0 &= 0xFD; P7M1 |= 0x02;
#define  P72_IN_EN     P7M0 &= 0xFB; P7M1 |= 0x04;
#define  P73_IN_EN     P7M0 &= 0xF7; P7M1 |= 0x08;
#define  P74_IN_EN     P7M0 &= 0xEF; P7M1 |= 0x10;
#define  P75_IN_EN     P7M0 &= 0xDF; P7M1 |= 0x20;
#define  P76_IN_EN     P7M0 &= 0xBF; P7M1 |= 0x40;
#define  P77_IN_EN     P7M0 &= 0xEF; P7M1 |= 0x80;
#define  P7_IN_EN      P7M0  = 0x00; P7M1 = 0xFF;

