---
title: Electronics
parent: Assembly
nav_order: 1
---

<p align="left">
  <img src="../Abbildungen/Electronics/PCB_Iso.png" width=350>
</p>


<!-- Table of Contents -->
# Electronics
{: .no_toc }

## Table of Contents
{: .no_toc .text-delta }

1. TOC
{:toc}



---

## Components

### PCB - ToDo: Gerberfiles, Manufacturing Instructions


### Off-the-Shelf Components

| Name | Qty. | Manufacturer | Description / Value | Package | Type |
| :----: | :----: | :----: | :----: | :----: | :----: |
| Resistor | 3 | any | 4k7 | 0805 | SMD |
| Connector | 2 | JST | B2B-PH-K-S | PH | THT |
| Connector | 2 | JST | PHR-2 | PH | THT |
| Connector | 1 | JST | PHR-3 | PH | THT |
| Connector | 2 | JST | B4B-PH-K-S | PH | THT |
| Connector | 2 | JST | PHR-4 | PH | THT |
| Connector | 1 | JST | B5B-PH-K-S | PH | THT |
| Connector | 1 | JST | PHR-5 | PH | THT |
| Crimp contact | 50 | JST | BPH-002T-P0.5S | PH | - |
| Connector | 1 | TE-Connectivity | 282836-2 | - | THT |
| MOSFET | 1 | ALPHA & OMEGA | AO3400A | SOT-32 | SMD |
| Pin Socket | 4 | any | 1×14 pin socket, 2.54mm pitch, vertical, 8.5mm height | - | THT |
| Pin Hocket | 1 | any | 1×02 pin header, 2.54mm pitch, vertical | - | THT |
| Jumper | 1 | any | 1×02 jumper, 2.54mm pitch, vertical | - | - |


---

## PCB


### Schematics
<p align="left">
  <img src="../Abbildungen/Electronics/hyberflow_PCB_v1-2.svg" width=600>
</p>

### Pinout
<p align="center">
  <img src="../Abbildungen/Electronics/PCB_PinOut.svg" width=400>
</p>



### Assembly 
For PCB assembly use the [interactive instructions](#downloads).

---

## Wiring

{: .warning }
Cut the wires to a length that you are comfortable working with.
Bear in mind that crimp contacts can fail, so leave a safety margin for repair work.

{: .note }
The corresponding pins on the PCB and device connectors are labelled with the same names.

### Adafruit Metro Mini
<p align="center">
  <img src="../Abbildungen/Electronics/metroMini_assembly.png" width=600>
</p>

Solder the pinheaders to the Adafruit Metro Mini.

### Bartels Micropumps

<p align="center">
  <img src="../Abbildungen/Electronics/micropump_pinout.png" width=600>
  <img src="../Abbildungen/Electronics/conn_micropump.svg" width=400>
</p>

Use JST-PH crimp contacts to crimp eight stranded wires (0.25mm²) at one end.
Combine 4 cables each in one JST-PH4 connector.
Solder the other ends to the micropump connector (supplied with the micro pump) as shown in the pictures.

{: .note }
Label the cables, so you can differentiate the two pumps later on.


### Sensorion Flowsensor

<p align="center">
  <img src="../Abbildungen/Electronics/flowsensor_pinout.png" width=500>
  <img src="../Abbildungen/Electronics/conn_flowsensor.svg" width=400>
</p>

Use JST-PH crimp contacts to crimpt the Sensorion ribbon cable.
Plug the 5 cables into one JST-PH5 connector as shows in the pictures.

### Staiger valves

<p align="center">
  <img src="../Abbildungen/Electronics/valve_pinout.png" width=500>
  <img src="../Abbildungen/Electronics/conn_valve.svg" width=400>
</p>

Use JST-PH crimp contacts to crimp four stranded wires (0.25mm²) at both ends.
Combine two cables in one JST-PH2 connector on the one side and in one JST-PH3 connector on the other.
Use the ports located at the edge.
The JST-PH3 connector is fits the pitch of the valve pins.

---

## Downloads
<!-- Missing Link -->
- [Assembly Instructions]()
