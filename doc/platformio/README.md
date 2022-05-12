# PlatformIO Build

1. Install VScode for your given operating system

1. Open VScode and install the PlatformIO IDE extension
![POI Install](img/pio_build_1.png)
    1. Click on the Extensions icon in the left hand toolbar
    1. Type "platformio" in the search bar
    1. Click on the blue install button and wait it to complete

1. Download or clone this repository to your local computer by clicking the green "Code" button at the top of this github repo page

1. Open the PlatformIO project
![Open Project](img/pio_build_2.png)
    1. Open the PlatformIO extension by clicking on the new icon in the the left hand toolbar
    1. Click on "Open" to open the PlatformIO IDE in the main window
    1. Click on "Open Project"

1. Navigate to your downloaded (unzipped) or cloned repo directory
![Navigate to files](img/pio_build_3.png)
    1. Your path will be dependant on your particular setup
    1. Click "Open"

1. Trust the authors in the next dialog or you will not be able to build and upload in the next steps
![Trust Me](img/pio_build_4.png)

1. Modify the "printer.h" file (if needed) to select the correct board. If your particular board/config is not listed, you can add a new pin mapping as needed
![Board Selection](img/pio_build_5.png)
    1. Select the "printer.h" file
    1. Choose your board by removing the preceding 2 forward slashes (//) to uncomment and adding 2 forward slashes to the invalid entry. **NOTE:** Only 1 board should be "uncommented". If more than 1 is uncommented, your build may fail.
    1. Do not forget to save your changes!

1. Build and upload the firmware to your controller
![Build and upload](img/pio_build_6.png)
    1. Open the PlatformIO extension by clicking on the new icon in the the left hand toolbar
    1. Expand your build environment based on your board. **NOTE:** Ensure your board is plugged into your computer at this time. Driver installation should have been completed beforehand.
    1. Upload the web client filesystem image
    1. Click "Upload and Monitor" to build and upload the firmware. After the upload is complete, the serial monitor will load and you will be able to see whatever console messages the firmware generates, sometimes useful to troubleshoot.
