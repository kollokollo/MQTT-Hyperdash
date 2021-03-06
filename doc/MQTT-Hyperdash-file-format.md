## The .dash file format

for MQTT Hyperdash V.1.02 (c) by Markus Hoffmann


MQTT Hyperdash runs so-called dash-files. Dash-files are simple text files, 
consisting of UTF-8 encoded ASCII text. They can be edited by any text editor, or
by the special application "dashdesign" which comes with the MQTT-Hyperdash package.

The format is as follows:

Each line contains the definition of one element. If the last character of the
line is a '\' (backslash), the next line will be linked to this line. You can
use  this to better format the dash files, e.g. to split the definition of one 
element over multiple real lines. 

A line with an element definition has following structure: 

element-name : one or more KEY=VALUE pairs separated by whitespace.

All keywords are case insensitive. 

The order of KEY=VALUE pairs is irrelevant. They are all optional. 
If missing, default values apply.

String values should be enclosed by "" if they contain white-spaces and when
case matters.

Comments are marked with a '#' at the beginning of a line.

Empty lines will be ignored. 

Each dash file must contain exactly one "PANEL" element and 
one "BROKER" element. The PANEL element is expected to be defined 
at the beginning of the .dash file. Elements are drawn in the order
they appear in the file. If you want to place element B on top of 
element A, element A should be defined first. 


### Element types

Currently there exist following element types:


|Element       |   Recognized key-values                                    |
|--------------|:----------------------------------------------------------|
|PANEL         | W H TITLE FGC BGC                                         |
|BROKER        | URL USER PASSWD                                           |
|BOX           | X Y W H FGC                                               |
|PBOX          | X Y W H FGC BGC                                           |
|CIRCLE        | X Y W H FGC                                               |
|PCIRCLE       | X Y W H FGC BGC                                           |
|LINE          | X Y X2 Y2 FGC                                             |
|FRAME         | X Y W H REVERT                                            |
|FRAMETOGGLE   | X Y W H                                                   |
|BITMAP        | X Y BITMAP FGC                                            | 
|ICON          | X Y ICON                                                  |
|TEXT          | X Y H TEXT FGC FONT FONTSIZE                              |
|TOPICSTRING   | X Y W H TOPIC FGC BGC FONT FONTSIZE                       |
|TOPICNUMBER   | X Y H TOPIC FGC BGC FONT FONTSIZE FORMAT                  |
|HBAR          | X Y W H TOPIC FGC BGC AGC MIN MAX                         |
|VBAR          | X Y W H TOPIC FGC BGC AGC MIN MAX                         |
|METER         | X Y W H TOPIC FGC BGC AGC MIN MAX AMIN AMAX TYPE          |
|VMETER        | X Y W H TOPIC FGC BGC AGC MIN MAX		           |
|HMETER        | X Y W H TOPIC FGC BGC AGC MIN MAX		           |
|TEXTLABEL     | X Y H TOPIC BGC FONT FONTSIZE TEXT[n]                     |
|BITMAPLABEL   | X Y TOPIC BGC BITMAP[n]                                   |
|FRAMELABEL    | X Y W H TOPIC MATCH                                       |
|SCMDLABEL     | TOPIC CMD[n]                                              |
|SHELLCMD      | X Y W H CMD                                               |
|DASH          | X Y W H DASH                                              |
|TOPICINAREA   | X Y W H TOPIC VALUE QOS                                   |
|TOPICINSTRING | X Y W H TOPIC QOS                                         |
|TOPICINNUMBER | X Y W H TOPIC FORMAT MIN MAX QOS                          |
|HSCALER       | X Y W H TOPIC FORMAT MIN MAX TIC QOS BGC FGC AGC          |
|VSCALER       | X Y W H TOPIC FORMAT MIN MAX TIC QOS BGC FGC AGC          |
|TICKER        | X Y W H TOPIC FORMAT MIN MAX TIC QOS                      |
|PLOT          | X Y W H TOPIC TYPE N OFFSET MIN MAX AMIN AMAX BGC FGC AGC |
|TEXTAREA      | X Y W H TOPIC ALIGN FGC BGC FONT FONTSIZE                 |
|TOPICIMAGE    | X Y W H TOPIC                                             |
|COMPOUND      | X Y W H                                                   |


### Common keyvalue formats

