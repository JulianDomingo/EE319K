// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_cs (pin 6) connected to PA3 (SSI0Fss)
// CARD_cs (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include <stdlib.h>
#include "isaac.h"
#include "background.h"
#include "monsters.h"
#include <stdio.h>
#include "Timer0.h"
#include "DAC.h"
#define true 1
#define false 0
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
uint8_t xsprite = 120;
uint8_t ysprite = 20;
uint8_t frame = 0;
int frameleft = 0;
int frameright = 0;
uint16_t bubbley = 6;
uint8_t bubblex = 0;
uint8_t bubbley1 = 6;
int32_t bubblex1 = 0;
uint8_t bubbley2 = 6;
uint8_t bubblex2 = 0;
uint8_t bubbley3 = 6;
uint8_t bubblex3 = 0;
uint8_t trigger = 0;
uint8_t bottom = 0;
uint8_t right = 0;
uint8_t left = 0;
uint8_t top = 0;
uint8_t bubbleFrameCount = 0;
int16_t screenmovebottom = 0;
int16_t screenmovetop = -160;
uint16_t double_buffer[15105];
int8_t upcount = 0;
uint8_t m = 0;
uint8_t f = 0;
uint8_t stackCounter = 0;
uint8_t cs;
uint8_t speed = 4;
uint8_t havekey= true;
uint8_t monsterArrCount = 0;
uint8_t roomChanged;
uint8_t hit = 0;
uint8_t boss = 0;
uint8_t xcompare = 0;
uint8_t ycompare = 0;
uint8_t shootx = 0;
uint8_t shooty =0;
uint8_t shootx1 = 0;
uint8_t shooty1 =0;
int16_t shootx2 = 0;
uint8_t shooty2 =0;
uint8_t shootx3 = 0;
int16_t shooty3 =0;
int16_t IsaacHealth;
uint8_t ocount = 0;
uint8_t oFrameCount = 0;
uint8_t Dcount = 0, Pcount= 0, Scount = 0;
uint8_t	droopFrameCount = 0;
uint8_t	poopFrameCount = 0;
uint8_t	spiderFrameCount = 0;
const uint16_t *animation[10] = {frame2, frame3, frame4, frame5, frame6, frame7, frame8, frame9, frame0, frame1};
const uint16_t *animationright[6] = {frameleft0, frameleft11, frameleft22, frameleft3, frameleft4, frameleft5};
const uint16_t *animationleft[6] = {frameright0, frameright1, frameright2, frameright3, frameright4, frameright5};
const uint16_t *animationup[6] = {up1, up2, up3, up4, up5, up6};
uint32_t frameRefresh = 0;

uint16_t currentScore = 0;
uint8_t spikeDown1 = 0;
uint8_t spikeUp1 = 0;

uint8_t spikeDown2 = 0;
uint8_t spikeUp2 = 0;

uint8_t spikeRight3 = 0;
uint8_t spikeLeft3 = 0;

uint8_t spikeUp4 = 0;
uint8_t spikeDown4 = 0;


uint8_t Spider1 = 0;
uint8_t Spider2 = 0;
uint8_t Spider3 = 0;


uint8_t Droop1 = 0;
uint8_t Droop2 = 0;
uint8_t Droop3 = 0;

uint8_t Poop1 = 0;
uint8_t Poop2 = 0;
uint8_t Poop3 = 0;

typedef struct sprite{
	const uint16_t *bitmap;
	const uint16_t w,h;
} sprite;

const struct sprite spider[3] = {{spider1, 20, 9}, {spider2, 20, 9}, {spider3, 20, 9}};
const struct sprite droop[4] = {{droop1, 19, 22}, {droop2, 19, 22}, {droop3, 19, 22}, {droop4, 19, 22}};
const struct sprite poop[4] = {{poop1, 20, 18}, {poop2, 20, 18}, {poop3, 20, 18}, {poop4, 20, 18}};
//const struct sprite octomonster[3] = {{octo1, 70, 39},{octo2, 70, 39},{octo4,70,39}};

typedef struct entity{
		//vector pos,vel,acc;
	const struct sprite* frames;	//an array of sprites for an animation
	uint8_t w,h;
	uint8_t numframes;
	uint8_t x;
	uint8_t y;
	int8_t healthpoints;
	uint8_t index;
	uint8_t status;
} entity;

entity *monsterArr[10];

typedef struct room{
	const uint16_t *gamebuffer;
	uint8_t index;
	uint8_t n;
	uint8_t s;
	uint8_t w;
	uint8_t e;
	uint8_t nns;
	uint8_t sns;
	uint8_t wns;
	uint8_t ens;
	uint8_t type;
	uint8_t monsterNum;
	
} room;

	entity monsterDroop1;
	entity monsterDroop2;
	entity monsterDroop3;

	entity monsterPoop1;
	entity monsterPoop2;
	entity monsterPoop3;
	
	entity monsterSpider1;
	entity monsterSpider2;
	entity monsterSpider3;
	entity monsterSpider4;

	entity spikeBlock1;
	entity spikeBlock2;
	entity spikeBlock3;
	entity spikeBlock4;



	entity octo;
	/*
struct room backgroundset[11] = 
{{background95159,0, 1, 1, 1, 1,   9, 4, 1, 6,  'n', 0},	//checked
			 {Cathedral,1, 1, 0, 0, 1,   2, 1, 1, 0,  'd', 2}, //checked
			 {Basement, 2, 1, 1, 0, 0,   3, 1, 2, 2,  'p', 4},	//checked
			 {Basement, 3, 0, 1, 0, 1,   3, 2, 3, 10, 'd', 2},//checked
			 {Cathedral,4, 1, 1, 0, 0,   0, 5, 0, 0,  's', 4}, //checked
			 {Basement, 5, 1, 0, 0, 0,   4, 5, 5, 5,  's', 2}, //checked
			 {cellar ,6, 0, 0, 1, 1,   	 6, 6, 0, 7,  's', 4},	//checked
			 {Basement, 7, 0, 0, 1, 1,   7, 7, 6, 8,  'r', 4}, //checked
			 {Cathedral,8, 0, 0, 1, 0, 	 8, 8, 7, 8,  'b', 1}, //checked
			{Basement,9, 1, 1, 0, 0,    10, 0, 9, 9,  's', 4}, //checked
			{Cathedral,10, 0, 1, 1, 0,  10, 9, 3, 10, 'd', 3} //checked
};*/

