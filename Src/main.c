/*
完全关闭 20uA
DS1302 2mA

delay改成slp5ms 静态0.4mA
*/


#include "STC8.h"
#include "stc_common.h"
#include "main.h"
#include <intrins.h>


unsigned char mode = MODE_TIME;
unsigned char settings_focus = 0;

unsigned char shutdown_mode_flag = 0; // 0无 1关闭所有 2停止晶振省电

unsigned char now_time[5] = {12, 00, 10, 23, 7};
unsigned char display_num_data[4];

unsigned char BTN1_INT_FLAG=0, BTN2_INT_FLAG=0;
unsigned char btn1_press_time=0, btn2_press_time=0; // 记录各个按钮长按的累计时间
unsigned char btn_state = 0, add_state;

unsigned char time_12 = 0;
unsigned char edit = 0; // 是否处于编辑中
unsigned char lift = 0;

unsigned char max_[3]; // 各个位的最大数值，动态设置
unsigned char hot_num = 0; // 正在修改的是前两位还是后两位，0/1
unsigned char last_blink = 0;

unsigned char refresh_screen_now = 1;
unsigned int refresh_count = 0x01;
unsigned char add_count = ADD_DT;

sfr IRCBAND = 0x9D; // 频段
sfr LIRTRIM = 0x9E; // 频率微调寄存器
sfr IRTRIM = 0x9F; // 频率调整寄存器


void high_speed()
{
  TR0 = 0; // 关闭定时器
  P_SW2 = 0x80;
  
  IRC24MCR |= 0x80; // 打开HRC
  while(!(IRC24MCR & 1));
  CKSEL = 0; // 内部32K  
  CLKDIV = 15; // 恢复默认  
  IRC32KCR &= 0x7F; // 关闭LRC
  P_SW2 = 0x00; 
}

void low_speed()
{
  P_SW2 = 0x80;
  IRC32KCR |= 0x80; // 打开LRC
  while(!(IRC32KCR & 1));
  CKSEL = 3; // 内部32K
  CLKDIV = 0;
  IRC24MCR &= 0x7F; // 关闭HRC

  P_SW2 = 0x00; 
  TR0 = 1; // 打开定时器
}

void delay(unsigned int time)
{
  unsigned int time_;
  P_SW2 = 0x80;
  if(CLKDIV==15)
  {
    P_SW2 = 0x00;
    for(time_=0;time_<time;time_++)
    {
      unsigned char i, j; // 2MHz
      i = 2;
      j = 239;
      do
      {
        while (--j);
      } while (--i);
    }
  }
  else
  {
    P_SW2 = 0x00;
    for(time_=0;time_<time;time_++)
    {
      unsigned char i; // 32KHz
      i = 8;
      while (--i);
    }
  }
}


void shutdown()
{
  // 标准双向
  P0M0 = 0x03;
  P0M1 = 0x03;
  P1M0 = 0x00;
  P1M1 = 0x00;
  P2M0 = 0x00;
  P2M1 = 0x00;
  P3M0 = 0x00;
  P3M1 = 0x0F;

  P4M0 = 0x00;
  P4M1 = 0x00;
  P5M0 = 0x00;
  P5M1 = 0x00;

  P0 = 0;
  P1 = 0;
  P2 = 0;
  P3 = 0;
  P4 = 0;
  P5 = 0;

  DS1302_VCC = DS1302_VCC_OFF;
  
  WKTCL=0xFE;  //掉电唤醒定时器关闭！
  WKTCH=0xFF;
  
  shutdown_mode_flag = 1;
  PCON = 0x02;
  _nop_();
  _nop_();
  _nop_();
  _nop_(); 
  
  //shutdown_mode_flag = 0;
  refresh_count = 1;
  refresh_screen_now = 0;
}


// DS1302
void DS1302_shift_out(unsigned char data_)
{
  unsigned char i;
  P36_OUT_EN; // DS1302_DAT
  for (i = 0; i < 8; i++)
  {
    // 1302先传入的是低位的
    DS1302_DAT = data_ >> i & 1;
    //上升沿写数据
    DS1302_CLK = 1;
    DS1302_CLK = 0;
  }
}

