---
title: Troubleshooting
nav_order: 4
---

# Troubleshooting

## Further Issues
Please report bugs via the [Git](https://github.com/LnBdd/hyberflow/issues) repository and feel encouraged to contribute to the project by submitting your own developments and improvements.

## Sealing
The two valves required for the hydraulic pressure module are each mounted in a connection socket in order to control the water flow. It is recommended to apply isopropanol to both the connection socket and the valve when inserting it into the connection socket in order to prevent damage to the O-rings on the valve. Before inserting the valve, it should also be checked that all outlets on the connection socket have been printed correctly and allow water to pass through, as the valve can not be removed from the connection socket without causing damage once it has been inserted.

Tube adapters are installed at various positions in the pressure module to enable the flow of water from components such as the connection sockets or the reservoir into the tubes. For this purpose, the tube adapters are fixed in the components using a thread. Attempts were made to print the hose connection directly onto the corresponding components. However, this either led to clogged connections or to the connections breaking off when the support structures were removed. To ensure a complete sealing between the tube adapter and the corresponding component, it is recommended to use thread sealing tape. In addition, there may still be air pockets in the hoses after assembly, which must be completely removed by test runs so that the flow sensor can perform correct measurements.

## Pressure equalization membranes
The self-adhesive pressure equalization membranes contain a membrane that is permeable to air but not to water. This limited permeability is used for ventilation in the reservoir as well as on the divider and syringe cap. If possible, the 3D printing of support structures should be avoided on the intended bonding surfaces for the pressure equalization membranes in order to obtain a surface that is as flat as possible and therefore ensure the adhesion of the self-adhesive membranes. It is also recommended to allow the pressure equalization membranes to dry for 24 hours after application and before first use. Repeated use of a pressure equalization membrane on the divider and syringe cap has shown that the air permeability decreases significantly as soon as the membrane comes into contact with liquid. However, after the pressure equalization membranes have been left to dry, the air permeability is restored and the membrane can be used again for venting in the syringe. It is therefore recommended to prepare several dividers and syringe caps with pressure equalization membranes.

## Reservoir
The reservoir has a maximum capacity of 40 ml. The bottom of the reservoir is angled so that the water flows to one side of the reservoir and is led out of the reservoir through a tunnel system.

## Divider
A divider separates the bioink from the actuating liquid in the syringe. The 3D-printed divider is fitted into a modified glass piston using an O-ring. The hollow glass piston is the glass piston originally supplied with the syringe and therefore has optimum friction properties with complete sealing between the glass syringe and glass piston. The glass bulb is cut into 20 mm thick slices and thus serves as an outer shell for the actual divider.

## Syringe cap
As part of the development process, it was considered to add another connection with a Luer lock system to the syringe cap. This connection could be used to fill the remaining volume of the drive fluid in the syringe after the syringe cap is closed. However, the mechanism described in the instructions proved to be fully functional to fill the remaining volume and the additional connection would pose a risk of possible leakage. The idea of an additional standardized connection was therefore rejected.

## Micropumps and valves
In order to control both micropumps, the mp-Highdriver4 is used in the hydraulic printing module, which can operate up to four micropumps.

Both valves are connected to the NO output of the relay module so that both valves are closed in the default position when there is no power. In this state, the direction of flow is from the reservoir to the syringe. This ensures the correct direction of flow through the micropumps even if the 12V power source fails.
