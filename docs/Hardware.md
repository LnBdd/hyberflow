---
title: Actuation Hardware
parent: Asselmbly
nav_order: 2
---

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
