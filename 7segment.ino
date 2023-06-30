byte font[] = {
    0b00111111,  //  0
    0b00000110,  //  1
    0b01011011,  //  2
    0b01001111,  //  3
    0b01100110,  //  4
    0b01101101,  //  5
    0b01111101,  //  6
    0b00000111,  //  7
    0b01111111,  //  8
    0b01101111,  //  9
    0b01110111,  //  A
    0b01111100,  //  B
    0b00111001,  //  C
    0b01011110,  //  D
    0b01111001,  //  E
    0b01110001,  //  F
    0b10000000,  // dot
    0b00000000   // blank
};

const int PIN_ZERO_OUT = A1;
const int PIN_ZERO_IN = A0;
const int PIN_MODE = A2;
const int PIN_LEFT_DIGIT = A3;
const int PIN_RIGHT_DIGIT = A4;

volatile bool isTristate = true;

void setup() {
  // set port C as INPUT by default
  DDRC = 0b0000000;
  PORTC = 0b0000000;

  // set port B as INPUT
  // this is the input data
  DDRB = 0b00000000;
  PORTB = 0b11111111;

  // set port D as OUTPUT
  // this is what is driving the LEDs
  DDRD = 0b11111111;
  PORTD = 0b00000000;

  pinMode(PIN_LEFT_DIGIT, OUTPUT);
  pinMode(PIN_RIGHT_DIGIT, OUTPUT);
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_ZERO_IN, INPUT_PULLUP);
  pinMode(PIN_ZERO_OUT, OUTPUT);

  digitalWrite(PIN_RIGHT_DIGIT, HIGH);
  digitalWrite(PIN_LEFT_DIGIT, HIGH);

  isTristate = !digitalRead(PIN_MODE); // If jumper is not soldered, this is pulled up, enable zero chaining. If it is soldered, enable tristate
}

void loop() {
    int value;
    int digit;
    bool zeroIn = false;
    bool outputEnable = true;
    
    value = PINB;

    if (isTristate) {
      outputEnable = digitalRead(PIN_ZERO_IN); // Output Enable is active high (defaults to enabled with pull up)
    } else {
      zeroIn = !digitalRead(PIN_ZERO_IN); // Zero In is active low (zero chaining defaults to disabled with pull up)
      digitalWrite(PIN_ZERO_OUT, value != 0); // Write zero to enable zero chaining in the next display, if both this display's digit are off
    }

    // left digit
    digit = value >> 4;
    if (!outputEnable) { digit = 17; }  // display nothing on the left digit to signify no data
    PORTD=(zeroIn && digit == 0) ? 0 : font[digit];

    digitalWrite(PIN_LEFT_DIGIT, HIGH);
    delay(1);
    digitalWrite(PIN_LEFT_DIGIT, LOW);

    // right digit
    digit = value & 0b00001111;
    if (!outputEnable) { digit = 16; } // display only the dot on the right digit to signify no data
    PORTD=(zeroIn && value == 0) ? 0 : font[digit];

    digitalWrite(PIN_RIGHT_DIGIT, HIGH);
    delay(1);
    digitalWrite(PIN_RIGHT_DIGIT, LOW);
}
