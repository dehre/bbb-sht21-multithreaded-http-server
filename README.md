## Multithreaded HTTP Server for the BeagleBone Black

A multithreaded HTTP server, running on the [BeagleBone Black](https://beagleboard.org/black), that serves the current temperature and humidity to its clients in JSON format.

The server is built using the [Poco C++ Libraries](https://pocoproject.org).

Data from the [SHT21](https://mou.sr/3WqDlyt) temperature and humidity sensor is read through [`sysfs`](https://en.wikipedia.org/wiki/Sysfs)
using [the pre-built LKM](https://docs.kernel.org/hwmon/sht21.html).

An [`ExpireCache`](https://docs.pocoproject.org/current/Poco.ExpireCache.html),
whose access is protected by a mutex,
prevents simultaneous http requests from concurrently accessing the sensor.

Last but not least, the server can be connected to a [fat-client Qt desktop application](https://github.com/dehre/bbb-sht21-fat-client-qt-app),
whose job is to retrieve the information and display it nicely to the user.

<br/>
<p align="center">
<img src="./readme_assets/walrus.svg" width=35% height=35%>
</p>

## JSON Structure

Data is served at `/`.  
Any other endpoint will serve a 404 response.

Here's how the JSON looks like if everything went fine:

```json-structure
{
    "version": <number>,
    "data": {
        "temperature": <number>,
        "humidity": <number>
    }
}
```

For an error:

```json-structure
{
    "version": <number>,
    "error": <string>
}
```

## Env Variables

Both the server's port number and the timeout for the [`ExpireCache`](https://docs.pocoproject.org/current/Poco.ExpireCache.html)
can be adjusted in [`http-server.properties`](./http-server.properties)

## Circuit

<img src="./readme_assets/circuit.svg" width=70% height=70%>

## Setup, Building, and Flashing

This [document](https://github.com/dehre/beaglebone-stuff/blob/main/cross-compilation-setup/README.md) goes in detail about how I set up my machine for cross-compilation.

The project uses a [global CMake file](./CMakeLists.txt) with different targets.  
Dependencies are managed through [Conan](./conanfile.txt).  
The file [`profile_crossbuild`](./profile_crossbuild) tells the latter which platform we're generating binary files for.

To build:

```sh
#
# On the VM
#

mkdir build
cd build

conan install .. --build=missing --profile=../profile_crossbuild

cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Build
cmake --build .
```

Finally `sftp` the executable and the environment variables to the board:

```sh
#
# On the VM
#

sftp-bbb
> put build/bin/http-server
> put http-server.properties
```

## Running the server

The program expects the [SHT21 LKM](https://docs.kernel.org/hwmon/sht21.html) to be loaded before starting:

```sh
#
# On the BeagleBone
#

# get root permissions
sudo su

# verify that the sht21 kernel module exists
find "/lib/modules/$(uname -r)/kernel/drivers/hwmon" -name sht21.ko.xz

# load the module
modprobe sht21

# let the kernel know to which bus the sensor is connected
echo sht21 0x40 > /sys/bus/i2c/devices/i2c-2/new_device

# no need for root permissions anymore
exit

# start the server
./http-server
```

## Details about the BBB and the OS

Processor: AM3358 ARM Cortex-A8

OS: Debian 10.3 Buster IoT (without graphical desktop)

Kernel release: 4.19.94-ti-r42

GCC version: 8.3.0

Link to the official docs [here](https://github.com/beagleboard/beaglebone-black/wiki/System-Reference-Manual)

## Yet another project with this damn SHT21 sensor?

Yes, mainly because:

1. it's cheap and easy to find;
2. I got to [know it quite well](https://github.com/dehre/sht21);
3. it's straightforward to connect and use;
4. the pre-built LKM abstracts over many details.

Focus of this project was the combination of a remote sensor running on Linux with a fat-client GUI app.  
Using the SHT21 has simply been the quickest way to get it done.

## ASIDE: The walrus...

The [nice cartoon sketch](https://pixabay.com/vectors/walrus-brown-shaded-shades-tusks-48347/) is offered by Pixabay users under their [generous free license](https://pixabay.com/service/license/).