<pre>
* X=<pixels> Horizontal position of upper right corner of the element in pixels
* Y=<pixels> Vertical position of upper right corner of the element in pixels
* X2=<pixels> Horizontal position of the endpoint of a line in pixel coordinates
* Y2=<pixels> Vertical position  of the endpoint of a line in pixel coordinates
* W=<pixels> Width of the element in pixels
* H=<pixels> Height of the element in pixels
* TITLE=<String> Title of the dashboard window.
* URL=<String> URL of the broker to connect to. Example: URL="tcp://localhost:1883"
* USER=<String> User-name for connection
* PASSWD=<String> Password for connection
* FGC=<color value> Foreground color. The color values are 32bit RGBA. 
  They can be specified using a hey value $rrggbbaa:
  e.g. FGC=$ff4532FF
* BGC=<color value> Background color. 
* BITMAP=<filename> Specify a bitmap file name relative to the bitmap path. The
  bitmap files need to be in .xbm (X-Window bitmap) format and can be generated 
  and edited with the simple application "bitmap". 
* ICON=<filename> Specify an image file name relative to the icon path. These 
  files must be .png (Potable network graphics) files.
* TEXT=<String> Specify a text to be shown.
* FONT=<fontname> Specify a text font to use. These fonts must be True-type Fonts
  usually installed in the standard path /usr/share/fonts/truetype/msttcorefonts
  There a file with .ttf ending is searched. 
* FONTSIZE=<value> Specify the size of the font. This defaults to 16.
* TOPIC=<TompicName> The name of the topic of a dynamic element. 
* REVERT=<0 or 1> Specifies if the frame is drawn in revert state.
* MATCH=<String> A String to match the topics content.
* TEXT[n] Are one or multiple Label definitions of the form 
  TEXT[n]="<match>|<text>|<fgc>". E.g. TEXT[0]="0|Hello|$ffffffff"
* BITMAP[n] Are one or multiple Label definitions of the form 
  BITMAP[n]="<match>|<bitmapfile>|<fgc>". E.g. BITMAP[0]="0|SmallTriangle|$ff0000ff"
* VALUE=<String> A String to apply to a topic.
* QOS=<0,1 or 2> Quality of service for publication to the broker. Defaults to 0.
* TIC=<number> An increment used by the ticker. The increment can aswell be negative.
* TYPE=<number> The type of the plot.
* OFFSET=<number> ignore the first n numbers of the topic content.
* N=<number> use only n numbers of the topic content.
</pre>

### Static elements 

Static elements are just drawing primitives. They do noch change with topics 
contents.

#### Panel: TITLE= W= H= [FGC= BGC=]

This sets the title of the window/screen and specifies its fixed size in pixels. 
Also a default foreground (FGC) and background color (BGC) can be specified. 
Each dash file must contain exactly one panel element. 

Example: 
<pre>
PANEL:  TITLE="Button Test Dashboard" W=512 H=400 FGC=$ffffffff BGC=$000040ff
</pre>

#### Broker: URL= [USER= PASSWD=]

This sets the mqtt broker url, and also user-name and password if required. 
Each dash file must contain exactly one broker element. 

Example: 
<pre>
BROKER: URL="tcp://localhost:1883"
</pre>

#### Line: X= Y= X2= Y2= FGC= LINEWIDTH=

Draws a simple line of given width in given color from (x,y) to (x2,y2). 

Examples:
<pre>
LINE:   X=0 Y=0 X2=32 Y2=32 FGC=$ffffffff
LINE:   X=32 Y=0 X2=0 Y2=32 FGC=$ff0000ff
</pre>

#### Text: X= Y= [H=] FGC= TEXT= FONT= FONTSIZE=

Draws a text at coordinates (X,Y) in given color with a given font. Fonts are
true type fonts (.ttf). The size (height) of the characters can be specified 
with FONTSIZE. H defines the text-box height. The text will be vertically 
centered to this height. If H is not specified this defaults to the Font height 
of the specified font. If the specified font cannot be found or loaded the 
font defaults to an ugly 8x8 pixel mono-spaced font. If not specified, the 
FONTSIZE defaults to 16. The text is transparent and has no background. This 
way, you can stuff multiple characters or texts on top of a bitmap or other
graphical elements. 

Example:
<pre>
TEXT: X=20 Y=10 H=60 FGC=$FFFFFFFF TEXT="Test Dashboard" FONT="Arial" FONTSIZE=36
</pre>

#### Box: X= Y=  W= H= FGC= [LINEWIDTH=]

Draws a simple box with the given foreground color. 

Example:
<pre>
BOX: X=20 Y=80 W=200 H=120 FGC=$FF00FFFF
</pre>

