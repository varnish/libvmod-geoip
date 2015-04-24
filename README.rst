==========
vmod_geoip
==========

---------------------------
Varnish GeoIP Lookup Module
---------------------------

:Author: Hauke Lampe
:Date: 2011-09-26
:Version: 1.0
:Manual section: 3

SYNOPSIS
========

import geoip;

DESCRIPTION
===========

This Varnish module exports functions to look up GeoIP country codes.
Requires GeoIP library (on Debian install libgeoip-dev)

Inspired by http://drcarter.info/2010/07/another-way-to-link-varnish-and-maxmind-geoip/


FUNCTIONS
=========

ip_country_code (not exported yet)
----------------------------------

Prototype
        ::

                ip_country_code(IP I)
Return value
	STRING
Description
	Returns two-letter country code string from IP address
Example
        ::

                set req.http.X-Country-Code = geoip.ip_country_code(client.ip);

country_code
------------

Prototype
        ::

                country_code(STRING S)
Return value
	STRING
Description
	Returns two-letter country code string
Example
        ::

                set req.http.X-Country-Code = geoip.country_code("127.0.0.1");


ip_country_name (not exported yet)
----------------------------------

Prototype
        ::

                ip_country_name(IP I)
Return value
	STRING
Description
	Returns country name string from IP address
Example
        ::

                set req.http.X-Country-Name = geoip.ip_country_name(client.ip);

country_name
------------

Prototype
        ::

                country_name(STRING S)
Return value
	STRING
Description
	Returns country name string
Example
        ::

                set req.http.X-Country-Name = geoip.country_name("127.0.0.1");


ip_region_name (not exported yet)
---------------------------------

Prototype
        ::

                ip_region_name(IP I)
Return value
	STRING
Description
	Returns region name string from IP address
Example
        ::

                set req.http.X-Region-Name = geoip.ip_region_name(client.ip);

region_name (not exported yet)
------------------------------

Prototype
        ::

                region_name(STRING S)
Return value
	STRING
Description
	Returns region name string
Example
        ::

                set req.http.X-Region-Name = geoip.region_name("127.0.0.1");


INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the varnishtest tool.

Install the GeoIP library headers::

 apt-get install libgeoip-dev

To check out the current development source::

 git clone git://github.com/varnish/libvmod-geoip.git
 cd libvmod-geoip; ./autogen.sh

Usage::

 ./configure

Make targets:

* make - builds the vmod
* make install - installs your vmod
* make check - runs the unit tests in ``src/tests/*.vtc``

In your VCL you could then use this vmod along the following lines::
        
        import geoip;

        sub vcl_recv {
                # This sets req.http.X-Country-Code to the country code
                # associated with the client IP address
                set req.http.X-Country-Code = geoip.country_code(client.ip);
        }

HISTORY
=======

No history yet.


COPYRIGHT
=========

The code is licensed to you under following MIT-style License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.TODO
