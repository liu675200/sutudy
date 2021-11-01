typedef unsigned int uint_32;
typedef void (*Handler)(void);
int main(void);
void Reset_Handler(void);

extern  uint_32 _estack;
extern  uint_32 __text_end__;
extern  uint_32 __data_start__;
extern  uint_32 __data_end__;
extern  uint_32 __bss_start__;
extern  uint_32 __bss_end__;

typedef struct{
	void* estack;
	Handler vector[256];
} Vectors;


const Vectors cortex_vectors = {
	&_estack,
	{[0] = Reset_Handler}
};


void Reset_Handler(void)
{
    uint_32 *flash, *ram;
	flash = &__text_end__;
	ram = &__data_start__;
	if(flash != ram)
	{
		for(;ram < &__data_end__;)
		{
			*ram = *flash;
			flash ++;
			ram ++;
		}
	}
	for(ram = &__bss_start__; ram < &__bss_end__; ram ++)
	{
		*ram = 0;
	}
	main();
}




