Whisper is a general purpose Mac/Windows C++ application framework. It's the succesor

to the Mac framework Raven. Like Raven Whisper is a modern framework that takes

advantage of templates, multiple inheritance, STL, and exceptions. It's also designed

around the notion of Design by Contract and includes numerous debugging tools.



There are a number of things I want to achieve with Whisper:



* I want to make it as easy as possible to write cross platform apps. Ideally this 

would involve merely a recompile. (At my last company we essentially achieved this 

using a more specialized framework than Whisper).



* I don't want to slight either platform: Whisper should be just as attractive to

a Windows programmer as to a Mac programmer.



* I've looked at and used a number of different application frameworks. For the most

part they're rather disapointing: most of them don't provide enough functionality,

make poor use of abstractions, have weak error handling, and don't take full advantage

of C++. I intend for Whisper to do better in all these areas. :-)



* I would like for Whisper to be better than even the platform specific frameworks.

Despite the flaws I alluded to above this is a tough job: the other framework vendors

have far more resources and they're not hampered by supporting multiple platforms.

However I believe that Whisper may achieve this goal in time.



* Writing any framework is a big job let alone a cross platform framework. My hope is

that if I provide a good start others will join in so that Whisper can evolve into

a real kick-ass framework.



Jesse Jones

jesjones@halcyon.com