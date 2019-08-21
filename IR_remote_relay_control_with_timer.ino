/* Being Engineers_IR Relay Control with Timer */
/* Version 1.0 */
/* READ YELLOW_LED AND YELLOW_STATE AS RELAY1 AND RELAY1_STATE RESPECTIVELY */
/* READ BLUE_LED AND BLUE_STATE AS RELAY2 AND RELAY2_STATE RESPECTIVELY */

#include "PinChangeInt.h" // Declare Pinchange Interrupt
#include <IRremote.h>     // Declare IRremote Interrupt
#include <TimerOne.h>     // Declare TimerOne Interrupt

int RECV_PIN = 11;        // Declare IR receive pin in ardiono 

int yellow_led=13, blue_led=12;             // I was prototyping this project assuming leds as ralay input, so names are given as yellow_led and blue_led, read this as relay1 and relay2
boolean yellow_state=0, blue_state=0;       // Variable for storing the current status of each relay
long no_of_call_required=0;                 // No of call required to complete the timer
long no_of_call_done=1;                     // No of call completed
int duration=0,final_duration=0, i;         // duration for relay1 ,duration1 is for relay2
int duration1=0;
int flash;
int first_num = 0, second_num = 0;          

IRrecv irrecv(RECV_PIN);

decode_results results;

#define A 2                     //
#define B 3                     //
#define C 4                     //
#define D 5                     //  Declaring the Segments
#define E 6                     //
#define F_SEG 7                 //
#define G 8                     //

// Pins driving common anodes
#define CA1 10                  //
#define CA2 9                   //  Declaring the Digits

// Pins for A B C D E F G, in sequence
const int segs[7] = { A, B, C, D, E, F_SEG, G };

// Segments that make each number
const byte numbers[15] = { 0b1000000, 0b1111001, 0b0100100, 0b0110000, 0b0011001, 0b0010010,
0b0000010, 0b1111000, 0b0000000, 0b0010000, 0b1000111 /*   L   */, 0b1000000 /*   o   */, 0b0101011  /*   n   */, 0b0001110  /*   f   */}; // Storing the segment values for each digits and alphabets in byte form

void setup()
{
  pinMode(yellow_led,OUTPUT);       //
  pinMode(blue_led,OUTPUT);         //
  pinMode(A, OUTPUT);               //
  pinMode(B, OUTPUT);               //
  pinMode(C, OUTPUT);               //
  pinMode(D, OUTPUT);               //
  pinMode(E, OUTPUT);               //
  pinMode(F_SEG, OUTPUT);           //   PinMode setup
  pinMode(G, OUTPUT);               //
  pinMode(CA1, OUTPUT);             //
  pinMode(CA2, OUTPUT);             //
  pinMode(A0, OUTPUT);              //
  digitalWrite(yellow_led, HIGH);   //  
  digitalWrite(blue_led, HIGH);     //
  attachPinChangeInterrupt(RECV_PIN, CHECK_IR, CHANGE);   // Attahcing iterrupt in pin RECV_PIN
  irrecv.enableIRIn(); // Start the receiver              // Enable this perticular Interrupt
} 

void loop()
{
  if(duration == 0 && duration1 == 0)
  {
    digitalWrite(CA1, LOW);
    digitalWrite(CA2, LOW);
  }
  if(duration != 0)
  {
    first_num = duration / 10;                            // if duration not equal to 0, print it in 2 X 7 display
    second_num = duration % 10;
    lightDigit1(numbers[first_num]);
    delay(7);
    lightDigit2(numbers[second_num]);
    delay(7);
  }
  else if(duration1 != 0)
  {
    first_num = duration1 / 10;                           // if duration1 not equal to 0, print it in 2 X 7 display
    second_num = duration1 % 10;
    lightDigit1(numbers[first_num]);
    delay(7);
    lightDigit2(numbers[second_num]);
    delay(7);
  }
  if(flash == 1)
  {
    digitalWrite(A0, HIGH);                               // Light up the led at the start of timer
  }
  else
  {
    digitalWrite(A0, LOW);
  }
  delay(8);
}

