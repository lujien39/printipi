Printipi
========

Printipi is a software package designed to bring 3d printing to the Raspberry Pi. It takes on all of the roles generally given to dedicated microcontrollers (interfacing with stepper drivers, temperature control of the hotend, and cooling fans) while also running under an operating system. This means that the same device that is running the firmware can also perform other tasks while printing, such as hosting a web interface like Octoprint.

Although called Printi<i>pi</i>, it is not necessarily limited to running on the Pi. The `Example` machine can compile and run on most Linux machines, as a proof of concept (it does no electrical I/O), and new machines can be supported by implementing a handful of interfaces (see the section below for more info).

Printipi also aims to support a multitude of printers including typical cartesian printers, delta-style printers like the Kossel, or polar-based printers - **without** the messy use of hundreds of #defines, some of which may not even be applicable to your printer. Instead, each machine type gets its own file and C++ class under src/machines that exposes its coordinate system and peripherals through a handful of public member functions. In this way it is possible to add support for a new type of printer without digging into the guts of Printipi.

**Note:** Printipi is in a very developmental state and only has limited documentation. As of this time, it is *not* recommended to users who aren't comfortable with digging into the source code to figure out how things work.

Demos
========

Printipi powering a Mini Kossel @ 120mm/sec (2014/10/19): http://youtu.be/gAruwqOEuPs  
Printipi powering a Mini Kossel (2014/08/18): http://youtu.be/g4UD5MRas3E  

License
========

With the exception of certain files*, Printipi is licensed under the MIT license. This means that you are free to use, modify, distribute, and sublicense the code as you see fit. While you are not obligated to do so by the license, it would be appreciated that you share any improvements you make (eg make a public fork on github containing your modifications and then submit a pull request to have it merged with the devel branch).

*util/rotation_matrix.py is (c) Edward d'Auvergne and reproduced here only to aid in calibration

Limitations
========

Printipi currently runs entirely in userland. While this makes development and usage trivial, it makes hardware management less safe. Printipi uses one of the Raspberry Pi's DMA channels in order to achieve precise output timing (2~4uS precision), however if another program tries to access the same DMA channel as Printipi, it **will** lead to errors.

Also, very heavy bus contention may degrade timing accuracy. Experiments show 500ksamples/sec (2uS resolution) to be dependable under most operating conditions, except heavy network/disk usage. 250ksamples/sec (4uS resolution) is dependable for at least 1 MB/sec network loads, and is the default data rate.

The Raspberry Pi has no user-accessible analog to digital (A/D) converters, meaning that it's slightly more complicated to read analog sensors, like thermistors and force-sensitive resistors (FSRs). Since both of these act as resistors, this limitation is bypassed by using an RC circuit - a capacitor of known capacitance is charged to its capacity, and the time it takes to discharge through the resistor is measured.

Lastly, only a limited set of gcode commands are currently supported. Namely, testing has been done using Cura for slicing.

Compiling
========

**Prereqs**: gcc >= 4.6 or another compiler with support for C++11  
gcc >= 4.7 is highly recommended because of the benefits gained from link-time optimization, which isn't supported in gcc 4.6 when using C++11  
gcc 4.7 can be installed in the stock version of Raspbian via `sudo apt-get install g++-4.7` and to use it over the system's gcc, compile with `make CXX=g++-4.7`

First, get the sources: `git clone https://github.com/Wallacoloo/printipi.git`  

To compile Printipi, navigate to the src directory and type `make MACHINE=<machine> <target>`, where `<machine>` is the C++ classname (fully-qualified) of the machine contained under src/machines, eg `rpi::KosselPi` or the `generic::Example` machine, and `<target>` is either debug, release, debugrel, profile, or minsize. Both are case-sensitive. A binary will be produced under build with the name `printipi`. Navigate to that folder and run the binary (you will want root permissions in order to elevate the priority of the task, so run eg `sudo ./printipi`).

Usage
========

The firmware can either be called with no arguments, in which case it will take gcode commands from the standard input (useful for testing & debugging). Or, you can provide the path to a gcode file. The provided file can be **any** file-like object, including device-files. This allows one to pass eg `/dev/ttyAMA0` to take commands from the serial port.

Using with Octoprint:
--------

**Prereqs**: install the program "socat". Eg `sudo apt-get install socat`

Because Octoprint prints to a serial-like Linux device-file, and Printipi can take commands from any file-like object, it's possible to create a *virtual* serial port to pipe commands from Octoprint to Printipi. This is just what the provided "launch-firmware.sh" file does. After running that script, a new device should be visible in the Octoprint web interface (a refresh will be required) to which you can connect. 

Configuration Files
========

The files under `src/machines` define classes of machines - deltabots, cartesian bots, polar bots, etc. Each one of these is analogous to a master "config file". That is to say, you should find the machine definition in that folder that is most similar to your own (eg `src/machines/rpi/kosselpi.h`), make a copy of it (eg copy it to `src/machines/rpi/customkossel.h` and be sure to rename the `KosselPi` C++ class contained in the file to `CustomKossel` in order to reflect the path change), and then customize it. Unless you are a developer, you should never have to edit code outside of your config file. To build your CustomKossel machine, type `make MACHINE=rpi::CustomKossel`.

Supporting Other CPU Architectures
========

While Printipi is under heavy development, this process may change slightly, but these are the basic steps to supporting new hardware:  
1. Add a folder under src/drivers for your platform.  
2. Implement `src/drivers/<platform>/IoPin`. An example implementation is drivers/rpi/RpiIoPin  
3. Implement `src/drivers/<platform>/hardwarescheduler.h`. This interface is outlined near the bottom of schedulerbase.h.  
4. Optionally implement `src/drivers/<platform>/chronoclock.h` and `src/drivers/<platform>/thisthreadsleep.h`. Doing so is not necessary, but if you have direct access to a clock source, then implementing these will reduce the number of calls made into the kernel.  
5. Make a new class for your machine in `src/machines/<platform>/`  
6. Type `make MACHINE=<platform>::<MachineName>`  

The Printipi build system will automatically detect the files you added to src/drivers/<platform> and will use those in place of the generic implementations, so there's no need to edit any files.

The Future
========

More effort will be put into the motion planning system, which currently has no concept of curves and thus forces a full deceleration to 0 at each joint in the path.

See the issues section for more info.
