# SYSC 3310

MSP-EXP432P401R projects for SYSC 3310, all written in C

## Notice to future students:
While I understand the temptation to look for already made solutions to your labs, you're doing youself and your fellow students a diservice if you chose to blatantly copy. The content of this course is foundational, and you'll find that it's later built on in future courses, by not understanding this content now you effectively shoot yourself in the foot later. You may say "It's on GitHub so it's free to use by everyone". While yes, GitHub faciliates the opensource community, and I encourage anyone to use any software accross all my repos for non-accademically dishonest purposes,however, GitHub is also increasingly the programmers portfolio. I put this here because I'm proud of my work and I want potential employeers to see it. By copying this material you open the future possibility of professors banning GitHub submissions, and robbing your collegues of the oppertunity to also contribute to their portfolios. I know this message may come off as nose held high, but please reconsider copying code from this repo to be accademically dishonest. If I've failed to deter you, I'll point you to my copyright (see MIT License), where this software cannot be reused without the explicit inclusion of that copyright(and therefore my name).Thank you for your time, and understanding.

## Lab 1
Just a proof lab, to prove you know how header files and C files work.

## Lab 2
Configure input as internal pull-down resistor on pins 1.1, and 1.4. Initial state of LEDs is LED 1.0 is on, RGB LED is off. Switch 1.1 alternates between the RED LED State and the RGB LED State. When in the RED LED state, switch 1.4 blinks the RED LED. When in the RGB LED state, switch 1.4 iterates between the 7 color states. When switching back to the red LED, the RGB LED must remember it's previous state.

## Lab 3
Same as Lab 2, except the switch's are set up as internal pull-up resistors, also the switches control LED states via interrupts instead of via polling

## Lab 4
Same as Lab 3, except LEDs iterate through states via timer (once a second), switch 1 changes between LEDs, while switch 2 stops the timer

## Lab 5
Utilizes Texas Instruments Education booster pack 2, through the BSP library. Project aims to map the different quadrents of the joystick to different frequencies to be played by
the buzzer. Dead centre on the joystick corrisponds to 800Mhz at a duty cycle of 0% (off).

## Lab 6
Similar to lab 2->4 however, both LEDs are being used at the same time. Right after system configuration the Red LED is on 0.1s later, the RGB LED is turned on, every 1s the Red LED changes states, every 1s the RGB LED changes states. I use two different timers (A0, and A1) to achieve this due to 0.1s offset

## Lab 7
This lab utilizes the Texas Instruments Education Booster Pack 2, through the BSP library. Project aims to time a process execution on the devices opperating system, and then display that time to the display on the booster pack.
