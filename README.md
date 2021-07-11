# CVM_Dataset_Generator
This is a linux application developed by Qt5 with OpenCV. It offers the basic functions to generate cross-view image pairs.
It's also the tool that used for generate cross-view image dataset() from original data of sensors.

# Requirements
Qt5, OpenCV3

# Usage
To generate cross-view image pairs, you need to prepare the satellite image with known GPS coordinate of image corners, txt file record the GPS coordinates and txt file record the heading data(optional). 

![Screenshot from 2021-04-30 13-13-33](https://user-images.githubusercontent.com/35421034/125183430-371bf080-e249-11eb-92c5-14ef91169685.png)

After set the image width of satellite image and image interval, you can get trajectory on satellite image and matched sat&grd images under the given path.

Notice that this application provide basic information message.

