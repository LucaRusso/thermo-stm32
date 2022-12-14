#include "Prototipi.h"
/*
-----RCC-----
*/
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_APB1ENR *(long*)0x40023840
#define RCC_APB2ENR *(long*)0x40023844
/*
-----GPIO-----
*/
#define GPIOB_MODER *(long*)0x40020400
/*
-----ADC-----
*/
#define ADC1_SR 		*(long*)0x40012000
#define ADC1_CR2 		*(long*)0x40012008
#define ADC1_SMPR2 	*(long*)0x40012010
#define ADC1_SQR3 	*(long*)0x40012034
#define ADC1_DR 		*(long*)0x4001204C
/*
-----TIM2-----
*/
#define TIM2_CR1	 	*(long*)0x40000000
#define TIM2_DIER 	*(long*)0x4000000C
#define TIM2_SR			*(long*)0x40000010
#define TIM2_EGR		*(long*)0x40000014
#define TIM2_CNT		*(long*)0x40000024
#define TIM2_PSC		*(long*)0x40000028
#define TIM2_ARR		*(long*)0x4000002C
/*
-----TIM3-----
*/
#define TIM3_CR1	 	*(long*)0x40000400
#define TIM3_DIER 	*(long*)0x4000040C
#define TIM3_SR			*(long*)0x40000410
#define TIM3_EGR		*(long*)0x40000414
#define TIM3_CNT		*(long*)0x40000424
#define TIM3_PSC		*(long*)0x40000428
#define TIM3_ARR		*(long*)0x4000042C
/*
-----NVIC-----
*/
#define NVIC_ISER0 *(long*)0xE000E100
/*
-----Prototipi Funzione-----
*/
void attiva_lettura_termistore(void);
double leggi_valore_termistore(void);
double leggi_temperatura(void);
void valore_temperatura_filtrato(void);
void monitora_temperatura(void);
void attiva_interrupt_termistore(void);
void disattiva_interrupt_termistore(void);
int stato_interrupt_termistore(void);
void attiva_interrupt_monitoraggio(void);
/*
Funzione che attiva la lettura del segnale del termistore e la conversione ADC
*/
void attiva_lettura_termistore(void){
	RCC_AHB1ENR |= 0x00000002; //Abilito RCC su GPIOB
	RCC_APB2ENR |= 0x00000100; //Abilito clock ADC1
	GPIOB_MODER |= 0x00000003; //Attivo PB0 in Analog Mode
	//in ADC_SQR1 settato di default ad una conversione
	ADC1_SQR3 |= 0x00000008; //Setto la conversione sul ADC1_IN8
	ADC1_SMPR2 |= 0x07000000; //Imposto il sample del ADC1_IN18 a 480 cicli
	ADC1_CR2 |= 0x00000003; //ADC attivato, Continous Conversation mode attivato
	ADC1_CR2 |= 0x40000000; //Conversion of regular Channel attivata
}
/*
Funzione che restituisce la temperatura letta dal termistore
*/
double leggi_temperatura(void){
	unsigned long V_conv = 0; 
	V_conv = ((unsigned long)ADC1_DR & 0xFFFFFFFF); //Valore restituito dalla conversione A/D
	double V_res = ((0.00081167)*V_conv) - 0.02647693; //Valore della tensione sul termistore
	double R = (10000 * V_res)/(5 - V_res); //valore resistenza termistore
	double invT = 9.20425684E-4 + (2.63029364E-4 * ln(R)) + (2.89615702E-8 * ln(R)*ln(R)*ln(R)); //Equazione di Steinhard-Hart
	double T = 1 / invT;
	return T;
}
/*
Funzione che restituisce il valore della temperatura applicandovi un filtro IIR
passa-basso a singolo polo:
		y[n] = bx[n] + a y[n-1]
dove:			b = 1 - d
					a = d
					x[n] = input
					y[n] = output
					y[n - 1] = output precedente
d --> valore di decadimento
*/
void valore_temperatura_filtrato(void){
	//d = 0.95
	temp = 0.15*(leggi_temperatura()- 273.15) + 0.85*(temp);
}
/*
Funzione che monitora la temperatura
*/
void monitora_temperatura(void){
	valore_temperatura_filtrato();
	if(temp > 30){
		abilita_interrupt_buzzer();
	}
	if(stato_interrupt_buzzer() && (temp <=30)){
		disabilita_interrupt_buzzer();
	}
}
/*
Funzione che attiva l' interrupt dell TIM9 per gestire la stampa 
della temparatura sull LCD
*/
void attiva_interrupt_termistore(void){
	RCC_APB1ENR |= 0x00000002; //Abilito clocksu TIM3
	TIM3_EGR |= 0x00000001; //Reinizializzo il contatore
	TIM3_PSC = 0xFFFF; //Clock del TIM10 --> SystemClock/65536
	long arr = (long)(((valore_clock() * 1000000)/65536) * 3); //Imposto l' interrupt ogni 3 secondi
	TIM3_ARR =arr;
	NVIC_ISER0 |=0x20000000; //Abilito interrupt del TIM3 sul NVIC (posizione 29)
	TIM3_DIER |= 0x0001; //Abilito interrupt
	TIM3_CR1 |= 0x0081; //Abilito contatore e attivo autoreload
}
/*
Funzione che disattiva interrupt del TIM9
*/
void disattiva_interrupt_termistore(void){
	TIM3_DIER &= 0xFFFE;
	TIM3_SR &= 0xFFFE;
	TIM3_CR1 &= 0xFFFE;
	NVIC_ISER0 &= 0xFEFFFFFF;
}
/*
Funzione che attiva il monitoraggio della temperatura
*/
void attiva_interrupt_monitoraggio(void){
	RCC_APB1ENR |= 0x00000001; //Abilito clocksu TIM2
	TIM2_EGR |= 0x00000001; //Reinizializzo il contatore
	TIM2_PSC = 0xFFFF; //Clock del TIM10 --> SystemClock/65536
	long arr = (long)((valore_clock() * 1000000)/65536); //Imposto l' interrupt ogni secondo
	TIM2_ARR =arr;
	NVIC_ISER0 |=0x10000000; //Abilito interrupt del TIM2 sul NVIC (posizione 28)
	TIM2_DIER |= 0x0001; //Abilito interrupt
	TIM2_CR1 |= 0x0081; //Abilito contatore e attivo autoreload
}
/*
Funzione che ritorna lo stato dell' interrupt del TIM11
Ritorna 1 se l' interrupt è abilitata, altrimenti 0
*/
int stato_interrupt_termistore(void){
	if((TIM3_DIER & 0x0001) == 0x0001){
		return 1;
	}
	else{
		return 0;
	}
}

void TIM3_IRQHandler(void){
	if((TIM3_SR & 0x0001) == 0x0001){
		scrivi_temperatura_LCD();
		TIM3_SR &= 0xFFFE;
	}
}
void TIM2_IRQHandler(void){
	if((TIM2_SR & 0x0001) == 0x0001){
		monitora_temperatura();
		TIM2_SR &= 0xFFFE;
	}
}