struct room backgroundset[11] = 
{{background95159,0, 1, 1, 1, 1,   9, 4, 1, 6,  'n', 0},	//checked
			 {Cathedral,1, 1, 0, 0, 1,   2, 1, 1, 0,  'd', 2}, //checked
			 {Basement, 2, 1, 1, 0, 0,   3, 1, 2, 2,  'p', 4},	//checked
			 {Basement, 3, 0, 1, 0, 1,   3, 2, 3, 10, 'd', 2},//checked
			 {Cathedral,4, 1, 1, 0, 0,   0, 5, 0, 0,  's', 4}, //checked
			 {Basement, 5, 1, 0, 0, 0,   4, 5, 5, 5,  's', 2}, //checked
			 {cellar ,6, 0, 0, 1, 1,   	 6, 6, 0, 7,  's', 4},	//checked
			 {Basement, 7, 0, 0, 1, 1,   7, 7, 6, 8,  'r', 4}, //checked
			 {Cathedral,8, 0, 0, 1, 0, 	 8, 8, 7, 8,  'b', 1}, //checked
			{Basement,9, 0, 1, 0, 0,    9, 0, 9, 9,  's', 4}, //checked
			{Cathedral,10, 0, 0, 1, 0,  10, 10, 3, 10, 'r', 4} //checked
};


typedef struct FSM{
 struct room* roomPt;
}FSM;

FSM gameEngine;
// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define fresh 1
#define stale 0
// Initialize Port F so PF1, PF2 and PF3 are heartbeats

uint32_t mailbox_Vertical, mailbox_Horizontal, flag, i = 0;


void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;			//Activate clock for Port F
	while(SYSCTL_RCGCGPIO_R == 0){}	//Allow time or clock to stabilize
	GPIO_PORTF_DEN_R	|= 0x0E;			//Enable PF1/2/3
	GPIO_PORTF_DIR_R	|= 0x0E;			//Heartbeats are all output
	GPIO_PORTF_AFSEL_R &= ~0x0E;		//Turn off alternating functions for heartbeats
}
	
void PortC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x04;			//Activate clock for Port F
	while(SYSCTL_RCGCGPIO_R == 0){}	//Allow time or clock to stabilize
	GPIO_PORTC_DEN_R	|= 0x10;			//Enable PF1/2/3
	GPIO_PORTC_DIR_R	&= ~0x10;			
	GPIO_PORTC_AFSEL_R &= ~0x0E;		//Turn off alternating functions for heartbeats
}


void SysTick_Init(void){
	
	NVIC_ST_CTRL_R = 0;									//Disable SysTick temporarily for setup
	NVIC_ST_RELOAD_R = 2000000-1;					//Load with value of period
	NVIC_ST_CURRENT_R = 0;							//Clear the CURRENT register
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; 	
	NVIC_ST_CTRL_R = 0x0007;						//Enable SysTick and with core clock and interrupts
	
}

uint32_t Convert(uint32_t input){
  uint32_t output;
	output = ((4125*input)+1438000)/10000;
	return(output);
}


const unsigned short bubble[] = {
 0xFFFF, 0x73AF, 0x7BF0, 0x7BF0, 0xFFFF, 0x2966, 0x2102, 0x946D, 0x1060, 0x31A7, 0x0000, 0xFFD8, 0xD6B5, 0xFFF9, 0x0000, 0x2104,
 0xCE36, 0xFFFD, 0xA50E, 0x2125, 0xFFFF, 0x2104, 0x0000, 0x2946, 0xFFFF,

};


void addprops(void){
	if(gameEngine.roomPt[cs].n == 0){
		DrawSprite(74, 18, rock ,13 ,13);
	}
	if(gameEngine.roomPt[cs].s == 0){
		DrawSprite(74, 82, rock, 13, 13);
	}
	if(gameEngine.roomPt[cs].w == 0){
		DrawSprite(138, 52, rock, 13, 13);
	}
	if(gameEngine.roomPt[cs].e == 0){
		DrawSprite(8, 54, rock, 13, 13);
	}
}

void buffer_write(uint8_t index){

	if(IsaacHealth!=0){
	for(int i = 0; i < 15105;i++){
		double_buffer[i] = gameEngine.roomPt[index].gamebuffer[i];
		
	}	
}
}

void SysTick_Handler(void){
	mailbox_Horizontal = ADC_In_Horizontal();									//obtain converted digital value from potentiometer
	mailbox_Vertical = ADC_In_Vertical();
	flag = fresh;													//flag is ready to accept new value
}


//DrawSprite
void DrawSprite(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h){
unsigned long limit, Xi, Yi;
limit = 0;
	Xi = x;
	Yi = y;
  int16_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int16_t originalWidth = w;              // save this value; even if not all columns fit on the screen, the image is still this width in ROM
  int i = w*(h - 1);

	
	 if(Yi >= 86){
		Yi = 86;
	}
	 if(Yi <= 22){
		Yi = 22;
	}
	
	if(Xi <= 8){
		Xi = 8;
	}
	if(Xi >= 138){
		Xi = 138;
	}
	  for(y=0; y<h; y=y+1){
    for(x=0; x<w; x=x+1){
			if(image[i] != 0x20FD){			//red
			if(image[i] != 0xFFFF){			//white
				limit = (159*95)-((95*(Xi+ x))-(Yi + y - h));
				double_buffer[limit] = image[i];
			}
		}

      i = i + 1;                        // go to the next pixel
			}

    i = i + skipC;
    i = i - 2*originalWidth;
  }
}



