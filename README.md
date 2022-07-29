Advantech DAQ demo code
=======================

### Requirements

- Ubuntu 20.04 with Kernel 5.4
- DAQNavi for Linux installed to /opt/advantech
- `libspdlog-dev`

### Build

```
$ cmake -Bbuild
$ cmake --build build -j $(nproc)
```

### Trying out blink demo


```
$ ./build/examples/blink/advantech-demo-blink "USB-4704,BID#0" # The argument is the device description. Find it via /opt/advantech/tools/dev_enum.
[2022-07-27 20:06:12.797] [debug] [main.cc:34] Using device USB-4704,BID#0
[2022-07-27 20:06:12.802] [debug] [main.cc:49] This device has 1 ports -> DO supported: true, DI supported: true
[2022-07-27 20:06:12.802] [debug] [main.cc:65] Write: 1
[2022-07-27 20:06:14.802] [debug] [main.cc:65] Write: 0
[2022-07-27 20:06:16.803] [debug] [main.cc:65] Write: 1
[2022-07-27 20:06:18.804] [debug] [main.cc:65] Write: 0
```

Measure the voltage at DO4 to see the above in action.

### Trying out button demo

Attach a button to any of the digital inputs. Run the follow program and press
the button.

```
$ build/examples/button/advantech-demo-button USB-4704,BID#0
[2022-07-29 00:01:30.483] [debug] [main.cc:35] Using device USB-4704,BID#0
[2022-07-29 00:01:30.488] [debug] [main.cc:50] This device has 1 ports -> DO supported: true, DI supported: true
[2022-07-29 00:01:30.489] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:30.789] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:31.090] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:31.390] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:31.691] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:31.992] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:32.292] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:32.593] [info] [main.cc:75] Read data: 11111110 - false
[2022-07-29 00:01:32.894] [info] [main.cc:75] Read data: 11111111 - true
```
