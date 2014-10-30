/*TP*/
#define GPIO0_DIR            				(*(unsigned long*)0x2009C000)
#define GPIO0_SET            				(*(unsigned long*)0x2009C018)
#define GPIO0_CLR            				(*(unsigned long*)0x2009C01C)

int main(void)
{
	unsigned int i;
	
	GPIO0_DIR = 0x1;
	while(1) {
	GPIO0_SET = 0x1;
		for(i=0;i<1000;i++);
		GPIO0_CLR = 0x1;
		for(i=0;i<1000;i++);
	}
}

