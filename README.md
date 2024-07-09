What is this?
-------------
This is a repository containing my efforts into reverse engineering the IR signals sent by the controller to my A/C unit in order to replicate them using a Raspberry PI.

Inside, we have all the attained info on reverse engineering on the `reverse_engineering` folder, from raw IR codes up to explanations of the protocol and how the signal is constructed in binary.

On the root, we have a way to build the `acsender` program, a CLI utility which builds and sends an IR signal that emulates the controller using the `LIRC` library from the linux kernel. More info can be seen [here](https://www.kernel.org/doc/html/v6.6/userspace-api/media/rc/lirc-dev.html?highlight=lirc).

# How to build
Simply run `make`.

Note that you must have the boost library to build the program, as well as run it on a system capable of sending signals via the `LIRC` interface. You can check if you have the device using `ls /dev/lirc*`. 

The program is hardcoded to send via `/dev/lirc0`, but this can be changed accordingly.