unsigned char DS1302_shift_in()
{
  unsigned char i, data_=0;
  P36_IN_EN; // DS1302_DAT
  for (i = 0; i < 8; i++)
  {
    //下降沿读数据 在DS1302_shift_out函数拉低之后马上读
    if (DS1302_DAT==1)
      data_ |= 1 << i;
    
    DS1302_CLK = 1;
    DS1302_CLK = 0;
  }
  return data_;
}

void DS1302_write(unsigned char address, unsigned char data_)
{
  DS1302_DAT = 0;

  DS1302_RST = 1;
  DS1302_shift_out(address);
  DS1302_shift_out(data_);
  DS1302_RST = 0;
  
}

unsigned char DS1302_read(unsigned char address)
{
  unsigned char data_;

  DS1302_DAT = 0;

  DS1302_RST = 1;
  DS1302_shift_out(address);
  data_ = DS1302_shift_in();
  DS1302_RST = 0;
  return data_;
}

void DS1302_power_on()
{
  DS1302_VCC = DS1302_VCC_ON;
  delay(5);
}

void DS1302_power_off()
{
  delay(5);
  DS1302_VCC = DS1302_VCC_OFF;
}

void DS1302_init()
{
  DS1302_power_on();
  DS1302_write(0x8E, 0x00); // 写保护off
  
  //DS1302_write(0x90, 0xA5); // 充电
  set_now_time();
  DS1302_write(0x8C, 0x22); // year
  DS1302_write(0x80, DS1302_read(0x81) & 0x7F); // 晶振起震 // bug:每次设置秒之后，毫秒会清零。所以在1s内不断reset的话，时间会卡住不动

  DS1302_write(0x8E, 0x80); // 写保护on
  DS1302_power_off();
}

void read_now_time()
{
  unsigned char time_data, power_flag;
  
  power_flag = DS1302_VCC;
  if(power_flag == DS1302_VCC_OFF)
    DS1302_power_on();

  time_data = DS1302_read(0x85); // hour
  now_time[NOW_TIME_HOUR] = (time_data >> 4 & 7) * 10 + (time_data & 15);

  time_data = DS1302_read(0x83); // min
  now_time[NOW_TIME_MINUTE] = (time_data >> 4 & 7) * 10 + (time_data & 15);

  time_data = DS1302_read(0x89); // month
  now_time[NOW_TIME_MONTH] = (time_data >> 4 & 7) * 10 + (time_data & 15);
  
  time_data = DS1302_read(0x87); // day
  now_time[NOW_TIME_DAY] = (time_data >> 4 & 7) * 10 + (time_data & 15);
  
  
  now_time[NOW_TIME_WEEKDAY] = DS1302_read(0x8B); // week
  
  if(power_flag == DS1302_VCC_OFF)
    DS1302_power_off();
}

void set_now_time()
{
  unsigned char power_flag;
  power_flag = DS1302_VCC;
  if(power_flag == DS1302_VCC_OFF)
    DS1302_power_on();
  
  DS1302_write(0x84, (now_time[NOW_TIME_HOUR] / 10 << 4) + now_time[NOW_TIME_HOUR] % 10); // hour
  DS1302_write(0x82, (now_time[NOW_TIME_MINUTE] / 10 << 4) + now_time[NOW_TIME_MINUTE] % 10); // min
  DS1302_write(0x88, (now_time[NOW_TIME_MONTH] / 10 << 4) + now_time[NOW_TIME_MONTH] % 10); // month
  DS1302_write(0x86, (now_time[NOW_TIME_DAY] / 10 << 4) + now_time[NOW_TIME_DAY] % 10); // day
  DS1302_write(0x8A, now_time[NOW_TIME_WEEKDAY]); // week
  
  if(power_flag == DS1302_VCC_OFF)
    DS1302_power_off();
}


// screen
void SCREEN_shift(unsigned char data_)
{
  SPDAT = data_;
  while (!(SPSTAT & 0x80));
  SPSTAT = 0xc0;   
}

void SCREEN_write_cmd(unsigned char cmd)
{
  SCREEN_DC = 0;
  SCREEN_shift(cmd);
}

void SCREEN_write_data(unsigned char data_)
{
  SCREEN_DC = 1;
  SCREEN_shift(data_);
}

void SCREEN_clear()
{
  unsigned char x,y;
  high_speed();
  for (y = 0; y < 8; y++)
  {
    SCREEN_write_cmd(0xB0 + y);//行
    SCREEN_write_cmd(0x00);//起始x
    SCREEN_write_cmd(0x10);//起始x
    SCREEN_DC = 1;
    for (x = 0; x < 128; x++)
    {
      SCREEN_shift(0x00);
    }
  }
  low_speed();
}

