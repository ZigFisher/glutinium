
Baoclone is a utility for programming handheld radios via a serial or USB
programming cable.  Supported radios:

 * Baofeng UV-5R
 * Baofeng UV-B5
 * Baofeng BF-888S
 * Yaesu FT-60R

Web site of the project: https://code.google.com/p/baoclone/wiki/About


=== Usage ===

Save device binary image to file 'device.img', and text configuration
to 'device.conf':

    baoclone [-v] port

Write image to device.

    baoclone -w [-v] port file.img

Configure device from text file.
Previous device image saved to 'backup.img':

    baoclone -c [-v] port file.conf

Show configuration from image file:

    baoclone file.img

Option -v enables tracing of a serial protocol to the radio:


=== Example ===

For example:
    C:\> baoclone.exe COM5
    Connect to COM5.
    Detected Baofeng UV-5R.
    Read device: ################################################## done.
    Radio: Baofeng UV-5R
    Firmware: Ver  BFB291
    Serial: 120801NB5R0001
    Close device.
    Write image to file 'device.img'.
    Print configuration to file 'device.conf'.


=== Configurations ===

You can use these files as examples or templates:
 * uv-5r-factory.conf     - Factory configuration for UV-5R.
 * uv-b5-factory.conf     - Factory configuration for UV-B5.
 * bf-888s-factory.conf   - Factory configuration for BF-888S.
 * uv-5r-sunnyvale.conf,
   uv-b5-sunnyvale.conf,
   bf-888s-sunnyvale.conf,
   ft-60r-sunnyvale.conf  - Configurations of my personal handhelds
                            for south SF Bay Area, CA.


=== Sources ===

Sources are distributed freely under the terms of MIT license.
You can download sources via SVN:

    git clone https://code.google.com/p/baoclone/

To build on Linux or Mac OS X, run:
    make
    make install

To build on Windows using MINGW compiler, run:
    gmake -f make-mingw

___
Regards,
Serge Vakulenko
KK6ABQ
