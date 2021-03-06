/* 

    REGULAR WORK


        Author: Jacob Causon            
                April 2014 

   Licensed under the Apache License, Version 2.0 (the "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software distributed
    under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.


*/

#include <util/delay.h>
#include <avr/interrupt.h>

#include "debug.h"
#include "lcd.h"
#include "RIOS.h"
#include "revRotDriver.h"
#include "avrDoge.h"

#define TRUE 1
#define FALSE 0

#define LED_ON PORTB |= _BV(PINB7)
#define LED_OFF PORTB &= ~_BV(PINB7)

#define ROTOR_TIME 1
// 37 * 1 = 37ms
#define BUTTON_TIME 40;
// 37 * 4 = 148ms
#define DRAW_TIME 60;
#define STEP_TIME 1

#define SPEED_MULTIPLIER 4

#define DROP_NUMBER 8
#define DROP_SIZE 10 
#define PLAYER_SIZE 30

int16_t iob_delta(void);
volatile int16_t delta;

// Functions which are only for use within the program
void pushed(char type);

int16_t testVal; //A signed int for use for testing certain things
rectangle player;

rectangle drop; //One used for all drops to save memory


int16_t dropX[DROP_NUMBER];
int16_t dropY[DROP_NUMBER];
uint8_t dropTimer[DROP_NUMBER]; //0 == should be displayed

//4 is 3 digits for the number and 1 for \0
char scoreString[8] = {'0', '\0'};

/*
    'N' North
    'S' South
    'E' East
    'W' West
*/
char buttonPressed = 'E';

char gameOver = FALSE;
uint16_t score = 0;

//Only have pause available if in debug mode
#if DEBUG
  char pause = FALSE;
#endif

// Set up the LED to show on GAME OVER
void init_LED() {
    DDRB |= _BV(PINB7); /* set LED as output */

    DDRB &= ~_BV(PINC0)   /* inputs */
 	  & ~_BV(PINC1);
 
    PORTC |= _BV(PINC0)   /* enable pull ups */
   	   | _BV(PINC1);
    
}

// Set the snake in the init position
// and clear the screen
void init_game()
{
    DDRC    = ddrc;  // Restore display configuration of Port C
    PORTC   = portc;
    _delay_ms(3);

    gameOver = FALSE;

    buttonPressed = 'E';
    score = 0;
    
    player.top = LCDHEIGHT-PLAYER_SIZE;
    player.bottom = LCDHEIGHT-1;
    player.right = PLAYER_SIZE;
    player.left = 1;

    //Draw drops
    uint8_t i = DROP_NUMBER;
    while(i) {
        --i;
        dropX[i] = (73*(i+1)) % (LCDWIDTH-DROP_SIZE);
        dropY[i] = 1;
        dropTimer[i] = i*5;
    }

    clear_screen();
    fill_rectangle(player, BLUE);
}

void rotor_task(void)
{
    static int8_t last;
    uint8_t ddrc, portc;
    uint8_t ddrd, portd;
    int8_t new, diff;
    uint8_t wheel;

    /* Save state of Port C & D */
    ddrc   = DDRC; portc  = PORTC;
    ddrd   = DDRD; portd  = PORTD;


    CZ C2_H C3_H C2_R C3_R
    D0_Z D1_L
    _delay_us(3);
    wheel = PINC;


    /*
    Scan rotary encoder
    ===================
    This is adapted from Peter Dannegger's code available at:
    http://www.mikrocontroller.net/attachment/40597/ENCODE.C
    */

    new = 0;
    if( wheel  & _BV(PC3) ) new = 3;
    if( wheel  & _BV(PC2) ) new ^= 1;			// convert gray to binary

    diff = last - new;			// difference last - new
    if( diff & 1 ){			// bit 0 = value (1)
        last = new;		       	// store new as next last
        delta += (diff & 2) - 1;	// bit 1 = direction (+/-)
    }

    /* Restore state of Port C */
    DDRC    = ddrc; PORTC   = portc;
    DDRD    = ddrd; PORTD   = portd;

    PORTA &= ~_BV(PA0);  /* ISR Timing End */
    sei();
}
// read two step encoder
int16_t iob_delta(){
    int16_t val;

    cli();
    val = delta;
    delta &= 1;
    sei();

    return val >> 1;
}

