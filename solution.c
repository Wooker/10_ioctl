#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "solution_node"
#define MAJOR_NUMBER 240

#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char *)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char *)

static char *s;

static int str_length = 0;
static int str_content = 0;

static long ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
  long len = 0, res = 0, sum = 0;
  s = (char *)(kmalloc(20, GFP_KERNEL));
  pr_info("kernel_mooc In ioctl, cmd: %d, arg: %zu", cmd, arg);

  switch (cmd) {
  case SUM_LENGTH:
    if (strncpy_from_user(s, (char *)arg, 20) == 0) {
      pr_err("kernel_mooc Data Write : Err!\n");
    }
    len = strlen(s);
    pr_info("kernel_mooc String = %s\n", s);
    pr_info("kernel_mooc Length = %lu\n", len);

    str_length += len;
    return str_length;
  case SUM_CONTENT:
    if (strncpy_from_user(s, (char *)arg, 20) == 0) {
      pr_err("kernel_mooc Data Read : Err!\n");
    }
    res = kstrtol(s, 0, &sum);
    if (res != 0) {
      pr_err("kernel_mooc kstrtol error");
    }
    pr_info("kernel_mooc String = %s\n", s);
    pr_info("kernel_mooc Content = %lu\n", sum);

    str_content += sum;
    return str_content;
  default:
    pr_info("kernel_mooc Default\n");
    break;
  }

  return 0;
}

static int solution_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "kernel_mooc Opened session\n");

  return 0;
}

static int solution_release(struct inode *inode, struct file *file) {

  printk(KERN_INFO "kernel_mooc Closed session\n");

  return 0;
}

static bool read = false;
static ssize_t solution_read(struct file *file, char __user *buf, size_t lbuf,
                             loff_t *ppos) {
  int res, len;
  char string[20];
  
  if (read) {
    read = false;
    return 0;
  }
  printk(KERN_INFO "kernel_mooc In read");
  read = true;

  len = snprintf(string, sizeof(string), "%d %d\n", str_length, str_content);
  printk(KERN_INFO "kernel_mooc Read the string: %s", string);
  res = copy_to_user(buf, string, len);
  if (res != 0) {
    printk(KERN_ERR "kernel_mooc copy_to_user returned 0");
    return -EFAULT; // Failed to copy sum to user space
  }

  return len;
}

// Register operations
static struct file_operations solution_fops = {
    .owner = THIS_MODULE,
    .open = solution_open,
    .read = solution_read,
    .release = solution_release,
    .unlocked_ioctl = ioctl,
};

static int __init solution_init(void) {
  int result;

  // Register the character device driver
  result = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &solution_fops);
  if (result < 0) {
    printk(KERN_ALERT "kernel_mooc Failed to register solution module %d\n",
           result);
    return result;
  }

  printk(KERN_INFO "kernel_mooc Solution module is loaded\n");
  return 0;
}

static void __exit solution_exit(void) {
  // Unregister the character device driver
  unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);

  printk(KERN_INFO "kernel_mooc Solution module is unloaded\n");
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakhar Semenov");
MODULE_DESCRIPTION("Solution");