void SCREEN_init()
{
  const unsigned char code init_cmds[] = {
    0xE2, // 软件复位
    
    0x2F, // 开升压器 开电压调整器 开电压跟随器
    
    0x24, // 粗调对比度
    0x81, 31, // 细调对比度

    0x40, // 设置page1
    0xC8, // 上下颠倒
    0xAF,// 打开显示
  };
  unsigned char i;
  
  SCREEN_RST = 0;
  delay(1);
  SCREEN_RST = 1;
  
  SCREEN_CS = 0;
  SCREEN_BG = 0;
  
  // SPI init
  SPCTL = 0xD0;                               //使能SPI主机模式 忽略SS主从机选择引脚
  SPSTAT = 0xc0;                              //清中断标志
  
  SCREEN_DC = 0; // cmd
  for (i = 0; i < 8; i++)
  {
    SCREEN_write_cmd(init_cmds[i]);
    delay(1);
  }
  SCREEN_clear();
}

void print_line(unsigned char start_x, unsigned char start_y, char words[])
{
  unsigned char x, character;
  SCREEN_write_cmd(0xB0 + start_y); //行
  SCREEN_write_cmd(0x00 + (start_x & 0xF)); //起始x 低4位
  SCREEN_write_cmd(0x10 + (start_x >> 4)); //起始x 高4位

  character = 0; // 传入的words结尾是\0 (后面用character的时候要-1)
  while (words[character++])
  {
    SCREEN_DC = 1; // data
    for (x = 0; x < 5; x++)
      SCREEN_shift(FONT[(words[character - 1] - START_CHAR) * 5 + x]);
    SCREEN_shift(0x00);
  }
}

void print_var(unsigned char start_x, unsigned char start_y, unsigned char num, unsigned char len)
{
  unsigned char x;
  SCREEN_write_cmd(0xB0 + start_y); //行
  SCREEN_write_cmd(0x00 + (start_x & 0xF)); //起始x 低4位
  SCREEN_write_cmd(0x10 + (start_x >> 4)); //起始x 高4位
  
  SCREEN_DC = 1; // data
  if(len==3)
  {
    for (x = 0; x < 5; x++)
      SCREEN_shift(FONT[('0' + num/100 - START_CHAR) * 5 + x]);
    SCREEN_shift(0x00);
  }
  if(len>=2)
  {
    for (x = 0; x < 5; x++)
      SCREEN_shift(FONT[('0' + num/10%10 - START_CHAR) * 5 + x]);
    SCREEN_shift(0x00);
  }
  if(len>=1)
  {
    for (x = 0; x < 5; x++)
      SCREEN_shift(FONT[('0' + num%10 - START_CHAR) * 5 + x]);
    SCREEN_shift(0x00);
  }
}


void print_big_num(unsigned char start_x, unsigned char start_y,unsigned char num)
{
  unsigned char x, y;
  for (y = 0; y < 5; y++)
  {
    SCREEN_write_cmd(0xB0 + start_y + y); //行
    SCREEN_write_cmd(0x00 + (start_x & 0xF)); //起始x 低4位
    SCREEN_write_cmd(0x10 + (start_x >> 4)); //起始x 高4位
    
    SCREEN_DC = 1; // data
    if(num==10)
      for (x = 0; x < 24; x++)
        SCREEN_shift(0x00);
    else
      for (x = 0; x < 24; x++)
        SCREEN_shift(BIG_NUM_FONT[num*120 + y*24+x]);
   }
}

void print_small_num(unsigned char start_x, unsigned char start_y,unsigned char num)
{
  unsigned char x, y;
  for (y = 0; y < 4; y++)
  {
    SCREEN_write_cmd(0xB0 + start_y + y); //行
    SCREEN_write_cmd(0x00 + (start_x & 0xF)); //起始x 低4位
    SCREEN_write_cmd(0x10 + (start_x >> 4)); //起始x 高4位
    
    SCREEN_DC = 1; // data
    if(num==10)
      for (x = 0; x < 16; x++)
        SCREEN_shift(0x00);
    else
      for (x = 0; x < 16; x++)
        SCREEN_shift(SMALL_NUM_FONT[num*48 + y*16+x]);
   }
}

