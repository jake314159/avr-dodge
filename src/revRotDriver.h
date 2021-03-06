/*
        Author: Klaus-Peter Zauner            March 2014 
            using components written by
            Steve Gunn and Peter Dannegger

   Licence: This work is licensed under the Creative Commons Attribution License.
            View this license at http:	//creativecommons.org/about/licenses/
*/

#define STDIO_BAUD  9600

#define D0_L  DDRD |=  _BV(PD0); PORTD &= ~_BV(PD0);
#define D0_H  DDRD |=  _BV(PD0); PORTD |=  _BV(PD0);
#define D0_R  DDRD &= ~_BV(PD0); PORTD |=  _BV(PD0);
#define D0_Z  DDRD &= ~_BV(PD0); PORTD &= ~_BV(PD0);

#define D1_L  DDRD |=  _BV(PD1); PORTD &= ~_BV(PD1);
#define D1_H  DDRD |=  _BV(PD1); PORTD |=  _BV(PD1);
#define D1_R  DDRD &= ~_BV(PD1); PORTD |=  _BV(PD1);
#define D1_Z  DDRD &= ~_BV(PD1); PORTD &= ~_BV(PD1);

#define C2_L  DDRC |=  _BV(PC2); PORTC &= ~_BV(PC2);
#define C2_H  DDRC |=  _BV(PC2); PORTC |=  _BV(PC2);
#define C2_R  DDRC &= ~_BV(PC2); PORTC |=  _BV(PC2);
#define C2_Z  DDRC &= ~_BV(PC2); PORTC &= ~_BV(PC2);

#define C3_L  DDRC |=  _BV(PC3); PORTC &= ~_BV(PC3);
#define C3_H  DDRC |=  _BV(PC3); PORTC |=  _BV(PC3);
#define C3_R  DDRC &= ~_BV(PC3); PORTC |=  _BV(PC3);
#define C3_Z  DDRC &= ~_BV(PC3); PORTC &= ~_BV(PC3);

#define C4_L  DDRC |=  _BV(PC4); PORTC &= ~_BV(PC4);
#define C4_H  DDRC |=  _BV(PC4); PORTC |=  _BV(PC4);
#define C4_R  DDRC &= ~_BV(PC4); PORTC |=  _BV(PC4);
#define C4_Z  DDRC &= ~_BV(PC4); PORTC &= ~_BV(PC4);

#define C5_L  DDRC |=  _BV(PC5); PORTC &= ~_BV(PC5);
#define C5_H  DDRC |=  _BV(PC5); PORTC |=  _BV(PC5);
#define C5_R  DDRC &= ~_BV(PC5); PORTC |=  _BV(PC5);
#define C5_Z  DDRC &= ~_BV(PC5); PORTC &= ~_BV(PC5);

/* #define DZ DDRD=0; PORTD=0; */
#define CZ DDRD=0; PORTD=0;
uint8_t ddrc, portc;

/*
   Extra constants to minimise the program code
   
   Author: Jacob Causon

   Licence: This work is licensed under the Creative Commons Attribution License.
            View this license at http:	//creativecommons.org/about/licenses/
*/

#define IF_BUTTON_S C2_H C3_H C4_H C5_L\
    _delay_ms(3);\
    if (!(PIND & _BV(PD0)))

#define IF_BUTTON_W C2_H C3_H C4_L C5_H\
    _delay_ms(3);\
    if (!(PIND & _BV(PD0)))

#define IF_BUTTON_N C2_H C3_L C4_H C5_H\
    _delay_ms(3);\
    if (!(PIND & _BV(PD0)))

#define IF_BUTTON_E C2_L C3_H C4_H C5_H\
    _delay_ms(3);\
    if (!(PIND & _BV(PD0)))

#define IF_BUTTON_C D0_Z D1_H D1_R C2_Z C3_Z C4_L C5_Z\
         if (!(PIND & _BV(PD1)))


