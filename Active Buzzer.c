#include "Prototipi.h"
/*
-----RCC-----
*/
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_APB2ENR *(long*)0x40023844
/*
-----GPIO-----
*/
#define GPIOC_MODER *(long*)0x40020800
#define GPIOC_ODR 	*(long*)0x40020814
/*
-----TIM9-----
*/
#define TIM9_CR1 	*(long*)0x40014000
#define TIM9_DIER *(long*)0x4001400C
#define TIM9_SR		*(long*)0x40014010
#define TIM9_EGR	*(long*)0x40014014
#define TIM9_CNT	*(long*)0x40014024
#define TIM9_PSC	*(long*)0x40014028
#define TIM9_ARR	*(long*)0x4001402C
/*
-----NVIC-----
*/
#define NVIC_ISER0 *(long*)0xE000E100
/*
-----Prototipi Funzioni-----
*/
void abilita_interrupt_buzzer(void);
void disabilita_interrupt_buzzer(void);
int stato_interrupt_buzzer(void);
/*
Funzione che abilita l' interrupt del TIM9 e la porta PC3
*/
void abilita_interrupt_buzzer(void){
	RCC_AHB1ENR |= 0x00000004; //Attivo clock su GPIOC
	RCC_APB2ENR |= 0x00010000; //Attivo clock TIM9
	GPIOC_MODER |= 0x00000040; //Attivo PC3 in General Purpose Output Mode
	TIM9_EGR |= 0x00000001; //Reinizializzo il contatore
	TIM9_PSC = 0xFFFF; //Clock del TIM10 --> SystemClock/65536
	long arr = (long)(((valore_clock() * 1000000)/65536) * 0.5); //Imposto l' interrupt ogni 0.5 secondi
	TIM9_ARR =arr;
	NVIC_ISER0 |=0x01000000; //Abilito interrupt del TIM9 sul NVIC (posizione 24)
	GPIOC_ODR |= 0x00000008; //Valore alto PC3
	TIM9_DIER |= 0x0001; //Abilito interrupt
	TIM9_CR1 |= 0x0081; //Abilito contatore e attivo autoreload
}
/*
Funzione che legge lo stato dell' interrupt del TIM9.
Ritorna 1 se l' interrupt è attiva, altrimenti 0
*/
int stato_interrupt_buzzer(void){
	if((TIM9_DIER & 0x0001) == 0x0001){
		return 1;
	}
	else{
		return 0;
	}
}
/*
Funzione che disabilita l' interrupt del TIM9
*/
void disabilita_interrupt_buzzer(void){
	TIM9_DIER &= 0xFFFE;	
	TIM9_SR &= 0xFFFE;
	GPIOC_ODR &= 0xFFFFFFF7;
}

void TIM1_BRK_TIM9_IRQHandler(void){
	if((TIM9_SR & 0x0001) == 0x0001){
		GPIOC_ODR = ~GPIOC_ODR;
	}
	TIM9_SR &=0xFFFE;
}
