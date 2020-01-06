## MQTT Rule Engines

Rule engines are ment to be applications running completely in the background. 
They do not interact directly with the user. Their function is to subscribe to a
set of topics, watch their updates and trigger a routine, which calculates 
something based on the input topics and finally puplish the result(s) to output
topics,  which then can trigger other rules or rule engines. They also can
perform actions on the machine they are running, like excecuting shell scripts
when a topic content matches a certain pattern. With multuple rule engines
running even on different computers using the same broker, one can implmenent a
full automation control, which would create the Internet of Things. However the
concept of rule engines is not new and can be found in other automation
concepts as well. This implementation is focused on high performance quick
reaction, and reliablility. Using them is not very complicated. 

### Quick Guide: demo.rule

You want to have a look at src/demo.rule. This es everything needed to implement
make a simple rule. The rule will be compiled into a native excecutable with

<pre>
make demo
</pre>

The file demo now can be excecuted, but it should not print anything to the
terminal console, becaue besided debug messages there is no direct output or
input via the console. 

If you see an error message, make sure that you run a broker on the 
machine (localhost) and that the access rights are correct.

You can as well excecute demo in the background, e.g.
like this:

<pre>
./demo 2>&1 >> /tmp/demo.log &
</pre>

I have put mine in the crontab, so the automatically start at boot 
(on a Raspberry pi):
<pre>
@reboot /home/pi/bin/demo  2>&1 > /dev/null
</pre>

#### The magic

The magic not happens in the background. To see, what happens, 
please open three terminal windows.

In Window number 1 please run 
<pre>
 mosquitto_sub -h localhost -t DEMO/ACTIVITY_DM 
</pre>

If demo is running, you shouls see an update here every second counting from 0
to 3. This indicates that the demo engine is running. (Maybe you want to make 
a dashboard, and put DEMO/ACTIVITY_DM to a BITMAPLABEL ...)

Now open Window number 2 and run
<pre>
 mosquitto_sub -h localhost -t DEMO/C 
</pre>

and windows number 3 and enter

<pre>
  mosquitto_pub -h localhost -t DEMO/A -m 12
  mosquitto_pub -h localhost -t DEMO/B -m 10
</pre>

You should find the results update in window number 2. 

Thats magic. Now you should be creative and think what you 
can to with this mechanism. 

I do
* calculate the filling percentage of a battery out of the voltage measured
* Do a rescaling of values from mSv/d to µSv/h.
* Have shell commands excecuted on a remote machine whenever I press a Button on
  a MQTT-Hyperdash dashboard.
* Recalculate and rescale the data for a plot in a dashboard whenever the 
  user sets a different scaling for X or Y.

The rule engines consume nearly no cpu power, they are so fast, that changes are
instantly trigger calculations on the fly. 

### Writing own rules

You should take demo.rule as a template and modify it according to your needs.

The rule syntax is C with some prerdefined macros. Rule files are actually C 
files and they are  compiled with gcc. SO you can add any subfunction you 
need for calculation. And the stdlib including math is readily available. 

This way the excecution of the rule function can be very fast, but there are 
nearly no limitations.

For advanced use, the rule definitions allow an initialization functions which 
is called once at startup of the rule engine excecutable and can pass
commandline parameters, ask for environment variables or open configuration
files, if needed. The demo rule does not make use of it.

<pre>
typedef struct {
  PARMDEF *params; /* The List of parameters/topics */
  int numparms;    /* Number of parameters defined*/
  void (*initrule)(); /* The initialization rule */
  void (*rule)();     /* The trigger rule */
} RULEDEF;
</pre>

Initrule gets the parameters (argc,argv) from the main function.
Parameters are defined using this struct:


<pre>
typedef struct {
  int type;     /* Type of the parameter, a combination of 
                   PARM_IN PARM_TRIGGER PARM_OUT PARM_ISNUMBER*/
  char *topic;  /* The topics name */
  int qos;      /* The Quality of Service to be used for 
                   subscription and publication*/
  int sub;      /* In index to the subscription table, initialize with 0*/
  char *format; /* A format string to format floating point numbers to 
                   the topics content, BASIC like or printf-like */
} PARMDEF;
</pre>

Maybe interesting is the qos parameter. It can make sense to set it to 2 in cases
where the trigger must not get lost and must occur not more than once. 
However this slows the rules down quite a lot. 

If you have questions or problems using this framework, please feel free to 
discuss this in the Issues forum of this repository.


