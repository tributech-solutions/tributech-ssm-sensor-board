# Introduction 
This program implements a USB host on a XMC RelaxKit. It is used to demonstrate how to provide data to a Tributech Sensor Security Module.
A Sensor Security Module is a data security device which ensures the integrity of the data provided and prevents manipulation of the data.
The RelaxKit in combination with an Infineon DPS310 preasure and temperature sensor is used to generate data. This data is then transfered via USB connection to the Sensor Security Module. 

# Getting Started
Software requirements:
1. [DAVE<sup><b>TM</b></sup>  Infineon Technologies India Pvt. Ltd](https://infineoncommunity.com/dave-download_ID645) version 4.4.2 or higher
2. [Segger J-Link Driver](https://www.segger.com/downloads/jlink/) version 600b or higher
3. Agent Companion

Hardware Requirements:
1. XMC4700 RelaxKit provided by Tributech <sup>*</sup>
2. Micro-USB to USB-C cable
3. Tributech Sensor Security Module (SSM)
4. [My IoT Adapter Infineon](https://www.infineon.com/cms/de/product/evaluation-boards/my-iot-adapter/)
5. [DPS310 barometric preasure sensor](https://www.infineon.com/cms/de/product/sensor/pressure-sensors/pressure-sensors-for-iot/dps310/)

<sup>*</sup> This project does not work with a standardized XMC4700 RelaxKit. The hardware has to be provided by Tributech.

## Set up 

First see that all the requirements above are met.
When all the requirements from above are met the project can be imported in Infineon DAVE<sup><b>TM</b></sup>.
The project has to be imported as an Infineon DAVE<sup><b>TM</b></sup> project. 
Before flashing the program to the RelaxKit check the ValueMetaDataIds in the program and
check that the SSM also has those ValueMetaDataIds in its memory.
This can be done with the Agent Companion provided by Tributech, which is also used to configure the SSM.

The last step is to power the RelaxKit and the SSM separately and connect them both.
Then the RelaxKit will provide values for the SSM which will process the values and forward them to the Tributech Data Space Kit. 


# Additional Resources:

https://www.tributech.io/