## Topic naming conventions

for MQTT Hyperdash V.1.02 (c) by Markus Hoffmann

### Motivation

To be able to automatically generate usable dashboards (.dash files) from
simple lists with MQTT topic names, one need to follow a convention so that
one can derive the topic type from its name. 
If the name doesn't match any convention it can only by default be interpreted as
a simple character string value which at best can be displayed as text.
If the topics content shall be interpreted as number or even as a number which 
can be modified by the user, it need to be recognized as such. Therefor we
suggest following standard:

### hddashgen conventions

A Topic name can have a path like filenames, using slashes. e.g.

<pre>
AREA/SYSTEM/SUBSYSTEM/PROPERTY
</pre>

Dashgen will create a tree-like system of dashboard files where you can navigate
with buttons to the next deeper tree level. Until it reaches the PROPERTY level, 
for which it creates dash elements, and predefined dash groups (which can form 
buttons, potentiometers, input fields etc...)

PROPERTY names must therefore match certain patterns.

#### DM AM DC AC SM SC

We use a postfix, like file name endings. If the ending is 
"_DM" this means "digital (integer number) value, measured"
"_DC" this means "digital (integer number) value, control value"
"_DS" this means "digital (integer number) value, simulated value"

"_AM" this means "analog (floating point number) value, measured"
"_AC" this means "analog (floating point number) value, set-point value"
"_AS" this means "analog (floating point number) value, simulation value"

"_SM" this means "String (text) value, automatically generated or measured"
"_SC" this means "String (text) value, set by user for control"
"_SD" this means "String (text) value, pure data"

"_BM" this means "Binary data, automatically generated or measured"
"_BC" this means "Binary data, used for control"
"_BD" this means "Binary data, pure data"

hddashgen makes control values changeable by the user and measure values just
display. Different colors are used as well. The actual appearance of the
dash groups generated for these topics also depends on the full name of the
PROPERTY part of the Topic name. Here special full names are treated specially: 

#### ACTIVITY_DM

For this property an animated activity indicator is generated (a spinning wheel
made out of a BITMAP-LABEL. The value of ACTIVITY_DM is expected to rotate 
between 0 - 1  - 2 - 3 - 0 - ... and so on, with an update rate of about one
second.

#### STATUS_SM

This property will be displayed as status text line.


#### STATUS_DM 

This property will be implemented as BITMAPLABEL to show different bitmaps and
colors depending on the state. E.g. a LED (circle) which changes from red to 
green.


#### ONOFF_DC

This property will generate a group of elements which form a radio-group of 
one ON button and one OFF button.

.... to be extended...

hddasgen is still work in progress and eventually new standard names will be 
added. 



## setpoint parameters and measured value parameters

The most important differentiation class of the parameters is the distinction
between setpoint parameters and actual/measured value parameters. These
parameters usually occur in pairs, e.g. the target current of a power supply, as
specified by the user, and the measured actual current. 

Another class of parameters are state parameters. State parameters are integer
parameters with a minimum value of 0 and a finite maximum value n. Each integer
between 0 and n-1 represents one of n states.

## Using colors

The colors are to be assigned in such a way that there are uniform assignments
for setpoints and actual values as well as for status messages, which make it
easier to find the information you are looking for when you look at a dashboard.
The use of colors does not happen according to (arbitrary) selection, for
example from an aesthetic point of view, but from the point of view that they
carry additional information. However, care must be taken because the loss of
this additional information should not result in significant restrictions for e.g.
color-blind surgeons. This can be maintained by using redundant appearance on
important visuals, e.g. a green circle means "everything is OK" and a red
square means "there are errors".

