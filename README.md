# Calculator - PIC16F877A

> حاسبة رقمية بسيطة تعمل على متحكم PIC16F877A مع شاشة LCD 16x2 ولوحة مفاتيح 4×4

---

## جدول المحتويات

- [نظرة عامة](#نظرة-عامة)
- [المكونات المطلوبة](#المكونات-المطلوبة)
- [مخطط التوصيل](#مخطط-التوصيل)
- [توصيل LCD](#توصيل-lcd)
- [توصيل Keypad](#توصيل-keypad)
- [توصيل PIC16F877A](#توصيل-pic16f877a)
- [خريطة أزرار الكيباد](#خريطة-أزرار-الكيباد)
- [الكود](#الكود)
- [البرمجة والتحميل](#البرمجة-والتحميل)
- [استكشاف الأخطاء](#استكشاف-الأخطاء)

---

## نظرة عامة

| البند | التفاصيل |
|-------|----------|
| المتحكم | PIC16F877A |
| تردد الكريستال | 20 MHz |
| الشاشة | LCD 16×2 — 8-bit mode |
| لوحة المفاتيح | Keypad 4×4 |
| لغة البرمجة | MikroC PRO for PIC |
| العمليات المدعومة | جمع، طرح، ضرب، قسمة |
| دقة النتيجة | خانتان عشريتان (Fixed-Point ×100) |

### المميزات

- عرض العملية كاملة على السطر الأول من الشاشة
- عرض النتيجة على السطر الثاني
- حماية من القسمة على صفر
- زر Clear لمسح العملية
- دعم الأعداد الكبيرة (long integer)

---

## المكونات المطلوبة

| المكون | الكمية | الملاحظات |
|--------|--------|-----------|
| PIC16F877A | 1 | المتحكم الرئيسي |
| LCD 16×2 | 1 | أي موديل HD44780 |
| Keypad 4×4 | 1 | ميمبرين أو ميكانيكي |
| Crystal 20MHz | 1 | لازم بالظبط |
| Capacitor 22pF | 2 | مع الكريستال |
| Capacitor 100µF | 1 | فلتر مصدر الطاقة |
| Capacitor 0.1µF | 1 | فلتر مصدر الطاقة |
| Resistor 10kΩ | 5 | 1 لـ MCLR + 4 Pull-up للـ Keypad |
| Resistor 220Ω | 1 | إضاءة الـ LCD |
| Potentiometer 10kΩ | 1 | Contrast الـ LCD |
| Voltage Regulator 7805 | 1 | لتنظيم الجهد |
| Breadboard | 1 | للتجميع |

---

## مخطط التوصيل

```
                    +5V        GND
                     │          │
              ┌──────┴──────────┴──────┐
              │      PIC16F877A        │
              │                        │
  LCD D0-D7 ──┤ RB0-RB7                │
   LCD RS   ──┤ RD6                RC0 ├──── Row 1 (Keypad)
   LCD EN   ──┤ RD7                RC1 ├──── Row 2 (Keypad)
              │                    RC2 ├──── Row 3 (Keypad)
              │                    RC3 ├──── Row 4 (Keypad)
       5V ───[10k]───────          RC4 ├──── Col 1 (Keypad)
       5V ───[10k]───────          RC5 ├──── Col 2 (Keypad)
       5V ───[10k]───────          RC6 ├──── Col 3 (Keypad)
       5V ───[10k]───────          RC7 ├──── Col 4 (Keypad)
              │                        │
   10kΩ ─── MCLR                  OSC1 ├──┐
              │                   OSC2 ├──┼── Crystal 20MHz
              │                        └──┼── 22pF × 2 → GND
              └────────────────────────┘
```

---

## توصيل LCD

```
LCD Pin   الاسم    التوصيل
───────────────────────────────────────
  1       VSS      GND
  2       VDD      5V
  3       VEE      Potentiometer 10kΩ (وسط) — لضبط الـ Contrast
  4       RS       RD6
  5       RW       GND  ← مش RD pin — وصّلها لـ GND مباشرة
  6       EN       RD7
  7       D0       RB0
  8       D1       RB1
  9       D2       RB2
  10      D3       RB3
  11      D4       RB4
  12      D5       RB5
  13      D6       RB6
  14      D7       RB7
  15      A (+)    5V عن طريق 220Ω
  16      K (-)    GND
```

### دائرة الـ Contrast

```
5V ──────────────────── طرف 1 (POT)
                        │
                    [10kΩ POT]
                        │
LCD Pin 3 (VEE) ──── وسط (POT)
                        │
GND ─────────────────── طرف 3 (POT)
```

> اضبط الـ Potentiometer لحد ما تظهر الحروف بوضوح على الشاشة.

---

## توصيل Keypad

### الأسلاك

```
Keypad Pin    النوع      PIC Pin    الاتجاه
─────────────────────────────────────────────
   1          Row 1      RC0        Output
   2          Row 2      RC1        Output
   3          Row 3      RC2        Output
   4          Row 4      RC3        Output
   5          Col 1      RC4        Input
   6          Col 2      RC5        Input
   7          Col 3      RC6        Input
   8          Col 4      RC7        Input
```

### Pull-up Resistors — مهم جداً

```
5V ──[10kΩ]──┬── RC4 (Col 1)
5V ──[10kΩ]──┼── RC5 (Col 2)
5V ──[10kΩ]──┼── RC6 (Col 3)
5V ──[10kΩ]──┴── RC7 (Col 4)
```

> **ملاحظة:** بدون Pull-up resistors على الـ Columns، الكيباد لن يستجيب أبداً على الهارد وير.

### إعداد الـ TRIS

```c
TRISC = 0xF0;   // 11110000
                // RC7-RC4 = Input  (Columns)
                // RC3-RC0 = Output (Rows)
```

---

## توصيل PIC16F877A

### الطاقة

```
VDD (Pin 11, 32) ──── 5V
VSS (Pin 12, 31) ──── GND
```

### MCLR (Reset)

```
5V ──[10kΩ]──── MCLR (Pin 1)
```

> لازم دايماً توصل MCLR لـ 5V عن طريق 10kΩ، وإلا الـ PIC مش هيشتغل.

### الكريستال

```
OSC1 (Pin 13) ──┬── طرف 1 (Crystal 20MHz)
                └── Capacitor 22pF ── GND

OSC2 (Pin 14) ──┬── طرف 2 (Crystal 20MHz)
                └── Capacitor 22pF ── GND
```

### مصدر الطاقة (7805)

```
12V DC ── IN (7805) ── [100µF] ── GND
                OUT (7805) ── 5V ── [0.1µF] ── GND
```

---

## خريطة أزرار الكيباد

```
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ A │  ← A = +  (جمع)
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ B │  ← B = -  (طرح)
├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ C │  ← C = ×  (ضرب)
├───┼───┼───┼───┤
│ * │ 0 │ # │ D │  ← D = ÷  (قسمة)
└───┴───┴───┴───┘
         ↑   ↑
         =   Clear
```

| زرار الكيباد | الوظيفة |
|-------------|---------|
| 0 - 9 | إدخال الأرقام |
| A | عملية الجمع (+) |
| B | عملية الطرح (−) |
| C | عملية الضرب (×) |
| D | عملية القسمة (÷) |
| # | يساوي (=) |
| * | مسح (Clear) |

---

## الكود

```c
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
    if(num == 0) { Lcd_Chr_CP('0'); return; }
    if(num < 0)  { Lcd_Chr_CP('-'); num = -num; }
    while(num > 0) { buf[i++] = (num % 10) + '0'; num /= 10; }
    while(i > 0)   { Lcd_Chr_CP(buf[--i]); }
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
    PORTB = 0; PORTC = 0; PORTD = 0;

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
            } else {
                num2 = num2 * 10 + digit;
                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Cmd(_LCD_FIRST_ROW);
                LCD_PrintNum(num1);
                Lcd_Chr_CP(op);
                LCD_PrintNum(num2);
            }
        }
        else if(mapped=='+'||mapped=='-'||mapped=='*'||mapped=='/') {
            op = mapped; entering = 1;
            Lcd_Cmd(_LCD_CLEAR);
            Lcd_Cmd(_LCD_FIRST_ROW);
            LCD_PrintNum(num1);
            Lcd_Chr_CP(op);
        }
        else if(mapped == '=') {
            if(op == '+')      result = (num1 + num2) * 100;
            else if(op == '-') result = (num1 - num2) * 100;
            else if(op == '*') result = (num1 * num2) * 100;
            else if(op == '/') {
                if(num2 == 0) {
                    Lcd_Cmd(_LCD_CLEAR);
                    Lcd_Out(1, 1, "DIV BY ZERO");
                    Delay_ms(1500);
                    num1 = 0; num2 = 0; op = 0; entering = 0;
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
            num1 = 0; num2 = 0; result = 0; op = 0; entering = 0;
            Lcd_Cmd(_LCD_CLEAR);
        }
        else if(mapped == 'C') {
            num1 = 0; num2 = 0; result = 0; op = 0; entering = 0;
            Lcd_Cmd(_LCD_CLEAR);
        }
    }
}
```

---

## البرمجة والتحميل

### المكتبات المطلوبة في MikroC

```
✅ Lcd
✅ Lcd_Constants
❌ كل الباقي — شيّل علامة الصح منه
```

### خطوات البناء

```
1. افتح MikroC PRO for PIC
2. Project → New Project
3. اختر: P16F877A — Clock: 20MHz
4. الصق الكود
5. Ctrl+F9 للـ Build
6. احرق الـ HEX على الـ PIC
```

### معلومات الـ HEX

```
RAM:  109 bytes (31%)
ROM:  2039 words (25%)
مناسب للنسخة التجريبية ✅
```

### أوامر Linux للبرمجة

```bash
# إيجاد الـ HEX file
find /home/$USER/.wine -name "MyProject.hex"

# نسخه للـ Desktop
cp "path/to/MyProject.hex" ~/Desktop/
```

---

## استكشاف الأخطاء

### الشاشة بتعرض مستطيلات فقط

```
السبب: الـ Contrast مش متظبط
الحل:  اضبط الـ Potentiometer على Pin 3 (VEE)
```

### الكيباد مش بيستجاوب

```
السبب: Pull-up resistors مش موجودة
الحل:  ضيف 10kΩ من RC4-RC7 لـ 5V
```

### مفيش حاجة شغالة خالص

```
تحقق من:
□ Crystal 20MHz متوصل + كابسيترين 22pF
□ MCLR متوصل لـ 5V عن طريق 10kΩ
□ VDD على 5V و VSS على GND
□ الـ HEX اتحرق صح على الـ PIC
```

### الشاشة بتعرض حروف غريبة

```
السبب: الـ PIC مش شغال بالتردد الصح
الحل:  تأكد من Crystal 20MHz وليس أي تردد تاني
```

### القسمة بتطلع نتيجة غلط

```
الكود بيستخدم Fixed-Point ×100
يعني: 7 ÷ 2 = 3.50 (صح)
      7 ÷ 3 = 2.33 (صح)
```

---

## ملاحظات مهمة

> **RW (Pin 5 في LCD):** وصّلها لـ GND مباشرة — مش لأي Pin في الـ PIC

> **Pull-up Resistors:** الأهم في المشروع — بدونها الكيباد لن يشتغل أبداً

> **Crystal:** لازم بالظبط 20MHz لأن الكود متحسوب عليه

> **MCLR:** لو متوصلتش، الـ PIC هيكون في حالة Reset دايماً

---

*تم بناء هذا المشروع وتطويره خطوة بخطوة — الكود مختبر على Proteus ومعمول له Build ناجح على MikroC PRO v7.6.0*
