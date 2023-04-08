/*

Bryan Leung, ECE 231, Project 2: Temperature Sensing

In this project, I sensed the temperature by first using adc_init and get_adc to return the digital values of the thermal sensor 
in terms of DC voltage. Then, I do mathematical conversions to go from digital values to float based Farenheit and Celsius values. 

Then, by sucessfully labeling the digits on the LCD screen based on Hexidecimal notation, I then sending digits from PORTD. Finally,
I have the digits sent from PORTB to the LCD screen. To differentiate from Celsius and Farenheit values, I used a boolean that can
be reversed every time the switch is pressed.

*/


#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>  

void adc_init(void);
unsigned int get_adc(void);

#define PERSISTENCE 5
#define COUNTTIME 300

int main(void){

    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F,
    0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
    
    unsigned char ledDigitsDecimal[] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6,
    0xED, 0xFD, 0x87, 0xFF, 0xE7};
    /* _________
       |  8th  |         
  3rd  |       | 7th
       |-------|  
  4th  |  2nd  | 6th
       |_______| . 1st 
          5th      */
    ///### Example 1: 0x86 means 1000 0110. This means 1st, 6th, and 7th digit 
    ///### are activated leading to '1.' above in ledDigitsDecimal.

    ///### Example 2: 0x76 means 0111 0110. This means 2nd, 3rd, and 4th digit
    ///### are activated leading to 'X'      

    unsigned char DIG1, DIG2, DIG3, DIG4;

    DDRD = 0xFF;   /// All of PD{} channels are active. 
    DDRB = 0x1E;   /// PB4 is active from 1. Meanwhile, PB1, PB2, and PB3 are active from "E" hexidecimal.
    PORTC |= (1<<PORTC4);
    
    float tempC, tempF; 
    unsigned long int digitalValue, tempCFin, tempFFin; // Set digitalValue, tempCFin, and tempFFin from 0 to 2^16-1.
    
    adc_init();   // Activates Analog to Digital Converter. 

    while(1){
        digitalValue = get_adc();

        tempC = (digitalValue*0.11 - 50); /// Adjusted calculations to be as close to my thermometer. 
        tempCFin = (digitalValue*0.11 - 50)*10;
      
        tempFFin = (tempC*1.8 + 32)*10;

        
        char j; //Initialized j before continuing.
        
        for(j=0; j<COUNTTIME/PERSISTENCE/4; j++){
            // PORTD is assigned a digit. 
            // PORTB is where digits get uploaded to LCD depending on pin number. 
            if (!(PINC & (1<<PINC4)) == 0) { 
                // changes from Celsius to Farenheit if false
                
                 DIG4 = (tempFFin/100)%10;
                 PORTD = ledDigits[DIG4];
                 PORTB = ~ (1<<PB1);
                 _delay_ms(PERSISTENCE);

                 DIG3 = (tempFFin/10)%10;
                 PORTD = ledDigitsDecimal[DIG3];
                 PORTB = ~ (1<<PB2); 
                 _delay_ms(PERSISTENCE);

                 DIG2 = tempFFin%10;
                 PORTD = ledDigits[DIG2];
                 PORTB = ~ (1<<PB3);
                 _delay_ms(PERSISTENCE);

                 DIG1 = 0;
                 PORTD = 0x71;      // Farenheit symbol. 
                 PORTB = ~ (1<<PB4);
                 _delay_ms(PERSISTENCE);

             }
             else {
                //celsius initially when button not pressed
                DIG4 = tempCFin/100 %10;
                PORTD = ledDigits[DIG4];
                PORTB = ~ (1<<PB1);
                _delay_ms(PERSISTENCE);

                DIG3 = tempCFin/10 % 10;
                PORTD = ledDigitsDecimal[DIG3];
                PORTB = ~ (1<<PB2);
                _delay_ms(PERSISTENCE);

                DIG2 = tempCFin % 10;
                PORTD = ledDigits[DIG2];
                PORTB = ~ (1<<PB3);
                _delay_ms(PERSISTENCE);

                PORTD = 0x39;         // Celsius symbol. 
                PORTB = ~ (1<<PB4);
                _delay_ms(PERSISTENCE);

            }
        }

    }
    return 0;


}

/// Written by David McLaughlin. 
void adc_init(void){
    ADMUX = 0xC0; // Chose PC0 
    ADCSRA = 0x87; // enable ADC at 8*16+7*1=135 KHz
}

unsigned int get_adc(){
    /// Read ADC conversion. ///
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADIF)) == 0) { //wait till ADC is finished
        return ADCL | (ADCH << 8); //Reads ADCL first.
    }
}