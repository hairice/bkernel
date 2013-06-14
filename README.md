Bochs Kernel
============
This repo contains a monolithic x86 kernel for the bochs emulator (http://bochs.sourceforge.net/)


Setup
-------
The following setup has been tested in an Ubuntu 10.04 (Lucid Lynx) system.

1. Install bochs emulator <br />
<code>
$: sudo apt-get install bochs 
</code>
<br />
<code>
$: sudo apt-get install bochs-x
</code>

2. Compile and run the kernel in bochs <br />
<code>
$: cd path/to/bochs_os/
</code>
<code>
$: make beros
</code>


Main Components
---------------
This kernel is based on a layered approach with the following components,
* Memory manager
* Process manager (dispatcher, context switcher)
* IPC (direct, blocked, non-buffered, synchronous)
* Real-time clock
* Priority Signal 
* Keyboard device driver


