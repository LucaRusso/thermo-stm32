#include "Prototipi.h"
/*
-----RCC-----
*/
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_APB2ENR *(long*)0x40023844
/*
-----GPIOA-----
*/
#define GPIOA_MODER 	*(long*)0x40020000
#define GPIOA_OSPEEDR *(long*)0x40020008
#define GPIOA_ODR 		*(long*)0x40020014
/*
-----TIM10-----
*/
#define TIM10_CR1	 	*(long*)0x40014400
#define TIM10_DIER 	*(long*)0x4001440C
#define TIM10_SR		*(long*)0x40014410
#define TIM10_EGR		*(long*)0x40014414
#define TIM10_CNT		*(long*)0x40014424
#define TIM10_PSC		*(long*)0x40014428
#define TIM10_ARR		*(long*)0x4001442C
/*
-----NVIC-----
*/
#define NVIC_ISER0 *(long*)0xE000E100
/*
-----Prototipi Funzioni-----
*/
void led2_ON(void);
void led2_OFF(void);
void lampeggia_Led(int secondi);
void disabilita_lampeggio_led2(void);
/*
Funzione che accende il LED2
*/
void led2_ON(void){
	disabilita_lampeggio_led2();
	if((RCC_AHB1ENR & 0x00000001) != 0x00000001){
		RCC_AHB1ENR |= 0x00000001; //Attivo RCC su GPIOA
	}
	if((GPIOA_MODER & 0x00000400) != 0x00000400){
		GPIOA_MODER |= 0x00000400; //Imposto PA5 in General Purpose Output Mode
	}
	if((GPIOA_OSPEEDR & 0x00000400) != 0x00000400){
		GPIOA_OSPEEDR |= 0x00000400; //Imposto PA5 in Medium Speed
	}
	if((GPIOA_ODR & 0x00000020) != 0x00000020){
		GPIOA_ODR |= 0x00000020; //Alzo PIN PA5
	}
}
/*
Funzione che spegne il LED2
*/
void led2_OFF(void){
	disabilita_lampeggio_led2();
	if((GPIOA_ODR & 0x00000020) == 0x00000020){
		GPIOA_ODR &= 0xFFFFFFDF; //Abbasso PIN PA5
	}
}
/*
Funzione che fa lampeggiare il LED2 ogni tot. secondi indicati
*/
void lampeggia_Led(int secondi){
	led2_ON();
	RCC_APB2ENR |= 0x00020000; //Abilito clocksu TIM10
	TIM10_EGR |= 0x00000001; //Reinizializzo il contatore
	TIM10_SR &= 0xFFFE; //Azzero interrupt generata dalla reinizializzazione del contatore
	TIM10_PSC = 0xFFFF; //Clock del TIM10 --> SystemClock/65536
	double arr = ((double)((valore_clock() * 1000000)/65536) * (double)(secondi / 2.0));
	TIM10_ARR = (long)arr;
	NVIC_ISER0 |=0x02000000; //Abilito interrupt del TIM10 sul NVIC (posizione 25)
	TIM10_DIER |= 0x0001; //Abilito interrupt
	TIM10_CR1 |= 0x0081; //Abilito contatore, attivo autoreload
}
/*
Funzione che disabilta il lampeggio del LED2
*/
void disabilita_lampeggio_led2(void){
	TIM10_DIER &= 0xFFFE;
	TIM10_SR &= 0xFFFE;
	TIM10_CR1 &= 0xFFFE;
	NVIC_ISER0 &= 0xFDFFFFFF;
}

void TIM1_UP_TIM10_IRQHandler(void){
	if((TIM10_SR & 0x0001) == 0x0001){
		GPIOA_ODR = ~GPIOA_ODR;
	}
	TIM10_SR &= 0xFFFE;
}
