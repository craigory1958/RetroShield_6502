

# RetroShield 6502

#### Utilities for validating the RetroShield 6502 by [*8-Bit Force*](https://8bitforce.com/)

---

[*8-Bit Force*](https://8bitforce.com/) produces carrier boards for various 8-bit microprocessors.  The microprocessor of choice mounts on the corresponding RetroShield and the RetroShield plugs into a bank of GPIO pins on the Arduino Mega 2560 board (or a Teensy board.) 
The Arduino emulates all memory, I/O, and control signals needed by the MPU/CPU. For the 6502, [*8-Bit Force*](https://8bitforce.com/) also provides sketches to emulation an Apple I, Commodore 64, and KIM 1 and uses a *LCD Display/Keypad Shield* to interact with the emulations. The utilities here do not depend on an LCD display.  Instead the LED built into the Mega2560, and an oscilloscope are used to to validate proper operation.

### Mega2560_Blink_.ino


### Mega2560_TimerInterrupts_.ino


### 6502_Mega2560-RetroShield.ino

  `6502_Mega2560-RetroShield.ino` is a simple emulation of a 6502 like embedded microcomputer.  There are 4 `C` header files to configure 4 6502 relevant memory segments at startup:

| Segment | Start | End |
| :-: | :-: | :-: |
| Zero Page | 0x0000 | 0x00FF |
| Stack | 0x0100 | 0x01FF |
| RAM | 0x0200 | 0x02FF |
| ROM | 0xFFF0 | 0xFFFF |

The test program is a simple loop coded with a single JMP instruction. RAM.h can be modified to alter the program the 6502 is executing.


```
//                   LOC   CODE         LABEL         INSTRUCTION

0x4C, 0x00, 0x02, // 0200  4C 00 02     HERE          JMP HERE
```
*RAM.h with simple JMP loop.*


```
0x00, 0x00,    // NMI vector
0x00, 0x02,    // RST vector
0x00, 0x00,    // IRQ vector
```
*ROM.h with RST vector 'hardwired' to start at $0200 upon power-up.*



<img src="_assets/images/Monitor CLK and SYNC.bmp">

*Monitoring the CLK (CH1) and SYNC (CH2) signals while executing JMP program.*


---

# RetroShield Carrier


The `RetroShield Carrier` securely holds an Arduino Mega 2560, with a RetroShield 6502, and a 630 tie-point solderless breadboard.

<img src="_assets/images/RetroShield Carrier.png">
<br/>
<img src="_assets/images/RetroShield 6502.jpg">
