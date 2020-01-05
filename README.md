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
the gtk2 framework for user input. The dashboard runs on linux, 
but a port to other operating systems should be easy. 

Special features: 
* Very simple and basic set of base elements. 
* animated analog input elements, potentiometers.
* Fast update rates of 20 Hz are possible.
* Easy to use: Each panel/dashboard is configured in a single .dash file.
</pre>

Currently there is only the main application included: hyperdash. But we are
working on following complete set of applications: 

#### hyperdash 
hyperdash runs a panel/dashboard, which has been defined in a dash file. It opens
a window, draws all elements updates wich each new topic message and takes
user input with the mouse to manipulate topics contents. 

<img src="screenshots/Haussteuerung.png">


The project is still in beta phase. Please see the CONTRIBUTING.md how you can
help to improve the project. 

I had the impression, an application like this was still missing in 2019. 
So I started this project. Please let me know, if I am right....

## Installation

### Preparation

First you should make sure, that all necessary packages are installed. 
In doubt, do a 

<pre>
 sudo apt-get install libsdl-gfx1.2-dev libsdl1.2-dev libsdl-ttf2.0-dev
 sudo apt-get install libgtk2.0-dev 
</pre>

Also, it is essential, that the eclipse paho library 
(for the MQTT communication) is installed. 
Here is their homepage: https://www.eclipse.org/paho/

You will need the "C client library". Eclipse provides a download link
and installation instructions. However, if you do not want to install
binary packages, it is totally fine to strat from the git repository:
https://github.com/eclipse/paho.mqtt.c

The "Build instructions for GNU Make" worked for me. Especially you should 
install the package libssl-dev.

<pre>
 sudo apt-get install libssl-dev
</pre>
 
And finally, you may want to install the core set of true type fonts:

<pre>
sudo apt-get install ttf-mscorefonts-installer
</pre>

(If you do not have this package installed, you can still use MQTT Hyperdash, 
but it would be necessary to specify an alternative font path.)

### Compilation

When this is prepared, to make MQTT-Hyperdash, 
you can simply do a 

<pre>
 cd src/
 make
 sudo make install
</pre>

## Test run

Open a terminal and do a 

<pre>
hyperdash main.dash &
</pre>

have fun....
