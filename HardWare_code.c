// LCD Connections
sbit LCD_RS at RD6_bit;
sbit LCD_EN at RD7_bit;
sbit LCD_D0 at RB0_bit;
sbit LCD_D1 at RB1_bit;
sbit LCD_D2 at RB2_bit;
sbit LCD_D3 at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;
sbit LCD_RS_Direction at TRISD6_bit;
sbit LCD_EN_Direction at TRISD7_bit;
sbit LCD_D0_Direction at TRISB0_bit;
sbit LCD_D1_Direction at TRISB1_bit;
sbit LCD_D2_Direction at TRISB2_bit;
sbit LCD_D3_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;

char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

char Keypad_Scan() {
    char r, c;
    for(r = 0; r < 4; r++) {
        PORTC = 0x0F;
        PORTC &= ~(1 << r);
        Delay_us(10);
        for(c = 0; c < 4; c++) {
            if(!(PORTC & (1 << (c + 4)))) {
                Delay_ms(15);
                while(!(PORTC & (1 << (c + 4))));
                return keys[r][c];
            }
        }
    }
    return 0;
}

void LCD_PrintNum(long num) {
    char buf[12];
    char i;
    i = 0;

    if(num == 0) {
        Lcd_Chr_CP('0');
        return;
    }

    if(num < 0) {
        Lcd_Chr_CP('-');
        num = -num;
    }

    while(num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while(i > 0) {
        Lcd_Chr_CP(buf[--i]);
    }
}

void LCD_PrintResult(long num) {
    long whole;
    long frac;

    whole = num / 100;
    frac  = num % 100;

    Lcd_Out(2, 1, "                ");
    Lcd_Cmd(_LCD_SECOND_ROW);

    LCD_PrintNum(whole);

    if(frac != 0) {
        Lcd_Chr_CP('.');
        if(frac < 10) Lcd_Chr_CP('0');
        LCD_PrintNum(frac);
    }
}

void main() {
    long num1, num2, result;
    char op, entering, key, mapped, digit;

    num1 = 0; num2 = 0; result = 0;
    op = 0; entering = 0;

    TRISB = 0x00;
    TRISC = 0xF0;
    TRISD = 0x00;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1, 1, "Calculator");
    Delay_ms(1000);
    Lcd_Cmd(_LCD_CLEAR);

    while(1) {

        key = Keypad_Scan();
        if(!key) continue;

        mapped = key;
        if     (key == 'A') mapped = '+';
        else if(key == 'B') mapped = '-';
        else if(key == 'C') mapped = '*';
        else if(key == 'D') mapped = '/';
        else if(key == '#') mapped = '=';
        else if(key == '*') mapped = 'C';

        if(mapped >= '0' && mapped <= '9') {
            digit = mapped - '0';

            if(!entering) {
                num1 = num1 * 10 + digit;
                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Cmd(_LCD_FIRST_ROW);
                LCD_PrintNum(num1);
            }
            else {
                num2 = num2 * 10 + digit;
                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Cmd(_LCD_FIRST_ROW);
                LCD_PrintNum(num1);
                Lcd_Chr_CP(op);
                LCD_PrintNum(num2);
            }
        }

        else if(mapped=='+'||mapped=='-'||mapped=='*'||mapped=='/') {
            op = mapped;
            entering = 1;
            Lcd_Cmd(_LCD_CLEAR);
            Lcd_Cmd(_LCD_FIRST_ROW);
            LCD_PrintNum(num1);
            Lcd_Chr_CP(op);
        }

        else if(mapped == '=') {
            if(op == '+')
                result = (num1 + num2) * 100;
            else if(op == '-')
                result = (num1 - num2) * 100;
            else if(op == '*')
                result = (num1 * num2) * 100;
            else if(op == '/') {
                if(num2 == 0) {
                    Lcd_Cmd(_LCD_CLEAR);
                    Lcd_Out(1, 1, "DIV BY ZERO");
                    Delay_ms(1500);
                    num1 = 0; num2 = 0;
                    op = 0; entering = 0;
                    continue;
                }
                result = (num1 * 100) / num2;
            }

            Lcd_Cmd(_LCD_CLEAR);
            Lcd_Cmd(_LCD_FIRST_ROW);
            LCD_PrintNum(num1);
            Lcd_Chr_CP(op);
            LCD_PrintNum(num2);
            LCD_PrintResult(result);

            Delay_ms(2000);
            num1 = 0; num2 = 0; result = 0;
            op = 0; entering = 0;
            Lcd_Cmd(_LCD_CLEAR);
        }

        else if(mapped == 'C') {
            num1 = 0; num2 = 0; result = 0;
            op = 0; entering = 0;
            Lcd_Cmd(_LCD_CLEAR);
        }
    }
}