// Game over
void set_gameOver()
{
    gameOver = TRUE;
    display_string_in_place("GAME OVER", 80, 100);
}

void draw_task(void)
{
    #if DEBUG
        IF_BUTTON_C { 
            pause = !pause;
        }
        if(pause) {
            iob_delta(); //Consume any rotory data
            return;
        }
    #endif

    DDRC    = ddrc;  // Restore display configuration of Port C
    PORTC   = portc;

    _delay_ms(3);
    
    if(gameOver) {

        IF_BUTTON_C { 
            LED_OFF;
            init_game(); 
        } else {
            LED_ON;
        }
        DDRC    = ddrc;  /* Restore display configuration of Port C */
        PORTC   = portc;
        return;
    }

    //draw the player
    uint16_t val = iob_delta();
    if(val != 0) {
        val *= -SPEED_MULTIPLIER;
        fill_rectangle(player, BLACK);
        
        if(val < PLAYER_SIZE) {
            player.right -= val;
            if(player.right < PLAYER_SIZE) {
                player.right += val;
            } else {
                player.left -= val;
            }
            
        } else {
            player.right -= val;
            if(player.right > LCDWIDTH) {
                player.right += val;
            } else {
                player.left -= val;
            }
        }
        fill_rectangle(player, BLUE);
    }

    //Draw drops
    uint8_t i = DROP_NUMBER;
    while(i) {
        --i;

        if(dropTimer[i] == 0) {
            if(dropY[i] > LCDHEIGHT) {
                dropTimer[i] = dropX[i] % 30;
                dropY[i] = 0;
                dropX[i] = ((dropX[i]+(i*i))*41) % (LCDWIDTH-DROP_SIZE);
                score++; //inc score for every doged thing
            }
            drop.top = dropY[i]-1;
            drop.bottom = dropY[i] + DROP_SIZE +1;
            drop.left = dropX[i]-1;
            drop.right = dropX[i] + DROP_SIZE +1;
            
            fill_rectangle(drop, BLACK);
            drop.top += DROP_SIZE;
            drop.bottom += DROP_SIZE;
            dropY[i] += DROP_SIZE;

            // If touching the player end the game            
            if(drop.bottom > player.top && drop.top < player.bottom && (
                    (drop.right > player.left && drop.right < player.right) || 
                    (drop.left > player.left && drop.left < player.right)) 
                    ) {
                fill_rectangle(player, BLUE);
                fill_rectangle(drop, YELLOW);
                set_gameOver();
                return;
            }
            fill_rectangle(drop, RED);
        } else {
            dropTimer[i]--;
        }
    }

    drop.top = 0;
    drop.bottom = LCDHEIGHT;
    drop.left = 0;
    drop.right = LCDWIDTH;
    draw_rectangle(drop, YELLOW);

    //display_char_inPlace('Z', 10, 10);
    snprintf(scoreString, 4, "%d", score);
    display_string_in_place(scoreString, 10, 10);

    D1_Z
    D0_H
    D0_R
}

//Actually init things
int main(void)
{
    /* Free up port C: disable JTAG (protected): */
    MCUCR |= _BV(JTD); /* Requires 2 writes      */
    MCUCR |= _BV(JTD); /* within 4 clock cycles  */

    init_LED();

    init_lcd();
    orientation o = South;
    set_orientation(o);
    portc  = PORTC;  /* Store display configuration of Port C */
    ddrc   = DDRC;
    
    init_game();

    task t;

    // Make the rotor task
    t.period = ROTOR_TIME;
    t.elapsedTime = ROTOR_TIME;
    t.running = 0;
    t.TickFct = &rotor_task;
    addTask(t);

    // Make the draw screen task
    t.period = DRAW_TIME;
    t.elapsedTime = DRAW_TIME;
    t.running = 0;
    t.TickFct = &draw_task;
    addTask(t);

    init_processor(STEP_TIME); //Step size of 1 ms
 
    while(1);   
}