void rockCheck(void){			
	
	

	if(cs == 7 || cs == 10){
	if(cs == 10){
		if(ysprite <= 78 && ysprite >= 74 && xsprite <= 60 && xsprite >= 8){
		ysprite = 70;
			}
	}
	if(ysprite >= 20 && ysprite <= 78 && xsprite >= 120 && xsprite <= 124){
		xsprite = 124;
	}
	if(ysprite >=66 && ysprite <= 84 && xsprite >= 82 && xsprite <= 86){
		xsprite = 88;
	}
	if(ysprite >= 18 && ysprite <= 54 && xsprite >= 82 && xsprite <= 86){
		xsprite = 88;
	}
	if(ysprite >= 18 && ysprite <= 74 && xsprite > 88 && xsprite <= 92){
		xsprite = 88;
	}
	if(ysprite >= 82 && ysprite <= 90 && xsprite >= 96 && xsprite <= 120){
		ysprite = 86;
	}
	if(ysprite >= 58 && ysprite <= 66 && xsprite >= 64 && xsprite <= 80){
		ysprite = 62;
	}
	if(ysprite <= 78 && ysprite >= 74 && xsprite <= 60 && xsprite >= 20){
		ysprite = 70;
	}
	if(ysprite >= 68 && ysprite <= 72 && xsprite >= 58 && xsprite <= 64){
		xsprite = 60;
	}
	if(ysprite <= 92 && ysprite >= 76 && xsprite >= 6 && xsprite <= 18){
		xsprite = 12;
	}
	if(ysprite >= 38 && ysprite <= 44 && xsprite >= 14 && xsprite <= 64){
		ysprite = 42;
	}
	if(ysprite >= 38 && ysprite <= 54 && xsprite <= 64 && xsprite >= 56){
		xsprite = 56;
	}
	if(ysprite >= 18 && ysprite <= 34 && xsprite > 8 && xsprite <= 18){
		xsprite = 8;
	}
}
//*********************************LEFT DOOR BLOCK*******************************************************
					
	//right side of left rock
	if((gameEngine.roomPt[cs].w == 0)&&((xsprite >= 118)&&((ysprite == 46)))){                                   
		xsprite = 118;
		ysprite = 46;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((xsprite >= 118)&&((ysprite == 50)))){                                   
		xsprite = 118;
		ysprite = 50;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((xsprite >= 118)&&((ysprite == 54)))){                                   
		xsprite = 118;
		ysprite = 54;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((xsprite >= 118)&&((ysprite == 58)))){                                   
		xsprite = 118;
		ysprite = 58;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((xsprite >= 118)&&((ysprite == 62)))){                                   
		xsprite = 118;
		ysprite = 62;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((xsprite >= 118)&&((ysprite == 66)))){                                   
		xsprite = 118;
		ysprite = 66;
	}
	
	//top side of left rock
	if((gameEngine.roomPt[cs].w == 0)&&((ysprite == 42 || ysprite == 46)&&(xsprite == 138))){
		xsprite = 138;
		ysprite = 42;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((ysprite == 42 || ysprite == 46)&&(xsprite == 134))){
		xsprite = 134;
		ysprite = 42;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((ysprite == 42 || ysprite == 46)&&(xsprite == 130))){
		xsprite = 130;
		ysprite = 42;
	}
	
	//bottom side of left rock
	if((gameEngine.roomPt[cs].w == 0)&&((ysprite >= 62 && ysprite <= 74)&&(xsprite == 138))){
		xsprite = 138;
		ysprite = 78;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((ysprite >= 62 && ysprite <= 74)&&(xsprite == 134))){
		xsprite = 134;
		ysprite = 78;
	}
	if((gameEngine.roomPt[cs].w == 0)&&((ysprite >= 62 && ysprite <= 74)&&(xsprite == 130))){
		xsprite = 130;
		ysprite = 78;
	}

	
//*********************************RIGHT DOOR BLOCK*******************************************************
	
	//left side of right rock
	if((gameEngine.roomPt[cs].e == 0)&&((xsprite == 20 || xsprite == 16 || xsprite == 18)&&(ysprite == 46))){
		xsprite = 20;
		ysprite = 46;
	}
	if((gameEngine.roomPt[cs].e == 0)&&((xsprite == 20 || xsprite == 16 || xsprite == 18)&&(ysprite == 54))){
		xsprite = 20;
		ysprite = 54;
	}
	if((gameEngine.roomPt[cs].e == 0)&&((xsprite == 20 || xsprite == 16 || xsprite == 18)&&(ysprite == 50))){
		xsprite = 20;
		ysprite = 50;
	}
	if((gameEngine.roomPt[cs].e == 0)&&((xsprite == 20 || xsprite == 16 || xsprite == 18)&&(ysprite == 58))){
		xsprite = 20;
		ysprite = 58;
	}
	if((gameEngine.roomPt[cs].e == 0)&&((xsprite == 20 || xsprite == 16 || xsprite == 18)&&(ysprite == 62))){
		xsprite = 20;
		ysprite = 62;
	}
	if((gameEngine.roomPt[cs].e == 0)&&((xsprite == 20 || xsprite == 16 || xsprite == 18)&&(ysprite == 66))){
		xsprite = 20;
		ysprite = 66;
	}
	
	//top side of right rock
	if((gameEngine.roomPt[cs].e == 0)&&(((ysprite == 42 || ysprite == 46)&&(xsprite == 8 || xsprite == 10)))){
		xsprite = 8;
		ysprite = 42;
	}
	if((gameEngine.roomPt[cs].e == 0)&&(((ysprite == 42 || ysprite == 46)&&(xsprite == 12)))){
		xsprite = 12;
		ysprite = 42;
	}

	//bottom side of right rock
	if((gameEngine.roomPt[cs].e == 0)&&(((ysprite >= 62 && ysprite <= 74)&&((xsprite == 8 || xsprite == 10))))){
		xsprite = 8;
		ysprite = 78;
	}
	if((gameEngine.roomPt[cs].e == 0)&&((ysprite >= 62 && ysprite <= 74)&&((xsprite == 12)))){
		xsprite = 12;
		ysprite = 78;
	}
	
	
//*********************************UP DOOR BLOCK*******************************************************
	
	//bottom side of top rock
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite <= 38)&&(xsprite == 64 || xsprite == 62))){
		xsprite = 64;
		ysprite = 38;
	}
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite <= 38)&&(xsprite == 68 || xsprite == 66))){
		xsprite = 68;
		ysprite = 38;
	}
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite <= 38)&&(xsprite == 72 || xsprite == 70))){
		xsprite = 72;
		ysprite = 38;
	}
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite <= 38)&&(xsprite == 76 || xsprite == 74))){
		xsprite = 76;
		ysprite = 38;
	}
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite <= 38)&&(xsprite == 80 || xsprite == 78))){
		xsprite = 80;
		ysprite = 38;
	}
	
	//left side of top rock
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite == 22)&&(xsprite == 88 || xsprite == 84 || xsprite == 86))){
		xsprite = 88;
		ysprite = 22;
	}
	
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite == 26)&&(xsprite == 88 || xsprite == 84 || xsprite == 86))){
		xsprite = 88;
		ysprite = 26;
	}
	
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite == 30)&&(xsprite == 88 || xsprite == 84 || xsprite == 86))){
		xsprite = 88;
		ysprite = 30;
	}
	
	//right side of top rock
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite == 22)&&(xsprite == 52 || xsprite == 56 || xsprite == 58 || xsprite == 60))){
		xsprite = 52;
		ysprite = 22;
	}
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite == 26)&&(xsprite == 52 || xsprite == 56 || xsprite == 58 || xsprite == 60))){
		xsprite = 52;
		ysprite = 26;
	}
	if((gameEngine.roomPt[cs].n == 0)&&((ysprite == 30)&&(xsprite == 52 || xsprite == 56 || xsprite == 58 || xsprite == 60))){
		xsprite = 52;
		ysprite = 30;
	}