void CHECK_IR(){                                          // Check IR receive data and match it to certain tasks.
  if(irrecv.decode(&results)){
  if (results.value==0x20DFC639)
  {
    toggle_yellow();                                      // Button1 for relay1 toggle
  }
  if (results.value==0x20DF8679)
  {
    toggle_blue();                                        // Button2 for relay2 toggle
  }
  if(results.value==0x20DF40BF)
  {
    if(duration < 99)
    {
    increase_duration();                                  //
    }                                                     //
  }                                                       //
  if(results.value==0x20DFC03F)                           // Increase/decrease value of duration
  {                                                       //
    if(duration > 0)                                      //
    {                                                     //
      decrease_duration();
    }
  }
  if(results.value==0x20DF00FF)
  {
    if(duration1 < 99)
    {
    increase_duration1();
    }                                                     //
  }                                                       //
  if(results.value==0x20DF807F)                           //
  {                                                       // Increase/decrease value of duration1
    if(duration1 > 0)                                     //
    {                                                     //
      decrease_duration1();                               //
    }                                                     
  }                                                        
  if(results.value==0x20DF827D)                           
  {                                                       //
    duration = 0;                                         //  Cancel the timer with the press of this button
    duration1 = 0;                                        //
  }
  if(results.value==0x20DF22DD)                           // If OK button pressed, start the timer
  {
    if(duration!=0)
    final_duration = duration*60;
    else if(duration1!=0)
    final_duration = duration1*60;
    no_of_call_required = final_duration;                 // Calculate the value of no_of_call_required depending upon user input
    Timer1.initialize(1000000);                           // Initilizze a timer of 1 second, every one second it will reset
    Timer1.attachInterrupt(toggle);                       // Attaching a ISR to this tomer interrupt.
    flash = 1;                                            // Changing flash value to 1, which will light up the led to denote, timer has started
  }
  irrecv.resume();                                        // After checking all the possible results, start receiving IR data again
  }
}

void toggle(void)                                         // ISR for timer1
{
  if(no_of_call_required != 0)
  no_of_call_done++;
  if(no_of_call_done == no_of_call_required && flash == 1)// Checking if no_of_call_done is equal to no_of_call_required, if yes, relay has to be toggled, if not carry with calculation and increament value of no_of_call_done by 1
  {
    //Timer1.stop();
    if(duration != 0)
    {
    digitalWrite(yellow_led, !digitalRead(yellow_led));   // Checking which relay has to toggled, relay1 or relay2
    yellow_state = !yellow_state;
    }
    else if(duration1 != 0)
    {
    digitalWrite(blue_led, !digitalRead(blue_led));       // Checking which relay has to toggled, relay1 or relay2
    blue_state = !blue_state;
    }
    digitalWrite(CA1, LOW);
    digitalWrite(CA2, LOW);
    duration=0;
    duration1=0;
    noInterrupts();
    no_of_call_done = 0;                                  // Set all variables value back to 0.
    interrupts();
    no_of_call_required = 0;
    flash = 0;
  }
}


void toggle_yellow()                                      // This function will toggle relay1
{
  digitalWrite(yellow_led, !digitalRead(yellow_led));
  yellow_state = !yellow_state;
  seven_disp_char("L1");
  if(yellow_state)
  seven_disp_char("on");
  else
  seven_disp_char("of");
}

void toggle_blue()                                        // This function will toggle relay2
{
  digitalWrite(blue_led, !digitalRead(blue_led));
  blue_state = !blue_state;
  seven_disp_char("L2");
  if(blue_state)
  seven_disp_char("on");
  else
  seven_disp_char("of");
}

void increase_duration()                                  // This function will increase the value of variable duration
{
  duration = duration + 1;
}

void decrease_duration()                                  // This function will decrease the value of variable duration
{
  duration = duration - 1;
}

void increase_duration1()                                 // This function will increase the value of variable duration1
{
  duration1 = duration1 + 1;
}

void decrease_duration1()                                 // This function will decrease the value of variable duration1
{
  duration1 = duration1 - 1;
}

void seven_disp_char(char text[])                         // This whole section deals with displaying timer value on seven segment display
{
   if(text == "L1")
   {
      for (i = 0; i < 500; i++) {
        lightDigit1(numbers[10]);
        delay(7);
        lightDigit2(numbers[1]);
        delay(7);
      }
   }
   else if(text == "on")
   {
      for (i = 0; i < 500; i++) {
        lightDigit1(numbers[11]);
        delay(7);
        lightDigit2(numbers[12]);
        delay(7);
      }  
   }
   else if(text == "L2")
   {
      for (i = 0; i < 500; i++) {
        lightDigit1(numbers[10]);
        delay(7);
        lightDigit2(numbers[2]);
        delay(7);
      } 
   }
   else if(text == "of")
   {
      for (i = 0; i < 500; i++) {
        lightDigit1(numbers[11]);
        delay(7);
        lightDigit2(numbers[13]);
        delay(7);
      } 
   }
   digitalWrite(CA1, LOW);
   digitalWrite(CA2, LOW);
}

void lightDigit1(byte number) {
  digitalWrite(CA1, HIGH);
  digitalWrite(CA2, LOW);
  lightSegments(number);
}

void lightDigit2(byte number) {
  digitalWrite(CA1, LOW);
  digitalWrite(CA2, HIGH);
  lightSegments(number);
}

void lightSegments(byte number) {
  for (int i = 0; i < 7; i++) {
    int bit = bitRead(number, i);
    digitalWrite(segs[i], bit);
  }
}
