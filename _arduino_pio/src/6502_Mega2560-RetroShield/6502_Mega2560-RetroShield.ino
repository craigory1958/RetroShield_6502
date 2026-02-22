
//
//  6502_Mega2650-RetroSheid.ino
//
//  This is a minumalist emulation of everything needed to support a 6502 CPU.  An 
//  Arduino Mega 2650 emulates the hardware, and a 6502 is mounted on a 
//  RetroSheild (www.8bitforce.com) connected to a header on the Arduino Mega.
//
//  version:
//    0.0.5   Code derived from Apple ][ example (Erturk Kocalar (8-Bit Force)) - Craig Gregory
//    1.0.0   Code debugged - Erturk Kocalar (8-Bit Force)
//    1.0.1   Final code refactor - Craig Gregory
//    1.2.0   Add silent mode in uP_tick() - Craig Gregory
//    1.2.1   Simplify memory emulation -  Craig Gregory
//    1.3.0   Monve SYNC input to D@ - Craig Gregory
//


#include <Arduino.h>

#include <PIO_Mega2560.h>
#include <PortIO.h>


// 0 - Fully manual single step (w/trace)
// 1 - Auto-step thru uP reset (w/trace)
// 2 - Fully auto-step (w/trace)
// 3 - Fully auto-step (silent)

#define SingleStepMode 2


// 0 - Halt at specified address
// 1 - Use SYNC signal from uP and detect 0xFF instruction

#define HaltMode 1
unsigned int HaltMode_Address = 0x0205;
unsigned int HaltMode_Instruction = 0xFF;


//
// 6502 Microprocessor (uP) GPIO/Pin assignments
//

/*                                  */
/*  ---Mega2560----       --6502--  */
/*                                  */

/* PinID  ARD  GPIO       LBL  PIN  */
/*   52   D52   PB1       CLK0  37  */  #define   uP_CLK0     D52
/*   50   D50   PB3       ~IRQ   4  */  #define   uP_IRQ      D50
/*   51   D51   PB2       ~NMI   6  */  #define   uP_NMI      D51
/*   38   D38   PD7       ~RST  40  */  #define   uP_RST      D38
/*   39   D39   PG2       RDY    2  */  #define   uP_RDY      D39
/*   40   D40   PG1       R/~W  34  */  #define   uP_RW       D40
/*   41   D41   PG0       ~SO   38  */  #define   uP_SOB      D41
/*    4  ~D2    PE4       SYNC   7  */  #define   uP_SYNC     D2

/* PinID  ARD  GPIO       LBL  PIN  */
/*   22   D22   PA0       A0     9  */  #define   uP_ADDR_L   PA
/*   23   D23   PA1       A1    10  */
/*   24   D24   PA2       A2    11  */
/*   25   D25   PA3       A3    12  */
/*   26   D26   PA4       A4    13  */
/*   27   D27   PA5       A5    14  */
/*   28   D28   PA6       A6    15 * /
/*   29   D29   PA7       A7    16  */

/* PinID  ARD  GPIO       LBL  PIN  */
/*   37   D37   PC0       A8    17  */  #define   uP_ADDR_H   PC
/*   36   D36   PC1       A9    18  */
/*   35   D35   PC2       A10   19  */
/*   34   D34   PC3       A11   20  */
/*   33   D33   PC4       A12   22  */
/*   32   D32   PC5       A13   23  */
/*   31   D31   PC6       A14   24  */
/*   30   D30   PC7       A15   25  */

/* PinID  ARD  GPIO       LBL  PIN  */
/*   49   D49   PL0       D0    33  */  #define   uP_DATA     PL
/*   48   D48   PL1       D1    32  */
/*   47   D47   PL2       D2    31  */
/*   46  ~D46   PL3       D3    30  */
/*   45  ~D45   PL4       D4    29  */
/*   44  ~D44   PL5       D5    28  */
/*   43   D43   PL6       D6    27  */
/*   42   D42   PL7       D7    26  */

#define   uP_ADDR()   ( (unsigned int)( pio__PortToPIN(PC) << 8 | pio__PortToPIN(PA)) )

// End uP Definition


word ADDR;
byte DATA;
byte IR;
bool RST;
bool RW;
bool SYNC;

bool done = false;
int reset = 0;


//
//
//

void setup() {

  Serial.begin(115200);

  Serial.println("Begin ...");
  dumpMem();

  uP_init();
  uP_initiate_reset();
}

void loop() {

  while (true) {

    if (IR == HaltMode_Instruction && !done) {
      done = true;

      Serial.println("End ...");
      dumpMem();
    }

    if (!done) {

      if (reset > 0) {
        reset--; // Count down number of cycles to reset
        if (reset == 0)
          pio_setPin(uP_RST);
      }

      uP_tick();
    }
  }
}


