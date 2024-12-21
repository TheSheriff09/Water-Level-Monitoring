#line 1 "D:/micro_proj/automatic_water_level_monitoring_System/program.c"






sbit LCD_RS at RD0_bit;
sbit LCD_EN at RD1_bit;
sbit LCD_D7 at RD7_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D4 at RD4_bit;
sbit LCD_RS_Direction at TRISD0_bit;
sbit LCD_EN_Direction at TRISD1_bit;
sbit LCD_D7_Direction at TRISD7_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD4_bit;

int counter = 0;
int flag1 = 0;
unsigned short cc = 0;
unsigned short take;
unsigned int i = 0;
int prevs = 0;
int prevs1 = 0;
unsigned int NB = 0;
int t0 = 0;
int t1 = 0;
int t2 = 0;
int prev = 0;
int level;
int prev1 = 0;
int test = 0;

void interrupt() {
 if (INTCON.INTF == 1) {
 t2 = 1;
 INTCON.INTF = 0;
 }
 if (INTCON.T0IF == 1) {
 INTCON.T0IF = 0;
 t0 = 1;
 }
 if (INTCON.RBIF) {
 if (PORTB.RB4) {
 t1 = 1;
 }
 INTCON.RBIF = 0;
 }
}

void main() {
 char display[16];
 char strValue[4];
 int prevs = 0;
 unsigned int adcValue;

 TRISA.RA0 = 1;
 ADC_Init();

 TRISB.RB3 = 0;
 TRISB.RB5 = 0;
 TRISB.RB1 = 0;
 TRISB.RB2 = 0;
 TRISB.RB0 = 1;
 TRISB.RB4 = 1;
 TRISC.RC3 = 0;
 TRISC.RB7 = 1;
 INTCON.GIE = 1;
 INTCON.RBIE = 1;
 INTCON.INTE = 1;
 OPTION_REG = 0b00000111;
 TMR0 = 0;
 NB = 61;

 Lcd_Init();

  PORTB.RB5  = 0;
  PORTB.RB2  = 0;
  PORTB.RB1  = 0;
 PORTB.RB3 = 0;
 Sound_Init(&PORTC, 3);

 counter = EEPROM_Read(0x33);

 while (1) {
 adcValue = (ADC_Read(0) / 10) - 1;
 level = (adcValue) * 10;

 if (t1 == 1 && test == 0) {
  ( PORTB.RB5  = 0) ;
  ( PORTB.RB2  = 1) ;
 Lcd_Cmd(_LCD_CLEAR);
 Lcd_Out(2, 2, "Manual Pump");
 PORTB.RB3 = 1;
 test = 1;
 } else if (t0 == 1) {
 NB--;
 if (NB == 0) {
  ( PORTB.RB1  = 1) ;
  ( PORTB.RB2  = 0) ;
  ( PORTB.RB5  = 0) ;
 PORTB.RB3 = 1;
 Lcd_Out(2, 2, "Critical Level");
 prev1 = 0;
 prev = 1;
 NB = 61;
 TMR0 = 0;
 t0 = 0;
 INTCON.T0IE = 0;
 }
 } else if (t2 == 1) {
 Lcd_Cmd(_LCD_CLEAR);
 Lcd_Out(1, 1, "RESET SYSTEM");

  ( PORTB.RB5  = 1) ;
  ( PORTB.RB1  = 0) ;
  ( PORTB.RB2  = 0) ;
 PORTB.RB3 = 0;
 counter = 0;
 prev1 = 0;
 t2 = 0;
 t1 = 0;
 } else if (((level < 100) || (level > 1000)) && prev == 0) {
 Sound_Play(2000,1000);
 counter++;
 INTCON.T0IE = 1;
 EEPROM_Write(0x33, counter);
 } else if (((level >= 100) && (level <= 1000)) && prev1 == 0) {
 Lcd_Cmd(_LCD_CLEAR);
 Lcd_Out(2, 2, "Normal level");
  ( PORTB.RB1  = 0) ;
  ( PORTB.RB5  = 1) ;
  ( PORTB.RB2  = 0) ;
 prev = 0;
 prev1 = 1;
 PORTB.RB3 = 0;
 test = 0;
 }

 if (PORTB.RB7) {
 Lcd_Cmd(_LCD_CLEAR);
 IntToStr(counter, display);
 Lcd_Out(1, 1, "Critical Count:");
 Lcd_Out(2, 1, display);
 delay_ms(2000);
 }
 }
}
