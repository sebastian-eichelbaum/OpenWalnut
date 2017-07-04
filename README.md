# OpenWalnut

OpenWalnut is an open source tool for multi-modal medical and brain data
visualization. Its universality allows it to be easily extended and used in a
large variety of application cases. It is both, a tool for the scientific user
and a powerful framework for the visualization researcher.  OpenWalnut is
licensed under the terms of the GNU Lesser General Public License. Written
entirely in Standard C++ and using a number of portable libraries (e.g.
OpenSceneGraph and  Qt), it runs on all Windows platforms, OSX, and
GNU/Linux operating systems.

- please refer to http://www.openwalnut.org for extensive documentation 
  and information.

# AUTHORS AND CONTRIBUTORS

See the files "AUTHORS" and "CONTRIBUTORS".

# LICENSE

OpenWalnut is licensed under the terms of the LGPLv3.
See "COPYING" and "COPYING.LESSER".

# CONTACT

If you have questions, comments, problems or find Bugs, contact us using:
 
* Mailing list: openwalnut@lists.informatik.uni-leipzig.de
 * Very low traffic at the moment ( < 1 mail per month), you might want to try
   the developer mailing list below.
 * To join the list visit 
   http://lists.informatik.uni-leipzig.de/mailman/listinfo/openwalnut and follow
   the instructions.
* Developer Mailing list: openwalnut-dev@lists.informatik.uni-leipzig.de
 * Medium traffic ( < 10 mails per week).
 * To join the list visit
   http://lists.informatik.uni-leipzig.de/mailman/listinfo/openwalnut-dev and
   follow the instructions.
* IRC: Occasionally you can contact us at irc.freenode.net #ow.

# INSTALLING

You have several options for installing OpenWalnut.
 * Build it -> see next section
 * Download a binary archive from our website and extract it
    * Inside the extracted directory, run bin/openwalnut
 * Download one of our binary packages for your distribution
 * Check the NeuroDebian repository
    * We are currently working on a OpenWalnut package which is intended to be
      released in NeuroDebian (http://neuro.debian.net/)

# BUILDING

As you currently read this README, we presume that you already have the source.
If not, check http://www.openwalnut.org. There, you can download source archives
and get the URL to our mercurial repository.

## Required Dependencies:

* Qt4 (>= 4.6)
* OpenSceneGraph (>= 2.8.0)
* Boost (>= 1.42.0)

## Compilation on Unix-like systems:

If you have installed all required third party libs, you can compile OpenWalnut
by using the common routine:

```sh
cd build
cmake ../src
make
make install -- this is optional and installs everything.
```

After that, OpenWalnut can be used inside the build directory.

If you want to install only a part of OpenWalnut, you can issue

```sh
make list_install_tarets
```

The listed targets will then only install the specified part.

## Compilation on Windows and Mac:

This is a little bit more complicated. We give you a step-by-step explanation
on http://www.openwalnut.org.

# BINARY DISTRIBUTION

If you have received OpenWalnut in binary form, you can always acquire the
code at http://www.openwalnut.org or from your distributor. In the binary
packages we provide, we might include open source, third-party libraries. 
If so, the licenses are re-distributed along the libraries. We did not modify
the sources. However, you can get the sources of these libraries by contacting
us.

# USING

After starting OpenWalnut, press F1 to get an introductory help. More 
information is available online at http://www.openwalnut.org

# DEVELOPING

If you want to develop modules for OpenWalnut, have a look at 
src/modules/template, which contains an extensively documented example module.
More information is available online. The wiki, especially the development
section, is very helpful. Besides this, an online API documentation is available
at http://api.openwalnut.org.