void print_time(unsigned char start_x, unsigned char start_y,unsigned char nums[])
{
  unsigned char x,digit;
 
  last_blink = 1 - last_blink;
  if (edit == 1 && last_blink==0 && btn_state!=4)
  {
    if (hot_num == 0)
    {
      display_num_data[0] = 10;
      display_num_data[1] = 10;
    }
    else
    {
      display_num_data[2] = 10;
      display_num_data[3] = 10;
    }
  }
  
  if(display_num_data[0] == 0)
    display_num_data[0] = 10;
  
  for(digit=0;digit<4;digit++)
  {
    print_big_num(start_x+NUM_START_OFFSET[digit], start_y,nums[digit]);
  }
  
  start_x += NUM_START_OFFSET[4];
  SCREEN_write_cmd(0xB1 + start_y); //行
  SCREEN_write_cmd(0x00 + (start_x & 0xF)); //起始x 低4位
  SCREEN_write_cmd(0x10 + (start_x >> 4)); //起始x 高4位
  
  SCREEN_DC = 1; // data
  for (x = 0; x < 4; x++)
    SCREEN_shift(0x7E);
  
  SCREEN_write_cmd(0xB3 + start_y); //行
  SCREEN_write_cmd(0x00 + (start_x & 0xF)); //起始x 低4位
  SCREEN_write_cmd(0x10 + (start_x >> 4)); //起始x 高4位
  
  SCREEN_DC = 1; // data
  for (x = 0; x < 4; x++)
    SCREEN_shift(0x7E);
}

void update_display_num_data()
{
  // 正常显示主时间

  if(time_12 && now_time[NOW_TIME_HOUR]>12)
  {
    display_num_data[0] = (now_time[NOW_TIME_HOUR] - 12) / 10;
    display_num_data[1] = (now_time[NOW_TIME_HOUR] - 12) % 10;
  }
  else
  {
    display_num_data[0] = now_time[NOW_TIME_HOUR] / 10;
    display_num_data[1] = now_time[NOW_TIME_HOUR] % 10;
  }

  display_num_data[2] = now_time[NOW_TIME_MINUTE] / 10;
  display_num_data[3] = now_time[NOW_TIME_MINUTE] % 10;
}


void draw_class(unsigned char day)
{
  // day 从零开始
  unsigned char line, x, lesson;
  unsigned int temp;

  for (line = 0; line < 2; line++)
  {
    SCREEN_write_cmd(0xB2 +line); //行
    SCREEN_write_cmd(0x00); //起始x 低4位
    SCREEN_write_cmd(0x10); //起始x 高4位
    
    SCREEN_DC = 1; // data
    for(lesson = 0; lesson<9; lesson++)
    {
      temp = lesson_table[lesson+day*9]*28+line*14; 
      if(lesson==5)
      {
        SCREEN_shift(0x00);
        SCREEN_shift(0x00);
        SCREEN_shift(0x00);
      }
      for (x = 0; x <14; x++)
      {
        SCREEN_shift(CHINESE_FONT[temp+x]);
      }
    }
  }
}

void print_small_time(unsigned char start_x, unsigned char start_y, unsigned char now_time_hour, unsigned char now_time_min)
{
  if(time_12 && now_time_hour>12)
    now_time_hour = now_time_hour%12;

   
  if(now_time_hour>=10)
    print_var(start_x, start_y, now_time_hour,2);
  else
  {
    print_line(start_x, start_y, " ");
    print_var(start_x+5, start_y, now_time_hour,1);
  }
  
  print_line(start_x+12, start_y, ":");
  print_var(start_x+17, start_y, now_time_min,2);

}

void print_next_time(unsigned char start_x, unsigned char start_y)
{
  unsigned char i;
  unsigned int now_time_hour, now_time_min;
  now_time_min = now_time[NOW_TIME_HOUR]*60+now_time[NOW_TIME_MINUTE];
  for(i=0;i<23;i++)
  {
    if(now_time_min<time_table[i])
      break;
  }
  now_time_hour = time_table[i]/60;
  
  print_small_time(start_x, start_y, now_time_hour, time_table[i]%60);
}

