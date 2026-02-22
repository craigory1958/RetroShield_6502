

# RetroShield 6502

#### Utilities for validating the RetroShield 6502 by [*8-Bit Force*](https://8bitforce.com/)

---

[*8-Bit Force*](https://8bitforce.com/) produces carrier boards for various microprocessors.  The microprocessor of choice mounts on the corresponding RetroShield and the RetroShield plugs into a bank of GPIO pins on the Arduino Mega 2560 board. 
The Arduino emulates all memory, I/O, and control signels needed by the MPU/CPU. [*8-Bit Force*](https://8bitforce.com/) provides emulations of an Apple I, Commodore 64, and KIM 1 and uses a *LCD Display/Keypad Shield* to interact with the emulations.


The `6502_Mega2560-RetroShield.ino` sketch does not use an LCD display.  It depends on an ossilisocpe to monitor key 6502 pins to validate proper operation.  The implementation here is only the most basic; a loop coded with a single JMP instruction.


RAM.h with simple JMP loop.
```
//                   LOC   CODE         LABEL         INSTRUCTION

0x4C, 0x00, 0x02, // 0200  4C 00 02     HERE          JMP HERE
```


ROM.h with RST vector hardwired to start at $0200 upon power-up.
```
0x00, 0x00,    // NMI vector
0x00, 0x02,    // RST vector
0x00, 0x00,    // IRQ vector
```


<img src="_assets/images/6502 Memory Cycle.png">


Monitoring the CLK (CH1) and SYNC (CH2) signals while executing JMP program.

<img src="_assets/images/Monitor CLK and SYNC.bmp">

---

# RetroShield Carrier


The `RetroShield Carrier` securely holds an Arduino Mega 2560, with a RetroShield 6502, and a 630 tie-point solderless breadboard.

<img src="_assets/images/RetroShield Carrier.png">
<br/>
<img src="_assets/images/RetroShield 6502.jpg">
