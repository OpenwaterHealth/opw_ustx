# Openwater Ultrasound Transmit Module (USTX)

This repository contains the mechanical designs and code for the USTX module. 

The USTX module is a **Work In Progress**, but represents Openwater's work towards a standalone, low-cost ultrasound transmit module for the Open-LIFU platform.

# Operating Instructions
## How to bring up USTX
1. Install STM32CubeIDE in order to download embedded microcontroller firmware
2. Clone the This repository from github
	- Create USTX folder (e.g. C:/Openwater/USTX/) 
	- Open a command prompt (windows->cmd)
	- git clone https://github.com/OpenwaterHealth/opw_ustx.git
3. Open the STM32CubeIDE
	- Set the workspace to: embedded/ustx_cpp/
	- Import Project: embedded
	- Check the ustxtest folder
	- In the left panel, you should see a blue IDE icon that says ustxtest, if you click the expand arrow you can see all the associated files.
	- Go to “Project” menu, and click “Build All”
4. Power on USTX
	- For programming power provided from the usb-C cable is sufficient, no need for 12V barrel jack connector).

5. Plug in the JTAG debug dongle (6pin) to the J-link tool and to USTX
	- Once connected, in the project explorer (menu on the left), click ustxtest.elf.launch, and then click the green run arrow at the top of the screen
	- If successful, green indicator light should come on
	- To test: Open Putty, connect to ustx by selecting serial, entering COM port, and using baudrate 921600. If it connects, the green indicator light should start flashing
