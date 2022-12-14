#include "Prototipi.h"
char data_precedente[15];
/*
-----RCC-----
*/
#define RCC_AHB1ENR *(long*)0x40023830
#define RCC_APB2ENR *(long*)0x40023844
/*
-----GPIO-----
*/
#define GPIOA_MODER 	*(long*)0x40020000
#define GPIOA_OSPEEDR *(long*)0x40020008	
#define GPIOA_ODR 		*(long*)0x40020014	

#define GPIOB_MODER 	*(long*)0x40020400
#define GPIOB_OSPEEDR *(long*)0x40020408
#define GPIOB_ODR 		*(long*)0x40020414
/*
-----TIM11-----
*/
#define TIM11_CR1	 	*(long*)0x40014800
#define TIM11_DIER 	*(long*)0x4001480C
#define TIM11_SR		*(long*)0x40014810
#define TIM11_EGR		*(long*)0x40014814
#define TIM11_CNT		*(long*)0x40014824
#define TIM11_PSC		*(long*)0x40014828
#define TIM11_ARR		*(long*)0x4001482C
	/*
-----NVIC-----
*/
#define NVIC_ISER0 *(long*)0xE000E100	
/*
-----Prototipi Funzioni
*/
void inizializza_LCD(void);
void invia_istruzione(int rs, int rw, int d7, int d6, int d5, int d4, int d7_2, int d6_2,int d5_2, int d4_2);
void setta_schermo_4bit_data(void);
void scrivi_stringa_LCD(char *stringa);
void attiva_interrupt_LCD(void);
void disattiva_interrupt_LCD(void);
int stato_interrupt_LCD(void);
/*
Funzione che inizializza l' LCD
*/
void inizializza_LCD(void){
	RCC_AHB1ENR |= 0x00000003; //Attivo RCC su GPIOA e GPIOB
	GPIOA_MODER |= 0x01550000; //Attivo PA8/PA9/PA10/PA11/PA12 in General Purpose Output Mode
	GPIOB_MODER |= 0x00100400; //Attivo PB5/PB10 in General Purpose Output Mode
	//Inizializzazione
	GPIOA_ODR |= 0x00001800; //Imposto alto D5/D4
	GPIOB_ODR |= 0x00000400;//Imposto alto Enable per fargli acquisire le informazioni
	delay_LCD();
	GPIOB_ODR &= 0xFFFFFBFF; //Imposto basso Enable
	delay_LCD();
	GPIOB_ODR |= 0x00000400;//Imposto alto Enable per fargli acquisire le informazioni
	delay_LCD();	
	GPIOB_ODR &= 0xFFFFFBFF; //Imposto basso Enable
	delay_LCD();
	GPIOB_ODR |= 0x00000400;//Imposto alto Enable per fargli acquisire le informazioni
	delay_LCD();	
	GPIOB_ODR &= 0xFFFFFBFF; //Imposto basso Enable
	delay_LCD();
	GPIOA_ODR &= 0xFFFFE7FF; //Imposto basso D5/D4
}
/*
Funzione che invia l' istruzione inserita in input al metodo all' LCD
*/
void invia_istruzione(int rs, int rw, int d7, int d6, int d5, int d4, int d7_2, int d6_2,int d5_2, int d4_2){
	#define MASK_ENABLE 	0x00000400
	#define MASK_DISABLE 	0xFFFFFBFF
	#define MASK_RESET_A 	0xFFFFE0FF
	#define MASK_RESET_B 	0xFFFFF8DF
	//Prima istruzione da 6 bit
	//RS RW D7 D6 D5 D4
	long set_pin_A = ((d4 * potenza(2,12)) + (d5 * potenza(2,11)) + (d6 * potenza(2,10)) + (rs * potenza(2,9)) + (rw * potenza(2,8)));
	GPIOA_ODR |= set_pin_A;
	long set_pin_B = (d7 * potenza(2,5));
	GPIOB_ODR |= set_pin_B;
	GPIOB_ODR |= MASK_ENABLE; //Alzo PIN Enable
	delay_LCD(); //Delay
	GPIOB_ODR &= MASK_DISABLE; // Abbasso PIN Enable
	delay_LCD();
	GPIOA_ODR &= MASK_RESET_A;//Resetto PIN Porta A
	GPIOB_ODR &= MASK_RESET_B;//Resetto PIN Porta B
	//Seconda istruzione da 6 bit
	//RS RW D7_2 D6_2 D5_2 D4_2
	set_pin_A = ((d4_2 * potenza(2,12)) + (d5_2 * potenza(2,11)) + (d6_2 * potenza(2,10)) + (rs * potenza(2,9)) + (rw * potenza(2,8)));
	GPIOA_ODR |= set_pin_A;
	set_pin_B = (d7_2 * potenza(2,5));
	GPIOB_ODR |= set_pin_B;
	GPIOB_ODR |= MASK_ENABLE; //Alzo PIN Enable
	delay_LCD(); //Delay
	GPIOB_ODR &= MASK_DISABLE; // Abbasso PIN Enable
	delay_LCD();
	GPIOA_ODR &= MASK_RESET_A;//Resetto PIN Porta A
	GPIOB_ODR &= MASK_RESET_B;//Resetto PIN Porta B
}
/*
Funzione che setta i parametri principali dell' LCD, come il numero delle linee del display,
se visualizzare o no il cursore, etc.
*/
void setta_schermo_4bit_data(void){
	inizializza_LCD();
	GPIOA_ODR |= 0x00000800; //Alzo Pin D5
	GPIOB_ODR |= 0x00000400; //Alzo PIN Enable
	delay_LCD(); //Delay
	GPIOB_ODR &= 0xFFFFFBFF; // Abbasso PIN Enable
	GPIOB_ODR &= 0xFFFFF7FF;//Abbasso PIN D5
	delay_LCD(); //Delay
	//Function Set --> 00001010--
	invia_istruzione(0, 0, 0, 0, 1, 0, 1, 0, 0, 0); //Data Lenght: 4 bit, Display Lines: 2, Characacter Font: 5 x 8
	//Display ON/OFF Control --> 0000001000
	invia_istruzione(0, 0, 0, 0, 0, 0, 1, 0, 0, 0); //Display OFF, Cursor OFF, Blinking OFF
	//Clear Display --> 0000000001
	invia_istruzione(0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	//Entry Mode Set --> 0000000110
	invia_istruzione(0, 0, 0, 0, 0, 0, 0, 1, 1, 0); //Cursore verso destra, Shift OFF
	//Display ON/OFF Control --> 0000001110
	invia_istruzione(0, 0, 0, 0, 0, 0, 1, 1, 0, 0); //Display ON, Cursor OFF, Blinking OFF
}
/*
Funzione che scrive la stringa sull' LCD
*/
void scrivi_stringa_LCD(char *stringa){
	int carattere[8];
	int *punt_car = &carattere[0];
	while(*stringa != '\0'){
		//Setto a 0 tutti i bit dell'array "carattere"
		punt_car = &carattere[0];
		for(int i = 0; i < 8; i++){
			*punt_car = 0;
			punt_car++;
		}
		//Calcolo il codice ascii del carattere corrente e lo invio all'LCD per stamparlo
		punt_car = &carattere[7];
		leggi_ascii_carattere(*stringa, punt_car);
		invia_istruzione(1, 0, carattere[0], carattere[1], carattere[2], carattere[3], carattere[4], carattere[5], carattere[6], carattere[7]);
		stringa++;
	}
}
/*
Funzione che attiva l' interrupt dell TIM11 per gestire la stampa 
della temparatura sull LCD
*/
void attiva_interrupt_LCD(void){
	RCC_APB2ENR |= 0x00040000; //Abilito clocksu TIM11
	TIM11_EGR |= 0x00000001; //Reinizializzo il contatore
	TIM11_PSC = 0xFFFF; //Clock del TIM10 --> SystemClock/65536
	long arr = (long)(((valore_clock() * 1000000)/65536)); //Imposto l' interrupt ad 1 secondo
	TIM11_ARR =arr;
	NVIC_ISER0 |=0x04000000; //Abilito interrupt del TIM11 sul NVIC (posizione 26)
	TIM11_DIER |= 0x0001; //Abilito interrupt
	TIM11_CR1 |= 0x0081; //Abilito contatore e attivo autoreload
}
/*
Funzione che disattiva interrupt del TIM11
*/
void disattiva_interrupt_LCD(void){
	TIM11_DIER &= 0xFFFE;
	TIM11_SR &= 0xFFFE;
	TIM11_CR1 &= 0xFFFE;
	NVIC_ISER0 &= 0xFBFFFFFF;
}
/*
Funzione che ritorna lo stato dell' interrupt del TIM11
Ritorna 1 se l' interrupt è abilitata, altrimenti 0
*/
int stato_interrupt_LCD(void){
	if((TIM11_DIER & 0x0001) == 0x0001){
		return 1;
	}
	else{
		return 0;
	}
}

void TIM1_TRG_COM_TIM11_IRQHandler(void){
	if((TIM11_SR & 0x0001) == 0x0001){
		scrivi_data_ora_LCD(data_precedente);
		TIM11_SR &= 0xFFFE;
	}
}
