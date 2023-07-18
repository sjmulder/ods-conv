ods-conv
========
Convert from OpenDocument spreadsheet format (OpenOffice, LibreOffice)
to text.

**ods-conv** [**-F** *format*] *file*

**format** currently has one option, *tsv*, which is the default.

Installation
------------
Requires [libxml2](https://gitlab.gnome.org/GNOME/libxml2/-/wikis/home)
and [libzip](https://libzip.org/).

With *make*:

    $ make
    # make install [DESTDIR=] [PREFIX=/usr/local]

A CMakeList.txt is also included.

Authors
-------
Sijmen J. Mulder (<ik@sjmulder.nl>). See LICENSE.md.
