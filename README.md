# ECE5780_Final_Project

Project presentation and demonstration: [Click Here](https://youtu.be/P-vR60nBt4Y)


 
 - Helpful visualization of working software, schematics, circuit diagrams, utilization, etc. Very important to watch!

Software Setup:
1. Zip all files in repo, and download.
2. Download most recent version of STM32CubeMX and your board is properly configured with up to date packages.
3. Download Keil uVision 5 (MDK-ARM bulk package is good to download off of keil's website as well). After install, in Keil's package manager, be sure to install/update all packages relevant to your specific board (in this case, the STM32F072RB). Download CMSIS package/library.
4. Download PuTTy (this is for UART communication during use of program and for debugging)
5. Download the FTDI VCP Driver needed (this is to enable your UART to TTL bredboard chip as COM3 when plugged into your PC via USB). Download link: [Click Here](https://ftdichip.com/wp-content/uploads/2021/08/CDM212364_Setup.zip)
6. After all this done, unzip your download of this repo in the directory you wish to install this project.
7. To run board setup, run the .ioc file  (FinalProject->core->.ioc file(STM32MX's board pin configurations etc.)
8. Then, under the zip file's FinalProject->MDK-ARM-> uvision project, open this! This is where all your work is being managed/executed inside of uVision!

Hardware Setup:
1. Plug both your STM32F072RB board and USB to TTL (FT232RL) chip into the PC's usb ports (via a Mini USB to USB A cable, 2 needed).
2. Using jumper cables, connect the following pins FROM the FT232RL TO the STM32 board. (RX->PB10, TX->PB11, GND->GND).
3. Using jumper cables, connect the following pins to your hardware circuit (the circuit designed to take in op-amped, filtered EKG electrode signals (up to 8 ADC channels configured).
4. The following pinouts are configured/available channels to process ADC of your incoming signals (on the STM32 board): (Channel1,Channel2,Channel3,Channel4,Channel5,Channel6,Channel7,Channel8) -> (PA7,PB0,PB1,PC0,PC1,PC2,PC3,PC4)
5. Our hardware circuit needed for acquiring/handling analog signals all included in our video link above!!! **
6. Preferably a 15V battery used to power the circuit for all components to work accordingly.

How to use:
1. FinalProject->MDK-ARM-> uvision project, open this!
2. Top left corner, click rebuilt all batch files/project.
3. Click 'Flash' to send this code to your STM32 board.
4. Launch PuTTy, click on option 'serial', enter COM3, and a speed of 115200. Then connect to this server.
5. On STM board, click 'reset' button to start program. Blue LED should come up on board indicating start of program. Print statement in PuTTy should appear saying "type 'start' to begin calibrating".
6. Type 'start.' (with a period after, this is the terminating character)
7. Green LED comes on, message states that program is in calibration mode
8. Calibration mode: Waits until User button pressed. Once pressed, Program rapidly polls all 8 ADC inputs, filtering them recursively with a kalman filter, then storing final filtered data in an array, which is printed in PuTTy.
9. Testing mode: After all calibration gestures entered, testing mode is activated by typing 'test.' into putty which is visually shown as an orange LED. Similar to calibration mode, this relies on the user button to activate. Instead of storing the kalman filtered result, this data is compared against all previously calibrated gesture data in order to identify a match. A match is identifed using relative approximation of all EKG electrodes. PuTTy outputs whether the gesture (data) was identified, unidentified, or an error occurred.
10. Neural network: Project contains a 3-layer neural network to enable classifiers, filters, and pattern recognition in interpreting incoming ADC inputs to categorize them each as unique gestures. This aspect of the project is currently not implemented, due to the unreliability of ADC inputs not containing noise. However, all packages and its framework are already included in project and may be implemented once ADC input data is reliably received.
11. End of program: Once the number of testing gestures is finished, or if a critical error occurs, the relevant message is printed via PuTTy and a red LED turns on.


Additional Information:
1. In the main method, you mau alter the number of gestures or calibrations you'd like to make. This allows you to set the count to 1 if you're testing, and any number you need to store/test. All storage is managed with a 2D array structure.
2. While the neural network libraries, matlab code, and methods are all in the project, they are not implemented in working design. Please get reliable ADC input data of the EKG electrodes (or even reconfigure this project for any ADC input data you need better filtering/storing/identifying!!)
3. On that note, this program can be easily reconfigured for classifying, storing, and filtering any ADC data (up to 8 channels). It heavily relies on reliable ADC data, as unreliable data isn't reliably classified.
4. ADC data is configured and compartmentalized to be individually polled, with continuous mode handled by clock timers on breaking a while loop to enable a variety of different ways and logic to utilize ADC functionality. This is very important to preserve the STM32's processing load to only execute what is absolutely necessary at the time.
5. To improve and fix hardware circuit, it is more preferable to use previously made PCB boards to handle multiple EKG inputs. Use this board as a replacement for the differential amplifier, and just before notch filter and low pass filter in the hardware chain. Link: [Click Here](https://www.olimex.com/Products/Duino/Shields/SHIELD-EKG-EMG/open-source-hardware](https://youtu.be/P-vR60nBt4Y)


Below are some links behind tips on filters/op-aamps in our hardware circuit configuration, ADC input configuration, and neural network configuration.

Sources:
1. Making Circuit for an EKG Electrocardiogram (analog singal acquisiton of EKG's): [Click Here](https://www.instructables.com/Make-Your-Own-Electrocardiogram-ECG/)
3. Congifguring multiple ADC inputs without DMA: [Click Here](https://controllerstech.com/stm32-adc-multi-channel-without-dma/)
5. Deploying MATLAB to C code for STM32 use: [Click Here]([URL](https://www.youtube.com/watch?v=LAcnV_5GruY))
7. Reliable PCB for up to 6 ADC inputs: [Click Here]([URL](https://www.olimex.com/Products/Duino/Shields/SHIELD-EKG-EMG/open-source-hardware))
9. Our project's presentation, explanation and demo: [Click Here](https://youtu.be/P-vR60nBt4Y)