void draw_chinese(unsigned char start_x, unsigned char start_y, unsigned char chinese)
{
  unsigned char line,x;
  
  for (line = 0; line < 2; line++)
  {
    SCREEN_write_cmd(0xB0 + start_y + line); //行
    SCREEN_write_cmd(start_x & 0x0F); //起始x 低4位
    SCREEN_write_cmd(0x10 + (start_x>>4)); //起始x 高4位
    
    SCREEN_DC = 1; // data
    for (x = 0; x <14; x++)
    {
      SCREEN_shift(CHINESE_FONT[chinese*28+line*14+x]);
    }
  }
}
void print_day_and_date()
{
  last_blink = 1-last_blink;
  
  if(edit && last_blink==0)
  {
    if(hot_num==0)
      print_line(2,0,"  ");
    if(hot_num==1)
      print_line(18,0,"  ");
    if(hot_num==2)
      print_line(35,0, "          ");
  }
  else
  {
    print_line(13, 0, ".");
    print_var(2,0,now_time[NOW_TIME_MONTH], 2);
    print_var(18,0,now_time[NOW_TIME_DAY], 2);
    print_line(35,0, WEEK_NAME[now_time[NOW_TIME_WEEKDAY]-1]);
  }
}

void draw_count_down()
{
  unsigned char x;
  signed int remained_day;
  
  remained_day = MONTH_TO_COUNTDOWN[now_time[NOW_TIME_MONTH] - 1] - now_time[NOW_TIME_DAY];
  if(remained_day<0)
    remained_day = 0;
    
  if(remained_day>99)
    print_small_num(60,5,remained_day/100);
  else
    print_small_num(60,5,10);
  
  if(remained_day>9)
    print_small_num(78,5,(remained_day/10)%10);
  else
    print_small_num(78,5,10);
  
  print_small_num(96,5,remained_day%10);
  
  for (x = 0; x < 4; x++)
    draw_chinese(14*x,6,14+x);

  draw_chinese(114,6,18);
}

void draw_count_down_big()
{
  signed int remained_day;
  
  remained_day = MONTH_TO_COUNTDOWN[now_time[NOW_TIME_MONTH] - 1] - now_time[NOW_TIME_DAY];
  if(remained_day<0)
    remained_day = 0;
  
  if(remained_day>99)
    print_big_num(22,2,remained_day/100);
  else
    print_big_num(22,2,10);
  
  if(remained_day>9)
    print_big_num(54,2,(remained_day/10)%10);
  else
    print_big_num(54,2,10);
  
  print_big_num(86,2,remained_day%10);
  
  draw_chinese(2,2,14);
  draw_chinese(2,5,15);
  draw_chinese(114,5,18);
}

void draw_calendar()
{
  unsigned char x, y, i, day_count, start_day, month_days;
  
  start_day = (35+now_time[NOW_TIME_WEEKDAY]-now_time[NOW_TIME_DAY])%7;

  month_days = MAX_MONTH_DAYS[now_time[NOW_TIME_MONTH]-1];

  day_count = 0;
  for(x=0;x<start_day;x++)
  {
    print_line(x*19+4, 2, " ");

  }
  for(x=start_day;x<7;x++)
  {
    day_count ++;
    print_var(x*19+4, 2, day_count, 1);
    
  }
  for(y=1;y<6;y++)
  {
    for(x=0;x<7;x++)
    {
      day_count ++;
      if(day_count > month_days)
        print_line(x*19+1, y+2, "  ");
      else
      {
        if(day_count<10)
        {
          print_line(x*19+1, y+2, "  ");
          print_var(x*19+4, y+2, day_count, 1);
        }
        else
          print_var(x*19+1, y+2, day_count, 2);
        
      }

    }
  }
  
  // table
  for(y=0;y<6;y++)
  {
    SCREEN_write_cmd(0xB2 + y); //行
    for(x=0;x<6;x++)
    {
      SCREEN_write_cmd(0x00 + ((x*19+16) & 0xF)); //起始x 低4位
      SCREEN_write_cmd(0x10 + ((x*19+16) >> 4)); //起始x 高4位
      SCREEN_write_data(0xFF);
    }
  }
  
  // 标出今天
  
  y = (start_day + now_time[NOW_TIME_DAY]-1) / 7;
  SCREEN_write_cmd(0xB2 + y); //行
  SCREEN_write_cmd(0x00 + ((now_time[NOW_TIME_WEEKDAY]*19-21) & 0xF)); //起始x 低4位
  SCREEN_write_cmd(0x10 + ((now_time[NOW_TIME_WEEKDAY]*19-21) >> 4)); //起始x 高4位

  SCREEN_write_data(0xFF);
  SCREEN_write_data(0xFF);
  
  SCREEN_write_cmd(0x00 + ((now_time[NOW_TIME_WEEKDAY]*19-5) & 0xF)); //起始x 低4位
  SCREEN_write_cmd(0x10 + ((now_time[NOW_TIME_WEEKDAY]*19-5) >> 4)); //起始x 高4位

  SCREEN_write_data(0xFF);
  SCREEN_write_data(0xFF);
  
  
  // 星期
  SCREEN_write_cmd(0xB1); //行
  SCREEN_write_cmd(0x00); //起始x 低4位
  SCREEN_write_cmd(0x10); //起始x 高4位
  
  SCREEN_DC = 1; // data   
  for (i = 0; i < 3; i++)
    SCREEN_shift(0x20); 

  for(x=0;x<7;x++)
  { 
    SCREEN_shift(0x00); 
    for (i = 0; i < 5; i++)
      SCREEN_shift(FONT[(WEEK_NAME[x][0] - START_CHAR) * 5 + i]);
    SCREEN_shift(0x00); 
    for (i = 0; i < 6; i++)
      SCREEN_shift(0x20);
    SCREEN_shift(0xE0); 
    for (i = 0; i < 5; i++)
      SCREEN_shift(0x20);
  }


}