![image](https://github.com/OpenwaterHealth/opw_ustx/assets/6217005/e549e51b-4d97-4533-b4d7-55df59b73eec)

## How to run and test the USTX
In order to run and test the USTX, there is a USTX-Onda Utility function that allows you to program it to work with a specific probe, at a specific frequency, for various foci.

### To use the USTx-Onda Utility:
1. Navigate to: C:/Openwater/system/util/ustx-onda/msvc/
	- Open: ustx-onda.sln
	- In the ‘Solution Explorer’ panel, right click on ustx-onda, scroll down to ‘project only’ and select ‘build only ustx-onda’
  
### To Run:
1. Follow these steps to safely power up the board:  
    1. Plug in a 12V barrel jack.
    2. Plug in the USB cable and connect to the PC.
    3. Turn on input power through +/-VDD.
    4. Program the board using the PC.
    5. Turn on the trigger signal.

Please note that plugging the USB cable before the barrel jack will prevent the board from starting properly, the barrel jack needs to be plugged in first.

2. Open a command prompt (windows-> cmd)
  1. cd /system/util/ustx-onda/msvc/x64/Release
  2. ustx-onda.exe <json file> <ustx com port number>
	
## Troubleshooting:
- If you place the json configuration file in the same directory as the ustx-onda.exe you can just call the file by name, otherwise you will need to provide a full path to the file you want to use
- To check the com port number of ustx, open the Device Manager and look under the COM heading when you plug in the ustx
- Make sure you have the right parameters in your configuration file (see example below, or look at system/component/inc/ustx.h for a more detailed explanation of the parameters)

## Example JSON File:
```
{
  "frequency" : 5e6,
  "waveforms": 1,
  "repeat_count": 5,
  "repeat_time": 0.001,
  "cw_mode": false,
  "speed_sound": 1500.0,
  "n_elements": 128,
  "pitch": 3e-4,
  "max_angle": 0.78,
  "f_number": 1,
  "probe": "L11-5v",
  "radius": 0,
  "max_elements": 30,
  "foci": [
    [0.000, 0.025],
    [-0.010, 0.010],
    [0.010, 0.010]
    ]
}
```

- Frequency in Hz
- Waveforms * repeat_count = number of cycles
- Max waveforms = 4, max repeats = 32
- Repeat_time uses ‘elastography mode’, IMPT: this overwrites repeat count. It will allow you to run the ustx for >128 cycles
- cw_mode (always leave this as false, it’s no longer supported)
- Speed_sound (meters / sec)
- N_elements: number of elements in your array
- Pitch: m
- Max_angle: Should probably get rid of this, was an alternative to f-number
- Probe: this is important for distinguishing between L/C/P type arrays, see ustx.cpp to see where this is used
- Radius: only necessary for curvilinear arrays
- Max_elements: max elements to use for curvilinear array
- Foci: x and z focal locations in meters



## What are the differences between v1.0, 1.1, 1.2 and where are the physical boards and electronic design files?
- V1.0 (red) were initial boards. They have some mistakes (you can see post-fab rework on them). Probably best not to use these if possible. They can be found in the ultrasound cabinet.
- V1.1 (blue) was the 2nd rev. Improvements include updated power supply, and fixes to v1.0 mistakes. Still had 1 issue requiring manual rework. These boards are mostly distributed in the structural labs and at Explora. Any remaining ones should be stored in the ultrasound cabinet.
- V1.2 (green). Essentially the same as 1.1, however the reworked part is fixed at the fab level. These are stored in a box from Tempo in the cubby where John was bringing up Strickland. They need to have the embedded microcontroller code flashed when they are first brought up (see next question).
- All of the Altium design files live in the Electronics/ustx directory on the drive. Often there are PDF schematics in there as well.


## How does USTx work?
### Architecture:
- USTx is communicated to through the ustx.cpp (and ustx.h) API. A USTx object creates a serial connection (you specify the COM port) that can send and receive commands to the microcontroller that drives the board. The USTx object is configured via json (either a file, or an object) specified parameters. Before running USTx, you generate a list of foci (saved as a .hex file). This is downloaded into the flash memory on the USTx board. When USTx receives a trigger (to the “input” BNC jack), it will fire the computed pattern on the desired pins to generate a focus. It is architected such that the pulse will always arrive at the desired focus at the same time after the trigger signal, regardless of depth.
- USTx will only emit a focus when it receives an input trigger. It does not have the capability to output a TTL trigger signal for synchronization.
- There are four TX7332 chips on USTx. Each chip has 32 lanes. Each lane drives one pin on the Verasonics connector, for a total of up to 128 elements.
- There is a usb-C cable that provides power to the board, and serial communication to the microcontroller.
- There is an optional barrel jack that can provide power to the board (generally not used).
- There is a plug for a high voltage DC power supply. Note: We have 1 extra, unused, power supply
  - Sample order of power supply
  - Note: Most recent orders (SN: 527830 & 527831) registered at  https://www.aimtti.com/webform/product-registration for extended 3 yr warranty
- N.B.: ORDER OF PLUGGING THINGS IN MATTERS: First plug in the USB, then plug in the barrel jack, last plug in the DC power supply. We have fried USTxs by doing this wrong.

### Settings:
#### CW mode vs pulsed mode
##### CW mode
- Original (true CW mode) is no longer available. It had serious limitations, the worst of which was that it could only drive frequencies that were multiples of a particular clock cycle (ie: 6.25MHz not 5MHz)
  - Pretty sure that the available frequencies follow the master clock frequency divided by a power of two, (200 MHz / 2n), hence 200 MHz / 25 = 6.25 MHz
- Current CW mode (referred to in TX7332 documentation as elastography mode): if ‘repeat_time_’ variable is set, USTx will drive output for that duration. Note that, empirically speaking, the board tends to go into thermal shutdown (there is a query function for thermal shutdown (ThermalShutdown()), as well as a red indicator light), if driven for a repeat_time_ of more than ~2ms
##### Pulsed mode
- We have defined a pattern to generate an approximation of a sine wave. Because we are not using multiple different waveforms, we can take advantage of both the ‘waveform_’ (max 4) and the ‘repeat_count_’ (max 32) variables to generate up to 128 cycles (4 * 32) per trigger.
  - Note: because we multiply the waveform_ * repeat_count_ to generate the number of cycles, there are certain numbers that are not accessible. Currently OSSP (scanner.cpp, InitializeUSTx function) will print a warning if it rounds the requested number of cycles to something that is achievable
#### JSON configuration
- JSON file: USTx can be configured using a json file (see ustx.h for a sample file, or ustx-onda utility section of this document). That allows you to directly edit the file to specify the different transducer parameters. The ustx-onda.exe uses this method of configuring USTx.
- JSON object: OSSP uses a json object in the scanner.cpp (InitializeUSTx function) to configure USTx. The desired parameters are read in from the scan_metadata file and set during initialization using the LoadJSON function (defined in ustx.cpp).
- Bug notes: There are some functions defined in ustx.h/ustx.cpp that allow for individual setting of waveforms (SetWaveformCycles). This function may have a bug, and it’s generally recommended to use the two methods of json configuration described above.
#### Incrementing vs non-incrementing mode
- Non-incrementing mode (typically used for onda characterization using USTx). In this mode, each time USTx receives a trigger to the ‘input’ BNC jack, it will fire the focus at the current focus index (to query focus index: GetFocus, to set: SetFocus, to get focus location in mm, use GetFocusCoordinates) in the focus list that has been downloaded via the .hex file. To set non-incrementing mode use (SetFocus(int focus_index, bool increment = false)). You will have to call SetFocus to output a different focus index (this is built into the ustx-onda.exe utility).
  - Notes about focus list: The focus list is retained in memory until a new focus list is downloaded (DownloadHex), even if you disconnect, unplug, etc, if you don’t download a new list, it will use the last one available
- Incrementing mode (used in OSSP). In this mode, USTx will automatically increment to the next focus index after receiving a trigger from the ‘input’ BNC jack. When it reaches the end of the focus list, it will increment back to index 0.
  - Bug notes: Every once in a while the software will hang up while downloading the hex file (the indicator light may get stuck green instead of flashing). If things aren’t working, there is a reset button (next to the indicator LED). Try pushing that, the LED should start blinking again. Known bug: after reset, the focus incrementing seems to get off by one, which will cause a row-repeat in OSSP. It should correct itself after a call to SetFocus, which is done automatically if a row is repeated in OSSP.
#### Foci
- Before USTx is run, foci are added to a list (AddFocus) using their x&z locations [this function must be called by the user who generated the USTx object]
  - When this function is called, internally ustx.cpp computes the necessary pattern for each element needed to generate this focus. It uses a number of private functions that compute the delay for each element, and determine whether that element should be turned on or not.
    - The private ComputeDelay function uses simple geometry to calculate the delay for a given element and a given focus. However, it is slightly more complex for a curvilinear probe.
- Once AddFocus has been called for all the desired foci, the user should then call ComputeFoci and DownloadHex. These functions will compute the necessary delays/patterns for each element, and save them to the .hex file. Then this .hex file should be downloaded to the USTx flash memory (this may take a while, there is a ‘waiting’ output but it tends to underestimate the time needed).
  - There is a private function ComputePulsed (and it’s equivalent, ComputeCW) which calls ComputeDelay for each element, and assigns the result to a particular lane (32 available lanes per chip) on a particular TX7332 chip (4 chips per USTx board). This function also maps the element number for a particular transducer to the output pin on the Verasonics connector using the verasonics_pinmap variable. This variable comes directly from the verasonics documentation, and is generally applicable for all verasonics probes. For some transducers, such as the P4-1, there is a second mapping which goes from the element number to the verasonics pinout (p4_1_elementmap variable). This map is obtained from the verasonics documentation as well (computeTrans.m, Trans.Connector variable).
    - Note: If you edit the ComputePulsed function to add different element mapping, you should also edit the equivalent lines in ComputeCW to prevent any surprises if for some reason anyone ever goes back to CW mode.
#### Timing
- USTx is designed to ensure that every pulse on a given focus list hex file arrives at its focal point the same time after the trigger signal, regardless of depth or lateral distance.
  - After a focus list has been computed, this global delay between the trigger signal and the arrival of the pulse to its focal location can be queried using GetDelay
- This delay is determined by hardware limitations on the TX7332 chip, therefore, in structural/pulsed systems a second delay before the trigger to USTx (‘channel B delay’) may be used to ensure the laser pulse and the sound wave all reach the focus at the same time.
Currently, OSSP is designed so that the laser pulse occurs when the ultrasound is at the focal spot, not when it is in the gel wax layer. Simon has experimented with adjusting the timing to get the laser pulse to arrive when the sound wave has reached the gel wax layer.


# License
open-LIFU is licensed under the GNU Affero General Public License v3.0. See [LICENSE](LICENSE) for details.

# Investigational Use Only
CAUTION - Investigational device. Limited by Federal (or United States) law to investigational use. The system described here has *not* been evaluated by the FDA and is not designed for the treatment or diagnosis of any disease. It is provided AS-IS, with no warranties. User assumes all liability and responsibility for identifying and mitigating risks associated with using this software.
