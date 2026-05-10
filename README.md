# Calculator - PIC16F877A
### مشروع حاسبة رقمية — شرح أكاديمي شامل

> حاسبة رقمية تعمل على متحكم PIC16F877A مع شاشة LCD 16x2 ولوحة مفاتيح 4×4  
> الشرح مبني على محاضرات مادة Real-Time Embedded Systems (RTES)

---

## جدول المحتويات

1. [ما هو الـ Embedded System؟](#1-ما-هو-الـ-embedded-system)
2. [ليه اخترنا PIC16F877A؟](#2-ليه-اخترنا-pic16f877a)
3. [معمارية الـ PIC16F877A](#3-معمارية-الـ-pic16f877a)
4. [الذاكرة في المشروع](#4-الذاكرة-في-المشروع)
5. [الـ I/O Ports وليه اخترنا كل بورت](#5-الـ-io-ports-وليه-اخترنا-كل-بورت)
6. [الـ TRIS Registers — التحكم في اتجاه البيانات](#6-الـ-tris-registers--التحكم-في-اتجاه-البيانات)
7. [مصدر الطاقة والكريستال](#7-مصدر-الطاقة-والكريستال)
8. [توصيل LCD بالتفصيل](#8-توصيل-lcd-بالتفصيل)
9. [توصيل Keypad بالتفصيل](#9-توصيل-keypad-بالتفصيل)
10. [شرح الكود سطر بسطر](#10-شرح-الكود-سطر-بسطر)
11. [طريقة الـ Fixed-Point وليه اخترناها](#11-طريقة-الـ-fixed-point-وليه-اخترناها)
12. [Polling vs Interrupts في المشروع](#12-polling-vs-interrupts-في-المشروع)
13. [مخطط التوصيل الكامل](#13-مخطط-التوصيل-الكامل)
14. [استكشاف الأخطاء](#14-استكشاف-الأخطاء)

---

## 1. ما هو الـ Embedded System؟

**تعريف من المحاضرات:**
> "An embedded system is a combination of computer hardware and software, and perhaps additional mechanical or other parts, designed to perform a specific function."

**مشروعنا كـ Embedded System:**

الحاسبة بتاعتنا هي embedded system لأنها:
- مبنية من **hardware** (PIC + LCD + Keypad + مقاومات + كريستال)
- و**software** (الكود المكتوب بـ MikroC)
- مصممة لتؤدي **وظيفة واحدة محددة** وهي العمليات الحسابية

**الفرق بين مشروعنا والكمبيوتر العادي:**

| الحاسبة (Embedded) | الكمبيوتر العادي (General Purpose) |
|---------------------|--------------------------------------|
| وظيفة واحدة محددة | يؤدي أي وظيفة |
| PIC16F877A = متحكم بسيط | معالج معقد |
| موارد محدودة (RAM = 368 byte) | موارد ضخمة (GB من الـ RAM) |
| تكلفة منخفضة | تكلفة مرتفعة |
| لا يحتاج مستخدم | يحتاج مستخدم |

**من المحاضرة:** "If an embedded system is designed well, the existence of the processor and software could be completely unnoticed by a user of the device."

---

## 2. ليه اخترنا PIC16F877A؟

**من المحاضرات:**
> "Why PIC16F877A is very popular? This is because PIC16F877A is very cheap. Apart from that it is also very easy to be assembled. Additional components that you need to make this IC work is just a 5V power supply adapter, a 20MHz crystal oscillator and 2 units of 22pF capacitors."

### مميزات PIC16F877A في مشروعنا

| الميزة | القيمة | استخدامنا |
|--------|--------|-----------|
| MAX Frequency | 20 MHz | استخدمنا 20MHz بالظبط |
| Flash Program Memory | 8K words | الكود بتاعنا = 2039 words (25%) |
| Data RAM | 368 bytes | استخدمنا 109 bytes (31%) |
| I/O Ports | PORTA-PORTE | استخدمنا PORTB + PORTC + PORTD |
| Pins | 40 Pin DIP | سهل التوصيل على Breadboard |
| قابلية البرمجة | 10,000 مرة | جربنا الكود أكتر من مرة |

### مميزات PIC بشكل عام (من المحاضرة)

PIC هو **RISC processor** يعني:
- عدد تعليمات قليل (35 تعليمة بس)
- كل تعليمة بتتنفذ في cycle واحد (ما عدا الـ branches)
- سرعة عالية مع بساطة في التصميم
- معمارية **Harvard** = ذاكرة البرنامج والبيانات منفصلين

### عيوب PIC16F877A

> "This IC has no internal oscillator so you will need an external crystal or other clock source."

ده اللي خلانا نحتاج Crystal خارجي + كابسيترين 22pF.

### ليه ما اخترناش microcontroller تاني؟

| المتحكم | السبب |
|---------|-------|
| Arduino UNO | أغلى + أكبر + مش embedded بشكل خالص |
| PIC16F84A | قليل الـ I/O ports (مش كافي للـ LCD + Keypad) |
| PIC18F | أغلى + مش محتاجين قوته لمشروع بسيط |
| **PIC16F877A** | ✅ الأنسب: رخيص + كافي + شائع |

---

## 3. معمارية الـ PIC16F877A

### Harvard Architecture

**من المحاضرة:**
> "The instructions and data have different memory spaces with separate address, data, and control buses for each memory space. Advantages: instruction and data fetches can occur concurrently."

مشروعنا بيستفيد من ده لأن:
- الـ Program Memory (كود الحاسبة) منفصل عن الـ Data Memory (المتغيرات)
- الـ CPU يقدر يجيب تعليمة ويقرأ بيانات في نفس الوقت = أسرع

### RISC Architecture

**من المحاضرة:** PIC هو RISC:
- 35 تعليمة بس
- كل تعليمة = 14-bit word
- Pipeline: بيجيب التعليمة الجاية وهو بينفذ الحالية

```
Instruction Pipeline في مشروعنا:
Cycle 1: Fetch(LCD_Init)     | Execute(nothing)
Cycle 2: Fetch(Lcd_Cmd)      | Execute(LCD_Init)
Cycle 3: Fetch(Lcd_Out)      | Execute(Lcd_Cmd)
```

### STATUS Register

**من المحاضرة:** الـ STATUS register بيحتوي على:
- **Z (Zero bit):** بيتساوى 1 لو نتيجة العملية = صفر
- **C (Carry bit):** بيتساوى 1 لو فيه حمل في الجمع
- **RP0, RP1:** بيختار الـ Bank في الـ RAM

**في مشروعنا:** MikroC بيتعامل مع الـ STATUS تلقائياً، بس لما بنعمل:
```c
TRISB = 0x00;   // ده بيكتب في Bank 1
PORTB = 0;      // ده بيكتب في Bank 0
```
الـ Compiler بيضيف كود لتغيير الـ Bank تلقائياً.

---

## 4. الذاكرة في المشروع

**من المحاضرة:** PIC16F877A عنده 3 أنواع ذاكرة:

### 1. Program Memory (Flash)
> "Flash memory — used for storing compiled code (user's program). Program Memory capacity is 8K × 14 bit."

```
مشروعنا:
- الحجم المتاح: 8192 words
- اللي استخدمناه: 2039 words (25%)
- المتبقي: 6153 words (75%)
```

الكود بتاعنا بعد الـ Compile بيتحول لـ HEX file ويتحرق في الـ Flash دي.

### 2. Data Memory (RAM)
> "Memory storage for variables. Data Memory consists of two components: General purpose register file (same as RAM) and Special purpose register file."

```
مشروعنا:
- الحجم المتاح: 368 bytes
- اللي استخدمناه: 109 bytes (31%)
- المتبقي: 243 bytes (69%)

متغيراتنا في الـ RAM:
long num1   = 4 bytes (الرقم الأول)
long num2   = 4 bytes (الرقم التاني)
long result = 4 bytes (النتيجة)
char op     = 1 byte  (العملية)
char buf[12]= 12 bytes (buffer للطباعة)
```

### 3. EEPROM
> "A memory that allows storing the variables as a result of burning the written program. Used for holding system parameters to be retained during power-off."

في مشروعنا **ما استخدمناش الـ EEPROM** لأن الحاسبة مش محتاجة تحتفظ بأي بيانات لما تتقفل.

### ليه استخدمنا `long` بدل `int`؟

```c
long num1, num2, result;  // 4 bytes كل واحدة
```

لأن `int` في MikroC = 16-bit = max قيمة 32767. لو ضربنا رقمين كبار النتيجة هتـ overflow. الـ `long` = 32-bit = max قيمة ~2 مليار.

---

## 5. الـ I/O Ports وليه اخترنا كل بورت

**من المحاضرة:**
> "PIC16F877 has 5 I/O ports: PORT A has 6 bit wide, PORT B, C, D have 8 bit wide, PORT E has 3 bit wide, all Bidirectional."

### توزيع البورتات في مشروعنا

```
PORTA  (RA0-RA5): غير مستخدم — ممكن يستخدم للـ ADC
PORTB  (RB0-RB7): LCD Data (D0-D7) — 8 pins
PORTC  (RC0-RC7): Keypad (4 Rows + 4 Cols) — 8 pins
PORTD  (RD0-RD7): LCD Control (RS + EN) — 2 pins فقط
PORTE  (RE0-RE2): غير مستخدم
```

---

### PORTB للـ LCD — ليه؟

**السبب:**
- PORTB هو الوحيد اللي عنده **8 pins كاملة متتالية** بسهولة على الـ DIP40
- الـ LCD في 8-bit mode محتاج 8 data pins متتالية
- PORTB (RB0-RB7) = Pins 33-40 في الـ DIP40 = متتالية ومتقاربة

**لو وصلنا الـ LCD على بورت تاني:**

| البورت | المشكلة |
|--------|---------|
| PORTA | بس 6 pins مش 8 |
| PORTC | هنحتاجه كله للـ Keypad |
| PORTD | ممكن، بس هنستخدم RD6 و RD7 للـ RS و EN |
| PORTE | بس 3 pins مش كافية |

```c
// في الكود:
TRISB = 0x00;  // كل PORTB = Output (للـ LCD)
PORTB = 0;     // Initialize
```

---

### PORTC للـ Keypad — ليه؟

**السبب:**
- الـ Keypad محتاج 8 pins (4 Rows + 4 Cols)
- PORTC عنده 8 pins
- PORTC بيسمح بـ **mixed direction** — بعض الـ pins Input وبعضها Output في نفس الوقت

```c
TRISC = 0xF0;  // 11110000
               // RC7-RC4 = Input  (Columns يقرأ منهم)
               // RC3-RC0 = Output (Rows يكتب فيهم)
```

**لو وصلنا الـ Keypad على بورت تاني:**

| البورت | المشكلة |
|--------|---------|
| PORTB | محجوز للـ LCD |
| PORTD | محجوز RD6/RD7 للـ LCD Control |
| PORTA | بس 6 pins مش 8 |

---

### PORTD للـ LCD Control — ليه؟

بنستخدم **RD6 (RS) و RD7 (EN) بس** من PORTD.

**السبب:**
- RD6 و RD7 هما آخر 2 pins في PORTD
- في الـ DIP40 هما Pins 27 و 28 = قريبين من بعض ومن الـ LCD
- الـ Pins الباقية من PORTD (RD0-RD5) ممكن تتستخدم لأي حاجة تانية في المستقبل

**ليه ما استخدمناش PORTA للـ RS و EN؟**
- PORTA فيها pins مشتركة مع الـ ADC — لو فعلنا ADC مستقبلاً هيبوظ
- PORTD أنظف وأكثر مرونة

---

## 6. الـ TRIS Registers — التحكم في اتجاه البيانات

**من المحاضرة:**
> "TRISx sets whether each pin is an input(1) or output(0). PORTx sets their output bit levels or contain their input bit levels."

### القاعدة

```
TRIS bit = 1 → Pin = INPUT  (بيقرأ)
TRIS bit = 0 → Pin = OUTPUT (بيكتب)
```

### في مشروعنا

```c
TRISB = 0x00;  // 00000000 = كل PORTB Output (LCD Data)
TRISC = 0xF0;  // 11110000 = RC7-RC4 Input, RC3-RC0 Output
TRISD = 0x00;  // 00000000 = كل PORTD Output (LCD Control)
```

**شرح TRISC = 0xF0 بالتفصيل:**

```
0xF0 = 1111 0000
       ↑↑↑↑ ↑↑↑↑
       ||||  ||||
       RC7  RC3
       RC6  RC2
       RC5  RC1
       RC4  RC0

RC7-RC4 = 1 = Input  ← الـ Columns بنقرأ منهم
RC3-RC0 = 0 = Output ← الـ Rows بنكتب فيهم
```

**ليه الـ Columns = Input والـ Rows = Output؟**

لأن طريقة فحص الـ Keypad (Row Scanning) بتشتغل كده:
1. بنحط **0** في Row واحدة (Output)
2. بنقرأ الـ Columns (Input) — لو زرار متضغط هيقرأ 0

```c
// من الكود:
PORTC = 0x0F;           // كل الـ Rows = 1 (High)
PORTC &= ~(1 << r);    // الـ Row المحددة = 0 (Low)
// نقرأ الـ Columns ونشوف أي واحد بقى 0
```

---

## 7. مصدر الطاقة والكريستال

### الكريستال 20MHz

**من المحاضرة:**
> "Clock from the oscillator enters a microcontroller via OSC1 pin where internal circuit divides the clock into four even clocks Q1, Q2, Q3 and Q4 which make up one instruction cycle."

```
Crystal 20 MHz
÷ 4 (Q1+Q2+Q3+Q4)
= 5 MIPS (Million Instructions Per Second)
```

يعني الحاسبة بتنفذ **5 مليون تعليمة في الثانية**.

**ليه 20MHz بالذات؟**
- ده الـ Maximum Operating Frequency لـ PIC16F877A
- أعلى تردد = أسرع استجابة للـ Keypad
- ليه ما اخترناش أقل؟ — لأن الـ Delay_ms في الكود محسوب على 20MHz

**الكابسيترين 22pF:**
```
OSC1 ──┬── Crystal ──┬── OSC2
       │              │
     [22pF]         [22pF]
       │              │
      GND            GND
```

دول بيستقروا تردد الكريستال ويمنعوا الضوضاء.

**من المحاضرة:**
> "Crystals in particular are sensitive to poor PCB layout. It is important to exclude parasitic resistance by having very short tracks, locating the crystal close to the body of the microcontroller."

على الـ Breadboard خلينا الكريستال قريب من الـ PIC قدر الإمكان.

---

### مصدر الطاقة (7805)

**من المحاضرة:**
> "Traditionally, much logic circuitry is supplied at 5V, arising from the voltage specified for the TTL family."

**الدائرة:**
```
بطارية 9V → [7805] → 5V منظم

7805:
- IN  = بطارية (+9V)
- GND = بطارية (-)
- OUT = 5V ثابت

كابسيتر 100µF: يمتص تذبذبات الجهد الكبيرة
كابسيتر 0.1µF: يمتص الضوضاء عالية التردد
```

**ليه ما وصلناش البطارية مباشرة؟**

البطارية 9V لو وصلناها مباشرة للـ PIC (المصمم يشتغل على 5V):
- ممكن تحرق الـ PIC
- الجهد مش منظم وبيتغير مع الحمل

---

### MCLR (Master Clear Reset)

```c
// التوصيل:
5V ──[10kΩ]──── MCLR (Pin 1)
```

**ليه محتاج مقاومة 10kΩ؟**
- لو MCLR اتوصل على 0V مباشرة = الـ PIC في Reset دايماً
- لو MCLR في الهواء (Floating) = قيمة عشوائية = غير مستقر
- المقاومة 10kΩ بتضمن إن MCLR = 5V (High = تشغيل)

**ليه 10kΩ تحديداً؟**
- كبيرة بما يكفي إنها ما تسحبش تيار كتير من الـ 5V
- صغيرة بما يكفي إنها تعمل على MCLR = High بشكل مضمون

---

## 8. توصيل LCD بالتفصيل

**LCD 16x2 = 16 عمود × 2 صف**

### جدول التوصيل الكامل

```
Pin LCD   الاسم     التوصيل           السبب
────────────────────────────────────────────────────────
  1       VSS       GND               أرضي المكون
  2       VDD       5V                طاقة الـ LCD
  3       VEE       وسط POT 10kΩ      Contrast الشاشة
  4       RS        RD6 (Pin 27)      Register Select
  5       RW        GND               Read/Write
  6       EN        RD7 (Pin 28)      Enable
  7       D0        RB0 (Pin 33)      Data Bit 0
  8       D1        RB1 (Pin 34)      Data Bit 1
  9       D2        RB2 (Pin 35)      Data Bit 2
  10      D3        RB3 (Pin 36)      Data Bit 3
  11      D4        RB4 (Pin 37)      Data Bit 4
  12      D5        RB5 (Pin 38)      Data Bit 5
  13      D6        RB6 (Pin 39)      Data Bit 6
  14      D7        RB7 (Pin 40)      Data Bit 7
  15      A (+)     5V عن طريق 220Ω  إضاءة الـ Backlight
  16      K (-)     GND               أرضي الإضاءة
```

### شرح كل Pin

**RS (Register Select) على RD6:**
- RS = 0 → الـ LCD يستقبل **أوامر** (Command مثل Clear أو Cursor)
- RS = 1 → الـ LCD يستقبل **بيانات** (Data مثل حروف وأرقام)

**RW (Read/Write) على GND:**
- RW = 0 → **Write** (نكتب للـ LCD)
- RW = 1 → **Read** (نقرأ من الـ LCD)
- وصلناه على GND لأننا بنكتب بس، مش بنقرأ → توفير Pin في الـ PIC

**EN (Enable) على RD7:**
- لما EN ينتقل من 1 إلى 0 = الـ LCD بيستقبل الأمر أو البيانات
- الـ LCD Library في MikroC بتتحكم فيه تلقائياً

**VEE (Contrast) على POT:**
```
5V ──── طرف 1 POT
        ↓
    [10kΩ POT] ← اضبطه لحد ما تظهر الحروف
        ↓
LCD Pin 3 (VEE) ← التوصيل هنا
        ↓
GND ──── طرف 3 POT
```
- لو VEE = 5V → مفيش Contrast → الشاشة بيضاء
- لو VEE = 0V → Contrast عالي جداً → مستطيلات
- الصح = وسط حاجة بين 0 و 5V

**مقاومة 220Ω للـ Backlight:**
```
5V ──[220Ω]── Pin 15 (A)
              Pin 16 (K) ── GND
```
المقاومة لتحديد التيار المار في الـ LED الخاص بالإضاءة (بدونها ممكن تحرق).

### 8-bit Mode ليه؟

في مشروعنا استخدمنا **8-bit mode** (D0-D7 كلهم):

| 8-bit Mode | 4-bit Mode |
|------------|------------|
| 8 Data Pins | 4 Data Pins (D4-D7 بس) |
| بيانات كاملة في مرة واحدة | بيانات في خطوتين |
| أسرع | أبطأ شوية |
| يحتاج 10 pins في الـ PIC | يحتاج 6 pins في الـ PIC |

اخترنا 8-bit لأننا عندنا pins كافية ولأنه أبسط في الكود.

---

## 9. توصيل Keypad بالتفصيل

### Keypad 4×4 = Matrix Keypad

الـ Keypad مش 16 زرار منفصل — ده Matrix:
- 4 Rows (صفوف)
- 4 Columns (أعمدة)
- مجموع الأسلاك = 8 بدل 16

```
       Col1  Col2  Col3  Col4
        │     │     │     │
Row1 ───┼─[1]─┼─[2]─┼─[3]─┼─[A]
        │     │     │     │
Row2 ───┼─[4]─┼─[5]─┼─[6]─┼─[B]
        │     │     │     │
Row3 ───┼─[7]─┼─[8]─┼─[9]─┼─[C]
        │     │     │     │
Row4 ───┼─[*]─┼─[0]─┼─[#]─┼─[D]
```

### جدول التوصيل

```
Keypad Pin    الوظيفة    PIC Pin        اتجاه      مقاومة
──────────────────────────────────────────────────────────
   1          Row 1      RC0 (Pin 11)   Output     لا
   2          Row 2      RC1 (Pin 12)   Output     لا
   3          Row 3      RC2 (Pin 13)   Output     لا
   4          Row 4      RC3 (Pin 14)   Output     لا
   5          Col 1      RC4 (Pin 15)   Input      10kΩ → 5V
   6          Col 2      RC5 (Pin 16)   Input      10kΩ → 5V
   7          Col 3      RC6 (Pin 17)   Input      10kΩ → 5V
   8          Col 4      RC7 (Pin 18)   Input      10kΩ → 5V
```

### Pull-up Resistors — ليه ضرورية؟

**المشكلة:** لما مفيش زرار متضغط، الـ Column Pin في الهواء (Floating).
الـ Floating Pin بيقرأ قيم عشوائية (0 أو 1) → الـ PIC بيفتكر إن زرار اتضغط.

**الحل:**
```
5V ──[10kΩ]──── RC4 (Col 1)

لما مفيش زرار: RC4 = 5V (High = 1) ← قرأ إن مفيش ضغط ✅
لما زرار يتضغط: RC4 اتوصل بـ GND عن طريق الـ Row
                 RC4 = 0V (Low = 0) ← قرأ إن فيه ضغط ✅
```

**لو ما حطناش Pull-up:**
```
مفيش زرار: RC4 = ؟؟؟ (Floating) ← أرقام عشوائية ❌
```

**ده بالظبط اللي حصل في مشروعنا:** لما ما حطناش المقاومات، الشاشة كانت بتعرض أرقام عشوائية لوحدها.

### طريقة فحص الـ Keypad (Row Scanning)

**من المحاضرة:** المحاضرة اتكلمت عن Polling:
> "The first is busy waiting or polling, where the processor continuously checks the device's status register until the device is ready."

مشروعنا بيستخدم **Polling** لفحص الـ Keypad:

```c
char Keypad_Scan() {
    char r, c;
    for(r = 0; r < 4; r++) {       // جرب كل Row
        PORTC = 0x0F;               // كل Rows = High
        PORTC &= ~(1 << r);        // الـ Row دي = Low

        Delay_us(10);               // استنى تستقر

        for(c = 0; c < 4; c++) {   // افحص كل Column
            if(!(PORTC & (1 << (c + 4)))) {  // لو Column = Low
                Delay_ms(15);       // Debounce
                while(!(PORTC & (1 << (c + 4)))); // استنى يترفع
                return keys[r][c];  // رجع الزرار
            }
        }
    }
    return 0;  // مفيش زرار
}
```

**شرح خطوة بخطوة:**

```
مثال: المستخدم ضغط زرار "5"

1. r=0: PORTC = 0x0E (Row1 = Low)
   فحص Cols → مفيش استجابة (5 مش في Row1)

2. r=1: PORTC = 0x0D (Row2 = Low)
   فحص Col2 → RC5 بقى Low! (5 في Row2, Col2)

3. Debounce 15ms → تأكيد الضغطة مش ارتداد

4. رجع keys[1][1] = '5'
```

**Debounce ليه؟**

لما زرار ميكانيكي يتضغط، بيعمل Bounce (اهتزاز سريع) لمدة ~10ms:
```
الضغطة الحقيقية: 0 0 1 0 1 1 1 1 1 1  ← Bounce في الأول
بعد Debounce:    0 0 0 0 0 0 0 0 0 0  ← استنينا 15ms
```

### Keypad Mapping

```c
mapped = key;
if     (key == 'A') mapped = '+';  // جمع
else if(key == 'B') mapped = '-';  // طرح
else if(key == 'C') mapped = '*';  // ضرب
else if(key == 'D') mapped = '/';  // قسمة
else if(key == '#') mapped = '=';  // يساوي
else if(key == '*') mapped = 'C';  // مسح
```

**ليه استخدمنا متغير `mapped` منفصل؟**
- لو غيرنا `key` مباشرة، لو `C` اتحول لـ `*`، الـ `*` بعدين ممكن يتحول لـ `C` تاني = تضارب
- `mapped` بيحل المشكلة دي بشكل نظيف

---

## 10. شرح الكود سطر بسطر

### تعريف الـ LCD Pins

```c
sbit LCD_RS at RD6_bit;
```

`sbit` = Special Bit declaration في MikroC  
ده بيقول: "المتغير LCD_RS هو نفس الـ Bit 6 في PORTD"  
يعني لما نقول `LCD_RS = 1` = نفس `RD6 = 1`

```c
sbit LCD_RS_Direction at TRISD6_bit;
```
ده بيحدد اتجاه الـ Pin (Input/Output) — الـ LCD Library بتستخدمه تلقائياً.

### دالة LCD_PrintNum

```c
void LCD_PrintNum(long num) {
    char buf[12];
    char i;
    i = 0;

    if(num == 0) { Lcd_Chr_CP('0'); return; }  // حالة خاصة للصفر

    if(num < 0) { Lcd_Chr_CP('-'); num = -num; }  // علامة السالب

    // حول الرقم لحروف (معكوس)
    while(num > 0) {
        buf[i++] = (num % 10) + '0';  // آخر رقم
        num /= 10;                      // شيل آخر رقم
    }

    // اطبع الحروف بترتيب صح
    while(i > 0) {
        Lcd_Chr_CP(buf[--i]);
    }
}
```

**مثال: num = 123**
```
Step 1: buf[0] = 3, num = 12
Step 2: buf[1] = 2, num = 1
Step 3: buf[2] = 1, num = 0
الطباعة: buf[2]='1', buf[1]='2', buf[0]='3' = "123" ✅
```

**ليه ما استخدمناش `IntToStr` أو `LongToStr`؟**
- دي Functions موجودة في الـ Conversions Library
- لما أضفناها كانت الكود بيكبر ويتعدى الـ Demo Limit في MikroC
- كتبنا دالة خاصة بينا أصغر وأسرع وبتشتغل من غير libraries إضافية

### دالة LCD_PrintResult

```c
void LCD_PrintResult(long num) {
    long whole;
    long frac;
    whole = num / 100;   // الجزء الصحيح
    frac  = num % 100;   // الجزء العشري (× 100)

    Lcd_Out(2, 1, "                ");  // امسح السطر الثاني
    Lcd_Cmd(_LCD_SECOND_ROW);           // روح السطر الثاني

    LCD_PrintNum(whole);   // اطبع الجزء الصحيح

    if(frac != 0) {        // لو فيه كسر
        Lcd_Chr_CP('.');
        if(frac < 10) Lcd_Chr_CP('0');  // صفر قبل الكسر لو أقل من 10
        LCD_PrintNum(frac);
    }
}
```

**مثال: num = 350 (يعني النتيجة الحقيقية = 3.50)**
```
whole = 350 / 100 = 3
frac  = 350 % 100 = 50
الطباعة: "3.50" ✅
```

### Main Function

```c
void main() {
    long num1, num2, result;
    char op, entering, key, mapped, digit;

    // تهيئة القيم
    num1 = 0; num2 = 0; result = 0;
    op = 0; entering = 0;
```

**ليه عرّفنا المتغيرات في الأول وبعدين حطينا القيم؟**  
MikroC يستخدم C89 standard — لا يسمح بتعريف متغير مع قيمة إلا في بداية الـ function.

```c
    TRISB = 0x00;  // PORTB كله Output (LCD Data)
    TRISC = 0xF0;  // RC7-4 Input (Cols), RC3-0 Output (Rows)
    TRISD = 0x00;  // PORTD كله Output (LCD Control)
```

```c
    while(1) {  // حلقة لانهائية — العلامة الأساسية للـ Embedded System
        key = Keypad_Scan();
        if(!key) continue;  // لو مفيش ضغطة، كمّل
```

**ليه `while(1)`؟**  
الـ Embedded System مش بيوقف أبداً — بيشتغل طالما فيه طاقة. الحاسبة لازم تفضل شغالة دايماً.

---

## 11. طريقة الـ Fixed-Point وليه اخترناها

### المشكلة الأصلية

الأرقام الكسرية (3.14, 7.5) محتاجة `float` في C:
```c
float result = 7.0 / 2.0;  // = 3.5
```

لكن `float` في MikroC Demo:
- بتزود حجم الكود بشكل كبير (float library ضخمة)
- الكود كان بيتعدى الـ 2048 words = Demo Limit

### الحل: Fixed-Point × 100

بدل ما نخزن 3.5، بنخزن **350** (مضروبة في 100):

```
نتيجة حقيقية:  3.50
Fixed-Point:   350 (نوع long = كفاءة عالية)

عند الطباعة:
350 / 100 = 3    (الجزء الصحيح)
350 % 100 = 50   (الجزء العشري)
الناتج: "3.50"
```

### العمليات في Fixed-Point

```c
// جمع: (3 + 2) = 5 → × 100 = 500
result = (num1 + num2) * 100;

// طرح: (5 - 2) = 3 → × 100 = 300
result = (num1 - num2) * 100;

// ضرب: (3 × 2) = 6 → × 100 = 600
result = (num1 * num2) * 100;

// قسمة: (7 ÷ 2) = 3.5 → × 100 = 350
result = (num1 * 100) / num2;
//        ↑
//     نضرب أولاً قبل القسمة عشان نحافظ على الكسر
```

**ليه نضرب قبل القسمة في حالة الـ Division؟**
```
لو قسمنا الأول:
7 / 2 = 3 (integer division, الكسر اتضاع)
3 × 100 = 300 (غلط!)

لو ضربنا الأول:
7 × 100 = 700
700 / 2 = 350 (صح! = 3.50)
```

### مميزات Fixed-Point

| Fixed-Point | Float |
|-------------|-------|
| كود أصغر | كود أكبر |
| أسرع (integer ops) | أبطأ (FPU needed) |
| دقة 2 خانة عشرية | دقة 6-7 خانات |
| مناسب للـ Demo Limit | يتعدى الـ Demo Limit |

---

## 12. Polling vs Interrupts في المشروع

### Polling (ما استخدمناه)

**من المحاضرة:**
> "The first is busy waiting or polling, where the processor continuously checks the device's status register until the device is ready. This wastes the processor's time but is the simplest to implement."

في مشروعنا:
```c
while(1) {
    key = Keypad_Scan();  // ← الـ PIC بيفحص الـ Keypad باستمرار
    if(!key) continue;    // لو مفيش زرار، يكمل يفحص
    // ...
}
```

الـ PIC بيكون مشغول 100% بفحص الـ Keypad — ده Polling.

### Interrupts (البديل)

**من المحاضرة:**
> "Interrupts free the processor from having to continuously check the I/O devices. The processor may continue with other tasks."

لو استخدمنا Interrupts:
- الـ PIC يعمل حاجات تانية
- لما زرار يتضغط، الـ Interrupt بيوقف الـ PIC ويعالج الضغطة

### ليه اخترنا Polling وموش Interrupts؟

| Polling | Interrupts |
|---------|-----------|
| أبسط في الكود | أعقد |
| الـ PIC مشغول دايماً | الـ PIC فاضي معظم الوقت |
| مناسب لمشروع بسيط | مناسب لمشاريع معقدة |
| لا يحتاج تعريف ISR | يحتاج Interrupt Service Routine |

في مشروع الحاسبة البسيط، مش محتاجين الـ PIC يعمل حاجة تانية → Polling أبسط وكافي.



---

## 13. مخطط التوصيل الكامل
                         PIC16F877A (DIP40)
                   ┌─────────────────────────┐
      MCLR ─[10kΩ]─┤1  MCLR           RB7 40 ├─── LCD D7
               5V  │                  RB6 39 ├─── LCD D6
              RA0  ┤2                 RB5 38 ├─── LCD D5
              RA1  ┤3                 RB4 37 ├─── LCD D4
              RA2  ┤4                 RB3 36 ├─── LCD D3
              RA3  ┤5                 RB2 35 ├─── LCD D2
              RA4  ┤6                 RB1 34 ├─── LCD D1
              RA5  ┤7                 RB0 33 ├─── LCD D0
              VSS  ┤8 GND             VDD 32 ├─── 5V
             OSC1  ┤9 ──[Crystal]──  OSC2 31 ┤
                   │   20MHz+22pF×2       30 │RA7
             RC0   ┤11 → Row1        VSS 31  ├─── GND
             RC1   ┤12 → Row2        VDD 20  ├─── 5V
             RC2   ┤13 → Row3        RD0 21  │
             RC3   ┤14 → Row4        RD1 22  │
    [10k]─5V─RC4   ┤15 ← Col1        RD2 23  │
    [10k]─5V─RC5   ┤16 ← Col2        RD3 24  │
    [10k]─5V─RC6   ┤17 ← Col3        RD4 25  │
    [10k]─5V─RC7   ┤18 ← Col4        RD5 26  │
              VSS  ┤19 GND       RD6 27      ├─── LCD RS
              VDD  ┤20 5V        RD7 28      ├─── LCD EN
                   └─────────────────────────┘


LCD 16x2:
Pin1(VSS)→GND | Pin2(VDD)→5V | Pin3(VEE)→POT وسط
Pin4(RS)→RD6  | Pin5(RW)→GND | Pin6(EN)→RD7
Pin7-14(D0-D7)→RB0-RB7
Pin15(A)→[220Ω]→5V | Pin16(K)→GND


Keypad 4×4:
Pin1(Row1)→RC0 | Pin2(Row2)→RC1
Pin3(Row3)→RC2 | Pin4(Row4)→RC3
Pin5(Col1)→RC4→[10kΩ]→5V
Pin6(Col2)→RC5→[10kΩ]→5V
Pin7(Col3)→RC6→[10kΩ]→5V
Pin8(Col4)→RC7→[10kΩ]→5V


Power Supply:
9V Battery → IN[7805]OUT → 5V
[100µF] و [0.1µF] بين OUT وGND
```

```text
                                    PIC16F877A - PIN MAPPING
                
                        ┌─────────────────────────────────────────────┐
                MCLR/VPP  1 ── Reset (10K to +5V)          RB7/PGD 40 ── LCD D7
                RA0/AN0   2                                RB6/PGC 39 ── LCD D6
                RA1/AN1   3                                     RB5 38 ── LCD D5
                RA2/AN2   4                                     RB4 37 ── LCD D4
                RA3/AN3   5                                     RB3 36 ── LCD D3
                RA4/T0CKI 6                                     RB2 35 ── LCD D2
                RA5/AN4   7                                     RB1 34 ── LCD D1
                RE0/AN5   8                                     RB0 33 ── LCD D0
                RE1/AN6   9                                     VDD 32 ── +5V
                RE2/AN7  10                                     VSS 31 ── GND
                VDD       11 ── +5V                              RD7 30 ── LCD E
                VSS       12 ── GND                              RD6 29 ── LCD RS
                OSC1/CLKI 13 ── 20MHz Crystal                    RD5 28
                OSC2/CLKO 14 ── 20MHz Crystal                    RD4 27
                RC0/T1OSO 15 ── Keypad Row 1                RC7/RX 26 ── Keypad Column 4
                RC1/T1OSI 16 ── Keypad Row 2                RC6/TX 25 ── Keypad Column 3
                RC2/CCP1  17 ── Keypad Row 3               RC5/SDO 24 ── Keypad Column 2
                RC3/SCK   18 ── Keypad Row 4               RC4/SDI 23 ── Keypad Column 1
                RD0       19                                     RD3 22
                RD1       20                                     RD2 21
                        └─────────────────────────────────────────────┘
```

---

## 14. استكشاف الأخطاء

### الشاشة بتعرض مستطيلات فقط

```
السبب:  الـ Contrast مش متظبط (VEE = 0V)
الحل:   اضبط الـ Potentiometer حتى تظهر الحروف
```

### الكيباد بيكتب أرقام عشوائية لوحده

```
السبب:  مفيش Pull-up Resistors على الـ Columns
        الـ Pins في الهواء (Floating) بيقرأ قيم عشوائية
الحل:   وصّل 10kΩ من كل Column (RC4-RC7) لـ 5V
```

### صف معين في الـ Keypad مش بيستجاوب

```
السبب:  وصلة الـ Row مقطوعة أو ضعيفة
الحل:   افحص التوصيل بالملتيميتر على Continuity
        Row1 = RC0 (Pin 11), Row2 = RC1 (Pin 12)
        Row3 = RC2 (Pin 13), Row4 = RC3 (Pin 14)
```

### مفيش حاجة شغالة خالص

```
تحقق بالترتيب:
1. قِس الجهد: 5V بين VDD و VSS
2. MCLR متوصل لـ 5V عن طريق 10kΩ
3. Crystal 20MHz متوصل + كابسيترين 22pF
4. الـ HEX اتحرق صح (مش فاضي)
```

### الكود اشتغل على Proteus بس مش على الهارد وير

```
السبب المحتمل:
- LVP (Low Voltage Programming) كان Enabled
  → بيحجز RB3 = LCD D3 مش شغال
الحل:
  Project → Edit Project
  Low-Voltage Programming → Disabled
  عمل Build تاني وحرق الـ HEX الجديد
```

---

## ملاحظات مهمة للمناقشة

### أسئلة متوقعة وإجاباتها

**س: ليه استخدمت PIC وموش Arduino؟**  
ج: PIC هو embedded microcontroller بمعنى الكلمة — hardware محدد، software محدد، وظيفة واحدة. Arduino أعقد وأغلى وده embedded system أنقى.

**س: إيه الفرق بين PORTB و TRISD؟**  
ج: PORTB بيكتب أو يقرأ القيم الفعلية على الـ Pins. TRISB بيحدد الاتجاه — هل الـ Pin Input أو Output.

**س: ليه الـ Keypad محتاج Pull-up Resistors؟**  
ج: لأن الـ Column Pins لو في الهواء (Floating) بتقرأ قيم عشوائية. الـ Pull-up بيضمن إن القيمة تبقى High (1) لما مفيش زرار متضغط.

**س: إيه الفرق بين Polling والـ Interrupt في فحص الـ Keypad؟**  
ج: Polling = الـ PIC بيفحص باستمرار في loop. Interrupt = الـ PIC بيشتغل في حاجة تانية ولما زرار يتضغط يوقف ويعالجه. استخدمنا Polling لبساطته.

**س: ليه Crystal 20MHz تحديداً؟**  
ج: ده الـ Maximum للـ PIC16F877A = أعلى أداء. الـ Delay functions في MikroC محسوبة عليه.

**س: إيه هو الـ Harvard Architecture وإزاي بيفيد مشروعنا؟**  
ج: ذاكرة البرنامج والبيانات منفصلتين. الـ CPU يقدر يجيب تعليمة ويقرأ بيانات في نفس الوقت = أسرع.

**س: ليه استخدمت Fixed-Point بدل Float؟**  
ج: Float Library كبيرة كانت بتتعدى الـ Demo Limit في MikroC. Fixed-Point × 100 بيعطي دقة خانتين عشريتين من غير استخدام float.

---

*تم بناء هذا المشروع خطوة بخطوة — الكود مختبر على Proteus ومشتغل على الهارد وير الفعلي*  
*المعلومات الأكاديمية مستخرجة من محاضرات RTES — Dr. Rania F. Ahmed*
