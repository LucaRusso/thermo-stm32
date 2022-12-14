/*
-----Registri FPU-----
*/
#define CPACR 	*(long*)0xE000ED88
/*
-----Prototipi Funzioni-----
*/
int abilita_fpu(void);
int disabilita_fpu(void);
int stato_fpu(void);

/*
Funzione che abilita la Floating Point Unit
Ritorna 1 se è stata abilitata, altrimenti 0
*/
int abilita_fpu(void){
	if(!(stato_fpu())){
		CPACR = 0x00F00000; //CoProcessor in Full Access
		if((CPACR & 0x00F00000) == 0x00F00000){
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		return 1;
	}
}
/*
Funzione che disabilita la Floating Point Unit
Ritorna 1 se è stata disabilitata correttamente, altrimenti 0
*/
int disabilita_fpu(void){
	if(stato_fpu()){
		CPACR = 0x00000000; //CoProcessor access denied
		if((CPACR & 0x00000000) != 0x00000000){
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 1;
	}
}
/*
Funzione che legge se la Floaing Point Unit è attiva oppure no
Ritorna 1 se è attiva, altrimenti 0
*/
int stato_fpu(void){
	if((CPACR & 0x00F00000) == 0x00F00000){
		return 1;
	}
	else{
		return 0;
	}
}
