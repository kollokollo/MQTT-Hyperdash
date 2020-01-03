## The .dash file format

for MQTT Hyperdash V.1.00 (c) by Markus Hoffmann


MQTT Hyperdash runs so-called dash-files. Dash-files are simple text files, 
consisting of UTF-8 encoded ASCII text. They can be edited by any text editor.  

Each line contains the definition of one element. If te last charackter of the
line is a '\' (backslash), the next line will be linked to this line. You can use 
this to better format the dash files, e.g. to split the definition of one 
element over mutliple real lines. 

A line with an element definition has following structure: 

elementname : one or more KEY=VALUE pairs separated by whitespace.

All keywords are case insensitive. 

The order of KEY=VALUE pairs is irrelevant. They are all optional. 
If missing, default values apply.

String values should be enclosed by "" if they contain whitespaces and when
case matters.

Comments are marked with a '#' at the beginning of a line.

Empty lines will be ignored. 

Each dash file must contain excactly one "PANEL" element and 
one "BROKER" element. 


### Element types

Currently there exist following element types:


|Element       |   Recognized keyvalues                        |
|--------------|:----------------------------------------------|
|PANEL         | W H TITLE FGC BGC                             |
|BROKER        | URL USER PASSWD                               |
|BOX           | X Y W H FGC                                   |
|PBOX          | X Y W H FGC BGC                               |
|CIRCLE        | X Y W H FGC                                   |
|PCIRCLE       | X Y W H FGC BGC                               |
|LINE          | X Y X2 Y2 FGC                                 |
|FRAME         | X Y W H REVERT                                |
|FRAMETOGGLE   | X Y W H                                       |
|BITMAP        | X Y BITMAP FGC                                | 
|ICON          | X Y ICON                                      |
|TEXT          | X Y H TEXT FGC FONT FONTSIZE                  |
|TOPICSTRING   | X Y H TOPIC FGC BGC FONT FONTSIZE                         |
|TOPICNUMBER   | X Y H TOPIC FGC BGC FONT FONTSIZE FORMAT                  |
|HBAR          | X Y W H TOPIC FGC BGC AGC MIN MAX                         |
|VBAR          | X Y W H TOPIC FGC BGC AGC MIN MAX                         |
|METER         | X Y W H TOPIC FGC BGC AGC MIN MAX AMIN AMAX 	           |
|VMETER        | X Y W H TOPIC FGC BGC AGC MIN MAX		           |
|HMETER        | X Y W H TOPIC FGC BGC AGC MIN MAX		           |
|TEXTLABEL     | X Y H TOPIC BGC FONT FONTSIZE TEXT[n]                     |
|BITMAPLABEL   | X Y TOPIC BGC BITMAP[n]                                   |
|FRAMELABEL    | X Y W H TOPIC MATCH                                       |
|SHELLCMD      | X Y W H CMD                                               |
|DASH          | X Y W H DASH                                              |
|TOPICINAREA   | X Y W H TOPIC VALUE QOS                                   |
|TOPICINSTRING | X Y W H TOPIC QOS                                         |
|TOPICINNUMBER | X Y W H TOPIC FORMAT MIN MAX QOS                          |
|HSCALER       | X Y W H TOPIC FORMAT MIN MAX TIC QOS BGC FGC AGC          |
|VSCALER       | X Y W H TOPIC FORMAT MIN MAX TIC QOS BGC FGC AGC          |
|TICKER        | X Y W H TOPIC FORMAT MIN MAX TIC QOS                      |
|PLOT          | X Y W H TOPIC TYPE N OFFSET MIN MAX AMIN AMAX BGC FGC AGC |


### Common keyvalue formats

<pre>
* X=<pixels> Horizontal position of upper right corner of the element in pixels
* Y=<pixels> Vertical position of upper right corner of the element in pixels
* X2=<pixels> Horizontal position of the endpoint of a line in pixel coordinates
* Y2=<pixels> Vertical position  of the endpoint of a line in pixel coordinates
* W=<pixels> Width of the element in pixels
* H=<pixels> Height of the element in pixels
* TITLE=<String> Title of the dashbord window.
* URL=<String> URL of the broker to connect to. Example: URL="tcp://localhost:1883"
* USER=<String> Username for connection
* PASSWD=<String> Password for connection
* FGC=<color value> Foreground color. The color values are 32bit RGBA. 
  They can be specified using a hey value $rrggbbaa:
  e.g. FGC=$ff4532FF  
* BGC=<color value> Background color. 
* BITMAP=<filename> Specify a bitmap file name relative to the bitmap path. The
  bitmap files need to be in .xbm (X-Window bitmap) format and can be generated 
  and edited with the simple application "bitmap". 
* ICON=<filename> Specify an image file name relative to the icon path. These 
  files must be .png (Potable network grapics) files.