#### Circle: X= Y=  W= H= FGC= [LINEWIDTH=]

Draws a simple ellipse fitting in the box X,Y,W,H with the given 
foreground color. 

Example:
<pre>
CIRCLE: X=20 Y=80 W=200 H=120 FGC=$FFFF00FF
</pre>

#### PBox:  X= Y=  W= H= FGC= BGC= [LINEWIDTH=]

Draws a filled box. Fill color is BGC, border color is FGC. 

Example:
<pre>
PBOX: X=40 Y=100 W=50 H=30 BGC=$444444FF
</pre>

#### PCircle:  X= Y=  W= H= FGC=  BGC= [LINEWIDTH=]

Draws a filled ellipse fitting in the box X,Y,W,H with the given 
foreground color. Fill color is BGC. 

Example:
<pre>
PCIRCLE: X=20 Y=80 W=200 H=120 FGC=$FFFF00FF BGC=$222222FF
</pre>

#### Frame: X= Y= W= H= [REVERT=]

Draws a shadowed frame. Revert can be 0 or 1. If REVERT=1, the frame is 
drawn in reverted state. This element can be used do draw buttons or other
stylistic 3D effects around other elements. 

Example:
<pre>
FRAME:   X=90 Y=290 W=220 H=36 
</pre>

#### Icon: X= Y= ICON=

Draws an image, usually a png-file, at position x and y. The file name must be
specified with ICON. The file is searched for in the icon-path. 

Example:
<pre>
ICON: X=320 y=5 ICON="My-logo.png"
</pre>

#### Bitmap: X= Y= BITMAP= FGC=

Draws a monochrome bitmap image using the FGC color specified. 
Bitmaps are drawn with transparent
background, so one can 
draw multiple bitmaps on top of another to combine more complicated graphics.
The file name must be
specified with BITMAP. The file is searched for in the bitmap path.

<img src="images/BITMAP.png">

Example:
<pre>
BITMAP:      X=450 y=300 BITMAP="Bulb" FGC=$ffffffff
</pre>

### Dynamic output Elements

Dynamic elements are controlled by the content of a topic. They change their
appearance when the topic content changes. But they cannot take user input. They
are used to display data. 

#### TopicString: X= Y= W= [H=] TOPIC= FGC= BGC= FONT= FONTSIZE=

Displays the topic messages as they arrive using the specified FONT. When new
data arrives, the old text need to be cleared, therefore a width (and height)
of the box to be cleared with background color (BGC) needs to be specified. If
H is omitted, the font height is used. 

Example:
<pre>
TOPICSTRING: X=10 Y=190 W=140 H=16 TOPIC=HOME/LOAD_SM FGC=$FFFF00FF BGC=$40FF \
             FONT="Arial_Bold" FONTSIZE=16
</pre>

#### TopicNumber: X= Y= [W=] [H=] TOPIC= FGC= BGC= FONT= FORMAT=

Evaluates the topic message as a number and then uses FORMAT to display it. The
formatter should either be a string also used in BASIC "PRINT USING" statements
(also EXCEL) or a C style "printf() format" string. When new data arrives, the
old text need to be cleared, therefore a width (and height) of the box to be
cleared with background color (BGC) needs to be specified. If H is omitted, the
font height is used. If W is omitted, the size of the FORMAT string is used.

Example:
<pre>
TOPICNUMBER: X=400 Y=282 W=80 H=10 TOPIC=HOME/SOLAR/CELL3 FORMAT="##.### V" \
             FGC=$FFFF00FF BGC=$7722FFFF
</pre>

#### FrameLabel:  X= Y= W= H= TOPIC= MATCH= 

Draws a Frame, and if the topics content matches MATCH, it is drawn in 
reversed state, otherwise in normal state. Usually this element is used to 
simulate a (radio) button.

 <img src="images/radiobuttons.png">

Example:
<pre>
FRAMELABEL:  x=120 y=120 w=50 h=25 TOPIC=HOME/SOLAR/LADE/CMD_DS MATCH="OFF"
</pre>

#### Meter: X= Y= W= H= TOPIC= MIN= MAX= AMIN= AMAX=

Draws a round meter using the topics content interpreted as a value between min
and max. The meter is placed inside a box of the size x,y,w,h. The starting 
angle and the ending angle of the hand is defined with AMIN and AMAX.

<img src="images/METER.png">

Example:
<pre>
METER: TOPIC="SYSMEASURE/SYSDISKUSAGE_AM" \
       X=120 Y=100 W=100 h=100 FGC=$FF0000FF AGC=$FFFFFFFF BGC=$40FF \
       MIN=0 MAX=100 AMIN=225 AMAX=-45

