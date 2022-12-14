#include "Prototipi.h"
/*
-----Prototipi Funzioni-----
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
void delay(void);
double esponenziale(double n);
double ln(double n);
void double_to_char(double n, char*string);
int char_to_int(char *stringa);
/*
Funzione che converte un intero in una stringa
*/
void int_to_char(int integer, char *string){
	int i = 0;
	int j = 0;
	int tem = integer;
	while(tem > 0){
			tem /= 10;
			i++;
	}
	string +=(i - 1);
	while(integer > 0){
	  j = integer % 10;
		*string = (char) j + '0';
		integer /= 10;
		string--;
	}
	*(string + (i + 1)) = '\0';
}
/*
Funzione che concatena due stringhe
*/
void concat_due_stringhe(char str1[], char str2[], char results[]){
	int i = 0;
	while(str1[i] != '\0'){
		results[i] = str1[i];
		i++;
	}
	for(int j = 0; str2[j] != '\0'; j++){
		results[i] = str2[j];
		i++;
	}
	results[i] = '\0';
}
/*
Funzione che pulisce il contenuto della stringa
*/
void pulisci_stringa(char *stringa){
	while(*stringa != '\0'){
		*stringa = '\0';
		stringa++;
	}
}
/*
Funzione che converte dal formato HH:MM:SS in HHMMSS.
*/
void converti_orario(char *stringa, char* result){
	int cont_verifica = 0;
	int num_valori = 0;
	int cont = 0;
	while(num_valori < 8){
		while(*stringa != '\0'){
			//controllo che sia un numero
			if(((int)*stringa >= 48) && ((int)*stringa <= 57)){
				*result = *stringa;
				result++;
				cont_verifica++;
				cont++;
			}
			else{
				cont_verifica = 0;
			}
			stringa++;
			//controllo che venga rispettato il formato richiesto
			if(cont_verifica > 2){
				pulisci_stringa(&risposta_utente[0]);
				stringa -= cont;
				result -= cont;
				cont = 0;
				scrivi_usart2("Errore formato ora. Per favore reinseriscila rispettando il formato HH:MM:SS : ");
				while(flag_usart == 0);
				flag_usart = 0;
				while(flag_error == 1){
					pulisci_stringa(&risposta_utente[0]);
					flag_error = 0;
					scrivi_usart2("Errore di lettura. Per favore riprova: ");
					while(flag_usart == 0);
					flag_usart = 0;
				}
				cont_verifica = 0;
				num_valori = 0;
			}
			else{
				num_valori++;
			}
		}
	}
	*result = '\0';
}
/*
Funzione che converte dal formato GG:MM:AA in GGMMAA.
*/
void converti_data(char *stringa, char* result){
	int count_res = 0;
	int count_str = 0;
	int count_verifica = 0;
	char tem;
	int num_valori = 0;
	while(num_valori < 8){
		while(*stringa != '\0'){
			//controllo che sia un numero
			if(((int)*stringa >= 48) && ((int)*stringa <= 57)){
				*result = *stringa;
				result++;
				count_res++;
				count_verifica++;
			}
			else{
				count_verifica = 0;
			}
			stringa++;
			count_str++;
			//controllo che venga rispettato il formato richiesto
			if(count_verifica > 2){
				pulisci_stringa(&risposta_utente[0]);
				stringa -= count_str;
				result -= count_res;
				count_str = 0;
				count_res = 0;
				scrivi_usart2("Errore formato data. Per favore reinseriscila rispettando il formato GG-MM-AA: ");
				while(flag_usart == 0);
				flag_usart = 0;
				while(flag_error == 1){
					pulisci_stringa(&risposta_utente[0]);
					flag_error = 0;
					scrivi_usart2("Errore di lettura. Per favore riprova: ");
					while(flag_usart == 0);
					flag_usart = 0;
				}
				count_verifica = 0;
				num_valori = 0;
			}
			else{
				num_valori++;
			}
		}
	}
	//Inverto Giorno e Anno per rispettare la posizione nei registri del RTC
	*result = '\0';
	tem = *(result - count_res);
	*(result - count_res) = *(result - 2);
	*(result - 2) = tem;
	tem = *(result - (count_res - 1));
	*(result - (count_res - 1)) = *(result - 1);
	*(result - 1) = tem;
}
/*
Funzione che eleva la base all' esponente inserito
*/
long potenza(int base, int esp){
	long result = 1;
	for(int i = 0; i < esp; i++){
		result *= base;
	}
	return result;
}
/*
Funzione che converte un esadecimale sotto forma di stringa (es. "0x00111111") 
nel rispettivo valore in long
*/
long converti_hex_string_to_long(char *stringa){
	int cont = 0;
	long result = 0;
	int flag = 0;
	//controllo in che posizione si trova l'x che specifica l'inizio del numero esadecimale
	while(*stringa != '\0'){
		if(flag){
			cont++;
		}
		if(*stringa == 'x'){
			flag = 1;
		}
		stringa++;
	}
	stringa -= cont;
	//converto da esadecimale a long. Non controllo i valori >9 in quanto so che 
	//non possono essere inseriti né per specificare la data né per l'ora
	while(*stringa != '\0'){
		result += ((*stringa - '0') * potenza(16, (cont - 1)));
		cont--;
		stringa++;
	}
	return result;
}
/*
Confronta due stringhe.
Se queste sono uguali ritorna 1, altrimenti 0
*/
int confronta_stringhe(char str1[], char str2[]){
	int len1 = 0;
	int len2 = 0;
	int flag = 1;
	//Confronta lunghezza stringhe. Se non sono lunghe uguali significa che non sono uguali e posso
	//evitare di controllare i caratteri
	for(int i = 0; str1[i] != '\0'; i++){
		len1++;
	}
	for(int i = 0; str2[i] != '\0'; i++){
		len2++;
	}
	if(len1 != len2){
		return 0;
	}
	else{
		//Controllo che tutti i caratteri siano uguali. Basta che uno solo sia diverso che
		//la variabile flag sia settata a 0
		for(int i = 0; i < len1; i++){
			if(str1[i] == str2[i]){
				flag *= 1;
			}
			else{
				flag *= 0;
			}
		}
		if(flag){
			return 1;
		}
		else{
			return 0;
		}
	}
}
/*
Funzione che scrive nell' array "ascii_code" l' ascii code corrispondente al 
carattere "car"
*/
void leggi_ascii_carattere(char car, int *ascii_code){
	int ascii = (int) car;
	while(ascii > 0){
		*ascii_code = (ascii % 2);
		ascii /= 2;
		ascii_code--;
	}
}
/*
Funzione che gestisce il delay per la scrittura sul LCD
*/
void delay_LCD(void){
	int ritardo = valore_clock() * 500;
	for(int i = 0; i < ritardo; i++);
}
/*
Funzione che svolge e^x
*/
double esponenziale(double x){
	double term = 1.0;
	double eValue = 0.0;
	for(int n=1; term >= 1.0E-2; n++)
	{
    eValue += term;
    term = term * x / n;
	}
	return eValue;
}
/*
Funzione che svolge il logaritmo in base e
*/
double ln (double n){
	double min = 0.0; //so che il minimo è e^0
	double max = 15.0; //so che il massimo valore del termistore non supera e^15
	double trial = 0.0;
	while((max - min) > 1E-2){
		trial = (max + min) / 2.0;
		if((n - esponenziale(trial)) > 0.0){
			min = trial;
		}
		else{
			max = trial;
		}
	}
	return trial;
}
/*
Funzione che converte il double in carattere
*/
void double_to_char(double n, char*string){
	int parte_intera = (int) n;
	double parte_decimale = n - parte_intera;
	int te = 0;
	int_to_char(parte_intera, string);
	while(*string != '\0'){
		string++;
	}
	*string = '.';
	string++;
	te = (int) (parte_decimale * 10);
	*string = (char) te + '0';
	string++;
	*string = '\0';
}
/*
Funzione che restituisce l' intero scritto in una stringa
*/
int char_to_int(char *stringa){
	int val = 0;
	int cont = 0;
	while(*stringa != '\0'){
		cont++;
		stringa++;
	}
	stringa -= cont;
	cont--;
	while(*stringa != '\0'){
		val += ((int)*stringa - '0') * potenza(10, cont);
		stringa++;
		cont--;
	}
	return val;
}
