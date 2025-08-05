---
title: Software
parent: Manufacturing Instructions
nav_order: 3
---

# Software Implementation of the Hydraulic Pressure Module
{: .no_toc }

When controlling the pressure module, there are several tasks that must be performed on the software side. These include implementing a user interface for entering the test parameters, regulating the flow rate, and collecting and processing 
all data for evaluation at the end of each test.
Both the Arduino IDE and Python are used for implementation.

{: .note }
The Arduino script and GUI can be downloaded [here](/docs/Downloads.md).

## Table of Contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Communication between Python, Arduino, and the User

The user interface is implemented in such a way that the user only communicates with Python at the beginning of a print 
cycle. 
The rest of the control is managed using a continuous data stream via the serial interface between Python and Arduino.
At the start, the user can enter all target speeds to be achieved in the printing cycle with the corresponding times in chronological order in Python. 
Python saves the transferred printing parameters and sends the current target speed to the Arduino at a frequency 
of 2.5 Hz from the start of printing. 
The Arduino reads the data from the serial interface at a frequency of 5 Hz and transfers the current target speed to 
the control cycle.
During the control cycle, the difference between the current flow rate and the target flow rate is calculated, and a new voltage value is calculated using PI control and feedforward control.
This voltage value is transmitted to the micro pump, which generates an adjusted flow rate. The new flow rate is 
measured by the flow sensor and transmitted back to the Arduino. 
The Arduino sends the flow sensor measurements to Python at a frequency of 5 Hz throughout the entire printing process.
Python reads the data from the serial interface at the same frequency and stores it.
Once printing is complete, Python provides the user with direct feedback on the flow rates achieved using a graph that 
plots the flow rate over the printing time.
In addition, all data is exported to an Excel file for further evaluation.

<p align="center">
  <img src="../Abbildungen/Software_Communication.svg" width=800>
</p>

---

## Feedforward Control

Various methods were tried out to improve the regulation of the flow velocity.
- Control without feedforward control
- Control with feedforward control
- Control with reset of the integral component and the average value memory
- Control with last voltage value before setpoint change as feedforward control and reset of integral component and 
- average value memory


Evaluation of these methods has shown that feedforward control is the most effective method. Here, a performance 
characteristic curve of the micropump is recorded under the current environmental conditions before each pressure cycle
This performance characteristic curve is then used in the PI control to improve the control speed.

