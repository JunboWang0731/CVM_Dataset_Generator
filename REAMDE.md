# This is a Qt project for generating cross-view image pairs
## Environment Requirement
Qt 5.9 or higher version
OpenCV 3

## Usage
To generate matched ground-to-aerial cross-view image pairs, at first you need provide:
1. Satellite map with GPS coordinate stamps on its top left and bottum right corner. 
2. Txt file recording the GPS coordinate stamps(each stamp corresponding to a ground-level panoramic image).
3. Panoramic images.
4. (Optional) Txt file recording the heading data which is synchronized with 

Here are main functions:
1. Generate a image which shows the data collection trajectory on satellite map.
2. Generate satellite image block in given spatial resulotioin, width(height) and interval.
3. Generate ground-level image corresponding to GPS stamps and given interval, the orientation of ground-level images can be adjust mannually.
4. Generate correspondence between cross-view image pairs in csv format.



