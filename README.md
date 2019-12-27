# Code-3 Programmer's Editor

This is a code-oriented editor with strong C++ support.

![screenshot](https://github.com/pragmaware/code3/raw/master/doc/screenshot20191227.png "Screenshot")

Features:

- Lightweight: on a reasonable development workstation starts in less than a second
- Syntax highlighting
- Smart code completion and suggestions
- Strong C++ support
- Powerful find and replace
- Lightweight workspace model
- Execution of custom external commands via shortcuts
- Useful macros for codes like JSON formatting, Base64 encoding/decoding etc.
- Cross platform (currently tested on linux only)

# Building

Dependancies:

- Qt5
- CMake

To build:

```sh
mkdir build
cd build
ccmake ..
make
make install
```

# Author

Szymon Tomasz Stefanek. Write to <sts at pragmaware dot net>.

# License

GPLv3. See [LICENSE](LICENSE.md).

# History

I've been writing and using my own text editor for quite a while now. The first attempts
can be traced back to 1998. In 2002 there was a reasonably working version and it was
named SSEX: Szymon Stefanek's Editor for X. In 2008 the project was ported to Qt 3 and
in the meantime substantially redesigned. In 2015 we got again a major redesign and the
port to Qt5.

Now, in 2019 the project is being made public as Code 3, which is a better sounding
and less controversial name. "Code 3" reflects the fact that it's the third major design
and sounds a bit like "Code Free" (as in "Free Speech" not "Free Beer"). You may also
find it spelled as Code-3, Cod3, Cod^3 or Cod<sup>3</sup>.

