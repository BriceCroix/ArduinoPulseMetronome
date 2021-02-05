/********** Preprocessor options **********************************************/

/** In order for the program to print messages on serial */
#define VERBOSE


/********** Include libraries *************************************************/



/********** IO pins ***********************************************************/

/** The pin on which the vibration motor is connected */
#define PIN_VIBRATOR 2

/** The pin on which the control button is connected */
#define PIN_BUTTON 12


/********** Program constants *************************************************/

/** The number of times the user has to press the button to set pulse */
#define NB_PRESS_CALIBRATION 4

/** Duration of a pulse, in milliseconds */
#define PULSE_DURATION 50

/********** Global variables **************************************************/

/** The bpm, in milliseconds */
unsigned long int pulse_delay = 1000;

/** The offset of the pulse when compared to the core clock, in milliseconds */
unsigned long int pulse_offset = 0;


/********** Additionnal functions *********************************************/

void detectPulse()
{
#ifdef VERBOSE
    Serial.println("Now detecting");
#endif

    // Disable vibrator
    digitalWrite(PIN_VIBRATOR, LOW);

    // Wait for button to be off
    while(digitalRead(PIN_BUTTON) != HIGH);

    // Wait for user to press
    while(digitalRead(PIN_BUTTON) != LOW);
    unsigned long int first_press = millis();

    // Wait for user to press good amount of times
    for(unsigned int i = 0; i < NB_PRESS_CALIBRATION-1; i++){
        digitalWrite(PIN_VIBRATOR, HIGH);
        // Wait for button to be off
        while(digitalRead(PIN_BUTTON) != HIGH);
        digitalWrite(PIN_VIBRATOR, LOW);
        // Wait for user to press
        while(digitalRead(PIN_BUTTON) != LOW);
    }
    // Get time now
    unsigned long int time = millis();
    digitalWrite(PIN_VIBRATOR, HIGH);

    // Wait for button to be off to finally disable vibrator
    while(digitalRead(PIN_BUTTON) != HIGH);
    digitalWrite(PIN_VIBRATOR, LOW);

    // Finally average duration and set values (pressed N times so N-1 intervals)
    pulse_delay = (time - first_press)/(NB_PRESS_CALIBRATION-1);
    pulse_offset = time % pulse_delay;

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
#ifdef VERBOSE
    Serial.begin(9600);
#endif
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_VIBRATOR, OUTPUT);
}

void loop()
{
    // Check if user wants to set different pulse
    if(digitalRead(PIN_BUTTON) == LOW){
        digitalWrite(PIN_VIBRATOR, LOW);
        detectPulse();
    }

    // Vibrate at requested time
    if((millis() - pulse_offset) % pulse_delay < PULSE_DURATION){
        digitalWrite(PIN_VIBRATOR, HIGH);
    }else{
        digitalWrite(PIN_VIBRATOR, LOW);
    }
}