* TEXT=<String> Specify a text to be shown.
* FONT=<fontname> Specify a text font to use. These fonts must be TrueType Fonts
  usually installed in the standard path /usr/share/fonts/truetype/msttcorefonts
  There a file with .ttf ending is searched. 
* FONTSIZE=<value> Specify the size of the font. This defaults to 16.
* TOPIC=<TompicName> The name of the topic of a dynamic element. 
* REVERT=<0 or 1> Specifys if the frame is drawn in revert state.
* MATCH=<String> A String to match the topics content.
* TEXT[n] Are one or multiple Labeldefinitions of the form 
  TEXT[n]="<match>|<text>|<fgc>". E.g. TEXT[0]="0|Hello|$ffffffff"
* BITMAP[n] Are one or multiple Labeldefinitions of the form 
  BITMAP[n]="<match>|<bitmapfile>|<fgc>". E.g. BITMAP[0]="0|SmallTriangle|$ff0000ff"
* VALUE=<String> A String to apply to a topic.
* QOS=<0,1 or 2> Qiality of service for publication to the broker. Defaults to 0.
* TIC=<number> An increment used by the ticker. The increment can aswell be negative.
* TYPE=<number> The type of the plot.
* OFFSET=<number> ignore the first n numbers of the toic content.
* N=<number> use only n numbers of the toic content.
</pre>

### Static elements 

Static elements are just drawing primitives. They do noch change with topics 
contents.

PANEL: TITLE=<title> W=<width in pixels> H=<height in pixels> FGC=<colorspec> BGC=<colorspec>

This sets the title of the window/screen and specifies its fixed size in pixels. 
Also a default foreground and background color can be specified. 

BROKER: URL=<broker url> USER=<username> PASSWD=<password>

This sets the mqtt broker url, and also username and password if required. 

LINE: X=<x-coordinate> Y=<y-coordinate> X2=<x-coordinate> Y2=<y-coordinate> FGC=<colorspec> LINEWIDTH=<line width in pixels>

Draws a simple line of given width in given color. 

TEXT: X=<x-coordinate> Y=<y-coordinate> H=<height> FGC=<colorspec> FGC=<colorspec> TEXT=<the text> FONT=<fontspec> FONTSIZE=<size>

Draws a text at coordinates (X,Y) in given color with a given font. Fonts are
true type fonts (.ttf). The size (height) if the Charackters can be specified with
FONTSIZE. H defines the textbox height. The text will be vertically centered to this height.

BOX: X=<x-coordinate> Y=<y-coordinate>  W=<width in pixels> H=<height in pixels> FGC=<colorspec> LINEWIDTH=<line width in pixels>

Draws a simple box. 

PBOX:  X=<x-coordinate> Y=<y-coordinate>  W=<width in pixels> H=<height in pixels> FGC=<colorspec> BGC=<colorspec> LINEWIDTH=<line width in pixels>

Draws a filled box. 

FRAME: X=<x-coordinate> Y=<y-coordinate>  W=<width in pixels> H=<height in pixels>  REVERT=<0/1>

Draws a shadowed frame.

ICON: X=<x-coordinate> Y=<y-coordinate>  NAME=<icon name>

Draws an image, usually a png-file.

BITMAP: X=<x-coordinate> Y=<y-coordinate>  NAME=<bitmap name> FGC=<colorspec>

Draws a monochrome bitmap image using the color specified. Bitmaps are drawn with transparent
background, so one can 
draw multiple bitmaps one after another to combine more complicated graphics.

### Dynamic output Elements

Dynamic elements are controlled by the content of a topic. They change appearance when the topic content
changes. But they cannot take user input. They are used to display data. 

TopicString: X=<x> Y=<y> TOPIC=<topic> FGC=<colorspec> BGC=<colorspec> FONT=<fontspec>

Displays the topic messages as they are. 

TopicNumber: X=<x> Y=<y> TOPIC=<topic> FGC=<colorspec> BGC=<colorspec> FONT=<fontspec> USING=<formatter>

Evaluates the topic message as a number and then uses formatter to display them. The Formatter should
be a string also used in BASIC PRINT USING statements. 

TopicFrame:  X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> MATCH=<content> 

Draws a Frame, and if the topics content matches MATCH-content, it is drawn in reversed state. 

#### Meter: X= Y= W= H= TOPIC= MIN= MAX= AMIN= AMAX=

Draws a round meter using the topics content interpreted as a value between min and max. 

#### HMeter: X= Y= W= H= TOPIC= MIN= MAX=

Draws a horizontal meter using the topics content interpreted as a value between min and max. 

#### VMeter: X= Y= W= H= TOPIC= MIN= MAX=

Draws a vertical  meter using the topics content interpreted as a value between min and max. 

