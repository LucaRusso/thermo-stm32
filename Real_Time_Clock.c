#include "Prototipi.h"
/*
-----RCC-----
*/
#define RCC_APB1ENR	*(long*)0x40023840
#define RCC_BDCR 		*(long*)0x40023870
/*
-----PWR-----
*/
#define PWR_CR *(long*)0x40007000
/*
-----RTC-----
*/
#define RTC_TR 				*(long*)0x40002800 
#define RTC_DR 				*(long*)0x40002804
#define RTC_CR 				*(long*)0x40002808 
#define RTC_ISR 			*(long*)0x4000280C 
#define RTC_PRER 			*(long*)0x40002810 
#define RTC_CALIBR 		*(long*)0x40002818
#define RTC_ALRMAR 		*(long*)0x4000281C 
#define RTC_ALRMBR 		*(long*)0x40002820 
#define RTC_WPR 			*(long*)0x40002824 
#define RTC_SSR 			*(long*)0x40002828 
#define RTC_ALRMASSR  *(long*)0x40002844 
#define RTC_ALRMBSSR  *(long*)0x40002848 
	
/*
-----Prototipi funzioni-----
*/
void abilita_rtc(void);
void leggi_data_ora(char data[], char ora[]);
void leggi_datetime(char datetime[]);
void imposta_datetime(void);
int riconosci_giorno(char *stringa);
long hex_giorno (int giorno);
/*
Funzione che ablita l' RTC
*/
void abilita_rtc(void){
	attiva_lsi();
	RCC_APB1ENR |= 0x10000000; //Abilito clock per PWR
	PWR_CR |= 0x00000100; //Sblocca la scrittura del RCC_BDCR
	RCC_BDCR |= 0x00008200; //Abilito RTC e imposto come suo clock source l' LSI
	RTC_WPR = 0x000000CA;	//Protection key			}
	RTC_WPR = 0x00000053; //Protection key			} Sblocca la protezione sulla scrittura di tutti i registri
	RTC_ISR |= 0x00000080; //Entro in modalità di inizializzazione
	while((RTC_ISR & 0x00000040) != 0x00000040); //Attendo che sia attivata la modalità di inizializzazione
	RTC_PRER = 0x000000FF; //Imposto il synchronous presacler a 256				}
	RTC_PRER |= 0x007C0000; //Imposto l' asynchronous prescaler a 125 		} --> LSI = 32 kHZ --> 32kHz/(256 * 125) = 1Hz
	RTC_TR = 0x00173000; //Impostato il tempo a 17:30:00
	RTC_DR = 0x00207229; //Impostato data a Martedì 29-12-20
	//RTC_CR impostato bene di default
	RTC_ISR &= 0xFFFFFF7F; //Esco dalla modalità di inizializzazione
}
/*
Funzione che restituisce data e ora in due sringhe separate
*/
void leggi_data_ora(char data[], char ora[]){
	pulisci_stringa(&data[0]);
	pulisci_stringa(&ora[0]);
	while((RTC_ISR & 0x00000020) != 0x00000020); //Attendo che vengano aggiornati i registri del calendario
	char tem[3];
	//Leggo l' ora riportata nei registri dell' RTC
	int hours = (10 * ((RTC_TR & 0x00300000) >> 20)) + ((RTC_TR & 0x000F0000) >> 16);
	int minutes = (10 * ((RTC_TR & 0x00007000) >> 12)) + ((RTC_TR & 0x00000F00) >> 8);
	int seconds = (10 * ((RTC_TR & 0x00000070) >> 4)) + (RTC_TR & 0x0000000F);
	int wdu = ((RTC_DR & 0x0000E000) >> 13);
	int year = (10 * ((RTC_DR & 0x00F00000) >> 20)) + ((RTC_DR & 0x000F0000) >> 16);
	int month = (10 * ((RTC_DR & 0x00001000) >> 12)) + ((RTC_DR & 0x00000F00) >> 8);
	int day = (10 * ((RTC_DR & 0x00000030) >> 4)) + (RTC_DR & 0x0000000F);
	//Aggiusto la formattazione dell' ora per essere sicuro che vengano rappresentati sempre lo stesso
	//numero di caratteri
	if(hours <= 9){
		tem[0] = '0';
		tem[1] = (char) hours + '0';
		tem[2] = '\0';
	}
	else{
		int_to_char(hours, &tem[0]);
	}
	concat_due_stringhe(ora, tem, ora);
	if(minutes <=9){
		tem[0] = '0';
		tem[1] = (char) minutes + '0';
		tem[2] = '\0';
	}
	else{
		int_to_char(minutes, &tem[0]);
	}
	concat_due_stringhe(ora, ":", ora);
	concat_due_stringhe(ora, tem, ora);
	if(seconds <= 9){
		tem[0] = '0';
		tem[1] = (char) seconds + '0';
		tem[2] = '\0';
	}
	else{
		int_to_char(seconds, &tem[0]);
	}
	concat_due_stringhe(ora, ":", ora);
	concat_due_stringhe(ora, tem, ora);
	switch(wdu){
		case 1:
			concat_due_stringhe(data, "LUN ", data);
			break;
		case 2:
			concat_due_stringhe(data, "MAR ", data);
			break;
		case 3:
			concat_due_stringhe(data, "MER ", data);
			break;
		case 4:
			concat_due_stringhe(data, "GIO ", data);
			break;
		case 5:
			concat_due_stringhe(data, "VEN ", data);
			break;
		case 6:
			concat_due_stringhe(data, "SAB ", data);
			break;
		case 7:
			concat_due_stringhe(data, "DOM ", data);
			break;
		default:
			concat_due_stringhe(data, "    ", data);
	}
	//Aggiusto formattazione della data come per l' ora
	if(day <= 9){
		tem[0] = '0';
		tem[1] = (char) day + '0';
		tem[2] = '\0';
	}
	else{
		int_to_char(day, &tem[0]);
	}
	concat_due_stringhe(data, tem, data);
	if(month <= 9){
		tem[0] = '0';
		tem[1] = (char) month + '0';
		tem[2] = '\0';
	}
	else{
		int_to_char(month, &tem[0]);
	}
	concat_due_stringhe(data, "-", data);
	concat_due_stringhe(data, tem, data);
	if(year <= 9){
		tem[0] = '0';
		tem[1] = (char) year + '0';
		tem[2] = '\0';
	}
	else{
		int_to_char(year, &tem[0]);
	}
	concat_due_stringhe(data, "-", data);
	concat_due_stringhe(data, tem, data);
}
/*
Funzione che restituisce data e ora nel formato Datetime
*/
void leggi_datetime(char datetime[]){
	pulisci_stringa(&datetime[0]);
	char data[10];
	char ora[15];
	leggi_data_ora(data,ora);
	concat_due_stringhe(ora, " ", datetime);
	concat_due_stringhe(datetime, data, datetime);
}
/*
Funzione che imposta la data e ora del RTC
*/
void imposta_datetime(void){
	char tem[10];
	//Leggo Data
	scrivi_usart2("Inserisci la data corrente (rispetta il seguente formato: GG-MM-AA): ");
	char hex_data[12];
	while(flag_usart == 0);
	flag_usart = 0;
	while(flag_error == 1){
		pulisci_stringa(&risposta_utente[0]);
		flag_error = 0;
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		while(flag_usart == 0);
		flag_usart = 0;
	}
	converti_data(&risposta_utente[0], &tem[0]);
	concat_due_stringhe("0x00", &tem[0], &hex_data[0]);
	pulisci_stringa(&risposta_utente[0]);
	//Leggo Giorno
	scrivi_usart2("Che giorno della settimana e': ");
	while(flag_usart == 0);
	flag_usart = 0;
	while(flag_error == 1){
		pulisci_stringa(&risposta_utente[0]);
		flag_error = 0;
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		while(flag_usart == 0);
		flag_usart = 0;
	}
	int giorno = riconosci_giorno(&risposta_utente[0]);
	pulisci_stringa(&risposta_utente[0]);
	long g = hex_giorno(giorno);
	//Leggo Ora
	scrivi_usart2("Inserisci l' orario corrente (rispetta il seguente formato HH:MM:SS): ");
	char hex_orario[12];
	while(flag_usart == 0);
	flag_usart = 0;
	while(flag_error == 1){
		pulisci_stringa(&risposta_utente[0]);
		flag_error = 0;
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		while(flag_usart == 0);
		flag_usart = 0;
	}
	//Converto i valori letti in codice esadecimale per poter settare i registri dell' RTC
	converti_orario(&risposta_utente[0], &tem[0]);
	concat_due_stringhe("0x00", &tem[0], &hex_orario[0]);
	pulisci_stringa(&risposta_utente[0]);
	long ora = converti_hex_string_to_long(&hex_orario[0]);
	long dat = (converti_hex_string_to_long(&hex_data[0]) + g);
	RTC_ISR |= 0x00000080; //Entro in modalità di inizializzazione
	while((RTC_ISR & 0x00000040) != 0x00000040); //Attendo che sia attivata la modalità di inizializzazione
	RTC_TR = ora;
	RTC_DR = dat;
	RTC_ISR &= 0xFFFFFF7F; //Esco dalla modalità di inizializzazione
}
/*
Funzione che riconosce il giorno inserito
Ritorna l' intero corrispondente al giorno della settimana (es. Lunedì ritorna 1, Giovedì ritorna 4)
Se non viene riconosciuto alcun giorno ritorna 0
*/
int riconosci_giorno(char *stringa){
	int count = 0;
	char tem[3];
	while(*stringa != '\0'){
		//To Upper Case
		if(((int)(*stringa) >= 97) && ((int)(*stringa) <= 122)){
			*stringa -= 32;
		}
		stringa++;
		count++;
	}
	stringa -= count;
	//Prendo solo le prime due lettere, sufficienti per avere una distinzione tra tutti i giorni della settimana
	//e fare un controllo più veloce
	for(int i = 0; i < 2; i++){
		tem[i] = *stringa;
		stringa++;
	}
	tem[2] = '\0';
	if(confronta_stringhe(tem, "LU")){
		return 1;
	}
	else if(confronta_stringhe(tem, "MA")){
		return 2;
	}
	else if(confronta_stringhe(tem, "ME")){
		return 3;
	}
	else if(confronta_stringhe(tem, "GI")){
		return 4;
	}
	else if(confronta_stringhe(tem, "VE")){
		return 5;
	}
	else if(confronta_stringhe(tem, "SA")){
		return 6;
	}
	else if(confronta_stringhe(tem, "DO")){
		return 7;
	}
	else{
		return 0;
	}
}
/*
Funzione che ritorna il codice esadecimale da scrivere nel registro per settare solo il giorno
*/
long hex_giorno (int giorno){
	switch(giorno){
		case 1:
			return 8192;
		case 2:
			return 16384;
		case 3:
			return 24576;
		case 4:
			return 32768;
		case 5:
			return 40960;
		case 6:
			return 49152;
		case 7:
			return 57344;
		default:
			return 0;
		}
}
