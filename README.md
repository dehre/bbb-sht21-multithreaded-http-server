// TODO LORIS: create script that loads sht21 module and starts program

```sh
# get root permissions
sudu su

# verify that the sht21 kernel module exists
find "/lib/modules/$(uname -r)/kernel/drivers/hwmon" -name sht21.ko.xz
# TODO: check error code

# load the module
modprobe sht21
# TODO: check error code

# let the kernel know to which bus the sensor is connected
echo sht21 0x40 > /sys/bus/i2c/devices/i2c-2/new_device
# TODO: check error code

# start server
./http-server
```