//
// Setup uP GPIO pins
//
void uP_init() {

  pio_setPortIO(uP_DATA, pio_PortAsInput);   // Set DATA bus for input to Arduino initially
  pio_setPortIO(uP_ADDR_H, pio_PortAsInput); // Set ADDR high bits for input to Arduino
  pio_setPortIO(uP_ADDR_L, pio_PortAsInput); // Set ADDR low bits for input to Arduino

  pio_setPinAsInput(uP_RW);
  pio_setPinAsOutput(uP_CLK0);
  pio_setPinAsOutput(uP_IRQ);
  pio_setPinAsOutput(uP_NMI);
  pio_setPinAsOutput(uP_RST);
  pio_setPinAsOutput(uP_RDY);
  pio_setPinAsOutput(uP_SOB);

  pio_setPin(uP_CLK0);
  pio_setPin(uP_IRQ); // Active low
  pio_setPin(uP_NMI); // Active low
  pio_setPin(uP_RST); // Active low
  pio_setPin(uP_RDY); // Active high
  pio_setPin(uP_SOB); // Active low

#if HaltMode == 1
  pio_setPinAsInput(uP_SYNC);
#endif
}


//
// uP Reset 
//
void uP_initiate_reset() {

  pio_resetPin(uP_RST); // Active low
  reset = 25;           // Reset uP for 25 cycles
}

//
// uP Clock Cycle Control Loop
//
inline __attribute__((always_inline))
void uP_tick() {

  pio_setPin(uP_CLK0); // Drive CLK high

  RST = pio_readPin(uP_RST); // Read the RST pin
  RW = pio_readPin(uP_RW);   // Read the R/W pin
  ADDR = uP_ADDR();          // Read the ADDR bus
  IR = 0x00;

#if HaltMode == 1
  SYNC = pio_readPin(uP_SYNC); // Read the SYNC pin
#endif

  if (RW) { // R/W = HIGH - read from memory transaction

    DATA = readMem(ADDR);

#if HaltMode == 1
    if (SYNC)
      IR = DATA;
#else
    if (ADDR == HaltMode_Address)
      IR = HaltMode_Instruction;
#endif

    pio_setPortIO(uP_DATA, pio_PortAsOutput); // Set DATA bus for output from Arduino to uP
    pio_writePort(uP_DATA, DATA);             // Write to DATA bus (data will be read by uP on falling edge)
  }
  else { // R/W = LOW - write to memory transaction
    DATA = pio_readPort(uP_DATA);
    writeMem(ADDR, DATA); // Read from DATA bus
  }

#if SingleStepMode != 3
  char msg[80];
  sprintf(msg, "reset: %02d,  RST: %01X  SYNC:  %01X  IR:  %02X  R/W:  %01X  ADDR:  %04X  DATA:  %02X >", reset, RST, SYNC, IR, RW, ADDR, DATA);
  Serial.println(msg);

  if (SingleStepMode == 0 || SingleStepMode == 1 && reset == 0) {
    while (!Serial.available()) {};
    Serial.read();
  }
#endif

  pio_resetPin(uP_CLK0);                   // Drive CLK low
  pio_setPortIO(uP_DATA, pio_PortAsInput); // Set DATA bus for input to Arduino from uP
}


//
// Memory Emulation
//

byte RAM[] = {
  #include "_ZP.h"
  #include "_STACK.h"
  #include "RAM.h"
};

byte ROM[] = {
  #include "ROM.h"
};

int ROMOffset = (0x10000 - sizeof(ROM)) ;


inline __attribute__((always_inline))
byte readMem(unsigned long addr) {

  if (addr < 0xF000) return RAM[addr];
  else return ROM[addr - ROMOffset];
}

inline __attribute__((always_inline))
void writeMem(word addr, byte data) {
  RAM[addr] = data;
}


//
//
//

void dumpMem() {
  dumpMemBlock((char *)"ZP", 0x0000, 0x001F);
  dumpMemBlock((char *)"STACK", 0x01E0, 0x01FF);
  dumpMemBlock((char *)"RAM", 0x0200, 0x020F);
  dumpMemBlock((char *)"ROM", 0xFFF0, 0xFFFF);
  Serial.println();
}

void dumpMemBlock(char *type, long start, long stop) {

  char s[100];

  sprintf(s, "\n%05s x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF", type);
  Serial.print(s);

  for (long b = start; b < stop + 1; b++) {

    if (b % 16 == 0) {
      sprintf(s, "\n %04X ", b);
      s[5] = 'x';
      Serial.print(s);
    }

    sprintf(s, "%02X ", readMem(b));
    Serial.print(s);
  }

  Serial.println();
}
