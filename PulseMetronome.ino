/********** Preprocessor options **********************************************/

/** In order for the program to print messages on serial */
//#define VERBOSE


/********** Include libraries *************************************************/

/* LowPower library for less energy consumption */
#include "LowPower.h"

/* in order to use macro function to enable/disable modules */
#include <avr/power.h>


/********** IO pins ***********************************************************/

/** The pin on which the vibration motor is connected */
#define PIN_PULSE 2

/** The pin on which the control button is connected */
#define PIN_BUTTON 12


/********** Program constants *************************************************/

/** The number of times the user has to press the button to set pulse */
#define NB_PRESS_CALIBRATION 4

/** Duration of a pulse, in milliseconds */
#define PULSE_DURATION 60

/********** Global variables **************************************************/

/** The bpm, in milliseconds */
unsigned long int pulse_delay = 1000;

/** The offset of the pulse when compared to the core clock, in milliseconds */
unsigned long int pulse_offset = 0;

/** The duration of idle mode between two pulses */
period_t pulse_sleep_duration = SLEEP_500MS;

/********** Additionnal functions *********************************************/

void detectPulse()
{
#ifdef VERBOSE
    Serial.println("Now detecting");
#endif

    // Disable vibrator
    digitalWrite(PIN_PULSE, LOW);

    // Wait for button to be off
    while(digitalRead(PIN_BUTTON) != HIGH);

    // Wait for user to press
    while(digitalRead(PIN_BUTTON) != LOW);
    unsigned long int first_press = millis();

    // Wait for user to press good amount of times
    for(unsigned int i = 0; i < NB_PRESS_CALIBRATION-1; i++){
        digitalWrite(PIN_PULSE, HIGH);
        // Wait for button to be off
        while(digitalRead(PIN_BUTTON) != HIGH);
        digitalWrite(PIN_PULSE, LOW);
        // Wait for user to press
        while(digitalRead(PIN_BUTTON) != LOW);
    }
    // Get time now
    unsigned long int time = millis();
    digitalWrite(PIN_PULSE, HIGH);

    // Wait for button to be off to finally disable vibrator
    while(digitalRead(PIN_BUTTON) != HIGH);
    digitalWrite(PIN_PULSE, LOW);

    // Finally average duration and set values (pressed N times so N-1 intervals)
    pulse_delay = (time - first_press)/(NB_PRESS_CALIBRATION-1);
    pulse_offset = time % pulse_delay;
    if(pulse_delay - PULSE_DURATION < 15){
        pulse_sleep_duration = SLEEP_15MS;
    }else if(pulse_delay - PULSE_DURATION < 30){
        pulse_sleep_duration = SLEEP_30MS;
    }else if(pulse_delay - PULSE_DURATION < 60){
        pulse_sleep_duration = SLEEP_60MS;
    }else if(pulse_delay - PULSE_DURATION < 120){
        pulse_sleep_duration = SLEEP_120MS;
    }else if(pulse_delay - PULSE_DURATION < 250){
        pulse_sleep_duration = SLEEP_250MS;
    }else if(pulse_delay - PULSE_DURATION < 500){
        pulse_sleep_duration = SLEEP_500MS;
    }else{
        pulse_sleep_duration = SLEEP_1S;
    }

#ifdef VERBOSE
    // Inform user
    Serial.print("Set to ");
    Serial.print(60000.0f/pulse_delay);
    Serial.println(" bpm");
#endif
}

/********** Arduino setup and loop ********************************************/

void setup()
{
    // disable ADC
    ADCSRA = 0;
    // turn off all modules
    power_all_disable ();
    // Except timer 0
    power_timer0_enable();
#ifdef VERBOSE
    Serial.begin(9600);
#endif
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_PULSE, OUTPUT);
}

void loop()
{
    // Wait for the pulse to happen
    while((millis() - pulse_offset) % pulse_delay > PULSE_DURATION);
    digitalWrite(PIN_PULSE, HIGH);
    // Sleep before turning off pulse
    LowPower.idle(SLEEP_60MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, SPI_OFF, USART0_OFF, TWI_OFF);
    digitalWrite(PIN_PULSE, LOW);

    // Check if user wants to set different pulse
    if(digitalRead(PIN_BUTTON) == LOW){
        detectPulse();
    }

    // Put everything on sleep except timer 0, used for millis() function
    LowPower.idle(pulse_sleep_duration, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, SPI_OFF, USART0_OFF, TWI_OFF);
}
