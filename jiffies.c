#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define procfs_name "jiffies"

static struct proc_dir_entry *our_proc_file;

ssize_t procfile_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    if (completed) {
        completed = 0;
        return 0;
    }
    completed = 1;
    rv = sprintf(buffer, "Current value of jiffies: %lu\n", jiffies);
    raw_copy_to_user(usr_buf, buffer, rv);
    return rv;
}
#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif
static int __init proc_init(void) {
    proc_create(procfs_name, 0644, NULL, &proc_file_fops);
    printk(KERN_INFO "/proc/%s created\n", procfs_name);
    return 0;
}

static void __exit proc_exit(void) {
    proc_remove(our_proc_file);
    printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");