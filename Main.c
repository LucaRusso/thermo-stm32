#include "Prototipi.h"

#define USART_SR 	*(long*)0x40004400 
int main(void){
	start();
	while(1){
		menu_principale();
	}
}