//*********************************DOWN DOOR BLOCK*******************************************************
	//top side of bottom rock
	if((gameEngine.roomPt[cs].s == 0)&&((ysprite >= 78)&&(xsprite == 64 || xsprite == 62))){
		xsprite = 64;
		ysprite = 74;
	}
	if((gameEngine.roomPt[cs].s == 0)&&((ysprite >= 78)&&(xsprite == 68 || xsprite == 66))){
		xsprite = 68;
		ysprite = 74;
	}
	if((gameEngine.roomPt[cs].s == 0)&&((ysprite >= 78)&&(xsprite == 72 || xsprite == 70))){
		xsprite = 72;
		ysprite = 74;
	}
	if((gameEngine.roomPt[cs].s == 0)&&((ysprite >= 78)&&(xsprite == 76 || xsprite == 74))){
		xsprite = 76;
		ysprite = 74;
	}
	if((gameEngine.roomPt[cs].s == 0)&&((ysprite >= 78)&&(xsprite == 80 || xsprite == 78))){
		xsprite = 80;
		ysprite = 74;
	}
}

void addProps(void){


	if(gameEngine.roomPt[cs].s == 0){
		DrawSprite(74, 82, rock, 13, 13);								//rock blocking bottom
	}
	if(gameEngine.roomPt[cs].n == 0){
		DrawSprite(74, 18, rock, 13, 13);								//rock blocking top
	}
	if(gameEngine.roomPt[cs].w == 0){
		DrawSprite(138, 52, rock, 13, 13);						  //rock blocking left
	}
	if(gameEngine.roomPt[cs].e == 0)
		DrawSprite(8, 54, rock, 13, 13);								//rock blocking right
}

void monsterSpawn(void){

	if(gameEngine.roomPt[cs].type == 's' && gameEngine.roomPt[cs].monsterNum == 4){
		monsterSpider1.x = 20;
		monsterSpider1.y = 20;
		monsterSpider2.x = 40;
		monsterSpider2.y = 40;
		monsterSpider3.x = 60;
		monsterSpider3.y = 60;
		monsterSpider4.x = 80;
		monsterSpider4.y = 80;
		monsterSpider1.healthpoints = 20;
		monsterSpider2.healthpoints = 20;
		monsterSpider3.healthpoints = 20;
		monsterSpider4.healthpoints = 20;
		monsterArr[0]= &monsterSpider1;
		monsterArr[1]= &monsterSpider2;
		monsterArr[2]= &monsterSpider3;
		monsterArr[3]= &monsterSpider4;
	}
	
	if(gameEngine.roomPt[cs].type == 's' && gameEngine.roomPt[cs].monsterNum == 2){
		monsterSpider1.x = 20;
		monsterSpider1.y = 20;
		monsterSpider2.x = 40;
		monsterSpider2.y = 40;
		monsterSpider1.healthpoints = 20;
		monsterSpider2.healthpoints = 20;
		
		monsterArr[0]= &monsterSpider1;
		monsterArr[1]= &monsterSpider2;


	}
	
	if(gameEngine.roomPt[cs].type == 'p' && gameEngine.roomPt[cs].monsterNum == 2){
		monsterPoop1.x = 20;
		monsterPoop1.y = 20;
		monsterPoop2.x = 40;
		monsterPoop2.y = 40;
		monsterPoop1.healthpoints = 50;
		monsterPoop2.healthpoints = 50;
		monsterArr[0]= &monsterPoop1;
		monsterArr[1]= &monsterPoop2;
	
	}
	
	if(gameEngine.roomPt[cs].type == 'p' && gameEngine.roomPt[cs].monsterNum == 4){
		monsterPoop1.x = 20;
		monsterPoop1.y = 20;
		monsterPoop2.x = 40;
		monsterPoop2.y = 40;
		monsterPoop3.x = 60;
		monsterPoop3.y = 60;
		monsterPoop1.healthpoints = 50;
		monsterPoop2.healthpoints = 50;
		monsterPoop3.healthpoints = 50;
		//monsterPoop4.x = 80;
		//monsterPoop4.y = 80;
		monsterArr[0]= &monsterPoop1;
		monsterArr[1]= &monsterPoop2;
		monsterArr[2]= &monsterPoop3;
		//monsterArr[3]=monsterSpider4;
	}
	
	if(gameEngine.roomPt[cs].type == 'd' && gameEngine.roomPt[cs].monsterNum == 2){
		monsterDroop1.x = 20;
		monsterDroop1.y = 20;
		monsterDroop2.x = 40;
		monsterDroop2.y = 40;
		monsterDroop1.healthpoints = 70;
		monsterDroop2.healthpoints = 70;
		monsterArr[0]= &monsterDroop1;
		monsterArr[1]= &monsterDroop2;

	}
	
	if(gameEngine.roomPt[cs].type == 'd' && gameEngine.roomPt[cs].monsterNum == 3){
		monsterDroop1.x = 20;
		monsterDroop1.y = 20;
		monsterDroop2.x = 40;
		monsterDroop2.y = 40;
		monsterDroop3.x = 60;
		monsterDroop3.y = 60;
		monsterDroop1.healthpoints = 70;
		monsterDroop2.healthpoints = 70;
		monsterDroop3.healthpoints = 70;
		monsterArr[0]= &monsterDroop1;
		monsterArr[1]= &monsterDroop2;
		monsterArr[2]= &monsterDroop3;
	}
	if(gameEngine.roomPt[cs].type == 'b' && gameEngine.roomPt[cs].monsterNum == 1){
		octo.x = 100;
		octo.y = 60;
		octo.healthpoints = 50;
		monsterArr[0] = &octo;
	}
	
}


