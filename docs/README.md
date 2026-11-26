

# RetroShield 6502

#### Utilities the RetroShield 6502 by [*8-Bit Force*](https://8bitforce.com/)

---

[*8-Bit Force*](https://8bitforce.com/) produces carrier boards for various 8-bit microprocessors.  The microprocessor of choice mounts on the corresponding RetroShield and the RetroShield plugs into a bank of GPIO pins on an Arduino Mega 2560 board (or a Teensy board.)  The Arduino emulates all memory, I/O, and control signals needed by the MPU/CPU. For the 6502, [*8-Bit Force*](https://8bitforce.com/) also provides sketches to emulation an Apple I, Commodore 64, and KIM 1 and uses a *LCD Display/Keypad Shield* to interact with the emulations. 

The utilities here do not depend on an LCD display.  Instead they use the built in LED of the Mega2560, and an oscilloscope to validate proper operation.

Ultimatly this project is to test software and hardare ideas for a multi-tasking operating system running on virtual memory hardware.


### Mega2560_Blink

This is the demo sketch packaged with the Arduino IDE.  It simple tests the development environment, and basic operation of the Mega2560.  It blinks the built in LED.


### 6502_Mega2560-RetroShield

`6502_Mega2560-RetroShield` is a simple emulation for an attached RetroShield 6502.  There are 4 `C` header files to configure 4 relevant memory segments of the 6502:

| Segment | Start | End | File |
| :-: | :-: | :-: | :-- |
| Zero Page | 0x0000 | 0x00FF | _ZP.h |
| Stack | 0x0100 | 0x01FF | _STACK.c |
| RAM | 0x0200 | 0x02FF | RAM.h |
| ROM | 0xFFF0 | 0xFFFF | ROM.h |


Testing is accomplished by executing a simple loop program consiting of a single JMP instruction and monitroring the CLK0 and SYNC signals of the 6502 under test.

```
                  // LOC   CODE         LABEL         INSTRUCTION

0x4C, 0x00, 0x02, // 0200  4C 00 02     HERE          JMP HERE
```
*RAM.h with simple JMP loop.*


RST vector wired to start execution to 0x0200 at power-up.  

```
0x00, 0x00,    // NMI vector
0x00, 0x02,    // RST vector
0x00, 0x00,    // IRQ vector
```
*ROM.h with vectors used upon power-up.*


Notes about the emulation (taken from the oscilloscope screen capture below):

* the W65C02 is operating at 272KHz
* the duty cycle of CLK0 is 61% (not symmetrical)
* the SYNC signal appears tied to the falling edge of CLK0
* the number of clock pulses per SYNC matches the documented number of cycles of the JMP instruction. 

<br/>

<img src="_assets/images/Monitor CLK and SYNC.bmp">

*Monitoring the CLK0 (CH1) and SYNC (CH2) signals while executing test program.*


#### Theory of Operation

<img src="_assets/images/W65C02 Clock Cycle.png">


### Mega2560_TimerInterrupts


---

# RetroShield Carrier


The `RetroShield Carrier` securely holds an Arduino Mega 2560, with a RetroShield 6502, and a 630 tie-point solderless breadboard.

<img src="_assets/images/RetroShield Carrier.png">
<br/>
<img src="_assets/images/RetroShield 6502.jpg">


---

# FNIRSI 2C53P

The FNIRSI 2C53P is used to monitor and validate RetroShield 6502 experiments.

<img src="_assets/images/FNIRSI 2C43P.png">

<br/>

Key Specifications

* Oscilloscope. Features 2 channels with a 50MHz analog bandwidth, 250MSa/s real-time sampling rate, 8-bit vertical resolution, and ±400V maximum measurement voltage
* Multimeter. Offers a 4.5-digit (19,999-count) True RMS meter supporting AC/DC voltage, AC/DC current, resistance, capacitance, diode, continuity, and temperature measurements
* DDS Signal Generator. Outputs 12 different waveforms (including sine, square, and sawtooth) up to 10MHz for sine waves and 5MHz for other waveforms, with adjustable amplitude (0.1V–3.0V) and duty cycle
* Display & Battery. Equipped with a 4.3-inch tempered glass IPS touchscreen (480×272 resolution) and a built-in 4,000mAh lithium battery charged via USB-C.
