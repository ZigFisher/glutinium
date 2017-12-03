
More info
---------
* https://github.com/rvbglas/gpio-watch
* https://github.com/larsks/gpio-watch

gpio-watch
==========

`gpio-watch` is a tool for running scripts in response to GPIO events.

Synopsis
========

    gpio-watch [-D script_directory] [-e default_edge] [pin[:edge]] ...

Description
===========

`gpio-watch` was written to make it easy to connect external
edge-triggered sensors -- like pushbuttons -- to a Raspberry Pi (or
other computer with available GPIO lines).  It will watch a set of
pins for events, and will trigger shell scripts in response to those
events.

`gpio-watch` accepts a list of pins to watch for events.  By default
it will trigger on both rising and falling events, but you can change
the default behavior with the `-e default_edge` command line option.

You may also modify the behavior per-pin by appending an edge mode
after the pin number, such as `4:rising` to monitor the rising edge on
pin 4.

Options
=======

- `-s script_directory` -- location in which `gpio-watch` will look
  for event handling scripts.  Scripts must be named after the pin
  number triggering the event.  For example, if you specify `-D
  /etc/gpio-scripts`, and `gpio-watch` processes an event on pin 4, it
  will attempt to run `/etc/gpio-scripts/4`.

  Defaults to `/etc/gpio-scripts`.

- `-e default_edge` -- specifies whether `gpio-watch` should monitor
  `rising`, `falling`, or `both` edges by default.  You can also specify
  edge detection per-pin.

  The special keyword `switch` activates switch debouncing logic.  In
  this mode, `gpio-watch` sets the edge mode to `both` but only
  activates the event script when the button has been released.  The
  release (falling) event must happen more than `DEBOUNCE_INTERVAL` (current
  hardcoded as 100000 nanoseconds) after the press (rising) event to 
  activate the script.

Example
=======

Watch switches connected to pins 21, 22, and 23:

    gpio-watch -e switch 21 22 23

If you were to press a button connected to pin 23, `gpio-watch` would
attempt to run:

    /etc/gpio-scripts/23 23 0

That is, the event script is called with both the pin number and the
current pin value to permit a single script to handle multiple events.

License
=======

gpio-watch, a tool for running scripts in response to gpio events
Copyright (C) 2014 Lars Kellogg-Stedman <lars@oddbit.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

