# vst_plugins
my vst directory

bring your own vst sdk damn it

only supports vst 2.x
because simple to make plugins

use visual studio 2022

build solution as win32

put thing in C:\Program Files (x86)\VSTPlugins

note you shoulld PROBABLY fix the sdk yourself it errors when compiling

target files audioeffectx.cpp on function AudioEffectX::resume
and vstplugmain.cpp (if your sdk doesnt support msvc)

when fixing
