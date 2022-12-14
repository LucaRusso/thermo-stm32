#include "Prototipi.h"
/*
-----RCC-----
*/
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_APB2ENR *(long*)0x40023844
/*
-----GPIOC-----
*/
#define GPIOC_PUPDR *(long*)0x4002080C
/*
-----NVIC-----
*/
#define NVIC_ISER1 *(long*)0xE000E104
/*
-----EXTI-----
*/
#define EXTI_IMR 	*(long*)0x40013C00
#define EXTI_RTSR *(long*)0x40013C08
#define EXTI_PR 	*(long*)0x40013C14
/*
-----SYSCFG-----
*/
#define SYSCFG_EXTICR4 *(long*)0x40013814
/*
-----Prototipi Funzioni-----
*/
void attiva_pulsante(void);

void attiva_pulsante(void){
	RCC_AHB1ENR |= 0x00000004; //Attvio clock su GPIOC
	RCC_APB2ENR |= 0x00004000; //Attivo clock SYSCFG
	//PC13 già attivo in Input Mode in GPIOC_MODER
	GPIOC_PUPDR |= 0x08000000; //Imposto PC13 in pull-down
	SYSCFG_EXTICR4 |= 0x00000020; //Imposto EXTI13 su PC13
	EXTI_IMR |= 0x00002000; //Abilito Interrupt EXTI13
	EXTI_RTSR |= 0x0002000; //Rising trigger EXTI13
	NVIC_ISER1 |= 0x00000100; //Attivo interrupt sul NVIC (posizione 40)
}
void EXTI15_10_IRQHandler(void){
	if((EXTI_PR & 0x00002000)== 0x00002000){
		if((stato_interrupt_LCD() == 1) && (stato_interrupt_termistore() == 0)){
			disattiva_interrupt_LCD();
			invia_istruzione(0, 0, 0, 0, 0, 0, 0, 0, 0, 1); //Clear display
			attiva_interrupt_termistore();
		}
		else if((stato_interrupt_LCD() == 0) && (stato_interrupt_termistore() == 1)){
			disattiva_interrupt_termistore();
			invia_istruzione(0, 0, 0, 0, 0, 0, 0, 0, 0, 1); //Clear display
			pulisci_stringa(&data_precedente[0]);
			attiva_interrupt_LCD();
		}
		else{
			disattiva_interrupt_termistore();
			invia_istruzione(0, 0, 0, 0, 0, 0, 0, 0, 0, 1); //Clear display
			pulisci_stringa(&data_precedente[0]);
			attiva_interrupt_LCD();
		}
	}
	EXTI_PR |= 0x00002000; //Resetto pending interrupt
}
