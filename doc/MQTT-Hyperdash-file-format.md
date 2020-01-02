## The .dash file format

--- for MQTT Hyperdash ---


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

Currently there exist following element types:


|Element       |   Recognized keyvalues                      |
|--------------|:-------------------------------------------:|
|PANEL         | W H TITLE FGC BGC                           |
|BROKER        | URL USER PASSWD                             |
|BOX           | X Y W H FGC                                 |
|PBOX          | X Y W H FGC BGC                             |
|CIRCLE        | X Y W H FGC                                 |
|PCIRCLE       | X Y W H FGC BGC                             |
|LINE          | X Y X2 Y2 FGC                               |
|FRAME         | X Y W H REVERT                              |
|FRAMETOGGLE   | X Y W H                                     |
|BITMAP        | X Y BITMAP FGC                              | 
|ICON          | X Y ICON                                    |
|TEXT          | X Y H TEXT FGC FONT FONTSIZE                |
|TOPICSTRING   | X Y H TOPIC FGC BGC FONT FONTSIZE           |
|TOPICNUMBER   | X Y H TOPIC FGC BGC FONT FONTSIZE FORMAT    |
|HBAR          | X Y W H TOPIC FGC BGC AGC MIN MAX           |
|VBAR          | X Y W H TOPIC FGC BGC AGC MIN MAX           |
|TOPICMETER    | X Y W H TOPIC FGC BGC AGC MIN MAX AMIN AMAX |
|TOPICVMETER   | X Y W H TOPIC FGC BGC AGC MIN MAX           |
|TOPICHMETER   | X Y W H TOPIC FGC BGC AGC MIN MAX           |
|TEXTLABEL     | X Y H TOPIC BGC FONT FONTSIZE TEXT[n]       |
|BITMAPLABEL   | X Y TOPIC BGC BITMAP[n]                     |
|FRAMELABEL    | X Y W H TOPIC MATCH                         |
|SHELLCMD      | X Y W H CMD                                 |
|DASH          | X Y W H DASH                                |
|TOPICINAREA   | X Y W H TOPIC VALUE QOS                     |
|TOPICINSTRING | X Y W H TOPIC QOS                           |
|TOPICINNUMBER | X Y W H TOPIC FORMAT MIN MAX QOS            |
|TOPICHSCALER  | X Y W H TOPIC FORMAT MIN MAX QOS            |
|TOPICVSCALER  | X Y W H TOPIC FORMAT MIN MAX QOS            |
|TICKER        | X Y W H TOPIC TIC FORMAT MIN MAX QOS        |


0. Common keyvalue formats
==========================
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


1. Static elements 
==================
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

2. Dynamic output Elements
==========================

Dynamic elements are controlled by the content of a topic. They change appearance when the topic content
changes. But they cannot take user input. They are used to display data. 

TopicString: X=<x> Y=<y> TOPIC=<topic> FGC=<colorspec> BGC=<colorspec> FONT=<fontspec>

Displays the topic messages as they are. 

TopicNumber: X=<x> Y=<y> TOPIC=<topic> FGC=<colorspec> BGC=<colorspec> FONT=<fontspec> USING=<formatter>

Evaluates the topic message as a number and then uses formatter to display them. The Formatter should
be a string also used in BASIC PRINT USING statements. 

TopicFrame:  X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> MATCH=<content> 

Draws a Frame, and if the topics content matches MATCH-content, it is drawn in reversed state. 

TopicMeter: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> MIN=<value> MAX=<value>

Draws a meter using the topics content interpreted as a value between min and max. 

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

3. Dynamic input Elements
=========================

They are like the dynamic output elements with the extra feature that they allow user input.
You can enter numbers, change states etc... 

TopicInString: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> 

Clicking in this area,  
the user can enter a string which is then published to the topic using QOS2.

TopicInNumber: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> MIN=<value> MAX=<value> USING=<formatter>

Clicking in this area,  
the user can enter a number which is then published to the topic using QOS2.

TopicInNumberVScaler: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic>  MIN=<value> MAX=<value> USING=<formatter> EPS=<step>

Clicking in this area,  
the user can slide a vertical scaler to produce a number between min and max 
which is then published to the topic using QOS2.
The eps value can specify the smalles change which can be applies by Up and Down tickers. 

TopicInNumberHScaler: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic>  MIN=<value> MAX=<value> USING=<formatter> EPS=<step>

Clicking in this area,  
the user can slide a horizontal scaler to produce a number between min and max 
which is then published to the topic using QOS2.
The eps value can specify the smalles change which can be applies by Up and Down tickers. 



TopicInArea: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> HIT=<content>

If the user clicks in the area, the content <content> will be published to the topic using QOS2.

TopicInFrame: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> HIT=<content>

If the user clicks in the area, the content <content> will be published to the topic using QOS2.
Also a FrameToggle will apply. If the content matches HIT, the Frame stays reversed.



4. Elements for Application control
===================================

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