void refresh_screen()
{
  unsigned char time_data;
  high_speed();
  
  if(mode==MODE_TIME)
  {
    time_data = DS1302_read(0x85); // hour
    now_time[NOW_TIME_HOUR] = (time_data >> 4 & 7) * 10 + (time_data & 15);

    time_data = DS1302_read(0x83); // min
    now_time[NOW_TIME_MINUTE] = (time_data >> 4 & 7) * 10 + (time_data & 15);

    update_display_num_data();
    
    if(now_time[NOW_TIME_HOUR]>12)
      print_line(2,0, "PM");
    else
      print_line(2,0, "AM");
    
    print_next_time(98, 0);

    print_time(2,2,display_num_data);
    SCREEN_BG = 0;
  }
  else if (mode == MODE_COUNT_DOWN)
  {
    read_now_time();
    
    print_day_and_date();
    
    print_small_time(98,0,now_time[NOW_TIME_HOUR], now_time[NOW_TIME_MINUTE]);
    
    draw_count_down_big();
  }
  else if (mode == MODE_CLASS)
  {
    read_now_time();
    print_day_and_date();
    
    // 6点钟之后显示第二天的课表
    if(now_time[NOW_TIME_HOUR] > 17)
    {
      print_line(99,0, "tomo");
      draw_class(now_time[NOW_TIME_WEEKDAY] % 7);
    }
    else
    {
      print_line(99,0, "today");
      draw_class(now_time[NOW_TIME_WEEKDAY] - 1);
    }
    draw_count_down();

    SCREEN_BG = 0;
  }
  else if (mode == MODE_CALENDAR)
  {
    read_now_time();
    
    print_day_and_date();
    
    draw_calendar();
  }
  else if (mode == MODE_SETTING)
  {
    SCREEN_BG = edit;

    print_line(2,0, "Countdown Clock");
    print_line(2,1, "Ver. 4.1");
    
    
    print_line(2,6, "- Designed by");
    print_line(2,7, "- Baoqi-zhong :D");

    //SCREEN_BG = 0;
  }
  
  low_speed();
}

void btn()
{
  if (lift == 1) // 还没长按够
  {
    if (BTN1_INT_FLAG == 1 && BTN2_INT_FLAG == 0)
      btn1_press_time += 1;
    else
      btn1_press_time = 0;
    
    if (BTN2_INT_FLAG == 1 && BTN1_INT_FLAG == 0)
      btn2_press_time += 1;
    else
      btn2_press_time = 0;
  }
  
  if (btn1_press_time > LONG_PRESS_TIME)
  {
    // btn1 长按
    btn_state = 2;
    btn1_press_time = 0;
    lift = 0;
  }
  if (btn2_press_time > LONG_PRESS_TIME)
  {
    // btn2 长按
    btn_state = 4;
    btn2_press_time = 0;
    lift = 0;
  }
}

