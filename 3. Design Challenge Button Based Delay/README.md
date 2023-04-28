#Design Challenge Button Based Delay
**Designed by Luke Wilkins**

##What is it?
This code makes the LED blink at a rate defined by how long you hold down button 2.3.
Button 4.1 resets the program back to the original time.

##How does it work?
Whenever you hold down the 2.3, it will increment a counter that is then assigned to
TB1CCR0. This overrides the default blink time with your own. Pressing 4.1 just sets
TB1CCR0 back to the initial value.

##Some Notes
The code does not like working as intended. Most of the time it works, though. It also
can't handle very long times. I attempted to fix that for a very long time but could never
implemenet a solution that actually worked. The timer also isn't exact but it's pretty close.