## Description

A multithreaded HTTP server, running on the [BeagleBone Black](https://beagleboard.org/black), that serves the ambient's temperature and humidity to its clients in JSON format.

The server is built using the [Poco c++ libraries](https://pocoproject.org).

The data from the SHT21 sensor is read through `sysfs`
using [the pre-built LKM](https://docs.kernel.org/hwmon/sht21.html).

An [`ExpireCache`](https://docs.pocoproject.org/current/Poco.ExpireCache.html),
whose access is protected by a mutex,
avoids simultaneous HTTP requests from reading the sensor too often.

The server can be connected to a [fat-client Qt desktop application](TODO LORIS),
whose job is to retrieve the data and display it nicely to the user.

## JSON Structure

Data is served at URI `\`.  
Any other endpoint will get a 404 response.

```json
{
    "version": number,
    "data: {
        "temperature": number,
        "humidity": number
    }
}
```

## Env Variables

Both the server's port number and the timeout for the `ExpireCache`
can be adjusted in [`http-server.properties`](./http-server.properties)

## Building and Flashing

Make sure you also upload `http-server.properties`

## Running the server

The program expects the [SHT21 LKM](https://docs.kernel.org/hwmon/sht21.html) to be loaded before starting.

For this reason, it won't be sufficient to simply run the executable.  
Instead, the application should be started through [`start.sh`](./start.sh).

```sh
sudo su
./start.sh
```

## Details about the BBB and the OS

Processor: AM3358 ARM Cortex-A8

OS: Debian 10.3 Buster IoT (without graphical desktop)

Kernel release: 4.19.94-ti-r42

GCC version: 8.3.0

Link to the official docs [here](https://github.com/beagleboard/beaglebone-black/wiki/System-Reference-Manual)

---

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
