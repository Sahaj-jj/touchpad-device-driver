#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/module.h>

#define DRIVER_NAME "ELAN0662:00 04F3:30BC Touchpad"

static struct i2c_device_id elan_touchpad_id[] = {{"ELAN0662:00", 0}, {}};
MODULE_DEVICE_TABLE(i2c, elan_touchpad_id);

static int elan_touchpad_probe(struct i2c_client *client,
                               const struct i2c_device_id *id);
static int elan_touchpad_remove(struct i2c_client *client);

static struct i2c_driver elan_touchpad_driver = {
    .probe = elan_touchpad_probe,
    .remove = elan_touchpad_remove,
    .id_table = elan_touchpad_id,
    .driver =
        {
            .name = DRIVER_NAME,
            .owner = THIS_MODULE,
        },
};

struct elan_touchpad_data {
    struct i2c_client *client;
    struct input_dev *input;
};

static int elan_touchpad_open(struct inode *inode, struct file *file);
static int elan_touchpad_release(struct inode *inode, struct file *file);
static ssize_t elan_touchpad_read(struct file *file, char __user *buf,
                                  size_t count, loff_t *pos);
static ssize_t elan_touchpad_write(struct file *file, const char __user *buf,
                                   size_t count, loff_t *pos);

static const struct file_operations elan_touchpad_fops = {
    .owner = THIS_MODULE,
    .read = elan_touchpad_read,
    .write = elan_touchpad_write,
    .open = elan_touchpad_open,
    .release = elan_touchpad_release,
};

static int elan_touchpad_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "elan_touchpad: Device has been opened\n");
    return 0;
}

static int elan_touchpad_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "elan_touchpad: Device has been closed\n");
    return 0;
}

static ssize_t elan_touchpad_read(struct file *file, char __user *buf,
                                  size_t count, loff_t *pos) {
    printk(KERN_INFO "elan_touchpad: Read operation not supported\n");
    return -EINVAL;
}

static ssize_t elan_touchpad_write(struct file *file, const char __user *buf,
                                   size_t count, loff_t *pos) {
    printk(KERN_INFO "elan_touchpad: Write operation not supported\n");
    return -EINVAL;
}

static int elan_touchpad_probe(struct i2c_client *client,
                               const struct i2c_device_id *id) {
    struct elan_touchpad_data *data;
    struct input_dev *input;
    printk(KERN_INFO "elan_touchpad: Touchpad device found\n");

    data = devm_kzalloc(&client->dev, sizeof(struct elan_touchpad_data),
                        GFP_KERNEL);
    if (!data) return -ENOMEM;

    input = input_allocate_device();
    if (!input) return -ENOMEM;
    data->client = client;
    data->input = input;

    input->name = "ELAN0662:00 04F3:30BC Touchpad";
    input->id.bustype = BUS_I2C;
    input->dev.parent = &client->dev;
    input_set_capability(input, EV_KEY, BTN_LEFT);
    input_set_capability(input, EV_KEY, BTN_RIGHT);
    input_set_drvdata(input, data);

    input->evbit[0] |= BIT_MASK(EV_KEY) | BIT_MASK(EV_SYN);

    if (input_register_device(input)) {
        dev_err(&client->dev, "Failed to register input device\n");
        input_free_device(input);
        return -ENOMEM;
    }

    i2c_set_clientdata(client, data);

    return 0;
}

static int elan_touchpad_remove(struct i2c_client *client) {
    struct elan_touchpad_data *data = i2c_get_clientdata(client);
    input_unregister_device(data->input);

    return 0;
}

static int __init elan_touchpad_init(void) {
    int ret;

    ret = i2c_add_driver(&elan_touchpad_driver);
    if (ret) {
        printk(KERN_ERR "elan_touchpad: Failed to register I2C driver: %d\n",
               ret);
        return ret;
    }

    printk(KERN_INFO "elan_touchpad: Driver initialized\n");
    return 0;
}

static void __exit elan_touchpad_exit(void) {
    i2c_del_driver(&elan_touchpad_driver);
    printk(KERN_INFO "elan_touchpad: Driver removed\n");
}

module_init(elan_touchpad_init);
module_exit(elan_touchpad_exit);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("ELAN Touchpad Driver");
MODULE_LICENSE("GPL");