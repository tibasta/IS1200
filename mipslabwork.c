/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdlib.h> /* Declaration of stand. library for C --> header file*/
#include "mipslab.h"  /* Declatations for these labs */

/* Define size of display/mips display*/

#define HEIGHT 32
#define WIDTH 128
#define size HEIGHT*WIDTH
#define PIXEL 10
#define PAGES 2

int snakeY = 10; 
int snakeX = 10; 


int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  volatile int* trise = (volatile int*) 0xbf886100;
  *trise &= ~0xff;
  // volatile int* trisf = (volatile int*) 0xbf886140;
  // *trisf |= 0x2;

  volatile int* trisd = (volatile int*) 0xbf8860C0;
  *trisd |= 0xfe0;

  volatile int* porte = (volatile int*) 0xbf886110;
  // volatile int* portf = (volatile int*) 0xbf886150;
  *porte = 0;
  TMR2 = 0;
  PR2 = (80000000/256) / 10;
  T2CON = 0x8070;
  return;
}

/* Version 2
void labinit( void )
{
    volatile int trisE = (volatile int*) 0xbf886100;
    *trisE &= ~(0xff);
    portE &= ~(0xff);

    TRISD = 0x0fe0; //0000 1111 1110 0000 -> s�tter bits fr�n 5 till 11 som input
    T2CON = 0x70;
    PR2 = (80000000/256)/10;  //0.1/(1/80000000)/256;
    TMR2 = 0;
    T2CONSET = 0x8000;

    IEC(0) = 0x100;
    IPC(2) = 0x4;
    enable_interrupt();

  return;
}
*/

int get_snake_head_position() { // Beräkna vilken "rad" i den virtuella displayen ormens huvud befinner sig på
  int row_in_display = snakeY / PIXEL;// Beräkna och returnera den linjära positionen av ormens huvud i char-arrayen
  int head_position = snakeX + row_in_display * 32;
  return head_position;
}

int calculateFoodPosition() {// Calculate the row index based on PIXEL_UNIT
  int rowIndex = foodY / PIXEL_UNIT;// Calculate the position in the array based on row and column
  int position = foodX + rowIndex * WIDTH;// Update the global variable posFood
    posFood = position;// Return the calculated position
  return position;
}

//här rensas pixlar i display-arrayen som inte representerar ormen eller maten genom att sätta deras värden till noll.
void update_display_pixel(int x, int y){
  int row_index = y / PIXEL_UNIT;   // Calculate the row index in the display array based on PIXEL_UNIT
  int bit_offset = y - row_index * PIXEL_UNIT;   // Calculate the bit offset within the byte in the display array
  display[x + row_index * WIDTH] =  1 << bit_offset;   // Set the specific bit in the display array to represent the pixel
}

//här rensas pixlar i display-arrayen som inte representerar ormen eller maten genom att sätta deras värden till noll.
void clear_non_snake_food_pixels() {
  for (int index = 0; index < SIZE_OF_PAGE; index++)
    display[index] &= (index == posArray || index == posFood) ? display[index] : 0;
}

/* Changes the snake's direction and updates its position based on velocity.*/
void move(int x, int y) {
  velocity = x + WIDTH * y;
  snakeX += x;
  snakeY += y;
  posArray += velocity;
}

move(-1, 0); // Move left
move(1, 0);  // Move right
move(0, -1); // Move up
move(0, 1);  // Move down

// Funktionen `set_game_speed` sätter svårighetsgraden baserat på switch-värdena.
// Parametrar:
// - Ingen
// Retur:
// - Ingen
void set_game_speed() {
    int sw = getsw();    // Hämta switch-värdet
    // Utvärdera switch-värdet och sätt svårighetsgraden
    if (sw == 0x3)
        difficulty = 50;
    else if (sw == 0x2)
        difficulty = 100;
    else if (sw == 0x1)
        difficulty = 200;
}
// Check if any buttons are pressed
 if(getbtns()){
    if (getbtns() & 4){  // Check if button 4 is pressed
    mytime &= 0x0fff;    // Clear the upper 4 bits of mytime and set them with the value of switches
    mytime |= getsw() << 12;
    }
    if (getbtns() & 2){    // Check if button 2 is pressed
    mytime &= 0xf0ff;     // Clear the middle 4 bits of mytime and set them with the value of switches
    mytime |= getsw() << 8;
    }
    if (getbtns() & 1){    // Check if button 1 is pressed
    mytime &= 0xff0f;     // Clear the lower 4 bits of mytime and set them with the value of switches
    mytime |= getsw() << 4;
    }
  }

  // Function to handle food collision