void movement(void){
				if(ysprite >=86){
						ysprite = 86;
					}
					if(ysprite <=20){
						ysprite = 20;
					}
					if(xsprite <= 8){
						xsprite = 8;
					}
					if(xsprite >= 138){
						xsprite = 138;
					}
				if((mailbox_Vertical < 2200)&&(mailbox_Vertical > 2000)&&(mailbox_Horizontal >2000)&&(mailbox_Horizontal <2200)){ //up
					frame = 0;
					frameleft = 0;
					frameright = 0;
					if(GPIO_PORTC_DATA_R&0x10){
						bottom =1;
						trigger =1;
						DrawSprite(xsprite, ysprite, blink1, 18, 22);
					}
					buffer_write(cs);	
					if(havekey == true){
						DrawSprite(xsprite, ysprite, isaackey, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animation[frame], 18, 22);
					}
				}	
				if(mailbox_Vertical > 3200){ //up
					buffer_write(cs);	
					ysprite -= speed;
					rockCheck();
					if(GPIO_PORTC_DATA_R&0x10){
						top = 1;
						trigger = 1;
					}
					if(ysprite >=86){
						ysprite = 86;
					}
					if((ysprite <= 20)&&((xsprite == 72)||(xsprite == 74)||(xsprite == 70))){
						cs = gameEngine.roomPt[cs].nns;
						Droop1 = 0;
						Droop2 = 0;
						Droop3 = 0;

						Spider1 = 0;
						Spider2 = 0;
						Spider3 = 0;

						Poop1 = 0;
						Poop2 = 0;
						Poop3 = 0;
						xsprite = 72;
						ysprite = 86;
						monsterSpawn();
					}
					else if(ysprite <=20){
						ysprite = 20;
					}
					DrawSprite(xsprite, ysprite, animationup[upcount], 18, 22);
				
				}
				
				if((mailbox_Vertical <= 3200)&&(mailbox_Vertical > 2250)){
					
					buffer_write(cs);
					if(GPIO_PORTC_DATA_R&0x10){
						top = 1;
						trigger = 1;
						DrawSprite(xsprite, ysprite, animationup[3], 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animationup[3], 18, 22);
					}
				}
				
				
				
				
				if(mailbox_Vertical < 600){   //down
					buffer_write(cs);	
					ysprite += speed;
					rockCheck();
					if(GPIO_PORTC_DATA_R&0x10){
						bottom =1;
						trigger = 1;
					}
					if(((xsprite == 72)||(xsprite == 74)||(xsprite == 70))&&(ysprite>=86)){
							cs = gameEngine.roomPt[cs].sns;
								Droop1 = 0;
								Droop2 = 0;
								Droop3 = 0;

								Spider1 = 0;
								Spider2 = 0;
								Spider3 = 0;


								Poop1 = 0;
								Poop2 = 0;
								Poop3 = 0;
								xsprite = 72;
								ysprite = 22;
								monsterSpawn();
						}
					
					if(ysprite <=20){
						ysprite = 20;
					}
					if(trigger==1){
					DrawSprite(xsprite, ysprite, blink1, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animation[frame], 18, 22);
					}
				}
				
				if((mailbox_Vertical < 1950)&&(mailbox_Vertical >= 600)){
					buffer_write(cs);
					if(GPIO_PORTC_DATA_R&0x10){
						bottom = 1;
						trigger = 1;
						DrawSprite(xsprite, ysprite, blink1, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animation[0], 18, 22);
					}
				}
				
				
				
				
				if(mailbox_Horizontal > 3200) {   //right
					buffer_write(cs);
					xsprite -= speed;
					rockCheck();
					if(GPIO_PORTC_DATA_R&0x10){
						right =1;				
						trigger = 1;
					}
					if((xsprite < 8)&&((ysprite==50)||(ysprite==52)||(ysprite==54))){
						cs = gameEngine.roomPt[cs].ens;
						Droop1 = 0;
						Droop2 = 0;
						Droop3 = 0;

						Spider1 = 0;
						Spider2 = 0;
						Spider3 = 0;

						Poop1 = 0;
						Poop2 = 0;
						Poop3 = 0;
						xsprite = 132;
						ysprite = 52;
						monsterSpawn();
					}
					if(xsprite <= 8){
						xsprite = 8;
					}
					else if(xsprite >= 138){
						xsprite = 138;
					}
					if(trigger==1){
					DrawSprite(xsprite, ysprite, leftblink, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animationright[frameleft], 21, 22);
					}
				}
				
				
				if((mailbox_Horizontal <= 3200)&&(mailbox_Horizontal >= 2200)){
					buffer_write(cs);
					if(GPIO_PORTC_DATA_R&0x10){
						right = 1;
						trigger = 1;
						DrawSprite(xsprite, ysprite, leftblink, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animationright[1], 21, 22);
					}
				}
				
				
				
				
				
				if(mailbox_Horizontal < 600) { //left
					buffer_write(cs);	
					xsprite += speed;
					rockCheck();
					if(GPIO_PORTC_DATA_R&0x10){
						trigger = 1;
						left =1;
					}
					if((xsprite>132)&&((ysprite==50)||(ysprite==52)||(ysprite==54))){
						cs = gameEngine.roomPt[cs].wns;
						Droop1 = 0;
						Droop2 = 0;
						Droop3 = 0;

						Spider1 = 0;
						Spider2 = 0;
						Spider3 = 0;

						Poop1 = 0;
						Poop2 = 0;
						Poop3 = 0;
						ysprite = 52;
						xsprite = 8;
						monsterSpawn();
			
					}
					if(xsprite <= 8){
						xsprite = 8;
					}
					else if(xsprite >= 138){
						xsprite = 138;
					}
					if(trigger==1){
					DrawSprite(xsprite, ysprite, rightblink, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animationleft[frameright], 21, 22);
				}
			}
				if((mailbox_Horizontal < 1880)&&(mailbox_Horizontal >= 600)){
					buffer_write(cs);
					if(GPIO_PORTC_DATA_R&0x10){
						left = 1;
						trigger = 1;
						DrawSprite(xsprite, ysprite, rightblink, 18, 22);
					}
					else{
					DrawSprite(xsprite, ysprite, animationleft[3], 21, 22);
					}
				}
				
			
			
		}

