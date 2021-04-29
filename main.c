#include "global.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UART_BUFFER_SIZE        20

#define REFRESH_RATE            1                               //Hz
#define TIMER_CLOCK             16000000/1024                   //prescaled timer clock of timer 1
#define TIMER_TICKS_PER_PERIOD  (TIMER_CLOCK/REFRESH_RATE)
#define ADC_VREF_VALUE          1.1                             //1.1 V Bandgap Reference

volatile uint8_t send_temperature_uart = 1;
char gl_char_uart_buffer[UART_BUFFER_SIZE];
int gl_int_temp_offset = -50;

//DECLARATION of Function Headers
//--------------------------------------------------------------

void UART_initDevice();
void UART_printTemperature( double temperature );
void UART_printChar( char c );
void UART_printText( char *s );
void UART_clearBuffer();

double readInternalTemperature(void);

//DECALARTION of Interrupt Service Routines
//--------------------------------------------------------------

//ISR of ADC Converson Complete Interrupt
//read the ADC Value from the selecte ADC-channel
ISR (ADC_vect) 
{
    float temperature_degc = readInternalTemperature();
    
    if (send_temperature_uart)
        UART_printTemperature(temperature_degc);
}

//ISR of Timer 1 Compare Match
//Trigger a new ADC conversion
ISR (TIMER1_COMPA_vect) 
{
    //Trigger a new ADC-Conversions
    ADCSRA |= (0x01  << ADSC);
}  


//DECLARATION of local functions
//--------------------------------------------------------------

double readInternalTemperature(void)
{
    uint16_t adc_rawvalue = 0;
    double adc_voltage = 0.0;
    double k = 0.0, offset = 0.0;
    double temperature = 0.0;


    adc_rawvalue = ADCL;
    adc_rawvalue += (ADCH << 8);

    //adc_rawvalue = ADC;

    //See datasheet chapter 24.7 - ADC Conversion Result
    adc_voltage = adc_rawvalue * ADC_VREF_VALUE / 1024.0;

    //See datasheet chaper 24.8 - Temperature Measurement
    if (adc_voltage < 0.314)
    {
        k = ((25.0+45.0)/(0.314-0.242));
        offset = -45 -k*0.242;
        temperature = adc_voltage * k + offset + gl_int_temp_offset;
    }
    else
    {
        k = ((85.0-25.0)/(0.380-0.314));
        offset = 25 - k*0.314;
        temperature = adc_voltage * k + offset + gl_int_temp_offset;
    }
    return temperature;
}

void UART_initDevice()
{
    UBRR0H = 0;
    UBRR0L = 34;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0A = (1 << U2X0);
}

void UART_printTemperature( double temperature )
{
    //Use sprintf to convert the floating point number to a text
    //TODO
    //Textvariable deklarieren: 
    char text[20] = "";
    //sprintf(text, "%2.2f\n", temperature);
    sprintf(text, "%2d\n", (int) temperature);
    UART_printText(text);
}

void UART_printChar( char c )
{
    while ( !( UCSR0A & (1 << UDRE0)) )
        ;
    UDR0 = c;
}

void UART_printText( char *s)
{
    for (uint8_t i = 0; s[i] != 0; i++)
    {
        UART_printChar(s[i]);
    }
}

char UART_getChar()
{
    if ( UCSR0A & (1 << RXC0) )
        return UDR0;
    else
        return '\0';
}

void UART_receiveString()
{
    uint8_t char_counter = 0;
    UART_clearBuffer();
    
    do
    {
        if(gl_char_uart_buffer[char_counter] != '\0')   char_counter++;
        if(char_counter == UART_BUFFER_SIZE)            char_counter=0;
        gl_char_uart_buffer[char_counter] = UART_getChar();
    }while(gl_char_uart_buffer[char_counter] != '\n');
}

void UART_clearBuffer()
{
    for(uint8_t i = 0; i < UART_BUFFER_SIZE; i++) gl_char_uart_buffer[i] = '\0';
}


//MAIN-Programm
//--------------------------------------------------------------

