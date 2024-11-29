// Pin definitions
const int irEmitterPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};  // Pins for 9 TSAL6100 IR emitters
const int ledPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30};
const int irReceiverPin = 11;       // Pin for TSOP38238 IR receiver output (digital)
const int emitterCount = 9;        // Total number of IR emitters
const byte SER_PIN = 53;    // Pin connected to DS (SER) of 74HC595
const byte SRCLK_PIN = 52;  // Pin connected to SH_CP (SRCLK) of 74HC595
bool selectedEmitters[emitterCount] = {false};

void setup() {
  pinMode(SER_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  // Set IR emitter pins as outputs
  for (int i = 0; i < emitterCount; i++) {
    pinMode(irEmitterPins[i], OUTPUT);
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(irReceiverPin, INPUT);   // Set IR receiver pin as input
  Serial.begin(9600);              // Serial for debugging
  shiftOut(SER_PIN, SRCLK_PIN, MSBFIRST, 0x00);
}

void loop() {
  // Loop through each IR emitter
  for (int i = 0; i < emitterCount; i++) {
    if (selectedEmitters[i]) continue;
    
    // Emit 38kHz signal from the current emitter
    tone(irEmitterPins[i], 38000);      
    delay(50);                       // Allow time for signal to stabilize

    // Check if the receiver detects the IR signal from the current emitter
    int receiverState = digitalRead(irReceiverPin);

    int ii = 0;
    while((receiverState == LOW) && (ii < 9)) {
      Serial.print("Pointing at Emitter ");
      Serial.println(i + 1);        // Print which emitter is detected (1-indexed)
      digitalWrite(ledPins[i], HIGH);
      digitalWrite(SRCLK_PIN, LOW);  // Start shifting
      digitalWrite(SER_PIN, HIGH);   // Send 1 (LED on)
      shiftClock();                   // Shift the clock
      ii++;
      delay(200);
      tone(irEmitterPins[i], 38000);      
      delay(50);                       // Allow time for signal to stabilize
      receiverState = digitalRead(irReceiverPin);
    }
    // Turn LEDs off one by one from 8 to 1
    for (byte i = 0; i < 9; i++) {
      digitalWrite(SRCLK_PIN, LOW);  // Start shifting
      digitalWrite(SER_PIN, LOW);    // Send 0 (LED off)
      shiftClock();                   // Shift the clock
    }
    
    if (ii == 9){
      selectedEmitters[i] = true;
      //printing stuff that is kinda pointless
      Serial.print(i + 1);
      if(i==0){
        Serial.print("st");}
      else if(i==1){
        Serial.print("nd");}
      else{
        Serial.print("th");}
      
      Serial.println(" floor selected");
      delay(1000);
    }
    else{
      digitalWrite(ledPins[i], LOW);
    }
    
    

    // Turn off the current emitter and add a quiet period
    noTone(irEmitterPins[i]);
    delay(50);                      // Additional delay for quiet period
  }
}

// Function to shift the clock signal
void shiftClock() {
  digitalWrite(SRCLK_PIN, HIGH);  // Trigger clock to shift the data
  digitalWrite(SRCLK_PIN, LOW);   // Reset clock
}
