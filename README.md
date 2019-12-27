# Code-3 Programmer's Editor

Code-3 is a code-oriented text editor with pretty good C++ support.

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

- Qt >= 5.0
- CMake

To build it on linux open a shell in the source root and type:

```sh
mkdir build
cd build
ccmake ..
```

Hit c a few times and if cmake doesn't complain hit g to generate the build scripts. Then:

```sh
make
make install
```

Start with 

```sh
code3
```

On Mac OS X the build steps are quite similar.

TODO: windows build instructions

# Author

Szymon Tomasz Stefanek. Write to &lt;sts at pragmaware dot net&gt;.

# License

GPLv3. See [LICENSE](LICENSE.md).

# History

I've been writing and using my own text editor for quite a while now. The first attempts
can be traced back to 1998. In 2002 there was a reasonably working version and it was
named SSEX: Szymon Stefanek's Editor for X. In 2008 the project was ported to Qt 3 and
in the meantime substantially redesigned. In 2015 the unavoidable port to Qt 5 triggered
another major redesign.

Now, in 2019 the project is being made public as **Code 3**, which is a better sounding
and less controversial name. "Code 3" reflects the fact that it's the third design iteration
and sounds a bit like "Code Free" (as in "Free Speech" not "Free Beer"). You may also
find the name spelled as Code-3, Cod3, Code^3 or Code<sup>3</sup>. I claim all these names :)