void shootCollision(void){
	
	for(uint8_t e = 0; e < gameEngine.roomPt[cs].monsterNum; e++){
		if(monsterArr[e]->healthpoints>0){
		if(((monsterArr[e]->x + monsterArr[e]->frames[0].w) >= shootx)&&(shootx >= monsterArr[e]->x)
			&&((monsterArr[e]->y - monsterArr[e]->frames[0].h)<=shooty)&&(shooty<=monsterArr[e]->y)){
			//DrawSprite(138, 22, key, 16, 22);
			monsterArr[e]->healthpoints--;
			hit = 1;
			
		}
		else if(((monsterArr[e]->x + monsterArr[e]->frames[0].w) >= shootx1)&&(shootx1 >= monsterArr[e]->x)
			&&((monsterArr[e]->y - monsterArr[e]->frames[0].h)<= shooty1)&&(shooty1<=monsterArr[e]->y)){
			//DrawSprite(138, 22, key, 16, 22);
			monsterArr[e]->healthpoints--;	
			hit = 1;
		}
	  else if(((monsterArr[e]->x + monsterArr[e]->frames[0].w) >= shootx2)&&(shootx2 >= monsterArr[e]->x)
			&&((monsterArr[e]->y - monsterArr[e]->frames[0].h)<=shooty2)&&(shooty2<=monsterArr[e]->y)){
			//DrawSprite(138, 22, key, 16, 22);
			monsterArr[e]->healthpoints--;	
			hit = 1;
		}
		else if(((monsterArr[e]->x + monsterArr[e]->frames[0].w) >= shootx3)&&(shootx3 >= monsterArr[e]->x)
			&&((monsterArr[e]->y - monsterArr[e]->frames[0].h)<=shooty3)&&(shooty3<=monsterArr[e]->y)){
			//DrawSprite(138, 22, key, 16, 22);
			monsterArr[e]->healthpoints--;	
			hit = 1;
		}
	}
		
	}

}	
void shoot(void){
			if((trigger == 1)&&(bottom==1)){
						shootx = xsprite + 2 - bubblex;
						shooty = ysprite - 5 + bubbley;
						shootCollision();
						if(hit != 1){
							DrawSprite(shootx, shooty, bluebubble, 7,6);
							bubbley+=10;
							if(bubbley >= 70){
							bubblex=0;
							bubbley=0;
							bottom=0;
							trigger = 0;
						}
						}
						hit = 0;
				}
					
			else if((trigger == 1)&&(left==1)){
						shootx1 =  (xsprite + 10 + bubblex1);
						shooty1 =	(ysprite - 5 + bubbley1);
					
						if(hit != 1){
							DrawSprite(shootx1, shooty1, bluebubble, 7,6);
							bubblex1+=10;
							shootCollision();
							if(bubblex1 >70){
							bubblex1=0;
							bubbley1=0;
							left = 0;
							trigger = 0;
						}
					} hit =0;
				}
			else if((trigger == 1)&&(right==1)){
						shootx2 = xsprite + 2 - bubblex2;
						shooty2 = (ysprite - 5 + bubbley2);
						if(shootx2<= 12){
							shootx2 = 12;
						}
						if(hit != 1){
							DrawSprite(shootx2, shooty2, bluebubble, 7,6);
							bubblex2+=10;
							shootCollision();
							if(bubblex2 >= 70){
							bubblex2=0;
							bubbley2=0;
								right = 0;
							trigger = 0;
						}
					} hit = 0;
					}
			else if((trigger == 1)&&(top==1)){
						shootx3 = (xsprite + 2 - bubblex3);
						shooty3 = ysprite - 5 - bubbley3;
						shootCollision();
						if(shooty3 <=24){
							shooty3 = 24;
						}		
						
						if(hit != 1){
						DrawSprite(shootx3, shooty3, bluebubble, 7,6);
						bubbley3+=10;
						
						if(bubbley3 >= 70){
						bubblex3=0;
						bubbley3=0;
						top = 0;
						trigger = 0;
						}
				}hit = 0;
			}
}

//Object collision check (rocks)

