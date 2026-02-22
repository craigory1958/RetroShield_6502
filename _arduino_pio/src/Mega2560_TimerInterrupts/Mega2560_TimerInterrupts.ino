
#include <Arduino.h>

// COMPA, CTC or OVR
#define TIMER_MODE "CTC"

// 1Hz, 10Hz, 1KHz or 1Mhz
#define TIMER_FREQ "1KHz"


#define TIMER_PRESCALER_1 ((0 << CS12) | (0 << CS11) | (1 << CS10))
#define TIMER_PRESCALER_8 ((0 << CS12) | (1 << CS11) | (0 << CS10))
#define TIMER_PRESCALER_64 ((0 << CS12) | (1 << CS11) | (1 << CS10))
#define TIMER_PRESCALER_256 ((1 << CS12) | (0 << CS11) | (0 << CS10))
#define TIMER_PRESCALER_1024 ((1 << CS12) | (0 << CS11) | (1 << CS10))


int cnt = 0;
bool ledOnOff;
String mode;
char msg[50];
int timer_start;


void setup() {

  Serial.begin(115200);

  cli();  // Disable interrupts

  TCCR1A = 0x00;
  TCCR1B = 0x00;

  if (TIMER_MODE == "COMPA")
    configureCompareTimer();

  if (TIMER_MODE == "CTC")
    configureClearOnCompareTimer();

  if (TIMER_MODE == "OVR")
    configureOverflowTimer();

  sei();  // Enable interrupts

  pinMode(LED_BUILTIN, OUTPUT);
}


void configureCompareTimer() {

  if (TIMER_FREQ == "1Hz") {
    TCCR1B |= TIMER_PRESCALER_1024;  // set pre-scalar of 1024
    OCR1A = 20000;
  }
  if (TIMER_FREQ == "10Hz") {}
  if (TIMER_FREQ == "1KHz") {}
  if (TIMER_FREQ == "1MHz") {}

  TIMSK1 |= (1 << OCIE1A);  // Set compare mode
  TCNT1 = 0;                // Initialize 1st count

  mode = "C";
}


void configureClearOnCompareTimer() {

  if (TIMER_FREQ == "1Hz") {
    TCCR1B |= TIMER_PRESCALER_1024;
    OCR1A = 15624;
  }
  if (TIMER_FREQ == "10Hz") {
    TCCR1B |= TIMER_PRESCALER_1024;
    OCR1A = 1561;
  }
  if (TIMER_FREQ == "1KHz") {
    TCCR1B |= TIMER_PRESCALER_256;
    OCR1A = 62;
  }
  if (TIMER_FREQ == "1MHz") {
    TCCR1B |= TIMER_PRESCALER_1;
    OCR1A = 15;
  }

  TIMSK1 |= (1 << OCIE1A);  // Set compare mode
  TCCR1B |= (1 << WGM12);   // Set clear on match compare
  TCNT1 = 0;                // Initialize 1st count

  mode = "c";
}

void configureOverflowTimer() {

  if (TIMER_FREQ == "1Hz") {
    TCCR1B |= TIMER_PRESCALER_256;  // set pre-scalar of 512
    timer_start = 3036;
  }

  if (TIMER_FREQ == "10Hz") {}
  if (TIMER_FREQ == "1KHz") {}
  if (TIMER_FREQ == "1MHz") {}

  TIMSK1 |= (1 << TOIE1);  // Set overflow mode
  TCNT1 = timer_start;     // Initialize 1st count

  mode = "O";
}


void loop() {}


ISR(TIMER1_COMPA_vect) {

  if (TIMER_MODE == "COMPA")
    TCNT1 = 0;  // Reset counter start value (not needed in "CTC" mode)

  processCount();
}


ISR(TIMER1_OVF_vect) {

  TCNT1 = timer_start;  // Set counter start value for next count
  processCount();
}

void processCount() {

  if (cnt % 1000 == 0) {
    if (cnt % 10000 == 0) {
      sprintf(msg, "\n (%10d) >", millis());
      Serial.print(msg);
    }
    Serial.print(mode);
  }

  cnt++;
  
  ledOnOff = !ledOnOff;
  digitalWrite(LED_BUILTIN, ledOnOff);
}
