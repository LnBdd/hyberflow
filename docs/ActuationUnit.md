---
title: Actuation Unit
parent: Assembly
nav_order: 2
---

# Actuation Unit

<p align="center">
  <img src="../Abbildungen/Elektronik_1.svg" width=1200>
</p>

## Assembly

Assemble [PCB](./Electronics.md) first
 
Mount the back wall **(8)** onto the main housing using four M2x8 screws **(38)**.

<p align="center">
  <img src="../Abbildungen/Montage_02.svg" width=200>
</p>

Attach the flow sensor **(3)** to the back wall **(8)** using two M2,5x4 screws **(36)**.

<p align="center">
  <img src="../Abbildungen/Montage_03.svg" width=200>
</p>

Insert a valve **(4)** in each of the two valve interfaces **(9)**.

<p align="center">
  <img src="../Abbildungen/Montage_04.svg" width=200>
</p>

Insert tube adapters **(10)** into the side of the valve interfaces **(9)** using thread sealing tape.

<p align="center">
  <img src="../Abbildungen/Montage_05.svg" width=200>
</p>

Attach the valve interfaces **(9)** to the back wall **(8)** using two M2x16 screws **(39)** each.

<p align="center">
  <img src="../Abbildungen/Montage_06.svg" width=200>
</p>

Fit the first micropump **(1)** in the micropump frame A **(11)**.

<p align="center">
  <img src="../Abbildungen/Montage_07.svg" width=200>
</p>

Mount the micropump frame A **(11)** with the micropump **(1)** to the back wall **(8)** using four M1,6x8 screws **(37)**.

<p align="center">
  <img src="../Abbildungen/Montage_08.svg" width=200>
</p>

Fit the second micropump **(1)** in the micropump frame B **(12)**.

<p align="center">
  <img src="../Abbildungen/Montage_09.svg" width=200>
</p>

Mount the micropump frame B **(12)** with the micropump **(1)** to the micropump frame A **(11)** using two M1,6x8 screws **(37)**.

<p align="center">
  <img src="../Abbildungen/Montage_10.svg" width=200>
</p>

Fit the pulsation damper **(2)** in the damper frame **(13)**.

<p align="center">
  <img src="../Abbildungen/Montage_11.svg" width=200>
</p>

Mount the damper frame **(13)** to the micropump frame B **(12)** using four M1,6x8 screws **(37)**.

<p align="center">
  <img src="../Abbildungen/Elektronik.png" width=200>
</p>

The front view of the back wall with all control components looks like the above figure.

## Wiring 

# TODO: Adapt!

# Hardwareseitige Umsetzung des hydraulischen Druckmoduls

<p align="center">
  <img src="../Abbildungen/Control.svg" width=600>
</p>

## Komponenten

### Regelungskomponenten

| Bauteil-Nr. | Name | Hersteller | Anzahl <br> pro Druckmodul |
| :----: | :----: | :----: | :----: |
| **(1)** | Mikropumpe mp6-liq | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/de/mikropumpen/) | 2 |
| **(2)** | Pulsationsdämpfer mp-damper | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/mp-damper-pulsation-damper/) | 1 |
| **(3)** | Flusssensor SLF3S-0600F | [Sensirion](https://sensirion.com/de/produkte/katalog/SLF3S-0600F) | 1 |
| **(4)** | 3/2-Wege Mikro-Schaltventil | [Staiger](https://www.staiger.de/ventil-online-shop/start/mikroventile/va-304-913-v-08-sap-12-1-detail) | 2 |
| **(5)** | Widerstand 4.7kΩ | beliebig| 2 |

### Steuerungskomponenten

| Bauteil-Nr. | Name | Hersteller | Anzahl <br> pro Druckmodul |
| :----: | :----: | :----: | :----: |
| **(6)** | mp-Highdriver4 | [Bartels Mikrotechnik](https://bartels-mikrotechnik.de/product/pump-driver/) | 1 |
| **(7)** | 5V Relais-Modul | [Tru Components](https://www.conrad.de/de/p/tru-components-tc-9927156-relais-modul-1-st-passend-fuer-entwicklungskits-arduino-2481789.html?hk=SEM&WT.mc_id=google_pla&gad_source=1&gclid=CjwKCAjwnK60BhA9EiwAmpHZw33gOABH-W5sgy4y142vKv9I7WKVQ1G17AHRNnxb6aWDHuf778MKgBoC0Q4QAvD_BwE&refresh=true) | 1 |
| **(8)** | Arduino Mega 2560 | [Arduino](https://store.arduino.cc/products/arduino-mega-2560-rev3) | 1 |
| **(9)** | Netzgerät | beliebig | 1 |

## Schaltplan und Steckbrett

<p align="center">
  <img src="../Abbildungen/Circuit_diagram.svg" width=600>
</p>

<p align="center">
  <img src="../Abbildungen/Breadboard.svg" width=900>
</p>

## Konfigurationshinweise

Um beide Mikropumpen ansteuern zu können, wird im hydraulischen Druckmodul der mp-Highdriver4 eingesetzt, der bis zu vier Mikropumpen bedienen kann.

Beide Ventile werden mit dem NO-Ausgang des Relais-Moduls verbunden, sodass im stromlosen Zustand beide Ventile in der Normalstellung geschlossen sind. In diesem Zustand zeigt die Flussrichtung vom Reservoir zur Spritze. So wird auch im Falle eines Ausfalls der 12V Spannungsquelle die korrekte Flussrichtung durch die Mikropumpen gewährleistet.



## Flow of the Actuation Fluid

<p align="center">
  <img src="../Abbildungen/Hydraulic.svg" width=500>
</p>

In the default setting, outputs 'R' and 'A' of the valves are connected. As soon as the valves are operated with 12 V, they switch over so that 'A' and 'P' are connected to each other.

While the flow sensor can measure bidirectionally, the micropump would be damaged if the actuating fluid would be pumped through the micropump in the opposite pumping direction.