void changeframes(void){
				frame = (frame+1)%10;
				frameleft = (frameleft+1)%6;
				frameright = (frameright+1)%6;
				droopFrameCount++;
			  poopFrameCount++;
				spiderFrameCount++;
				oFrameCount++;
				f = (f+1)%3;
				m = (m+1)%3;
				upcount = (upcount+1)%6;
}
void debugxy(void){/*
		ST7735_SetCursor(1,5);
		ST7735_OutString("v");
		LCD_OutFix(mailbox_Vertical);
		ST7735_SetCursor(3,8);
		ST7735_OutString("     ");
		ST7735_OutString("h");
		LCD_OutFix(mailbox_Horizontal);
	*/
		ST7735_SetCursor(6,0);	
		ST7735_OutString(" x = ");
		LCD_OutDec(xsprite);
		ST7735_SetCursor(6,1);	
		ST7735_OutString(" y = ");
		LCD_OutDec(ysprite);
}
void monsterMovement(entity *monster, int count){
	/*ST7735_SetCursor(0,10);	
	LCD_OutDec(monster->healthpoints);*/
	if(monster->healthpoints<=0){
		monster->x = 150;
		monster->y = 0;
	}
	
	if(monster->healthpoints!=0){
	if(monster->y < ysprite){
		monster->y += 1;
	}
	if(monster->y > ysprite){
		monster->y -= 1;
	}
	if(monster->x < xsprite){
		monster->x += 1;
	}
	if(monster->x > xsprite){
		monster->x -= 1;
	}

	
	xcompare = (monster->x + (monster->frames[count].w/2) - 3);
	ycompare = (monster->y - (monster->frames[count].h/2)+2);
	
	
	DrawSprite(monster->x, monster->y, monster->frames[count].bitmap, monster->frames[count].w, monster->frames[count].h);
}
	/*
	DrawSprite(monster->x + monster->frames[count].w , monster->y , bluebubble, 7,6);
	DrawSprite(monster->x + monster->frames[count].w , monster->y - monster->frames[count].h , bluebubble, 7,6);
	DrawSprite(monster->x, monster->y , bluebubble, 7,6);
	DrawSprite(monster->x, monster->y - monster->frames[count].h , bluebubble, 7,6);
	*/
}
void controlMonsters(void){
//Droop
				if(gameEngine.roomPt[cs].type == 'd' && gameEngine.roomPt[cs].monsterNum == 2){
					if(monsterDroop1.healthpoints > 0){
						monsterMovement(&monsterDroop1, Dcount);
					}
					else{
						if(Droop1 == 0){
							currentScore += 10;
							Droop1++;
						}
					}
					if(monsterDroop2.healthpoints > 0){
					monsterMovement(&monsterDroop2, Dcount);
					}
					else{
						if(Droop2 == 0){
							currentScore += 10;
							Droop2++;
						}
					}
				}
			
				if(gameEngine.roomPt[cs].type == 'd' && gameEngine.roomPt[cs].monsterNum == 3){
					if(monsterDroop1.healthpoints > 0){
						monsterMovement(&monsterDroop1, Dcount);
					}
					else{
						if(Droop1 == 0){
							currentScore += 10;
							Droop1++;
						}
					}
					if(monsterDroop2.healthpoints > 0){
						monsterMovement(&monsterDroop2, Dcount);
					}
					else{
						if(Droop2 == 0){
							currentScore += 10;
							Droop2++;
						}
					}
					if(monsterDroop3.healthpoints > 0){
						monsterMovement(&monsterDroop3, Dcount);
					}
					else{
						if(Droop3 == 0){
							currentScore += 10;
							Droop3++;
						}
					}
				}
			
//Spider
				if(gameEngine.roomPt[cs].type == 's' && gameEngine.roomPt[cs].monsterNum == 4){
					if(monsterSpider1.healthpoints > 0){
						monsterMovement(&monsterSpider1, Scount);
					}
					else{
						if(Spider1 == 0){
							currentScore += 10;
							Spider1++;
						}
					}
					if(monsterSpider2.healthpoints > 0){
						monsterMovement(&monsterSpider2, Scount);
					}
					else{
						if(Spider2 == 0){
							currentScore += 10;
							Spider2++;
						}
					}
					if(monsterSpider3.healthpoints > 0){
					monsterMovement(&monsterSpider3, Scount);
					}
					else{
						if(Spider3 == 0){
							currentScore += 10;
							Spider3++;
						}
					}
					//monsterMovement(&monsterSpider4, Scount);
				}
				if(gameEngine.roomPt[cs].type == 's' && gameEngine.roomPt[cs].monsterNum == 2){
					if(monsterSpider1.healthpoints > 0){
						monsterMovement(&monsterSpider1, Scount);
					}
					else{
						if(Spider1 == 0){
							currentScore += 10;
							Spider1++;
						}
					}
					if(monsterSpider2.healthpoints > 0){
						monsterMovement(&monsterSpider2, Scount);
					}
					else{
						if(Spider2 == 0){
							currentScore += 10;
							Spider2++;
						}
					}
				}
				
//Poop
				if(gameEngine.roomPt[cs].type == 'p' && gameEngine.roomPt[cs].monsterNum == 4){
					if(monsterPoop1.healthpoints > 0){
						monsterMovement(&monsterPoop1, Pcount);
					}
					else{
						if(Poop1 == 0){
							currentScore += 10;
							Poop1++;
						}
					}
					if(monsterPoop2.healthpoints > 0){
						monsterMovement(&monsterPoop2, Pcount);
					}
					else{
						if(Poop2 == 0){
							currentScore += 10;
							Poop2++;
						}
					}
					if(monsterPoop3.healthpoints > 0){
						monsterMovement(&monsterPoop3, Pcount);
					}
					else{
						if(Poop3 == 0){
							currentScore += 10;
							Poop3++;
						}
					}
					//monsterMovement(&monsterPoop4, Pcount);
				}
				if(gameEngine.roomPt[cs].type == 'p' && gameEngine.roomPt[cs].monsterNum == 2){
			  	if(monsterPoop1.healthpoints > 0){
						monsterMovement(&monsterPoop1, Pcount);
					}
					else{
						if(Poop1 == 0){
							currentScore += 10;
							Poop1++;
						}
					}
					if(monsterPoop2.healthpoints > 0){
						monsterMovement(&monsterPoop2, Pcount);
					}
					else{
						if(Poop2 == 0){
							currentScore += 10;
							Poop2++;
						}
					}
				}
				
				if(gameEngine.roomPt[cs].type == 'b' && gameEngine.roomPt[cs].monsterNum == 1){
					monsterMovement(&octo, ocount);

				}
				
				if(oFrameCount == 4){
					oFrameCount = 0;
					ocount = (ocount+1)%4;
				}
			//Droopy Monster		
				if(droopFrameCount == 4){
					droopFrameCount = 0;
					Dcount = (Dcount+1)%4;
				}

			//Poop Monster				
				if(poopFrameCount == 4){
					poopFrameCount = 0;
					Pcount = (Pcount+1)%4;
				}
			//Spider Monster
				if(spiderFrameCount == 3){
					spiderFrameCount = 0;
					Scount = (Scount+1)%3;
				}	
}
void score(void){
	
	ST7735_SetRotation(7);
	ST7735_SetCursor(0, 1);

	ST7735_OutString(" Score: ");
	LCD_OutDec(currentScore);
	ST7735_SetRotation(0);
	
}
void collision(void){

	if(frameRefresh == 15){
	for(uint8_t e = 0; e < gameEngine.roomPt[cs].monsterNum; e++){
		
		if((monsterArr[e] == &spikeBlock1)||(monsterArr[e] == &spikeBlock2) || (monsterArr[e] == &spikeBlock3) || (monsterArr[e] == &spikeBlock4)){
		if((xsprite+18 >= (monsterArr[e]->x + (monsterArr[e]->w / 2)))&&
			((monsterArr[e]->x + (monsterArr[e]->w / 2))>=xsprite)&&
			((ysprite-22)<=(monsterArr[e]->y + (monsterArr[e]->h/2)))&&
			((monsterArr[e]->y + (monsterArr[e]->h/2))<=ysprite)){
			IsaacHealth-=10;
		}
	}
		
		else if(((xsprite+18) >= (monsterArr[e]->x + (monsterArr[e]->w / 2) -3))
			&&(xcompare >= (monsterArr[e]->x + (monsterArr[e]->w / 2) -3))
			&&((ysprite-22)<=(monsterArr[e]->y + (monsterArr[e]->h/2) + 2))
			&&(ycompare<=(monsterArr[e]->y + (monsterArr[e]->h/2) + 2))){
			//DrawSprite(138, 22, key, 16, 22);
			IsaacHealth--;
		}
		

	}frameRefresh = 0;
}
	
	
}
void trapRoom(void){
	
	if((havekey == false)&&(cs == 10)){
	DrawSprite(8, 20, key, 10, 13);
	}
	if(((xsprite == 8)&&(ysprite == 20))&& (cs == 10)){
		havekey = true;
	}
	
	
	
	
//Oscillating spike block #1
		if(spikeDown1){
			spikeBlock1.y += 1;
			if(spikeBlock1.y == 88){
				spikeDown1 = 0;
				spikeUp1 = 1;
			}
		}
		else if(spikeUp1){
			spikeBlock1.y -= 1;
			if(spikeBlock1.y == 28){
				spikeDown1 = 1;
				spikeUp1 = 0;
			}
		}	
		DrawSprite(spikeBlock1.x, spikeBlock1.y, spikeblock, 20, 20);
		
//Oscillating spike block #2
		if(spikeDown2){
			spikeBlock2.y += 3;				//change critical points to be multiple of 3
			if(spikeBlock2.y == 87){
				spikeDown2 = 0;
				spikeUp2 = 1;
			}
		}
		else if(spikeUp2){
			spikeBlock2.y -= 3;
			if(spikeBlock2.y == 27){
				spikeDown2 = 1;
				spikeUp2 = 0;
			}
		}	
		DrawSprite(spikeBlock2.x, spikeBlock2.y, spikeblock, 20, 20);

//Oscillating spike block #3
		if(spikeLeft3){
			spikeBlock3.x += 2;
			if(spikeBlock3.x == 56){
				spikeLeft3 = 0;
				spikeRight3 = 1;
			}
		}
		else if(spikeRight3){
			spikeBlock3.x -= 2;
			if(spikeBlock3.x == 12){
				spikeLeft3 = 1;
				spikeRight3 = 0;
			}
		}	
		DrawSprite(spikeBlock3.x, spikeBlock3.y, spikeblock, 20, 20);
		
//Oscillating spike block #4
		if(spikeDown4){
			spikeBlock4.y += 4;				
			if(spikeBlock4.y == 88){
				spikeDown4 = 0;
				spikeUp4 = 1;
			}
		}
		else if(spikeUp4){
			spikeBlock4.y -= 4;
			if(spikeBlock4.y == 28){
				spikeDown4 = 1;
				spikeUp4 = 0;
			}
		}	
		DrawSprite(spikeBlock4.x, spikeBlock4.y, spikeblock, 20, 20);
		
//************Block Areas*****************
		
		DrawSprite(26,  18, rock ,13 ,13);
		DrawSprite(39,  18, rock ,13 ,13);
		DrawSprite(52,  18, rock ,13 ,13);
		DrawSprite(63,  18, rock ,13 ,13);
		DrawSprite(106, 18, rock ,13 ,13);
		DrawSprite(116, 18, rock ,13 ,13);
		DrawSprite(74,  31, rock ,13, 13);
		DrawSprite(116, 31, rock ,13 ,13);
		DrawSprite(116, 42, rock ,13 ,13);
		DrawSprite(116, 53, rock ,13 ,13);
		DrawSprite(116, 64, rock ,13 ,13);
		
		DrawSprite(106, 31, rock ,13 ,13);
		DrawSprite(106, 42, rock ,13 ,13);
		DrawSprite(106, 53, rock ,13 ,13);
		DrawSprite(106, 64, rock ,13 ,13);
		
		DrawSprite(74, 71, rock, 13, 13);
		DrawSprite(74, 40, rock, 13, 13);
	
		DrawSprite(63, 82, rock, 13, 13);
		DrawSprite(52, 82, rock, 13, 13);
		DrawSprite(41, 82, rock, 13, 13);
		DrawSprite(30, 82, rock, 13, 13);
		
}
void health(void){
	ST7735_SetRotation(7);
	ST7735_SetCursor(0, 0);
	
	ST7735_OutString(" Health: ");
	LCD_OutDec(IsaacHealth);
	ST7735_OutString(" ");
	ST7735_SetRotation(0);
}