</pre>


#### HMeter: X= Y= W= H= TOPIC= MIN= MAX=

Draws a horizontal meter using the topics content interpreted as a value between
min and max. 

Example:
<pre>
HMETER: x=38 y=100 w=140 h=25 MIN=-1 MAX=2  TOPIC=SINUS \
        AGC=$444444FF BGC=$000000FF FGC=$FFFFFFFF
</pre>

#### VMeter: X= Y= W= H= TOPIC= MIN= MAX=

Draws a vertical  meter using the topics content interpreted as a value between
min and max. 

<img src="images/VMETER.png">

Example:
<pre>
VMETER:  X=250 Y=100 W=25 H=140 MIN=0 MAX=4 TOPIC=HELIUMPRESSURE \
         AGC=$444444FF BGC=$000000FF FGC=$FFFFFFFF 
</pre>

#### VBar: X= Y= W= H= TOPIC= MIN= MAX= FGC= BGC= AGC=

Draws a vertical bar, which is filled by the percentage of the topics content 
interpreted as a value between min and max.

 <img src="images/VBAR.png">

Example:
<pre>
VBAR: x=490 y=300 w=10 h=50 MIN=0 MAX=2 \
      AGC=$ffffffff BGC=$000000ff FGC=$ff0000ff \
      TOPIC=HOME/SOLAR/LIION_OUT_CURRENT
</pre>

#### HBar: X= Y= W= H= TOPIC= MIN= MAX= FGC= BGC= AGC=

Draws a horizontal bar, which is filled by the percentage of the topics content interpreted as a value
between min and max. 

Example:
<pre>
HBAR: x=200 y=100 w=100 h=10 MIN=0 MAX=8 \
      AGC=$ffffffff BGC=$000000ff FGC=$ff0000ff TOPIC=DOSIMETER/CPM_AM
</pre>

#### TextLabel: X= Y= W= H= TOPIC= BGC= FONT= FONTSIZE= TEXT[n]="match|text|color"

According to the content of the topic, corresponding text is displayed. If one
of the match strings given in TEXT[n] matches the content, then the
corresponding text is displayed in the corresponding color. If the topics
content doesn't match at all, nothing is displayed. n can be 0 to 9.

Example:
<pre>
TEXTLABEL:   X=600 Y=350 W=20 H=20 TOPIC=KLYSTRON/HF_DM  \
             BGC=$44ff FGC=$ffff00ff FONT="Courier_New_Bold"  FONTSIZE=12 \
	     TEXT[0]="0|HF|$000000ff" \
	     TEXT[1]="1|HF|$00ff00ff"
</pre>


#### BitmapLabel: X= Y= TOPIC= BGC= BITMAP[n]="match|filename|color"

According to the content of the topic, a corresponding bitmap is displayed. If
one of the match strings given in BITMAP[n] matches the content, then the
corresponding bitmap is displayed in the  corresponding color. If the topics
content doesn't match at all, nothing is displayed. n can be 0 to 9. 

This way status indicators can be implemented. The whole set of bitmaps all use
the same background color.

Examples:
<pre>
BITMAPLABEL: X=600 Y=370 TOPIC=DOSIMETER/TREND_DM BGC=$44ff \
             BITMAP[0]="0|TrendNone|$ffffffff" \
	     BITMAP[1]="1|SmallTriagUp|$ffffffff" \
	     BITMAP[2]="2|SmallTriagDwn|$ffffffff" 

BITMAPLABEL: x=345 y=75 BGC=$000040ff TOPIC=ELECTRICITY/ACTIVITY \
  BITMAP[0]="0|Disc1|$ffffffff" \
  BITMAP[1]="1|Disc2|$ffffffff" \
  BITMAP[2]="2|Disc3|$ffffffff" \
  BITMAP[3]="3|Disc4|$ffffffff"

BITMAPLABEL: x=320 y=300 BGC=$000040ff TOPIC=SOLAR/LIION_FILLBAT \
  BITMAP[0]="0|Battery0|$ffaaaaff" \
  BITMAP[1]="1|Battery1|$aaaaaaff" \
  BITMAP[2]="2|Battery2|$aaaaaaff" \
  BITMAP[3]="3|Battery3|$aaaaaaff" \
  BITMAP[4]="4|Battery4|$aaffaaff"

