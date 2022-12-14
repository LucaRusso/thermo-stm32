#include "Prototipi.h"
int flag_usart = 0;
int flag_error = 0;
char risposta_utente[15];
/*
-----RCC-----
*/
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_APB1ENR *(long*)0x40023840
/*
-----PORTA A-----
*/
#define GPIOA_MODER 	*(long*)0x40020000
#define GPIOA_AFRL 	*(long*)0x40020020
/*
-----NVIC-----
*/
#define NVIC_ISER1 *(long*)0xE000E104
/*
-----USART2-----
*/
#define USART_SR 	*(long*)0x40004400 
#define USART_DR 	*(long*)0x40004404
#define USART_BRR *(long*)0x40004408
#define USART_CR1 *(long*)0x4000440C
/*
-----Prototipi Funzioni-----
*/
int stato_usart2(void);
int abilita_usart2(void);
void cambia_baudrate(int MHz);
void scrivi_usart2(char *stringa);
void leggi_usart2(char *stringa);

/*
Funzione per leggere lo stato dell' USART. 
- Ritorna 1 se USART, TE e RE e Interrupt attiv
- Ritorna 0 se USART disattivo
*/
int stato_usart2(void){
	if((USART_CR1 & 0x0000202C ) == 0x0000202C){
		return 1; //USART attivo, Transmitter e Receiver attivo
	}
	else{
		return 0; //USART non attivo
	}
}

int abilita_usart2(void){
	/*
	-----RCC-----
	*/
	RCC_AHB1ENR |= 0x00000001; //Abilito RCC GPIOA
	RCC_APB1ENR |= 0x00020000; //Abilito RCC USART2
	/*
	-----GPIOA-----
	*/
	GPIOA_MODER |= 0x000000A0; //Setto PA2 e PA3 in Alternate Function Mode
	GPIOA_AFRL |= 0x0007700; //Setto PA2 e PA3 in AF07
	/*
	-----USART-----
	*/
	USART_CR1 |= 0x0000202C;// USART, TE e RE enabled. Interrupt Receiver attiva
	NVIC_ISER1 |= 0x00000040; //Abilito Interrupt USART2 su NVIC (posizione 38)
	if((USART_CR1 & 0x0000202C) == 0x0000202C){
		return 1; 
	}
	else{
		return 0;
	}
}
/*
Funzione per cambiare il baudrate in base al SysClock
*/
void cambia_baudrate(int MHz){
	if(MHz == 16){
		if(stato_usart2() == 0){
			while(!(abilita_usart2()));
		}
		USART_BRR = 0x00000683; //Impostato Baud Rate a circa 9.6 kBps
	}
	else if(MHz == 24){
		if(stato_usart2() == 0){
			while(!(abilita_usart2()));
		}
		USART_BRR =0x000009C4; //Imposto Baud Rate a circa 9.6 kBps
	}
	else if(MHz == 32){
		if(stato_usart2() == 0){
			while(!(abilita_usart2()));
		}
		USART_BRR =0x00000D05; //Imposto Baud Rate a circa 9.6 kBps
	}
	else if(MHz == 42){
		if(stato_usart2() == 0){
			while(!(abilita_usart2()));
		}
		USART_BRR =0x00001117; //Imposto Baud Rate a circa 9.6 kBps
	}
}

void scrivi_usart2(char *stringa){
	while(*stringa != '\0'){
	USART_DR = *stringa;
	while((USART_SR & 0x000000C0) != 0x000000C0); //Attendo che il dato sia stato inviato
	stringa++;
	}
	USART_SR &= 0xFFFFFFBF;
}

void leggi_usart2(char *stringa){
	int cont = 0;
	while(USART_DR != '\0'){
		USART_SR &= 0xFFFFFF9F;
		*stringa = (char)USART_DR;
		USART_DR = *stringa;
		while((USART_SR & 0x000000C0) != 0x000000C0); //Attendo che il dato sia stato inviato
		stringa++;
		cont++;
		if(cont >= 15){
			flag_error = 1;
			break;
		}
	}
	*stringa = '\0';
	flag_usart = 1;
}
void USART2_IRQHandler(void){
	if((USART_SR & 0x00000020) == 0x00000020){
		while(flag_usart == 0){
			leggi_usart2(&risposta_utente[0]);
		}
		scrivi_usart2("\n\r");
	}
}