// *************************** Capture image dimensions out of BMP**********

int main(void){
	
	TExaS_Init();													//bus clock to 80 Mhz
	DisableInterrupts();
  ST7735_InitR(INITR_REDTAB); 					//initialize LCD screen
  PortF_Init();													//initialize LED for heartbeat
	PortC_Init();
  ADC_Init();       									  //turn on ADC, set channel to 1
	SysTick_Init();												//initialize systick for periodic interrupts (25 ms for reload value)
	ST7735_FillScreen(0x0000);
  Random_Init(1);
	havekey = false;
	DAC_Init();
	gameEngine.roomPt = &backgroundset[0];
	//init 3 monsterDroops
	monsterDroop1.frames = &droop[0];
	monsterDroop2.frames = &droop[0];
	monsterDroop3.frames = &droop[0];
	
	monsterDroop1.index = 0;
	monsterDroop2.index = 2;
	monsterDroop3.index = 4;
	
	spikeBlock1.x = 128;
	spikeBlock1.y = 28;
	spikeDown1 = 1;
	
	spikeBlock2.x = 84;
	spikeBlock2.y = 87;
	spikeUp2 = 1;
	
	spikeBlock3.x = 56;
	spikeBlock3.y = 42;
	spikeRight3 = 1;
	
	spikeBlock4.x = 12;
	spikeBlock4.y = 88;
	spikeUp4 = 1;
	
	

	
	monsterPoop1.frames = &poop[0];
	monsterPoop2.frames = &poop[0];
	monsterPoop3.frames = &poop[0];
	
	monsterPoop1.index = 0;
	monsterPoop2.index = 2;
	monsterPoop3.index = 4;
	
	
	//octo.frames = &octomonster[0];
	
	monsterSpider1.frames = &spider[0];
	monsterSpider2.frames = &spider[0];
	monsterSpider3.frames = &spider[0];
	monsterSpider4.frames = &spider[0];
	
	monsterSpider1.index = 0;
	monsterSpider2.index = 2;
	monsterSpider3.index = 4;
	monsterSpider4.index = 6;
	
	
	Timer0_Init();
	EnableInterrupts();
	IsaacHealth = 100;
	
	
	
while(1){
				WaitForInterrupt();
	
				movement();
				shoot();
				addprops();
				if(cs == 7 || cs == 10){
					monsterArr[0] = &spikeBlock1;
					monsterArr[1] = &spikeBlock2;
					monsterArr[2] = &spikeBlock3;
					monsterArr[3] = &spikeBlock4;
					spikeBlock1.h = 20;
					spikeBlock2.h = 20;
					spikeBlock3.h = 20;
					spikeBlock4.h = 20;
					spikeBlock1.w = 20;
					spikeBlock2.w = 20;
					spikeBlock3.w = 20;
					spikeBlock4.w = 20;
					trapRoom();
				}
				controlMonsters();
				collision();
				health();
				score();
				changeframes();
				/*
				ST7735_SetRotation(7);
				ST7735_SetCursor(4, 0);
				//ST7735_OutString("");
				LCD_OutDec();
				ST7735_SetRotation(0);
				*/
				
				
				if(IsaacHealth <=0){
					ST7735_FillScreen(0x0000);
				}
				while(IsaacHealth <= 0){
					
					ST7735_DrawBitmap(18, 159, dead, 50,35);	
				}
				frameRefresh++;
				ST7735_DrawBitmap(18, 159, double_buffer, 95,159);
				//debugxy();
			}
}
		
								



