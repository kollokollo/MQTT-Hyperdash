## MQTT Payload Conventions

for MQTT Hyperdash V.1.03 (c) by Markus Hoffmann

MQTT-Hyperdash tries to interpret the MQTT topics payload in such a way, that 
it can be presented to the user in a meaningful way. Either as a number or as a 
string or even as image data. Thus, the payload need to follow a few simple
conventions to be interpreted correctly. 


Currently there are following data types recognized: 

1. A string
2. A number (integer or rational or floating point, decimal or as a hexadecimal number)
3. Image data (in binary, must be the file content of a .png image file).
4. Simple JSON expansion.

### Strings

* The MQTT message payload is sent as a UTF-8 encoded string.
* String types are limited to 268435456 characters in length.
* An empty string ("") is a valid payload, however if the topic gets such
  an empty message, its retained value will be deletes and such mqtt-list-topics
  cannot find it anymore. 

### Numbers

* Any leading spaces of the payload are ignored. Spaces are also 
  space, form-feed ('\f'), newline ('\n'),  carriage  return
  ('\r'), horizontal tab ('\t'), and vertical tab ('\v').

* Numbers are UTF-8 encoded string literal representations of 64-bit signed 
  floating point numbers. Trailing zeros are allowed. -inf, inf and nan should 
  not be used. 

* Integer numbers ranging from -999999 up to 999999 will be represented by 
  normal integer notation, other values may default to scientific notation and
  will be treated like floating-point values.
  
* An empty message defaults to the number "0" (zero).

* Numbers starting with "$" or "0x" are evaluated as hexadecimal numbers.

* A topic payload with just a negative sign ("-") is not a valid payload. 
  (but it would evaluate to "0" (zero)).
  
* A boolean FALSE may be represented by an integer number 0 (zero), a 1 or -1 is
  interpreted to boolean TRUE.
  
* Any part of the topics payload following the number and being separated by 
  one or more spaces is ignored by default. It is possible to pass any 
  characters, for example indicating the Unit of the value, after the number. 
  But MQTT-Hyperdash will ignore it. 
  
### Units and Timestamps

Timestamps may be added to the topic payload as a string starting with "T"
immediately followed by a UNIX timestamps. They must be separated from the 
number value with at least one space:

"12.45 T1580480108.975"

It is expected, that the timestamp is the last word (or the most right part of)
the topics payload. This way also Units could be inserted. E.g:

"12.45 V T1580480108.975"

Here the first part is the actual properties value and the part starting with a
"T" is a timestamp. The "V" in the middle can be interpreted as a unit, but in
general it will be ignored. This extension would be compatible if clients which
do not evaluate timestamps, just ignore any part after the first blank.

### Arrays of numbers


An array of numbers is a topic payload with many numbers, separated by a blank/space. 
This forms an array a[n] of n integers or floats, all would have the same unit, 
and the same type.

In case of a history of e.g. temperatures the most recent value should be on the
left side. So any client not capable of processing arrays will at least see the
most recent value and ignore any other part after the first blank.

A timestamp may be in addition added to the right side. Any other part of the
payload which cannot be interpreted as a number will be ignored.

Of course, this way more complicated data structures cannot be realized (e.g.
(x,y) coordinates or (x,time) values. If you need this, One would use JSON. But
this cannot be interpreted by MQTT-Hyperdash.  It is a conceptual choice not to
support JSON, but keep everything really simple. 

But thinking in terms of the ancient programming languages (BASIC, FORTRAN) one
can make separate simple arrays for the X[n] and Y[n] and TIME[n] data. One
needs to make sure, that "n" is the same in all cases and maybe one wants to add
a Timestamp (T1580396855.826) as the last value (see another proposal), to make
sure the data corresponds to each other. This is not very elegant, but avoids a
complex parser. (not all Microcontrolles have space for this). And in total it
is not a limitation at all.

### JSON expansion

Simple key-value JSON container are supported. The JSON-keys will be extended 
to the topic name as if they where another level of the topic hirarchy.

Assume the main (real MQTT) topic is named "TEMPEATURE/VALUE", and the 
payload is typically: 
<pre>
{
  "reading" : 10.3, 
  "unit" : "°C", 
  "timestamp": 1580644118.778
}
</pre>

Then the main topic will be expanded into four new topics:

<pre>
TEMPEATURE/VALUE
TEMPEATURE/VALUE{reading}
TEMPEATURE/VALUE{unit}
TEMPEATURE/VALUE{timestamp}
</pre>

Where each of the additional three pseudo-subtopics contain a single value, which
can be used as if it was a real topic. Al though not yet implemented, the logic
would also work for complicated nested JSON data structures. e.g. leading to
TEMPEATURE/VALUE{sensor1/comment/english}. 

This way, most of the common smart home sensors emmiting or expecting a JSON
format on their topics can be read and processed by hyperdash and the rule
engine framework.

Also mqtt-list-topics does recognize the JSON subtopics and adds them
to its list of topics. 

Notes:

* JSON expansion currently only works on the first level. So nested objects
  will not further expand.

* If published to individual subtopics, always the whole real topic containing
  the JSON structure is published. This means that all the other values get 
  republished, even though they have not changed. 

* Implementation of subtopics in the rule engine framework is not yet done.
  
### Further reading

The is a community based convention called the "The Homie Convention" which is 
similar and also avoids the usage of JSON, thus having everything kept simple. 
This sounds quite interesting and we might think of extending MQTT-Hyperdash in
that direction. Especially the distinction between measured and set-point values, 
(a sort of topic naming convention) together with the storage of meta-information
about the topics sounds interesting. Meta-Information can be

<pre>
* NAME       -- the name of the topic
* ID         -- an unique ID (maybe obsolete)
* HOSTNAME   -- the name of the device which last produced a message/value
* FLAGS      -- tbd (maybe obsolete)
* TYPE       -- the type of the topic (integer/number/image data/WEB-URL)
* DIMENSION  -- in case of arrays
* SIZE       -- in case of arrays
* UNIT       -- the physical unit
* MINIMUM    -- a minimum value
* MAXIMUM    -- a maximum value
* RESOLUTION -- a resolution (smallest change possible)
* OFFSET     -- an offset (for calibration)
* SCALING    -- a scaling factor for calibration.
* INFO       -- Some human readable information about the topic
* TIMESTAMP  -- a timestamp of last message
* VALUE      -- the topics value
</pre>

Except for TIMESTAMP, VALUE, HOSTNAME, the rest of this meta-information is 
more or less static and need not be transported with low latency. So it could be
stored in a separate and slower database. However, the Homie Convention just uses
meta-topics to store them. Why not?

![The Homie Convention](https://github.com/homieiot/convention)


