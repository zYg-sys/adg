General information
===================
[![Build Status](https://travis-ci.org/ntd/adg.svg?branch=master)](https://travis-ci.org/ntd/adg)
[![Coverage Status](https://coveralls.io/repos/ntd/adg/badge.svg?branch=master)](https://coveralls.io/r/ntd/adg?branch=master)

Automatic Drawing Generation is GObject-based library that provides
a non-interactive cairo canvas specifically designed for generating
technical drawings. It can be viewed as a tool for programmatically
representing an abstract model into a layout (2D drafting).

If you want to develop an application that needs to show and print
drawings where some data change but the overall design is similar,
the ADG library can be quite useful. In manufacturing industries
this is often the case and an ADG-based application could help to
greatly improve the productivity of technical offices.

You could reach similar targets using parametric CADs but a custom
application provides the following benefits:

 * more customizable;
 * quicker and not so bloated as a parametric CAD;
 * can be easily connected to a database;
 * an ADG based filter can generate drawing on-fly: this feature
   could be used in web-based applications.

Visit the [ADG web site](http://adg.entidi.com/) for further
details.


Design overview
===============

The project is based on the [GObject](http://www.gtk.org/) library:
the ADG canvas is developed in plain C using an object-oriented
approach. Applications based on ADG are not expected to be
developed in C, though: the basic idea is to have a set of bindings
for higher level languages (the garbage-collected ones, above all).
An application can then be developed using any specific language
available, much in the same way as [GNOME](http://www.gnome.org/)
applications are conceived.

Lua bindings based on [LGI](http://github.com/pavouk/lgi) are
already availables and effectively used upstream to test the APIs.
The [adg-lua](http://dev.entidi.com/p/adg-lua/) project provides
code examples on how to use them.

The mathematical and geometrical algorithms, together with a bunch
of other useful functions for manipulating cairo paths, are kept
in a separated library called [CPML](http://adg.entidi.com/cpml/)
(Cairo Path Manipulation Library). Although actually embedded into
the ADG project, this library can be easily splitted on its own,
if needed.

The rendering leverages [cairo](http://cairographics.org/), so the
ADG canvas fully shares strong and weak points of that library.
Most notably, the availables export formats include PostScript,
SVG, PDF, PNG but no CAD specific ones (such as DXF). A complete
list can be [browsed online](http://cairographics.org/backends/).


External dependencies
=====================

The ADG library has the following dependencies:

 * [cairo](http://cairographics.org/) 1.7.4 or later, required by
   either CPML and ADG;
 * [GLib](http://www.gtk.org/) 2.38.0 or later, required by ADG;
 * [GTK+](http://www.gtk.org/) 3.0.0 or later (or GTK+ 2.12.0 or
   later for GTK+2 support) to optionally include GTK+ support and
   build the adg-demo program;
 * [pango](http://www.pango.org/) 1.18.0 or later (optional)
   to support a serious engine for rendering text instead of using
   the cairo "toy" APIs (only the pango-cairo module will be used);
 * [gtk-doc](http://www.gtk.org/gtk-doc/) 1.12 or later (optional),
   used to regenerate the API documentation
 * [GObject introspection](http://live.gnome.org/GObjectIntrospection)
   1.0.0 or later (optional) to dinamically generate bindings
   metadata.

The required packages must be installed prior the ADG building:
they should be availables on nearly every decent unix-like system.

The ADG is mainly developed on GNU/Linux but its dependecies are
known to be cross platform so a porting should be quite easy, if
not automatic. Anyway, the cross-compilation of a Windows
installer is supported upstream and it is based on the ArchLinux
[Fedora mingw port](http://github.com/ntd/aur-fedora-mingw).


Installation
============

The ADG build system is based on GNU autotools, the de-facto
standard for handling free software projects. The following
step-by-step instructions are included only for reference: you are
free to install ADG in the way you prefer. Just remember if you
are copying and pasting to put the proper package version instead
of the 0.6.0 placeholder used in the examples.

1. Download the ADG tarball and put it somewhere into your file
   system (your home directory is a good candidate). You can
   browse the old releases or get the latest tarball directly
   from [SourceForge](http://sourceforge.net/projects/adg/files/).

    ```
    cd $HOME
    wget http://sourceforge.net/projects/adg/files/adg/0.6.0/adg-0.6.0.tar.bz2
    ```
2. Unpack the tarball:

    ```
    tar xjvf adg-0.6.0.tar.bz2
    ```
3. Configure the build. You can set different options while
   configuring: check the output of ./configure --help to get a
   list of all the available customizations.

    ```
    cd adg-0.6.0
    ./configure
    ```
4. Build the ADG library.

    ```
    make
    ```
5. Once the building has finished, you are able to run the demo
   program without installing anything. Just try it out by
   launching the uninstalled executable:

    ```
    demo/adg-demo-uninstalled
    ```
6. Install the library. You must have root privileges, either
   by previously logging in as root or by using the sudo command,
   if you intend to install it system-wide (the default). Anyway
   you can install to a custom destination where you have write
   access by explicitely setting the --prefix option in the
   configuration step.

    ```
    sudo make install
    ```

Further details on the installation are availables in the INSTALL
text file distributed within the tarball.
