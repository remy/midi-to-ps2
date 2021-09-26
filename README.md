# MIDI to Keyboard over PS/2 (and USB)

This project was originally designed for [NextDAW](http://nextdaw.biasillo.com/) to let you control the software using a real MIDI keyboard.

The device can connect over PS/2 interface but also USB and announces itself as a keyboard - which means you can use this on a device like a MiSTer or even a PC.

The code should be relatively straight forward to update for your own means if you desire a different mapping.

## Materials

To build this project, the raw components you'll need (and approximate price at time of publishing) are as follow.

None of the links are affiliates and just the suggestions that I've used.

- Raspberry Pico - [£3.60 / Pimoroni](https://shop.pimoroni.com/products/raspberry-pi-pico?variant=32402092294227)
- USB host controller IC - [£5.76 / Hobby Tronics](https://www.hobbytronics.co.uk/usb-host/usb-host-dip)
- USB type A female connector - [£0.72 / Hobby Tronics](https://www.hobbytronics.co.uk/usb-type-a-socket)
- PS/2 6 Pin mini DIN female (if required) - [can be around £0.65, but here's an example on Amazon](https://www.amazon.co.uk/sourcingmap%C2%AE-Shielded-Female-Board-Connector-Silver/dp/B007PPHYFM)
- 16Mhz crystal oscillator + 2x18pF capacitors - [£0.54 / HobbyTronics](https://www.hobbytronics.co.uk/xtal-caps)
- 100nF capacitor
- 10uF capacitor
- 10K resistor

Once built, use the [latest release](https://github.com/remy/midi-to-ps2/releases/latest) and upload the firmware.

## Uploading firmware

With the Pico disconnected, hold down the white boot button, then attach to a PC using the USB port.

The Pico will become available as a mounted drive. Drag and drop the latest release on to the drive. It will automatically disconnect the Pico and the new firmware will be flashed and available.

## MIDI configuration

NextDAW supports 29 keys which are mapped across two (plus some) octaves from C2 to (nearly) C5 - from C2 to F4.

Currently on a single knob has been implemented with the specific settings of: CC 73, LO 0, HI 127, relative. This will move the play head.

More control can be added either as NextDAW is updated or as requested.

## TODO

- [ ] How to change key mappings
- [ ] Explain source code
- [ ] Notes on PCB and STL files
