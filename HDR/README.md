This is the readme file for source code for Project 1 of CMPE364 2016 fall

The project is HDR system

The code contains two folder images and src.

The folder images contains images for HDR system to process.

The folder src contains the source code file test.cpp for the project.

Usage:

To compile the project, simply make in the project folder.

LIB USED: OpenCV 3 is required for this program as only OpenCV3 contains code for tonemapping.

To run the program, type "./hdr ./images/Desk.txt". May use the other file "Street.txt" 

File pattern of list file:

First three lines contains g parameter for camera calibration.

The following three lines contains the filename and exposure time. The file must in decending order of their exposure time.

See Desk.txt as example

Warning:

The program do not have much boundary or valid check such as do not have the file listed in listfile, change the order of list file.