void btn_process()
{
  unsigned char power_flag;

  if (btn_state == 1 & edit==0 & settings_focus==0) //btn1 短
  {
    delay(1);
    mode = (mode + 1) % MAX_MODE;
    SCREEN_clear();
    refresh_screen_now = 1;
    btn_state = 0;
   
    return;
  }
  
  if (btn_state == 2) //btn1 长
  {
    if (mode!=MODE_SETTING)
    {
      refresh_screen_now = 1;
      if (edit==1)
      {
        edit = 0;
        //TMR0_DIS;
        DS1302_write(0x8E, 0x80);
        DS1302_power_off();
      }
      else
      {
        edit = 1;
        hot_num = 0;
        refresh_count = 0x2F;
        add_count = ADD_DT;
        //TMR0_EN;
        DS1302_power_on();
        DS1302_write(0x8E, 0x00);
      }
      btn_state = 0;
      return;
    }
    else
    {
      btn_state = 0;
      return;
    }
    
    if(mode==MODE_SETTING)
    {
      refresh_screen_now = 1;

      if (settings_focus==0)
        settings_focus = 1;
      else
        settings_focus = 0;
      
      btn_state = 0;
      return; 
    }
  }

  
  if(mode==MODE_TIME)
  {
    max_[0] = 24;
    max_[1] = 60; 
    if(edit==1)
    {
      if (btn_state == 1) //btn1 短
      {
        hot_num = 1 - hot_num;
        refresh_screen_now = 1;
        btn_state = 0;
        return;
      }
      if (btn_state == 3) //btn2 短
      {
        now_time[hot_num] = (now_time[hot_num] + 1) % max_[hot_num];
        DS1302_write(0x84, (now_time[NOW_TIME_HOUR] / 10 << 4) + now_time[NOW_TIME_HOUR] % 10); // hour
        DS1302_write(0x82, (now_time[NOW_TIME_MINUTE] / 10 << 4) + now_time[NOW_TIME_MINUTE] % 10); // min
        refresh_screen_now = 1;
        last_blink = 0;
        btn_state = 0;
        return;
      }
      
      if(btn_state == 4)// btn2 长
      {
        add_count--;
        if(add_count==0)
        {
          add_count = ADD_DT;
          now_time[hot_num] = (now_time[hot_num] + 1) % max_[hot_num];
          DS1302_write(0x84, (now_time[NOW_TIME_HOUR] / 10 << 4) + now_time[NOW_TIME_HOUR] % 10); // hour
          DS1302_write(0x82, (now_time[NOW_TIME_MINUTE] / 10 << 4) + now_time[NOW_TIME_MINUTE] % 10); // min
          refresh_screen_now = 1;
          last_blink = 0;
          return;
        }
      }
    }
    else
    {
      if (btn_state == 3) //btn2 短
      {
        
        time_12 = 1 - time_12;
        
        power_flag = DS1302_VCC;
        if(power_flag == DS1302_VCC_OFF)
          DS1302_power_on();
        
        DS1302_write(0x8E, 0x00);
        DS1302_write(0xD0, time_12);
        DS1302_write(0x8E, 0x80);
        
        if(power_flag == DS1302_VCC_OFF)
          DS1302_power_off();
        
        refresh_screen_now = 1;
        btn_state = 0;
        return;
      }
    }
  }

  
  else if (mode==MODE_CLASS | mode==MODE_COUNT_DOWN | mode==MODE_CALENDAR)
  {
    max_[0] = 12;
    max_[1] = 31; 
    max_[2] = 7; 
    if(edit==1)
    {
      if (btn_state == 1) //btn1 短
      {
        hot_num = (hot_num+1)%3;
        refresh_screen_now = 1;
        btn_state = 0;
        return;
      }
      if (btn_state == 3) //btn2 短
      {
        now_time[hot_num+2] = now_time[hot_num+2] % max_[hot_num] + 1;
        DS1302_write(0x88, (now_time[NOW_TIME_MONTH] / 10 << 4) + now_time[NOW_TIME_MONTH] % 10); // month
        DS1302_write(0x86, (now_time[NOW_TIME_DAY] / 10 << 4) + now_time[NOW_TIME_DAY] % 10); // day
        DS1302_write(0x8A, now_time[NOW_TIME_WEEKDAY]); // week
        refresh_screen_now = 1;
        btn_state = 0;
        return;
      }
      if(btn_state == 4)// btn2 长
      {
        add_count--;
        if(add_count==0)
        {
          add_count = ADD_DT;
          now_time[hot_num+2] = now_time[hot_num+2] % max_[hot_num] + 1;
          DS1302_write(0x88, (now_time[NOW_TIME_MONTH] / 10 << 4) + now_time[NOW_TIME_MONTH] % 10); // month
          DS1302_write(0x86, (now_time[NOW_TIME_DAY] / 10 << 4) + now_time[NOW_TIME_DAY] % 10); // day
          DS1302_write(0x8A, now_time[NOW_TIME_WEEKDAY]); // week
          refresh_screen_now = 1;
          last_blink = 0;
          return;
        }
      }
    }
  }
  
  if (btn_state==2 && BTN1_INT_FLAG == 0)
  {
    btn_state = 0;
    return;
  }
  if (btn_state==4 && BTN2_INT_FLAG == 0)
  {
    btn_state = 0;
    return;
  }
  if (btn_state==3)
  {
    btn_state = 0;
    return;
  }
}

