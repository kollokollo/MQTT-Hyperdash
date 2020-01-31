## Topic naming conventions

for MQTT Hyperdash V.1.02

### Motivation

Making dashboard files by hand with a text editor or even the graphical editor
can be a lot of annoying manual work. To help this, there is the tool hddashgen
to automatically create dash files from topic lists, so one has a good starting
point to work with. hddashgen will do this job in combination with
mqtt-list-topics.

For this reason, a dashboard generator is offered to support the dashboard
development. This generator generates one or more, optionally also interlinked
dashboards from a given set of MQTT parameters with a uniform arrangement of the
parameters and a consistent use of graphic elements and their layout. The
generator supports all parameter types and assigns graphical elements (or sets
of elements) that are adapted to the individual parameters.

The generator can also create a "tree" of hierarchically linked dashboards,
which corresponds to the structure of all parameter names of the MQTT system and
allows access to each parameter. To do this, it analyzes the namespace of all
parameters known to the broker and uses it to create a chain of dashboards,
which at the end of the tree in turn point to automatically generated dashboards
that are assigned to the direct control of a device or subsystem. 

To be able to automatically generate usable dashboards (.dash files) from simple
lists with MQTT topic names, one need to follow a uniform parameter naming
convention so that one can derive the topic type from its name. 



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

## Style Guides

These requirements should be supplemented by a convention, which gives a rough
guideline for the development of new dashboards. Here, for example,
the choice of color and the appearance of certain elements (e.g. simulated
buttons) or the placement and appearance of text information are specified. The
aim is to create a "MQTT user interface style guide" for the graphical user
interface, so that with dashboard development even with several people, uniform
operation with maximum ergonomics can be achieved.

## Standard for entering an analog parameter

The simulated potentiometer is the most important input element for the
quasi-continuous change in value of a parameter. Such a potentiometer has a
variable size and is usually made up of several sub-elements that were
automatically generated and placed. In addition to the controller mark to be
moved by the mouse, each potentiometer also includes an increment and
a decrement element (TICKER). With these elements, the value of the assigned parameter
can be increased or decreased with the finest possible resolution by "mouse
click". The control mark is moved using the mouse, the scroll wheel or the arrow
keys on the keyboard.

If the right mouse button is pressed on a potentiometer, a box is
displayed which allowes several setting of this potentiometer. In
addition to the name of the parameter, the fields also contain the currently
saved parameter value. A new value can be entered in this field using the
keyboard, which can then be applied with the button below. To prevent a setting
from being lost, a parameter value can be saved by activating the "Memory" button
and called up again if necessary. The associated memory field shows the last
saved value.

The gradation with which value changes can be applied to a parameter depends on
the defined resolution of the potentiometer (TICS). The resolution can be
individually defined by typing values in the fields for a lower and
an upper limit (AMIN and AMAX). 
In addition, three standard settings can be selected: the
minimum resolution, a resolution for a symmetrical value interval of 10% around
the currently set value, or the maximum resolution at which the interval is set
symmetrically around the current value so that exactly five increments in each
direction with the finest possible gradation are offered.

## Standard Dashboard categories


In most cases there are many different ways to display the information of a 
complex system. The main focus can be different and such the design of a 
dashboard results in different variants. 

To help this, we propose following (main) structure:

The entire dashboard structure should be divided into three logical and 
hirachical levels: 

* on the top level, the abstract dashboard structure is based on the function
  and the operating modes of the whole system to be set. The whole system can be 
  a complex machine, a factory hall, a production line or simply a smart home.
* The middle section contains all dashboards for controlling entire areas or 
  subsystems. These can be individual machines or different device functionallities like 
  temperature sensing, garage door handling etc...
* Finally, the lowest level contains all dashboards for direct, technically 
  oriented access to all devices. Here is a good place to have register access and
  reset functions on a hardware level.

Wherever possible, each dashboard is implemented in three variants: 

* a physical variant that gives abstract access to essential parameters;
  Thinking of a smart home these could be the main settings "I am home" or "I want
  to set comfort environment"...

* a technical variant (like a subsystem as a block diagram), which makes the 
  parameters of connected devices accessible from the perspective of a 
  technician; 

* and a geographical variant, which offers the locality of devices or subsystems 
  as essential information (in geographical context) and forms a supporting 
  function in troubleshooting in the event of a fault.