BITMAPLABEL: x=83 y=321 BGC=$000040ff TOPIC=SOLAR/LADE/STATUS \
  BITMAP[0]="-1|SmallCircle|$ff00ff" \
  BITMAP[1]="0|SmallCircle|$222222ff" 

</pre>



#### SCmdLabel: TOPIC= CMD[n]="match|shell command"

According to the content of the topic, a corresponding shell-command is 
executed. 
If one of the match strings given in CMD[n] matches the content, then the 
corresponding shell-command is executed. 
If the topics content doesn't match at all, nothing happens. 
n can be 0 to 9.
This is a very versatile but non portable element. You can use it for various 
things, but currently I can only think of the automatic trigger of sounds by
calling ogg123 or mplayer. 

Example: 

<pre>
SCMDLABEL:  TOPIC="SOUND_DC" \
            CMD[0]="1|mplayer /usr/share/sounds/window-slide.ogg &"
</pre>


#### Plot: X=<x> Y=<y> W=<w> H=<h> TOPIC=<topic> 

Displays a plot or chart of the numbers in the topic. 
The topic content is expected to be a comma separated or space separated list of
number values. If OFFSET is specified, the first offset numbers will be
ignored. If N is specified, only at maximum n values are used. These are then
plotted into a graph. 

With TYPE you specify the type of graph produced. e.g.
Type=0 only plots dots, 
TYPE=1 impulses,
TYPE=2 histogram lines, 
TYPE=3 normal lines, and so on.

The horizontal scaling can be influenced using AMIN and AMAX,
the vertical scaling is controlled by MIN and MAX. You can specify a background
color (BGC), a  foreground color (FGC) and a color for frame and axis (AGC). 
 
 <img src="images/PLOT.png">

Example: 

<pre>
PLOT: x=20 Y=100 w=300 h=200 TOPIC=DOSIMETER/PULSELENHIST_SM BGC=$000000ff \
      OFFSET=8 MIN=0 MAX=100 TYPE=2

</pre>

##### TopicImage: X= Y= [W= H=] TOPIC=

Draws an image at x,y of size w,h. The Image data is taken from the TOPICs 
content. The data must be in .png file format to be recognized. If W or H is
omitted, the image width and height is used. Currently the image doesn't clip to
the elements bounds, nor does it scale. You have to make sure, that the element
is big enough to contain the image.

Example: 

<pre>
TOPICIMAGE: x=5 y=70 W=100 H=100 TOPIC="IMAGE"
</pre>

##### TextArea:  X= Y= W= H= TOPIC= FGC= BGC= FONT= FONTSIZE= [ALIGN=]

Draws the topics content into a text area of size w,h. If the text is longer
than W it is automatically wrapped to the next line. TEXT can be UTF8 coded
ASCII text.  Newline characters are recognized to terminate a line.

ALIGN specifies which part of the text is shown in case the Elements box is too
small. ALIGN can be  TOP, BOTTOM, CENTER. Default is TOP.

Example: 

<pre>
TEXTAREA: x=5 y=180 W=250 H=100 TOPIC="LONGTEXT" FGC=$FFFFFFFF BGC=$000000FF \
          FONT="Courier_New_Bold" FONTSIZE=16
</pre>


## Dynamic input Elements

They are like the dynamic output elements with the extra feature that they allow
user input. You can enter numbers, change states etc... 

#### TopicInString: X= Y= W= H= TOPIC= [QOS=]

Clicking in this area, the user can enter a string which is then published to
the topic using QOS2 by default.

<img src="images/TOPICINSTRING.png">

Example: 

<pre>
TOPICINSTRING: TOPIC="TITLE_SC" x=110 y=152 w=100 h=30
</pre>

#### TopicInNumber: X= Y= W= H= TOPIC= MIN= MAX= FORMAT= [TIC=] [QOS=]

Clicking in this area, the user can enter a number which is then published to
the topic using QOS2 by default. The range of the entered number is checked
against MIN and MAX and clipped if necessary. The topic contend is finally
formatted using the FORMAT pattern.

Example: 

<pre>
TOPICINNUMBER:  X=10 Y=252 W=100 H=30 TOPIC=SETPOINT_AC FORMAT="%g" MIN=0 MAX=4
</pre>


#### VScaler: X= Y= W= H= TOPIC= MIN= MAX= TIC= FORMAT= [QOS=] [FGC BGC AGC]

