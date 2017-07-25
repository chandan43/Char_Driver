#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/init.h>
#include<asm/uaccess.h>
#include <linux/mutex.h>	
 
#define DEVICE_NAME "Char_Driver"
#define CLASS_NAME "Char"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xyx@gmail.com");
MODULE_DESCRIPTION("Linux Char Driver");
MODULE_VERSION(".1");

static int majornumber;
static char message[256]={0};
static short size_of_message;
static int numberopens;
static struct class * charclass=NULL;
static struct device * chardevice=NULL;
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static DEFINE_MUTEX(mychar_mutex);

static struct file_operations fops =
{
	.open=dev_open,
	.read=dev_read,
	.write=dev_write,
	.release=dev_release,
};

static int  __init mychar_init(void){
	printk(KERN_INFO "Char Driver:Initializing the Char Driver\n");
	mutex_init(&mychar_mutex);
	majornumber=register_chrdev(0,DEVICE_NAME,&fops);
	if(majornumber<0){
		printk(KERN_INFO "Char Driver: is Failed to Register a major\n");
		return majornumber;
	}
	printk(KERN_INFO "Char Driver: registered correctly with major number %d\n", majornumber);
	charclass=class_create(THIS_MODULE,CLASS_NAME);
	if(IS_ERR(charclass)){
		unregister_chrdev(majornumber,DEVICE_NAME);
		printk(KERN_INFO "Char Driver: class creation  is Failed \n");
		return PTR_ERR(charclass);
	}
		
	chardevice=device_create(charclass,NULL,MKDEV(majornumber,0),NULL,DEVICE_NAME);
	if(IS_ERR(charclass)){
		printk(KERN_INFO "Char Driver: Device creation  is Failed \n");
		class_destroy(charclass);
		unregister_chrdev(majornumber,DEVICE_NAME);
		return PTR_ERR(chardevice);
	}
	printk(KERN_INFO "Char Device: class  is created  correctly\n");	
	return 0;
}
static void __exit mychar_exit(void){
	mutex_destroy(&mychar_mutex);
	device_destroy(charclass,MKDEV(majornumber,0));
	class_unregister(charclass);
	class_destroy(charclass);
	unregister_chrdev(majornumber,DEVICE_NAME);
	printk(KERN_INFO "Char Device: Driver is saying GOOD bye .!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
	if(!mutex_trylock(&mychar_mutex)){    /// Try to acquire the mutex (i.e., put the lock on/down)
                                          /// returns 1 if successful and 0 if there is contention
     	 	printk(KERN_ALERT "MyChar: Device in use by another process");
      		return -EBUSY;
   	}	
	numberopens++;
	printk(KERN_INFO "Char Driver : Device has been opened %d time(s)\n", numberopens);
   	return 0;
}
ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	error_count=copy_to_user(buffer,message,size_of_message);	
	if (error_count==0){            // if true then have success
      		printk(KERN_INFO "Char Driver: Sent %d characters to the user\n", size_of_message);
      		return (size_of_message=0);  // clear the position to the start and return 0
   	}
   	else {
      		printk(KERN_INFO "Char driver: Failed to send %d characters to the user\n", error_count);
      		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   	}
}

ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	 //sprintf(message,"%s(%zu letters)", buffer, len);   // appending received string with its length
	int error_count = 0;
	memset(message,0,sizeof(message));
	error_count=copy_from_user(message,buffer,len);
	if(error_count==0){
   		size_of_message = strlen(message);                 // store the length of the stored message
   		printk(KERN_INFO "Char Driver: Received %zu characters from the user\n", len);
   //   		printk(KERN_INFO "Char Driver: Received %d characters to the user\n", size_of_message);
      		return len;  // clear the position to the start and return 0
	}
   	else {
     		printk(KERN_INFO "Char driver: Failed to Received %d characters to the user\n", error_count);
      		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   	}
}

static int dev_release(struct inode *inodep, struct file *filep){
	mutex_unlock(&mychar_mutex);
	printk(KERN_INFO "Char Driver: Device successfully closed\n");
   	return 0;
}

module_init(mychar_init);
module_exit(mychar_exit);


