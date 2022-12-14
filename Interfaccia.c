#include "Prototipi.h"
double temp = 0;
/*
-----TIM-----
*/
#define TIM2_ARR		*(long*)0x4000002C
#define TIM9_ARR		*(long*)0x4001402C
#define TIM11_ARR		*(long*)0x4001482C
#define TIM3_ARR		*(long*)0x4000042C
/*
-----Prototipi Funzioni-----
*/
void scrivi_data_ora_LCD(char data_prec[]);
void scrivi_temperatura_LCD(void);
void start(void);
void menu_principale(void);
void menu_frequenza(void);
void menu_rtc(void);
void menu_led2(void);
void aggiorna_clock(void);
/*
Funzione che scrive ora e data sul LCD
*/
void scrivi_data_ora_LCD(char data_prec[]){
	char ora[10];
	char data[15];
	char ora_LCD[16];
	char data_LCD[20];
	leggi_data_ora(data, ora);
	concat_due_stringhe("    ", ora, ora_LCD);
	concat_due_stringhe("  ", data, data_LCD);
	invia_istruzione(0, 0, 1, 0, 0, 0, 0, 0, 0, 0); //Cursore in posizione iniziale
	scrivi_stringa_LCD(&ora_LCD[0]);
	//Se la data è rimasta uguale non la riscrivo, così da risparmiare tempo
	if(!(confronta_stringhe(data_prec, data))){
		invia_istruzione(0, 0, 1, 1, 0, 0, 0, 0, 0, 0); //Cursore A Capo
		scrivi_stringa_LCD(&data_LCD[0]);
		int i = 0;
		while(data[i] != '\0'){
			data_prec[i] = data[i];
			i++;
		}
		data_prec[i] = '\0';
	}
}
/*
Funzione che scrive la temperatura sul LCD
*/
void scrivi_temperatura_LCD(void){
	char temperatura[16];
	pulisci_stringa(&temperatura[0]);
	char t[4];
	int b = 0;
	int w = 0;
	concat_due_stringhe(temperatura, "TEMPERATURA:", temperatura);
	int_to_char((int)temp, &t[0]);
	concat_due_stringhe(temperatura, t, temperatura);
	concat_due_stringhe(temperatura, " C", temperatura);
	invia_istruzione(0, 0, 1, 0, 0, 0, 0, 0, 0, 0); //Cursore in posizione iniziale
	scrivi_stringa_LCD(&temperatura[0]);
	invia_istruzione(0, 0, 1, 1, 0, 0, 0, 0, 0, 0); //Cursore A Capo
	//Riempio seconda riga come illustrato nel manuale utente
	if(temp > 14 && temp < 30){
		b = (int)temp - 14;
	}
	w = 16 - b;
	for(int i = 0; i < b; i++){
		invia_istruzione(1,0, 1, 1, 1, 1, 1, 1, 1, 1); //Riempio spazi 
	}
	for(int i = 0; i < b; i++){
		scrivi_stringa_LCD(" ");
	}
	if(temp > 30){
		invia_istruzione(0, 0, 1, 1, 0, 0, 0, 0, 0, 0); //Porto il cursore ad inizio seconda riga
		scrivi_stringa_LCD("   ALLARME!!!   ");
	}
}
/*
Funzione che avvia il microcontrollore con i settaggi preimpostati
*/
void start(void){
	while(!attiva_hsi()); //Mi assicuro che il clock sia a 16MHz e che sia stabile
	while(!abilita_fpu()); //Abilito Floating Point Unit
	setta_schermo_4bit_data(); //Avvio LCD
	abilita_rtc(); //Abilito Real Time Clock
	attiva_pulsante(); //Attivo funzionamento pulsante USER (blu)
	attiva_lettura_termistore(); //Attivo la lettura del segnale del termistore
	while(!abilita_usart2()); //Abilito l' usart
	scrivi_usart2("Connessione stabilita correttamente\n\r");
	attiva_interrupt_LCD(); //Avvio l'interrupt che scriva data e ora corrente sull'LCD
	temp = leggi_temperatura() - 273.15; 
	attiva_interrupt_monitoraggio(); //Avvia l'interrupt per monitorare la temperatura
}
/*
Funzione che stampa sulla seriale il menù principale
*/
void menu_principale(void){
	scrivi_usart2("L' attuale frequenza di clock e': ");
	char clock[3];
	int_to_char(valore_clock(),&clock[0]);
	scrivi_usart2(&clock[0]);
	scrivi_usart2("MHz \n\r");
	scrivi_usart2("Menu Principale\n\r");
	scrivi_usart2("1. Frequenza di clock \n\r2. Data e Ora \n\r3. LED \n\r");
	scrivi_usart2("Per fare la tua scelta invia il numero corrispondente al menu' desiderato: ");
	//Attendo che l'utente scriva qualcosa
	int prosegui = 0;
	while(flag_usart == 0);
	flag_usart = 0;
	//Gestisco proabili errori di lettura dalla seriale
	while(flag_error == 1){
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		flag_error = 0;
		pulisci_stringa(&risposta_utente[0]);
		while(flag_usart == 0);
		flag_usart = 0;
	}
	//Controllo la risposta inserita dall'utente
	while(prosegui == 0){
		if(risposta_utente[0] == '1'){
			pulisci_stringa(&risposta_utente[0]);
			menu_frequenza();
			prosegui = 1;
		}
		else if(risposta_utente[0] == '2'){
			pulisci_stringa(&risposta_utente[0]);
			menu_rtc();
			prosegui = 1;
		}
		else if(risposta_utente[0] == '3'){
			pulisci_stringa(&risposta_utente[0]);
			menu_led2();
			prosegui = 1;
		}
		else{
			scrivi_usart2("Hai inserito un valore errato! Riprova: ");
			pulisci_stringa(&risposta_utente[0]);
			while(flag_usart == 0);
			flag_usart = 0;
		}
	}
}
/*
Funzione che stampa sulla seriale il menù per la selezione della frequenza di clock
*/
void menu_frequenza(void){
	scrivi_usart2("Menu frequenza\n\r");
	scrivi_usart2("Scegli tra le seguenti frequenze: \n\r");
	scrivi_usart2("1. 16MHz\n\r2. 24MHz\n\r3. 32MHZ\n\r");
	scrivi_usart2("Per fare la tua scelta invia il numero corrispondente alla frequenza desiderata: ");
	int prosegui = 0;
	//Attendo che l'utente scriva qualcosa
	while(flag_usart == 0);
	flag_usart = 0;
	//Controllo che non ci siano problemi di lettura 
	while(flag_error == 1){
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		flag_error = 0;
		pulisci_stringa(&risposta_utente[0]);
		while(flag_usart == 0);
		flag_usart = 0;
	}
	//Controllo risposta utente
	while(prosegui == 0){
		if(risposta_utente[0] == '1'){
			pulisci_stringa(&risposta_utente[0]);
			attiva_hsi();
			prosegui = 1;
		}
		else if(risposta_utente[0] == '2'){
			pulisci_stringa(&risposta_utente[0]);
			attiva_pll(1);
			prosegui = 1;
		}
		else if(risposta_utente[0] == '3'){
			pulisci_stringa(&risposta_utente[0]);
			attiva_pll(2);
			prosegui = 1;
		}
		else{
			scrivi_usart2("Hai inserito un valore errato! Riprova: ");
			pulisci_stringa(&risposta_utente[0]);
			while(flag_usart == 0);
			flag_usart = 0;
		}
	}
	aggiorna_clock();
}
/*
Funzione che permette di cambiare ora e data
*/
void menu_rtc(void){
	scrivi_usart2("Menu Data/Ora\n\r");
	scrivi_usart2("1. Cambia Data/Ora\n\r2. Torna al menu' principale\n\r");
	scrivi_usart2("Per fare la tua scelta invia il numero corrispondente alla funzione desiderata: ");
	int prosegui = 0;
	while(flag_usart == 0);
	flag_usart = 0;
	while(flag_error == 1){
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		flag_error = 0;
		pulisci_stringa(&risposta_utente[0]);
		while(flag_usart == 0);
		flag_usart = 0;
	}
	while(prosegui == 0){
		if(risposta_utente[0] == '1'){
			pulisci_stringa(&risposta_utente[0]);
			disattiva_interrupt_LCD();
			imposta_datetime();
			attiva_interrupt_LCD();
			prosegui = 1;
		}
		else if(risposta_utente[0] == '2'){
			pulisci_stringa(&risposta_utente[0]);
			prosegui = 1;
		}
		else{
			scrivi_usart2("Hai inserito un valore errato! Riprova: ");
			pulisci_stringa(&risposta_utente[0]);	
			while(flag_usart == 0);
			flag_usart = 0;
		}
	}
}
/*
Funzione che permette di settare il funzionamento del led2
*/
void menu_led2(void){
	scrivi_usart2("Menu Led\n\r");
	scrivi_usart2("Scegli tra le seguenti modalita' di funzionamento per il Led: \n\r");
	scrivi_usart2("1. Led acceso fisso\n\r2. Led spento\n\r3. Led lampeggiante\n\r");
	scrivi_usart2("Per fare la tua scelta invia il numero corrispondente alla funzione desiderata: ");
	int prosegui = 0;
	while(flag_usart == 0);
	flag_usart = 0;
	while(flag_error == 1){
		scrivi_usart2("Errore di lettura. Per favore riprova: ");
		flag_error = 0;
		pulisci_stringa(&risposta_utente[0]);
		while(flag_usart == 0);
		flag_usart = 0;
	}
	while(prosegui == 0){
		if(risposta_utente[0] == '1'){
			pulisci_stringa(&risposta_utente[0]);
			led2_ON();
			prosegui = 1;
		}
		else if(risposta_utente[0] == '2'){
			pulisci_stringa(&risposta_utente[0]);
			led2_OFF();
			prosegui = 1;
		}
		else if(risposta_utente[0] == '3'){
			pulisci_stringa(&risposta_utente[0]);
			scrivi_usart2("Inserisci il tempo, in secondi, del lampeggio(Inserisci un numero intero compreso tra 1 e 100): ");
			while(flag_usart == 0);
			flag_usart = 0;
			int val = char_to_int(&risposta_utente[0]);
			while(val < 1 || val > 100){
				scrivi_usart2("Hai inserito un valore non ammesso. Per favore riprova: ");
				pulisci_stringa(&risposta_utente[0]);
				while(flag_usart == 0);
				flag_usart = 0;
				val = char_to_int(&risposta_utente[0]);
				flag_usart = 0;
			}
			pulisci_stringa(&risposta_utente[0]);
			lampeggia_Led(val);
			prosegui = 1;
		}
		else{
			scrivi_usart2("Hai inserito un valore errato! Riprova: ");
			pulisci_stringa(&risposta_utente[0]);
			while(flag_usart == 0);
			flag_usart = 0;
		}
	}
}
/*
Funzione che aggiorna i valori settati rispetto al clock selezionato
*/
void aggiorna_clock(void){
	cambia_baudrate(valore_clock());//Aggiorno Baud Rate
	long arr = 0;
	//Aggiorno valore arr dei vari TIM
	//Interrupt Termistore
	arr = (long)((valore_clock() * 1000000)/65536); //Imposto l' interrupt ogni secondo
	TIM2_ARR =arr;
	arr = (long)(((valore_clock() * 1000000)/65536) * 3); //Imposto l' interrupt ogni 3 secondi
	TIM3_ARR =arr;
	//Interrupt LCD
	arr = (long)((valore_clock() * 1000000)/65536); //Imposto l' interrupt secondo
	TIM11_ARR =arr;
	//Interrupt Buzzer
	arr = (long)(((valore_clock() * 1000000)/65536) * 0.5); //Imposto l' interrupt ogni 0.5 secondi
	TIM9_ARR =arr;
	
	led2_OFF(); //spengo led.
}
