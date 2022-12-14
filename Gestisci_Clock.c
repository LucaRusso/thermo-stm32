#include "Prototipi.h"
/*
-----RCC-----
*/
#define RCC_CR 	 		*(long*)0x40023800
#define RCC_PLLCFGR *(long*)0x40023804
#define RCC_CFGR 		*(long*)0x40023808
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_CSR 		*(long*)0x40023874
/*
-----PORTA A-----
*/
#define GPIOA_MODER *(long*)0x40020000
#define GPIOA_AFRH  *(long*)0x40020024

/*
-----Prototipi Funzioni-----
*/
int attiva_hsi(void);
int attiva_pll(int scelta);
void seleziona_clock(int scelta);
int stato_clock(void);
int disabilita_pll(void);
int attiva_lsi(void);
int disattiva_lsi(void);
int valore_clock(void);
/*
Funzione che abilita l' HSI.
Ritorna 1 se l' HSI è abilitato e stabile
*/
int attiva_hsi(void){
	if((RCC_CR & 0x01000000) == 0x01000000){ //Controllo se PLL attivo
		RCC_CR &= 0xFEFFFFFF; //PLL off
	}
	if((RCC_CR & 0x00000001) != 0x00000001){ //Controllo se HSI è attivo
		RCC_CR |= 0x00000001; 
	}
	while((RCC_CR & 0x00000003) != 0x00000003); //Aspetto che HSI è stabile
	cambia_baudrate(16);
	seleziona_clock(0);
	if(!stato_clock()){
		return 1;
	}
	else{
		return 0;
	}
}
/*
Funzione che attiva il PLL
Ritorna 1 se il PLL è stato attivato correttamente, altrimenti 0
*/
int attiva_pll(int scelta){
	//se scelta = 1 --> Clock: 24MHz
	//se scelta = 2 --> Clock: 32MHz
	switch(scelta){
		case 1:
			if(stato_clock()){
				while(!disabilita_pll());
			}	
			RCC_PLLCFGR = 0x24033010;
			break;
		case 2: 
			if(stato_clock()){
				while(!disabilita_pll());
			}	
			RCC_PLLCFGR = 0x24023010;
			break;
		default:
			if(stato_clock()){
				while(!disabilita_pll());
			}	
			attiva_hsi();
			scrivi_usart2("Scelta errata! Frequenza di default: 16MHz");
			break;
	}
	RCC_CR |= 0x01000000; //Abilito PLL
	while(!(RCC_CR & 0x02000000)); //Attendo che il PLL sia pronto
	seleziona_clock(1);
	if(stato_clock()){
		return 1;
	}
	else{
		return 0;
	}
}
/*
Funzione che seleziona il clock per il sistema
*/
void seleziona_clock(int scelta){
	//se scelta = 0 setto HSI, se scelta = 1 setto PLL
	switch(scelta){
		case 0:
			RCC_CFGR &= 0xFFFFFFFC; //Seleziono HSI come System Clock
			break;
		case 1:
			RCC_CFGR |= 0x00000002; //Seleziono PLL come System Clock
			break;
		default:
			scrivi_usart2("Scelta clock errata!");
	}
}
/*
Funzione che ritorna 1 se come System Clock è impostato il PLL, mentre ritorna 0 se è impostato l' HSI
*/
int stato_clock(void){
	if((RCC_CFGR & 0x0000000A) == 0x0000000A){
		return 1;
	}
	else{
		return 0;
	}
}
/*
Funzione che disabilita il PLL
Ritorna 1 se il PLL è stato disattivato correttamente, altrimenti 0
*/
int disabilita_pll(void){
	seleziona_clock(0); //seleziono HSI come System Clock
	RCC_CR &= 0xFEFFFFFF; //Diasbilito PLL
	if((RCC_CR & 0x01000000) == 0x00000000){
		return 1;
	}
	else{
		return 0;
	}
}

/*
Funzione che attiva LSI.
Ritorna 1 se l' LSI è stato attivato correttamente, altrimenti 0
*/
int attiva_lsi(void){
	RCC_CSR |= 0x00000001;
	while(!((RCC_CSR & 0x00000002) == 0x00000002)); //attendo che LSI sia pronto
	if((RCC_CSR & 0x00000002) == 0x00000002){
		return 1;
	}
	else{
		return 0;
	}
}
/*
Funzione che disabilita LSI
Ritorna 1 se LSI disattivato correttamente, altrimenti 0
*/
int disattiva_lsi(void){
	if((RCC_CSR & 0x00000002) == 0x00000002){
		RCC_CSR &= 0xFFFFFFFC;
	}
	if(((unsigned long)RCC_CSR | 0xFFFFFFFC) == 0xFFFFFFFC){
		return 1;
	}
	else{
		return 0;
	}
}
/*
Funzione che ritorna il valore del clock.
Ritorna 16 se clock = 16MHz, 24 se clock = 24MHz etc.
*/
int valore_clock(void){
	if(!stato_clock()){
		return 16;
	}
	else{
		if((RCC_PLLCFGR & 0x24033010) == 0x24033010){
			return 24;
		}
		else if((RCC_PLLCFGR & 0x24023010) == 0x24023010){
			return 32;
		}
		else{
			return 0;
		}
	}
}
