//UPDATE AUG/5/2014


Fixed a bug that caused crash on exit under mac
Added password MODE!!

to do:


When pressing Tab change focus between textboxes









//This is not finished yet!!! I update this constantly please re-visit 
and PLEASE DO open issues if you find any!!!


What is this addon?


This is a TEXTBOX for openFrameworks that supports all languages.


What's the difference from other textboxes outthere?

It's not a fake textbox. ..it's the real deal



ideally you should subclass this and only create it, or show it when the user is inputing text
when user is not inputing text  then hide it and draw in its place a white rect with the text in it.
(a gl renderable  textbox)



ofcourse, you can always use it directly!!!

by just doing:

textbox.draw(x,y,100,20);



just keep in mind that this is attached above your gl-window



///UPDATE 16/6/2014

Fixed a bug that did not give focus to the main window correctly on PC when clicking outside the textbox and multiple textboxes were available



UNICODE TEXT INPUT
DROP DOWN MENUS
COPY, PASTE, SELECT etc
SCROLL BAR
MULTILINE TEXT.

much more coming.



//UPDATE 10/4/2014

fixed some bugs that caused crashes.

setText() can be called both in Setup and in loop






//Update 3 28/3/2014


Added Support for V-Studio
Fixed minor bugs

Added multiline ability for both MAC and PC

Added a scroll-bar bot on MAC and PC


scroll-bar listens to mouse-scroll events 

in the mac version twofinger gestrue is also supported

added Hide() show() function for Mac+PC

added getText() for mac and PC



//Update 2 : 15/3/2014 - fixed autopositioning bug on osx


//Update 1

Tested it with OF 8.1

Added support for GLFW

//Update



*****************************************************************************************************************


A legit UTF8 - Unicode aware Textbox for openFrameworks (mac & pc)

This textbox - unlike all other OF textboxes - derives directly from System Classes is attached above open gl. - not rendered by it.

since its UTF8 / Unicode

All languages are supported. If your os supports the language then ofTextField supports it as well.

get the text as a UTF8 string and draw it in your OF app using fontstash or other unicode aware text rendering classes

legit copy paste menus by apple and microsoft.

spelling check and much much more!


How to use:


Just replace ofSystemUtils.cpp & ofSystemUtils.h inside  libs/openFrameworks/utils/


An example is included


*****************************************************************************************************************


Kyriacos Kousoulides