int main ()
{   
    cli();

    //UART SETTINGS
    UART_initDevice();
    
    //TIMER 1 - SETTINGS
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10); //Prescaler 1024

    OCR1AH = (TIMER_TICKS_PER_PERIOD >> 8);
    OCR1AL = TIMER_TICKS_PER_PERIOD & 0x00FF;
    //OCR1A = TIMER_TICKS_PER_PERIOD;

    TIMSK1 |= _BV(OCIE1A); 
    TIMSK1 |= 0x02;


    //ADC SETTINGS
    //Setup MUX, Reference, Speed, ...
    
    //MUX auf den Temperatureingang einstellen
    ADMUX |= (0x01 << MUX3);
    //Interne Bandgap Referenz auswählen
    ADMUX |= (0x01 << REFS1) | (0x01 << REFS0);
    //Einstellung des Prescaler auf den Wert 128 -> Langsamste Geschwindigkeit
    ADCSRA |= (0x01 << ADPS2) | (0x01 << ADPS1) | (0x01 << ADPS0);
    //Freigabe des ADCs Ineterrupts.
    //Es wird automatisch die ISR (Interrupt Service Routine) am Ende der Konvertierung aufgerufen
    ADCSRA |= (0x01 << ADIE);

    //Aktivierung der ADC Hardware
    ADCSRA |= (0x01 << ADEN);

    sei();

    UART_printText ("Excercise 12 - Temperature measurement\n");
    UART_printText ("--------------------------------------\n\n");
    UART_printText ("Press key 'b' or 's' to start/stop the measurement\n");

    while (1)
    {

        //receive a string via UART and save it in gl_char_uart_buffer
        UART_receiveString();
         
        //Pruefung mehrere Buchstaben
        /*
        char vergleich[]="begin";
        for(int i = 0; i < begin[i] != '\0';++i)
        {
            if (gl_char_uart_buffer[i] != begin[i])
            {
                //ungleich
                break;
            }
        }
        */


        switch (gl_char_uart_buffer[0])
        {
            case 'b':
                //Variable zum Senden setzen
                send_temperature_uart = 1;
                break;
            case 's':
                //Variable zum Beenden der Uebertragung loeschen
                send_temperature_uart = 0;
                break;
            case 'o':
                //Befehlsaufbau: commando argument
                //z.B. zur Änderung des Temperaturoff#include "global.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UART_BUFFER_SIZE        20

#define REFRESH_RATE            1                               //Hz
#define TIMER_CLOCK             16000000/1024                   //prescaled timer clock of timer 1
#define TIMER_TICKS_PER_PERIOD  (TIMER_CLOCK/REFRESH_RATE)
#define ADC_VREF_VALUE          1.1                             //1.1 V Bandgap Reference

volatile uint8_t send_temperature_uart = 1;
char gl_char_uart_buffer[UART_BUFFER_SIZE];
int gl_int_temp_offset = -50;

//DECLARATION of Function Headers
//--------------------------------------------------------------

void UART_initDevice();
void UART_printTemperature( double temperature );
void UART_printChar( char c );
void UART_printText( char *s );
void UART_clearBuffer();

double readInternalTemperature(void);

//DECALARTION of Interrupt Service Routines
//--------------------------------------------------------------

//ISR of ADC Converson Complete Interrupt
//read the ADC Value from the selecte ADC-channel
ISR (ADC_vect) 
{
    float temperature_degc = readInternalTemperature();
    
    if (send_temperature_uart)
        UART_printTemperature(temperature_degc);
}

//ISR of Timer 1 Compare Match
//Trigger a new ADC conversion
ISR (TIMER1_COMPA_vect) 
{
    //Trigger a new ADC-Conversions
    ADCSRA |= (0x01  << ADSC);
}  


//DECLARATION of local functions
//--------------------------------------------------------------

double readInternalTemperature(void)
{
    uint16_t adc_rawvalue = 0;
    double adc_voltage = 0.0;
    double k = 0.0, offset = 0.0;
    double temperature = 0.0;


    adc_rawvalue = ADCL;
    adc_rawvalue += (ADCH << 8);

    //adc_rawvalue = ADC;

    //See datasheet chapter 24.7 - ADC Conversion Result
    adc_voltage = adc_rawvalue * ADC_VREF_VALUE / 1024.0;

    //See datasheet chaper 24.8 - Temperature Measurement
    if (adc_voltage < 0.314)
    {
        k = ((25.0+45.0)/(0.314-0.242));
        offset = -45 -k*0.242;
        temperature = adc_voltage * k + offset + gl_int_temp_offset;
    }
    else
    {
        k = ((85.0-25.0)/(0.380-0.314));
        offset = 25 - k*0.314;
        temperature = adc_voltage * k + offset + gl_int_temp_offset;
    }
    return temperature;
}

void UART_initDevice()
{
    UBRR0H = 0;
    UBRR0L = 34;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0A = (1 << U2X0);
}

void UART_printTemperature( double temperature )
{
    //Use sprintf to convert the floating point number to a text
    //TODO
    //Textvariable deklarieren: 
    char text[20] = "";
    //sprintf(text, "%2.2f\n", temperature);
    sprintf(text, "%2d\n", (int) temperature);
    UART_printText(text);
}

void UART_printChar( char c )
{
    while ( !( UCSR0A & (1 << UDRE0)) )
        ;
    UDR0 = c;
}

void UART_printText( char *s)
{
    for (uint8_t i = 0; s[i] != 0; i++)
    {
        UART_printChar(s[i]);
    }
}

char UART_getChar()
{
    if ( UCSR0A & (1 << RXC0) )
        return UDR0;
    else
        return '\0';
}

void UART_receiveString()
{
    uint8_t char_counter = 0;
    UART_clearBuffer();
    
    do
    {
        if(gl_char_uart_buffer[char_counter] != '\0')   char_counter++;
        if(char_counter == UART_BUFFER_SIZE)            char_counter=0;
        gl_char_uart_buffer[char_counter] = UART_getChar();
    }while(gl_char_uart_buffer[char_counter] != '\n');
}

void UART_clearBuffer()
{
    for(uint8_t i = 0; i < UART_BUFFER_SIZE; i++) gl_char_uart_buffer[i] = '\0';
}


//MAIN-Programm
//--------------------------------------------------------------

int main ()
{   
    cli();

    //UART SETTINGS
    UART_initDevice();
    
    //TIMER 1 - SETTINGS
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10); //Prescaler 1024

    OCR1AH = (TIMER_TICKS_PER_PERIOD >> 8);
    OCR1AL = TIMER_TICKS_PER_PERIOD & 0x00FF;
    //OCR1A = TIMER_TICKS_PER_PERIOD;

    TIMSK1 |= _BV(OCIE1A); 
    TIMSK1 |= 0x02;


    //ADC SETTINGS
    //Setup MUX, Reference, Speed, ...
    
    //MUX auf den Temperatureingang einstellen
    ADMUX |= (0x01 << MUX3);
    //Interne Bandgap Referenz auswählen
    ADMUX |= (0x01 << REFS1) | (0x01 << REFS0);
    //Einstellung des Prescaler auf den Wert 128 -> Langsamste Geschwindigkeit
    ADCSRA |= (0x01 << ADPS2) | (0x01 << ADPS1) | (0x01 << ADPS0);
    //Freigabe des ADCs Ineterrupts.
    //Es wird automatisch die ISR (Interrupt Service Routine) am Ende der Konvertierung aufgerufen
    ADCSRA |= (0x01 << ADIE);

    //Aktivierung der ADC Hardware
    ADCSRA |= (0x01 << ADEN);

    sei();

    UART_printText ("Excercise 12 - Temperature measurement\n");
    UART_printText ("--------------------------------------\n\n");
    UART_printText ("Press key 'b' or 's' to start/stop the measurement\n");

    while (1)
    {

        //receive a string via UART and save it in gl_char_uart_buffer
        UART_receiveString();
         
        //Pruefung mehrere Buchstaben
        /*
        char vergleich[]="begin";
        for(int i = 0; i < begin[i] != '\0';++i)
        {
            if (gl_char_uart_buffer[i] != begin[i])
            {
                //ungleich
                break;
            }
        }
        */


        switch (gl_char_uart_buffer[0])
        {
            case 'b':
                //Variable zum Senden setzen
                send_temperature_uart = 1;
                break;
            case 's':
                //Variable zum Beenden der Uebertragung loeschen
                send_temperature_uart = 0;
                break;
            case 'o':
                //Befehlsaufbau: commando argument
                //z.B. zur Änderung des Temperaturoffset: o <Temperaturoffset>
                //o 20
                if (strchr(gl_char_uart_buffer, ' ') != NULL)
                {
                    //Leerzeichen wurde gefunden weil eine gueltige Adresse (nungleich NULL)
                    //zurueck geliefert wurde
                    gl_int_temp_offset = atoi(gl_char_uart_buffer+2);
                }
                break;
            case 'O':
                sprintf(gl_char_uart_buffer, "T-Offset: %d\n", gl_int_temp_offset);
                UART_printText(gl_char_uart_buffer);
                break;
            default:
                UART_printText("Unknown Command!\n");
                break;
            //Define a start and stop command for sending the temperature values via UART/USB to the PC
            //TODO
        }
    }
    return 0;
}set: o <Temperaturoffset>
                //o 20
                if (strchr(gl_char_uart_buffer, ' ') != NULL)
                {
                    //Leerzeichen wurde gefunden weil eine gueltige Adresse (nungleich NULL)
                    //zurueck geliefert wurde
                    gl_int_temp_offset = atoi(gl_char_uart_buffer+2);
                }
                break;
            case 'O':
                sprintf(gl_char_uart_buffer, "T-Offset: %d\n", gl_int_temp_offset);
                UART_printText(gl_char_uart_buffer);
                break;
            default:
                UART_printText("Unknown Command!\n");
                break;
            //Define a start and stop command for sending the temperature values via UART/USB to the PC
            //TODO
        }
    }
    return 0;
}
