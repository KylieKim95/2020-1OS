#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/unistd.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

char input_pid[128] = {0x0,};
char input_fname[128] = {0x0,};
char input_uid[128] = {0x0,};

void ** sctable ;

asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ; 
asmlinkage long (*orig_sys_kill)(pid_t pid, int sig);


//new system call : mousehole_sys_open, mousehole_sys_kill
asmlinkage int mousehole_sys_open(const char __user * filename, int flags, umode_t mode)
{
	char current_uid[128];
	char open_fname[256];
	
	sprintf(current_uid,"%s",get_current_user()->uid.val);
	copy_from_user(open_fname,filename,256);
	
	if(input_uid[0] != 0 && input_fname[0] != 0x0 && strcmp(input_uid,current_uid) == 0 && strcmp(input_fname,open_fname == 0))
		return -1;

	return orig_sys_open(filename, flags, mode) ;
}

asmlinkage long mousehole_sys_kill(pid_t pit, int sig)
{
	if(pid == input_pid)
		return -1;
	return orig_sys_kill(pid, sig);
}
	

//proc functions
static 
int mousehole_proc_open(struct inode *inode, struct file *file) {
	return 0 ;
}

static 
int mousehole_proc_release(struct inode *inode, struct file *file) {
	return 0 ;
}

static
ssize_t mousehole_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) 
{
	char buf[256] ;
	ssize_t toread ;

	sprintf(buf,"The 'uid:%s' pid:%s filename: %s.",input_uid,input_pid,input_fname);

	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset ;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT ;	

	*offset = *offset + toread;
	return toread;
}

static 
ssize_t mousehole_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) 
{
	char buf[128] ;
	if (*offset != 0 || size > 128)
		return -EFAULT ;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT ;

	sscanf(buf,"%s %s %s",input_uid,input_pid,input_fname);

	*offset = strlen(buf);
	return *offset;
}

static const struct file_operations mousehole_fops = {
	.owner = 	THIS_MODULE,
	.open = 	mousehole_proc_open,
	.read = 	mousehole_proc_read,
	.write = 	mousehole_proc_write,
	.llseek = 	seq_lseek,
	.release = 	mousehole_proc_release,
} ;

//system call handling : orig_sys_call -> mousehole_sys_call
static 
int __init mousehole_init(void) {
	unsigned int level ; 
	pte_t * pte ;

	proc_create("mousehole", S_IRUGO | S_IWUGO, NULL, &mousehole_fops) ;

	sctable = (void *) kallsyms_lookup_name("sys_call_table") ;

	orig_sys_open = sctable[__NR_open] ;
	orig_sys_kill = sctable[__NR_kill];

	pte = lookup_address((unsigned long) sctable, &level) ;
	if (pte->pte &~ _PAGE_RW) 
		pte->pte |= _PAGE_RW ;	

	sctable[__NR_open] = mousehole_sys_open ;
	sctable[__NR_kill] = mousehole_sys_kill ;

	return 0;
}

static 
void __exit mousehole_exit(void) {
	unsigned int level ;
	pte_t * pte ;
	remove_proc_entry("mousehole", NULL) ;

	sctable[__NR_open] = orig_sys_open ;
	sctable[__NR_kill] = orig_sys_kill ;
	pte = lookup_address((unsigned long) sctable, &level) ;
	pte->pte = pte->pte &~ _PAGE_RW ;

	return 0;
}

module_init(mousehole_init);
module_exit(mousehole_exit);