This element draws a vertical scaler (a simulated potentiometer). 
A foreground color (FGC) a 
background color (BGC) as well as a frame color (AGC) can be specified. 
The user can slide the scaler to change the number of the topics content. 
The user can also click into the scalers background, and a dialog to enter
the number will be offered. 
The range of the entered number is checked against MIN and MAX and clipped
if necessary. The topic content is finally formatted using the FORMAT pattern.
During sliding, the values are published to the TOPIC at each move of the mouse
if the results produced are bigger than TIC. During sliding the values are 
always published with QOS=0. Only the final value (after releasing the sliders
knob) will be published with the Quality of Service specified. 

Example: 

<pre>
VSCALER: X=300 Y=30 W=20 H=200 TOPIC=SETPOINT_AC MIN=0 MAX=4  TIC=0.05
</pre>

#### HScaler:  X= Y= W= H= TOPIC= MIN= MAX= TIC= FORMAT= [QOS=] [FGC BGC AGC]

This element draws a horizontal scaler. 
A foreground color (FGC) a 
background color (BGC) as well as a frame color (AGC) can be specified. 
The user can slide the scaler to change the number of the topics content. 
The user can also click into the scalers background, and a dialog to enter
the number will be offered. 
The range of the entered number is checked against MIN and MAX and clipped
if necessary. The topic content is finally formatted using the FORMAT pattern.
During sliding, the values are published to the TOPIC at each move of the mouse
if the results produced are bigger than TIC. During sliding the values are 
always published with QOS=0. Only the final value (after releasing the sliders
knob) will be published with the Quality of Service specified. 

 <img src="images/HSCALER.png">

Example: 

<pre>
HSCALER: X=100 Y=300 W=200 H=16 TOPIC=SETPOINT_AC MIN=0 MAX=4 TIC=0.05 AGC=$0
</pre>

#### TopicInArea: X= Y= W= H= TOPIC= VALUE= [QOS=] 

If the user clicks in the area, the content VALUE will be published to the 
TOPIC with the specified Quality of Service (QOS).
This element is used together with FRAMETOGGLE or FRAMELABEL to make a
button.

Example: 

<pre>
TOPICINAREA: x=180 y=120 w=50 h=25 TOPIC=SOLAR/LADE/CMD_DS VALUE="ON"
</pre>


#### Ticker: X= Y= W= H= TOPIC= TIC= MIN= MAX= [FORMAT=] [QOS=] 

If the user clicks in the area, the content of the TOPIC will be interpreted
as a number, incremented by TIC, formated and finally applied to the TOPIC 
with the specified Quality of Service (QOS). TIC can be negative. The range of 
the values will be checked against MIN and MAX and the number will be clipped
if necessary.

Example: 

<pre>
TICKER:      X=48 Y=208 W=34 H=34 TOPIC=TESTT TIC=0.2 MIN=0 MAX=4
TICKER:      X=48 Y=288 W=34 H=34 TOPIC=TESTT TIC=-0.2 MIN=0 MAX=4
</pre>


### Elements for Application control

These elements will not affect any topic. They have effects on the 
local machine, which the dashboard is displayed on.

#### FrameToggle: X= Y= W= H=

This element draws a frame in normal state. When clicked with the mouse it 
changes to reverse state until the mouse button is released. This element is
used to make buttons which the user can click on. It gives visual feedback on 
the click. Thats all. Usually this element will be combined with TopicInArea 
or one of the following elements for application control.

Example: 

<pre>
FRAMETOGGLE: x=240 y=400 w=100 h=25
</pre>

#### ShellCmd: X= Y W= H= CMD=

An invisible click area. When the user clicks it, a shell command will be 
executed.

Example: 

<pre>
SHELLCMD: x=20 y=260 w=240 h=25 \
          CMD="firefox https://github.com/kollokollo/MQTT-Hyperdash &"
</pre>

#### Dash: X= Y= W= H= DASH=

An invisible click area. When the user clicks it, 
another window with another dashboard opens. The file name must 
be specified with DASH (without the postfix ".dash"). 
The files are searched for in the dashboard-path.

Example: 

<pre>
DASH:        x=110 y=360 w=100 h=25 DASH="main"
</pre>

#### COMPOUND: X= Y= W= H= 

This special element has the function to mark a group of other elements, 
so that they can be moved or copied together. The compound element is 
only visible in the dashdesign application. 

Elements belonging to the group need to be fully inside the area defined
by the compound element, and they need to be defined before. Elements
defined after can be made part of the group by moving them to the 
background. 

#### Other elements

More element types can be thought of, but our goal is to keep everything as
simple as possible. At the moment we are thinking of maybe having 
