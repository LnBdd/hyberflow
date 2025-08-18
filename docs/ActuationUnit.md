---
title: Actuation Unit
parent: Assembly
nav_order: 2
---

<p align="left">
  <img src="../images/actuationUnit/banner_actuationUnit.png" width=300>
</p>

<!-- Table of Contents -->
# Actuation Unit
{: .no_toc }

## Table of Contents
{: .no_toc .text-delta }

1. TOC
{:toc}

<!--  -->

---

## Components 

<p align="center">
  <img src="../images/actuationUnit/actuationUnit_assamblyOverview.png" width=1200>
</p>

### Off-the-Shelf Components

| Part Number | Part                                                      | Supplier                                                                                                | Quantity |
|-------------|-----------------------------------------------------------|---------------------------------------------------------------------------------------------------------|----------|
| **(02)**    | mp-damper                                                 | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/mp-damper-pulsation-damper/)             | 1        |
| **(03)**    | Flow sensor SLF3S-0600F <br> (also availble in a dev kit) | [Sensirion](https://sensirion.com/de/produkte/katalog/SLF3S-0600F)                                      | 1        |
| **(05)**    | 3/2-way micro-switching valve                             | [Staiger](https://www.staiger.de/ventil-online-shop/start/mikroventile/va-304-913-v-08-sap-12-1-detail) | 2        |
| **(07)**    | Micropump mp6-liq                                         | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/de/mikropumpen/)                                 | 2        |
| **(12)**    | PCB                                                       |                                                                                                         | 1        |
| **(14)**    | Adafruit Metro Mini                                       | [Adafruit](https://www.adafruit.com/product/2590)                                                       | 1        |
| **(15)**    | mp-Highdriver4                                            | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/pump-driver/)                            | 1        |
| **(A1)**    | Tubing                                                    | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/mp-t-tygon-tubing/)                      | 50cm     |
| **(A2)**    | Hose clamp                                                | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/mp-hc-hose-clamp/)                       | 18       |
| **(A3)**    | Y-Hose connector                                          | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/mp-y-hose-connector/)                    | 2        |
| **-**       | Countersunk head screw M2.5x4                             | any                                                                                                     | 2        |
| **-**       | Cylinder head screw M1.6x8                                | any                                                                                                     | 10       |
| **-**       | Cylinder head screw M2x8                                  | any                                                                                                     | 4        |
| **-**       | Cylinder head screw M2x16                                 | any                                                                                                     | 4        |
| **-**       | Cylinder head screw M2.5x8                                | any                                                                                                     | 4        |


### 3D Printed Components

| Partnumber | Material | File                  | Quantity |
|------------|----------|-----------------------|----------|
| **(01)**   | PLA      | connectorSleeve.stl   | 1        |
| **(02)**   | PLA      | back_wall.stl         | 1        |
| **(04)**   | MED610   | valve_interface.stl   | 2        |
| **(06)**   | MED610   | tube_adapter.stl      | 6        |
| **(08)**   | PLA      | micropump_frame_A.stl | 1        |
| **(09)**   | PLA      | micropump_frame_B.stl | 1        |
| **(11)**   | PLA      | damper_frame.stl      | 1        |
| **(13)**   | PLA      | spacerPCB.stl         | 4        |

---

## Assembly

{: .note }
First, assemble the [PCB](./Electronics.md) and connect all cables to the devices but not to the PCB.

### Housing
 
<p align="center">
  <img src="../images/actuationUnit/pcb_backplate_assembly.png" width=400>
</p>

Mount the PCB **(12)** to the back wall **(2)** using four M2.5x8 screws and spacer rings.

<p align="center">
  <img src="../images/actuationUnit/sleeve_assembly.png" width=600>
</p>

Connect the back wall **(2)** with the sleeve **(1)** using four M2x8 screws.


### Actuation System

<p align="center">
  <img src="../images/actuationUnit/Montage_02.svg" width=200>
</p>

Attach the flowsensor **(3)** to the back wall **(2)** using two M2.5x4 screws. <br>
Connect the flowsensor to the flowsensor port on the PCB.


<p align="center">
  <img src="../images/actuationUnit/Montage_03.svg" width=200>
</p>


Insert a valve **(5)** in each of the two valve interfaces **(4)**.

<p align="center">
  <img src="../images/actuationUnit/Montage_04.svg" width=200>
</p>

Insert tube adapters **(6)** into the side of the valve interfaces **(4)** using thread sealing tape.

<p align="center">
  <img src="../images/actuationUnit/Montage_05.svg" width=200>
</p>

Attach the valve interfaces **(4)** to the back wall **(2)** using two M2x16 screws each. <br>
Connect the valves to the ports Valve01 and Valve02 on the PCB **(12)**.
It does not matter which one is connected to which port. 

<p align="center">
  <img src="../images/actuationUnit/Montage_06.svg" width=200>
</p>

Fit the first micropump **(7)** in the micropump frame A **(8)**.

<p align="center">
  <img src="../images/actuationUnit/Montage_07.svg" width=200>
</p>

Mount the micropump frame A **(8)** with the micropump **(7)** to the back wall **(2)** using four 
M1.6x8 screws. <br>
Connect the micropump to the port Pump01 on the PCB **(12)**.

<p align="center">
  <img src="../images/actuationUnit/Montage_08.svg" width=200>
</p>

Fit the second micropump **(7)** in the micropump frame B **(9)**.

<p align="center">
  <img src="../images/actuationUnit/Montage_09.svg" width=200>
</p>

Mount the micropump frame B **(9)** with the micropump **(7)** to the micropump frame A **(8)** using
two M1.6x8 screws. <br>
Connect the micropump to the port Pump02 on the PCB **(12)**.

<p align="center">
  <img src="../images/actuationUnit/Montage_10.svg" width=200>
</p>

Fit the pulsation damper **(10)** in the damper frame **(11)**.

<p align="center">
  <img src="../images/actuationUnit/Montage_11.svg" width=200>
</p>

Mount the damper frame **(11)** to the micropump frame B **(9)** using four M1.6x8 screws.

### Metro Mini and MP4-Highdriver

<p align="center">
  <img src="../images/actuationUnit/installation_uC.png" width=400
  >
</p>

Plug the Metro Mini and Highdriver into the PCB **(12)**.

## Hose System

<p align="center">
  <img src="../images/actuationUnit/Hydraulic.svg" width=500>
</p>

{: .warning }
While the flowsensor can measure bidirectionally, the micropump would be damaged if the actuating fluid would be pumped 
through the micropump in the opposite pumping direction.

{: .note }
In the default setting, outputs 'R' and 'A' of the valves are connected. 
As soon as the valves are operated with 12 V, 
they switch over so that 'A' and 'P' are connected to each other.

Use a sharp blade to cut the tubes down to the required length. 
Connect the silicon tubing **(A1)** according to the principle sketch.
Secure the tubes by using the **(A2)**.
Y-hose-connectors **(A3)** can be used to connect three tube ends.

The tube going to the reservoir and syringe should be longer to make installation of the bioprinting cartridge easier.


<p align="center">
  <img src="../images/actuationUnit/hoseSystem.png" width=200>
</p>

This is what the final result should look like.






