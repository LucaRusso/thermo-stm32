/*
-----Variabili globali-----
*/
extern double temp;
extern char data_precedente[15];
extern int flag_usart;
extern char risposta_utente[15];
extern int flag_error;
/*
-----Prototipi Seriale-----
*/
int stato_usart2(void);
int abilita_usart2(void);
void cambia_baudrate(int MHz);
void scrivi_usart2(char *stringa);
void leggi_usart2(char *stringa);
/*
-----Prototipi Gestisci Clock-----
*/
int attiva_hsi(void);
int attiva_pll(int scelta);
int disabilita_pll(void);
int attiva_lsi(void);
int disattiva_lsi(void);
int valore_clock(void);
/*
-----Prototipi Utility-----
*/
void int_to_char(int integer, char *string);
void concat_due_stringhe(char str1[], char str2[], char results[]);
void pulisci_stringa(char *stringa);
void converti_orario(char *stringa, char *result);
void converti_data(char *stringa, char *result);
long potenza(int base, int esp);
long converti_hex_string_to_long(char *stringa);
int confronta_stringhe(char str1[], char str2[]);
void leggi_ascii_carattere(char car, int *ascii_code);
void delay_LCD(void);
double esponenziale(double n);
double ln(double n);
void double_to_char(double n, char*string);
int char_to_int(char *stringa);
/*
-----Prototipi Real Time Clock-----
*/
void abilita_rtc(void);
void leggi_data_ora(char data[], char ora[]);
void leggi_datetime(char datetime[]);
void imposta_datetime(void);
/*
-----Floating Point Unit-----
*/
int abilita_fpu(void);
int disabilita_fpu(void);
/*
-----Prototipi LCD-----
*/
void invia_istruzione(int rs, int rw, int d7, int d6, int d5, int d4, int d7_2, int d6_2,int d5_2, int d4_2);
void setta_schermo_4bit_data(void);
void scrivi_stringa_LCD(char *stringa);
void attiva_interrupt_LCD(void);
void disattiva_interrupt_LCD(void);
int stato_interrupt_LCD(void);
/*
-----Prototipi Termistore-----
*/
void attiva_lettura_termistore(void);
double leggi_temperatura(void);
void valore_temperatura_filtrato(void);
void monitora_temperatura(void);
void attiva_interrupt_termistore(void);
void disattiva_interrupt_termistore(void);
int stato_interrupt_termistore(void);
void attiva_interrupt_monitoraggio(void);
/*
-----Prototipi Pulsante-----
*/
void attiva_pulsante(void);
/*
-----Prototipi Interfaccia-----
*/
void scrivi_data_ora_LCD(char data_prec[]);
void scrivi_temperatura_LCD(void);
void start(void);
void menu_principale(void);
/*
-----Prototipi led2-----
*/
void led2_ON(void);
void led2_OFF(void);
void lampeggia_Led(int secondi);
void disabilita_lampeggio_led2(void);
/*
-----Prototipi Buzzer-----
*/
void abilita_interrupt_buzzer(void);
void disabilita_interrupt_buzzer(void);
int stato_interrupt_buzzer(void);
