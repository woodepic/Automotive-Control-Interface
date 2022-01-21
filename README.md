# Automotive-Control-Interface
A seamless user experience is critical for a well-engineered product. This project integrates a retrofitted button into the car's existing button backlighting system. This project includes designing the button, control interface, animations, and firmware from scratch. 

I wanted to add a button to my car for aftermarket heated windshield wipers, however I wanted to integrate it seamlessly into the vehicle. My car uses a single dimming control to dim every button and every screen in the car, so I wanted this existing dimmer to also dim my new button. This led me to tap into an existing button's backlight, use a voltage divider in my pcb to allow the arduino to read the signal, and use the arduino to measure the pulse width. Using a simple mapping function and the transisor-switched 12V power on my PCB, I can now match the new button's backlight to that of the car's. 

Image of the control PCB and the button:

![alt text](https://github.com/woodepic/Automotive-Control-Interface/blob/main/src/pcb.png "Control PCB") ![alt text](https://github.com/woodepic/Automotive-Control-Interface/blob/main/src/button.png "Button")

I also wanted the button to look as though it belonged in the car. This led me to design and order a custom button. I designed a logo to indicate heated wipers, and ordered the button as a momentary, silver button, with a matching red backlight. Whenever the button is toggled on, a pulsing animation in the button's backlight begins. Whenever the button is off, the backlight matches that of other buttons, meaning in the daytime mode the backlight is off, and in the night-time mode the backlight is on. See the link at the bottom of this readme for a video demo. 

The arduino accepts various inputs: The momentary signal from the wiper button and the PWM signal from the car's existing button. The arduino calculates, and manages various outputs: power to the wiper button's backlight, a signal to a relay (toggles on/off the high-powered heated wipers), and an auxilary brightness output used to control the brightness of an aftermarket gauge.

As the cherry on top, all of the code including pulse animations are written to be non blocking, allowing other code to execute at the same time. I also implemented safety features such as built in fuses, a default to "wiper heat off" upon boot, and the power relay defaults to off in case of arduino failure.

Demo video of the project is found here: <https://youtu.be/XevvCuZYbWQ>
