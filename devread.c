#include"headers.h"
#include"declarations.h"

ssize_t dev_read(struct file *fileptr, char __user *ubuff, size_t size, loff_t *pos)
{
	struct Dev *ldev;
	char *kbuff;
	int ret;
	int nocsc;
	int nocsr;
	int i;
	unsigned char ch;
	unsigned char byt;
	unsigned char status;

	kbuff=NULL;
	ret=0;
	nocsc=0;
	nocsr=0;
	i=0;

	ldev = fileptr->private_data; 

	if(!ldev)
	{
		#ifdef DEBUG
			printk(KERN_ERR"Device Not Found!! \n");
		#endif
		goto OUT;
	}

	kbuff = kmalloc(sizeof(char)*(size+1),GFP_KERNEL);

	if(!kbuff)
	{
		#ifdef DEBUG
			printk(KERN_ERR"kmalloc failed!! \n");
		#endif
		goto OUT;
	}
	
	memset(kbuff,'\0',sizeof(char)*(size+1));	

	#ifdef DEBUG
		printk(KERN_ALERT"Read Operation Begins!! \n");
	#endif

	for(i=0;i<size;i++)
	{
		ch ^= ch;
		byt ^= byt;

		outb(32,CONTROL);

		do
		{
			status ^= status;
			status = inb(STATUS);
			status = status >> 7;
			status = status & 1;
		}while( status != 0x01);

		byt = inb(STATUS);
		byt = byt << 1;
		byt = byt >> 4;
		ch = byt;

		#ifdef DEBUG
			printk(KERN_ALERT"Byt Read : %x \n",byt);
		#endif
			
		outb(33,CONTROL);

		do
		{
			status ^= status;
			status = inb(STATUS);
			status = status >> 7;
			status = status & 1;
		}while( status != 0x00);

		byt = inb(STATUS);
		byt = byt >> 3;
		byt = byt << 4;
		ch = ch | byt;

		#ifdef DEBUG
			printk(KERN_ALERT"Byt Read : %x \n",byt);
			printk(KERN_ALERT"Character Read : %c \n",ch);
		#endif
			
		*(kbuff+i) = ch;

		outb(32,CONTROL);

		nocsr++;
	}

	#ifdef DEBUG
		printk(KERN_ALERT"Data Read : %s \n",kbuff);
	#endif

	ret = copy_to_user(ubuff,kbuff,size);

	nocsc = size - ret;

	#ifdef DEBUG
		printk(KERN_ALERT"Writing Done Successfully!! \n");
	#endif

	return nocsc;
OUT:
	return -1;	
}
