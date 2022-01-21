# Automotive-Control-Interface
A seamless user experience is critical for a well-engineered product. This project integrates a retrofitted button into the car's existing button backlighting system. This project includes designing the button, control interface, animations, and firmware from scratch. 

I wanted to add a button to my car for aftermarket heated windsheild wipers, however I wanted to integrate it seamlessly into the vehicle. My car uses a single dimming control to dim every button and every screen in the car, so I wanted this existing dimmer to also dim my new button. This led me to tap into an existing button's backlight, use a voltage divider in my pcb to allow the arduino to read the signal, and use the arduino to measure the pulse width. Using a simple mapping function and the transisor-switched 12V power on my PCB, I can now match the new button's backlight to that of the car's. 

I also wanted the button to look as though it belonged in the car. This led me to design and order a custom button. I designed a logo to indicate heated wipers, and ordered the button as a momentary, silver button, with a matching red backlight. 