void wake_up_init()
{
  shutdown_mode_flag = 0;
  
  P1_OUT_EN;
  P3_OUT_EN;
  P54_OUT_EN;
  P32_IN_EN;
  P33_IN_EN;
  
  DS1302_CLK = 0;  
  
  SCREEN_RST = 1;
  SCREEN_CS = 0;
  SCREEN_BG = 0;
  SCREEN_init();
  
  mode = MODE_TIME;
  read_now_time();
  refresh_screen_now = 1;
}


void INT0_Isr() interrupt 0 
{
  if(BTN_1 == 1) // 上升沿
  {
    if(shutdown_mode_flag==1)
      wake_up_init();
    else
    {
      BTN1_INT_FLAG = 1;
    }
  }
  else // 下降沿
  {
    if(shutdown_mode_flag==1)
      shutdown();
    else
    {
      if (btn1_press_time>0) //btn2 短按
        btn_state = 1;
      
      if(btn_state==2)
        btn_state = 0;
      
      lift = 1;
      BTN1_INT_FLAG = 0;
      btn1_press_time = 0;
    }
  }

}

void INT1_Isr() interrupt 2 
{
  if(BTN_2 == 1) // 上升沿
  {
    if(shutdown_mode_flag==1)
      wake_up_init();
    else
    {
      BTN2_INT_FLAG = 1;
    }
  }
  else // 下降沿
  {
    if(shutdown_mode_flag==1)
      shutdown();
    else
    {
      if (btn2_press_time>0) //btn2 短按
        btn_state = 3;
      
      if(btn_state==4)
        btn_state = 0;
      
      lift = 1;
      BTN2_INT_FLAG = 0;
      btn2_press_time = 0;
    }
  }
}

void main()
{
  low_speed();

  P1_OUT_EN;
  P3_OUT_EN;
  P54_OUT_EN;
  P32_IN_EN;
  P33_IN_EN;
  
  DS1302_RST = 0;
  DS1302_CLK = 0;
  DS1302_DAT = 0;


  SCREEN_init();
  
  DS1302_init();

  
  // 中断
  EX0 = 1;
  IT0 = 0; // 上升下降沿都产生中断
  EX1 = 1;
  IT1 = 0; // 上升下降沿都产生中断 
  
  EA = 1; // 总中断


  BUZZER = 0;

  while(1)
  {
    refresh_count--;
    if(refresh_count == 0) 
    {
      refresh_screen_now = 1;
     
      if(edit&&(mode!=MODE_SETTING))
      {
        refresh_count = 0x40;
      }
      else
      {
        refresh_count = 0x19BC; // 57s
        
        read_now_time();
        if(now_time[NOW_TIME_HOUR]>=23 || now_time[NOW_TIME_HOUR]<6 )
        {
          //TMR2_CLR;
          //TMR2_EN;
          shutdown();
        }
      }
    }
    
    if(refresh_screen_now)
    {
      refresh_screen_now = 0;
      if(shutdown_mode_flag)
        SCREEN_init();
      refresh_screen();
    }  
    btn();
  
    if(btn_state)
    {
      btn_process();    
    }

    //delay(5);
    WKTCL=0x0A;  //掉电唤醒定时器关闭！
    WKTCH=0x80;
    
    PCON = 0x02;
    _nop_();
    _nop_();
  }
}

