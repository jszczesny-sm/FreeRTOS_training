## Welcome

Hi and welcome to this training repository. Aim of the project is to get familiarized with the embedded platform development in C language. This course assumes you have prior knowledge in software programming. If you wrote a program this material can be slightly difficult to jump-in. 

Throughout this course we will build simple device 'hand cleaning machine' with extensive funtionality. Following subject are covered:

 - understanding of microprocessor/microcontroller operations
 - usage of embedded peripheral devices 
 - using different approach to create state machines
 - working with different peripheral devices
 - connecting with MQTT servers
 - using parameter storage and calibrating features
 - using CAN communication 
 - ... extend it with your wishes

## Sessions prepared


1. Session 1. Project structure and out-of-order state

## Session 1. Project structure and out-of-order state

This session we have a look at the available boards, we configure our systems and blink our boards for the very first time. Despite the fact this is only blinking example I would like to give you detailed introduction into several aspects of embedded systems that are usually omitted for the sake of simplifications:

After this session you shall be able to answer following questions: 
 
 + Material to be covered during our session:

 - How does the processor works, where it takes instruction from, how those are executed in the CPU and results becomes visible
 - How the C compiler work. How it translates the C language into executable code. What is compilation and what is linking.
 - What does the processor execute at the very beginning just after reset is released ? (Overview)
 - How our basic example is structurized and what are relations between the code in multiple files
 - How to write functions and apply them into the existing code?
 - How can I test my functions before I implement them - introduction to unit tests
 - What are FSM's and how can I write them?

 + For those who want to do more:
 - The ways we could implement state machines 
 - How could you structurize the code the way that we are indepentent from hardware application layer?
 - How could you give the end used ability to configure the timing in your designed state machine?
 - How can you add duty cycle control to our blinkig mechanism to make it heart-beat like pattern?
 - Do you know how to model FMS's in the UML ?

 + For those who cannot sleep at nights:
 - Can you propose the way instead of using our blinking function design it as configurable driver that could be reused for different purposes?

| Session type | Duration |
| ------ | ------ |
| Theory | 1.5hour |
| Practice | 2hour during shared session |
| Self work | 2h-8h (depending on the choosen scenario) |
