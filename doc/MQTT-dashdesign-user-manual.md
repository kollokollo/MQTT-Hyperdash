## MQTT Hyperdash Dashdesign 

for MQTT Hyperdash V.1.02 (c) by Markus Hoffmann

MQTT Hyperdash runs so-called dash-files. Dash-files are simple text files, 
consisting of UTF-8 encoded ASCII text. They can be edited by any text editor, or
by the special application "dashdesign" which comes with the 
MQTT-Hyperdash package.

dashdesign is a graphical editor for these dashboards. 

The graphic editor is used to select and arrange static and dynamic elements and
their assignment to MQTT parameters in a dashboard. This editor can create
elements, position them individually or in groups on a dashboard, move them and
provide them with various attributes (colors, fonts, line thicknesses, etc.).
Active elements can be "bound" to one or more MQTT parameters. Most elements are
available in different display variants (e.g. three different types of pointer
instruments) and can therefore be adapted to different tasks.

It is also common to first let hddashgen generate a generic dashboard from the
list of toics and then edit these dashboards with dashdesign. This way all buttons
are already there and bound to the MQTT topics and one need only rearange them so
that the dashboard looks nicer and serves more the specific needs of the operators.



