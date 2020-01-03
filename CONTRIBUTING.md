Guide to contributing to MQTT-Hyperdash
=======================================

MQTT-Hyperdash was started in 2019 from the fact that there did not exist any 
straight forward easy to use dashbord applications for linux, with which one 
could quickly design a graphical user interface for MQTT parameters.

However, such an application should not be too complicated and for performance 
reasons it whould be written in C. (There existed other dashboards written in 
Java-script and other languages, but they turned out to be way much too 
complicated to install and use.) 

However, for designing a panel, a simple text editor shall be used. 
The panels will be made from simple primitives. Some are static, some dynamic, 
which means that they change their appearance according to the content of a 
mqtt topic. Some allow user input, like buttons, potentiometers etc.

So there are things you can to. If you like, you can specify new dynamic elements
to be implemented. First they should be described in the doc section, and for 
implementing them there is a simple API.

If you are a programmer with C skills, you are welcome to debug and improve the 
code.

If you know how to make a package for WINDOWS including all the necessary 
dynamic object files of SDL and gtk, please help! It would be really cool 
if we could run hyperdash on WINDOWS. 

If you like to design the dashboards itself, please feel free to donate 
your .dash file, so we can include them in our example collection. 

If you are a graphics designer we are happy to receive nice monochrome 
bitmaps and color icons. 


More things left to do:
=======================
- test the thing and find more bugs,
- optimize a bit more, improve performance,
- the WINDOWS-version needs more work,
- work on the sound system, invent a cool SOUND element,
- documentation needs more work,
- translate the user manual into other languares (german, french,...)
- port it to apple ipad/iphone,
(etc. etc.)

## License and attribution

All contributions must be properly licensed and attributed. If you are
contributing your own original work, then you are offering it under a CC-BY
license (Creative Commons Attribution). If it is code, you are offering it under
the GPL-v2. You are responsible for adding your own name or pseudonym in the
Acknowledgments file, as attribution for your contribution.

If you are sourcing a contribution from somewhere else, it must carry a
compatible license. The project was initially released under the GNU public
licence GPL-v2 which means that contributions must be licensed under open
licenses such as MIT, CC0, CC-BY, etc. You need to indicate the original source
and original license, by including a comment above your contribution. 


## Contributing with a Pull Request

The best way to contribute to this project is by making a pull request:

1. Login with your Github account or create one now
2. [Fork](https://github.com/kollokollo/MQTT-Hyperdash#fork-destination-box) the MQTT-Hyperdash repository. Work on your fork.
3. Create a new branch on which to make your change, e.g.
`git checkout -b my_code_contribution`, or make the change on the `new` branch.
4. Edit the file where you want to make a change or create a new file in the `contrib` directory if you're not sure where your contribution might fit.
5. Edit `doc/ACKNOWLEGEMENTS` and add your own name to the list of contributors under the section with the current year. Use your name, or a github ID, or a pseudonym.
6. Commit your change. Include a commit message describing the correction.
7. Submit a pull request against the MQTT-Hyperdash repository.



## Contributing with an Issue

If you find a mistake and you're not sure how to fix it, or you don't know how
to do a pull request, then you can file an Issue. Filing an Issue will help us
see the problem and fix it.

Create a [new Issue](https://github.com/kollokollo/MQTT-Hyperdash/issues/new) now!



## Thanks

We are very grateful for your support. With your help, this implementation
will be a great project. 


So you are welcome to help.
