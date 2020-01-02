## MQTT-Hyperdash
<pre>
VERSION 1.00

(C) 2019-2020 by Markus Hoffmann
(kollo@users.sourceforge.net)

Name        : MQTT-Hyperdash
Version     : 1.00                              Author: Markus Hoffmann
Group       : Development/Languages             License: GPLv2
Packager    : Markus Hoffmann <kollo@users.sourceforge.net>
Summary     : A universal MQTT Dashboard.
Description :

This is a fast and efficient dashboard (GUI) for the MQTT framework. It uses
the SDL library for graphics rendering, the paho library for communication and
the gtk2 framework for User-Unput. The dashboard runs on linux, 
but a port to other operating systems should be easy. 

Special features: 
* Very simple and basic set of base elements. 
* animated analog input elements, potentiometers.
* Fast update rates of 20 Hz are possible.
* Easy to use: Each Panel/Dashboard is configured in a single .dash file.
</pre>

There are two applications included: 

hyperdash runs a panel/dashboard, which has been defined in a dash file. 

<img src="screenshots/Haussteuerung.png">

hyperdash-designer is a graphical tool to create the dash files. However 
this program is still stub and the dash files must be created and edited using 
a text editor by hand. However we also think of automatically create dash files 
from topic lists so one has a good starting point to work with. 

hyperdash-autocreate will do this job (in future). 

The project is still in alpha phase. Please see the CONTRIBUTING.md how you can
help to improve the project. 

I had the impression, an application like this was still missing in 2019. 
So I started this project. Please let me know, if I am right....

## Installation

First you should make sure, that all necessary packages are installed. In doubt, 
do a 

sudo apt-get install libsdl-gfx1.2-dev libsdl1.2-dev libsdl-ttf2.0-dev


You can simply do a 

cd src/

make

sudo make install

## Test run

Open a terminal and do a 

hyperdash main.dash &

have fun....