void handleFoodCollision() {
    displayPixel(foodX, foodY);  // Update display for food
    increaseScore();             // Increase score
}

// Update display for a specific pixel
void displayPixel(int x, int y) {
    setPixel(x, y);
}

// Increase the score
void increaseScore() {
    *porte += 0x01;
    score++;
}
//
// Function to check collision and handle accordingly
void checkCollision() {
    if (hasCollided()) {
        generateNewFoodPosition();
        handleFoodCollision();
    }
}

// Check if the snake has collided with the food
int hasCollided() {
    return snakeX == foodX && snakeY == foodY;
}

// Generate a new random position for the food
void generateNewFoodPosition() {
    foodX = rand() % 31;
    foodY = rand() % 31;
}

// Function to handle the food collision
void handleFoodCollision() {
    food();
}

// Function to move the snake based on the current velocity
void moveBasedOnVelocity() {
    switch (velocity) {
        case 1:
            snakeX++; // Increment snake's X-coordinate to move right
            break;
        case -1:
            snakeX--; // Decrement snake's X-coordinate to move left
            break;
        case 32:
            snakeY++; // Increment snake's Y-coordinate to move down
            break;
        case -32:
            snakeY--; // Decrement snake's Y-coordinate to move up
            break;
    }
}

/* Function to clear the specified part of the screen.*/ 
void clearScreen(int sections, uint8_t displayContent[]) {
    int index;
    // Iterate over sections, clearing them using display_image
    for (index = 0; index < sections * WIDTH; index += 32) {
        display_image(index, displayContent);
    }
}


/* Function to reset all game values and clear the screen */
void gameReset() {
    // Set initial snake position
    snakeX = 15;
    snakeY = 15;

    // Generate random positions for the food
    foodX = rand() % 31;
    foodY = rand() % 31;

    // Reset velocity, score, and clear the screen
    velocity = 0;
    *porte = 0x0;
    display_clear(NUMBER_OF_PAGES * 2, death);
}

/* Handle winning condition and prompt to restart */
void handleWinCondition() {
    const int maxScore = 300;

    if (score <= maxScore) {
        return;  // No need to proceed if the score is not maximum
    }

    int button;
    while (1) {
        display_update();
        display_string(0, "Congratulations!");
        display_string(1, "You Achieved Max Score");
        display_string(2, "To Restart the Game,");
        display_string(3, "Press Button 1");

        if (button = btn1()) {
            reset();
            break;
        }
    }
}

/* Initialize the game and generate seed. */
void initializeGame() {
    display_update();
    display_string(0, "Welcome to");
    display_string(1, "bitChomper v0.9");
    display_string(2, "Press BUTTON 1 to start");
    display_string(3, "Have fun!");

    int button;
    if (button = btn1()) {
        // Seed the random number generator
        srand(feedSeed());

        // Reset the game
        reset();

        // Set the initial stage of the game
        stageOfGame = 1;
    }
}
/* Check if the snake is out of bounds and handle game over */
void checkGameOver() {
    if (snakeX < 0 || snakeX > 31 || snakeY < 0 || snakeY > 31) {
        // Clear the screen
        for (int i = 0; i < WIDTH * NUMBER_OF_PAGES; i += WIDTH) {
            display_image(i, clear);
        }

        // Display game over message
        display_update();
        display_string(0, "GAME OVER!");
        display_string(1, "SCORE ON LIGHTS");
        display_string(2, "TO RESTART");
        display_string(3, "PRESS BUTTON 1");

        // Wait for button press to restart
        int button;
        while (1) {
            if (button = btn1()) {
                reset();
                break;
            }
            display_update();
        }
    }
}

void snakE(void) {
    switch (stageOfGame) {
        case 0:
            startup();
            break;

        case 1:
            while (1) {
                set_difficulty();
                delay(difficulty);
                checkGameOver();
                handleWinCondition();
                handleInput();
                clearScreen(NUMBER_OF_PAGES / 2, clear);
                moveSnake();
                updateDisplay();
            }
            break;
    }
}

void moveSnake() {
    move_Constant();
    get_posSnake();
    get_posFood();
    check_collide();
    set_pixel(foodX, foodY);
    set_pixel(snakeX, snakeY);
    update();
}

void handleInput() {
    buttons();
}

void updateDisplay() {
    display_image(0, display);
}



