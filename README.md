Advantech DAQ demo code
=======================

Requirements:

- Ubuntu 20.04 with Kernel 5.4
- DAQNavi for Linux installed to /opt/advantech
- `libspdlog-dev`

Build:

```
$ cmake -Bbuild
$ cmake --build build -j $(nproc)
```

Trying out:

```
$ ./build/examples/blink/advantech-demo-blink "USB-4704,BID#0" # The argument is the device description. Find it via /opt/advantech/tools/dev_enum.
[2022-07-27 20:06:12.797] [debug] [main.cc:34] Using device USB-4704,BID#0
[2022-07-27 20:06:12.802] [debug] [main.cc:49] This device has 1 ports -> DO supported: true, DI supported: true
[2022-07-27 20:06:12.802] [debug] [main.cc:65] Write: 1
[2022-07-27 20:06:14.802] [debug] [main.cc:65] Write: 0
[2022-07-27 20:06:16.803] [debug] [main.cc:65] Write: 1
[2022-07-27 20:06:18.804] [debug] [main.cc:65] Write: 0
```