TopicVBar: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> MIN=<value> MAX=<value> FGC=<colorspec> BGC=<colorspec>

Draws a vertial bar, which is filled by the percentage of the topics content interpreted as a value
between min and max. 

TopicHBar: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> MIN=<value> MAX=<value> FGC=<colorspec> BGC=<colorspec>

Draws a horizontal bar, which is filled by the percentage of the topics content interpreted as a value
between min and max. 


TopicTextLabel: TEXTLABEL=[text0,text1,text2,text3]

According to the content of the topic, which is interpreted as integer value (0,1,2,....) the 
corresponding text is displayed. If the integer does not match any text index, nothing is displayed. 

TopicBitmapLabel: BITMAPLABEL=[bitmap1,<colorspec1>;bitmap2,<colorspec2>;bitmap3,<colorspec3>;] BGC=<colorspec>

According to the content of the topic, which is interpreted as integer value (0,1,2,....) the 
corresponding monochrome bitmap with the given color is displayed. 
This way status indicators can be implemented. The whole set of bitmaps all use the same background color.

#### Plot: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> 
The topic content is expected to be a comma separated or space separated list of
number values. If OFFSET is specified, the first offset numbers will be ignored. 
If N is specified, only at maximum n values are used. 
These are then plotted into a graph. With TYPE you specify the
type of graph produced. e.g. Type=0 only plots dots, TYPE=1 impulses, 
TYPE=2 histogram lines, TYPE=3 normal lines, and so on. 
The horizontal scaling can be influenced using AMIN and AMAX, the vertical
scaling is controlled by MIN and MAX. You can specify a background color (BGC), a 
foreground color (FGC) and a color for frame and axis (AGC). 

### Dynamic input Elements

They are like the dynamic output elements with the extra feature that they allow user input.
You can enter numbers, change states etc... 

#### TopicInString: X= Y= W= H= TOPIC= [QOS=]

Clicking in this area,  
the user can enter a string which is then published to the topic using QOS2.

#### TopicInNumber: X= Y= W= H= TOPIC= MIN= MAX= FORMAT= [TIC=] [QOS=]

Clicking in this area,  
the user can enter a number which is then published to the topic using QOS2.
The range of the entered number is checked against MIN and MAX and clipped
if necessary. The topic contend is finally formatted using the FORMAT pattern.

#### VScaler: X= Y= W= H= TOPIC= MIN= MAX= TIC= FORMAT= [QOS=] [FGC BGC AGC]

This element draws a vertical scaler. A foreground color (FGC) a 
background color (BGC) as well as a frame color (AGC) can be specified. 
The user can slide the scaler to change the number of the topics content. 
The user can also click into the scalers background, and a dialog to enter
the number will be offered. 
The range of the entered number is checked against MIN and MAX and clipped
if necessary. The topic content is finally formatted using the FORMAT pattern.
During sliding, the values are published to the TOPIC at each move of the mouse
if the results produced are bigger than TIC. During sliding the values are 
always published with QOS=0. Only the final value (after releasing the sliders
knob) will be published with the Quality of Sevice specified. 

#### HScaler:  X= Y= W= H= TOPIC= MIN= MAX= TIC= FORMAT= [QOS=] [FGC BGC AGC]

This element draws a horizontal scaler. A foreground color (FGC) a 
background color (BGC) as well as a frame color (AGC) can be specified. 
The user can slide the scaler to change the number of the topics content. 
The user can also click into the scalers background, and a dialog to enter
the number will be offered. 
The range of the entered number is checked against MIN and MAX and clipped
if necessary. The topic content is finally formatted using the FORMAT pattern.
During sliding, the values are published to the TOPIC at each move of the mouse
if the results produced are bigger than TIC. During sliding the values are 
always published with QOS=0. Only the final value (after releasing the sliders
knob) will be published with the Quality of Sevice specified. 


#### TopicInArea: X= Y= W= H= TOPIC= VALUE= [QOS=] 

If the user clicks in the area, the content VALUE will be published to the 
TOPIC with the specified Quality of Service (QOS).


### Elements for Application control

ApplicationClickArea: X=<x> Y=<y> W=<w> H=<h> CMD=<shell command>

An invisible click area. When the user clicks it, a shell command will be excecuted.

ApplicationClickFrame: X=<x> Y=<y> W=<w> H=<h> CMD=<shell command>

The same as ApplicationClickArea but when the user clicks it, a frame toggle will be shown. 
This way a button can be simulated. 

DashClickArea: X=<x> Y=<y> W=<w> H=<h> DASH=<dashbord name>
DashClickFrame: X=<x> Y=<y> W=<w> H=<h> DASH=<dashbord name>

Same, but it opens another window with another dashboard.


More element types can be thought of, but our goal is to keep everything as simple
as possible.

