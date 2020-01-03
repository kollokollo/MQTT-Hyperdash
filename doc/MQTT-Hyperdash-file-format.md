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
|TOPICSTRING   | X Y W H TOPIC FGC BGC FONT FONTSIZE                       |
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

#### Panel: TITLE= W= H= [FGC= BGC=]

This sets the title of the window/screen and specifies its fixed size in pixels. 
Also a default foreground (FGC) and background color (BGC) can be specified. 
Each dash file must contain exactly one panel element. 

#### Broker: URL= [USER= PASSWD=]

This sets the mqtt broker url, and also username and password if required. 
Each dash file must contain exactly one broker element. 

#### Line: X= Y= X2= Y2= FGC= LINEWIDTH=

Draws a simple line of given width in given color from (x,y) to (x2,y2). 

#### Text: X= Y= [H=] FGC= TEXT= FONT= FONTSIZE=

Draws a text at coordinates (X,Y) in given color with a given font. Fonts are
true type fonts (.ttf). The size (height) of the charackters can be specified 
with FONTSIZE. H defines the textbox height. The text will be vertically 
centered to this height. If H is not specified this defaults to the Font height 
of the specified font. If the specified font cannot be found or loaded the 
font defaults to an ugly 8x8 pixel monospaced font. If not specified, the 
FONTSIZE defaults to 16. The text is transparent and has no background. This 
way, you can stuff multiple charackters or texts on top of a bitmap or other
graphical elements. 

#### Box: X= Y=  W= H= FGC= [LINEWIDTH=]

Draws a simple box with the given foreground color. 

#### Circle: X= Y=  W= H= FGC= [LINEWIDTH=]

Draws a simple ellipse fitting in the box X,Y,W,H with the given 
foreground color. 

#### PBox:  X= Y=  W= H= FGC= BGC= [LINEWIDTH=]

Draws a filled box. Fill color is BGC, border color is FGC. 

#### PCircle:  X= Y=  W= H= FGC=  BGC= [LINEWIDTH=]

Draws a filled ellipse fitting in the box X,Y,W,H with the given 
foreground color. Fill color is BGC. 


#### Frame: X= Y= W= H= [REVERT=]

Draws a shadowed frame. Revert can be 0 or 1. If REVERT=1, the frame is 
drawn in reverted state. This element can be used do draw buttons or other
stylistic 3D effects around other elements. 


#### Icon: X= Y= ICON=

Draws an image, usually a png-file, at position x and y. The file name must be
specified with ICON. The file is searched for in the iconpath. 

#### Bitmap: X= Y= BITMAP= FGC=

Draws a monochrome bitmap image using the FGC color specified. 
Bitmaps are drawn with transparent
background, so one can 
draw multiple bitmaps on top of another to combine more complicated graphics.
The file name must be
specified with BITMAP. The file is searched for in the bitmappath.

### Dynamic output Elements

Dynamic elements are controlled by the content of a topic. They change 
their appearance when the topic content
changes. But they cannot take user input. They are used to display data. 

#### TopicString: X= Y= W= [H=] TOPIC= FGC= BGC= FONT= FONTSIZE=

Displays the topic messages as they arrive using the specified FONT. When new
data arrives, the old text need to be cleared, therefor a width (and Height) of 
the box to be cleared with background color (BGC) need to be specified. 
If H is ommitted, the font height is used. 

#### TopicNumber: X= Y= [W=] [H=] TOPIC= FGC= BGC= FONT= FORMAT=

Evaluates the topic message as a number and then uses FORMAT to display them. 
The Formatter should
either be a string also used in BASIC PRINT USING statements (also EXCEL) or 
a C style printf() format string. When new
data arrives, the old text need to be cleared, therefor a width (and Height) of 
the box to be cleared with background color (BGC) need to be specified. 
If H is ommitted, the font height is used. If W is ommitted, the size of the
FORMAT string is used.  

#### FrameLabel:  X= Y= W= H= TOPIC= MATCH= 

Draws a Frame, and if the topics content matches MATCH, it is drawn in 
reversed state, otherwise in normal state. 

#### Meter: X= Y= W= H= TOPIC= MIN= MAX= AMIN= AMAX=

Draws a round meter using the topics content interpreted as a value between min and max. 

#### HMeter: X= Y= W= H= TOPIC= MIN= MAX=

Draws a horizontal meter using the topics content interpreted as a value between min and max. 

#### VMeter: X= Y= W= H= TOPIC= MIN= MAX=

Draws a vertical  meter using the topics content interpreted as a value between min and max. 

#### VBar: X= Y= W= H= TOPIC= MIN= MAX= FGC= BGC= AGC=

Draws a vertial bar, which is filled by the percentage of the topics content 
interpreted as a value between min and max. 

#### HBar: X= Y= W= H= TOPIC= MIN= MAX= FGC= BGC= AGC=

Draws a horizontal bar, which is filled by the percentage of the topics content interpreted as a value
between min and max. 


#### TextLabel: X= Y= W= H= TOPIC= BGC= FONT= FONTSIZE= TEXT[n]="match|text|color"

According to the content of the topic, 
corresponding text is displayed. If one of the match strings given in 
TEXT[n] matches the content, then the corresponding text is displayed in the 
corresponding color.  
If the topics content doesnt match at all, nothing is displayed. 
n can be 0 to 9.

#### BitmapLabel: X= Y= TOPIC= BGC= BITMAP[n]="match|filename|color"

According to the content of the topic, 
a corresponding bitmap is displayed. If one of the match strings given in 
BITMAP[n] matches the content, then the corresponding bitmap is displayed in the 
corresponding color.  
If the topics content doesnt match at all, nothing is displayed. 
n can be 0 to 9.
This way status indicators can be implemented. The whole set of bitmaps all 
use the same background color.

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

#### Ticker: X= Y= W= H= TOPIC= TIC= MIN= MAX= [FORMAT=] [QOS=] 

If the user clicks in the area, the content of the TOPIC will be interpreted
as a number, incremented by TIC, formated and finally applied to the TOPIC 
with the specified Quality of Service (QOS). TIC can be negative. The range of 
the values will be checked against MIN and MAX and the number will be clipped
if necessary.


### Elements for Application control

#### FrameToggle: X= Y= W= H=

This element draws a frame in normal state. When clicked with the mouse it 
changes to reverse state until the mouse button is released. This element is
used to make buttons which the user can click on. It gives visual feedback on 
the click. Thats all. Usually this element will be combined with TopicInArea 
or one of the following elements: 

#### ShellCmd: X= Y W= H= CMD=

An invisible click area. When the user clicks it, a shell command will be 
excecuted.

#### Dash: X= Y= W= H= DASH=

An invisible click area. When the user clicks it, 
another window with another dashboard opens. The file name must 
be specified with DASH. The files are searched for in the dashboardpath.


More element types can be thought of, but our goal is to keep everything as simple
as possible.

