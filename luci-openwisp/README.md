=============
LuCI OpenWISP
=============

.. image:: https://ci.publicwifi.it/buildStatus/icon?job=luci-openwisp
   :target: #

.. image:: http://img.shields.io/github/release/openwisp/luci-openwisp.svg
   :target: https://github.com/openwisp/luci-openwisp/releases

------------

`LuCI <https://github.com/openwrt/luci>`_ extensions for the OpenWISP project.

.. image:: https://raw.githubusercontent.com/openwisp/luci-openwisp/master/status-page-screenshot.png

.. contents:: **Table of Contents**:
 :backlinks: none
 :depth: 3

------------

Goal
----

The goal of this project is to provide a limited web interface for
`LEDE <https://lede-project.org/>`_ / `OpenWRT <https://openwrt.org/>`_ so
that users can configure only the bare minimum in order for their device to connect
to the `OpenWISP 2 Controller <https://github.com/openwisp/ansible-openwisp2>`_.

Use this web interface only if you have a similar use case, otherwise you should keep the
default LEDE / OpenWRT interface (*luci-mod-admin-full*).

Features
--------

- login with a different username and password combination than the root SSH password
- possibility to edit LAN settings
- status page (inherited from ``luci-admin-full``)
- upgrade firmware page
- reboot
- logout
- meta-packages for easier installation:
    - ``luci-openwisp``
    - ``luci-openwisp-mbedtls``
    - ``luci-openwisp-openssl``
    - ``luci-openwisp-polarssl``
    - ``luci-openwisp-cyassl``

Install precompiled packages
----------------------------

First run:

.. code-block:: shell

    opkg update

Then install one of the `latest builds <http://downloads.openwisp.org/luci-openwisp/>`_:

.. code-block:: shell

    opkg install <URL>

Where ``<URL>`` is the URL of the image that is suitable for your case.

For a list of the latest built images, take a look at `downloads.openwisp.org
<http://downloads.openwisp.org/luci-openwisp/>`_.

If you need to compile the package yourself see `Compiling luci-openwisp`_.

Configuration options
---------------------

UCI configuration options must go in ``/etc/config/luci_openwisp``.

- ``username``: username for the web interface, defaults to ``operator``
- ``password``: encrypted password for the web interface, defaults ``password`` (encrypted)

Change web UI password
----------------------

To change the default password for the web UI, use the ``openwisp-passwd`` script::

    openwisp-passwd
    Changing password for luci-mod-openwisp, username: operator
    New password:
    secret
    Retype password:
    secret
    luci-mod-openwisp password for user operator changed successfully

Packages
--------

This repository contains 2 packages:

1. ``luci-mod-openwisp``: contains the custom password validator and limited menu interface
2. ``luci-theme-openwisp``: contains a few adjustments to the general luci theme

Additionally a few meta-packages are also present in the ``luci-openwisp`` directory. These meta-packages
can be used in alternative to the commonly used ``luci`` or ``luci-ssl``.

- ``luci-openwisp``: depends on ``uhttpd``, ``uhttpd-mod-ubus``, ``luci-mod-openwisp`` and ``luci-theme-openwisp``
- ``luci-openwisp-mbedtls``: depends ``luci-openwisp``, ``libustream-mbedtls`` and ``px5g``
- ``luci-openwisp-openssl``: depends ``luci-openwisp``, ``libustream-openssl`` and ``px5g``
- ``luci-openwisp-cyassl``: depends ``luci-openwisp``, ``libustream-cyassl`` and ``px5g``
- ``luci-openwisp-polarssl``: depends ``luci-openwisp``, ``libustream-polarssl`` and ``px5g``

Compiling luci-openwisp
-----------------------

The following procedure illustrates how to compile the meta-package ``luci-openwisp``:

.. code-block:: shell

    git clone git://git.lede-project.org/source.git lede
    cd lede

    # configure feeds
    cp feeds.conf.default feeds.conf
    echo "src-git luciopenwisp https://github.com/openwisp/luci-openwisp.git" >> feeds.conf
    ./scripts/feeds update -a
    ./scripts/feeds install -a
    # any arch/target is fine because the packages are architecture indipendent
    arch="ar71xx"
    echo "CONFIG_TARGET_$arch=y" > .config;
    echo "CONFIG_PACKAGE_luci-openwisp=y" >> .config
    make defconfig
    make tools/install
    make toolchain/install
    make package/luci-openwisp/compile
    make package/luci-openwisp/install

Alternatively, you can configure your build interactively with ``make menuconfig``, in this case
you will need to select one of the available *luci-openwisp* variants by going to ``Luci > 1. Collections``:

.. code-block:: shell

    git clone git://git.lede-project.org/source.git lede
    cd lede

    # configure feeds
    cp feeds.conf.default feeds.conf
    echo "src-git luciopenwisp https://github.com/openwisp/luci-openwisp.git" >> feeds.conf
    ./scripts/feeds update -a
    ./scripts/feeds install -a
    make menuconfig
    # go to Luci > 1. Collections and select one of the variants
    make -j1 V=s

Changelog
---------

See `CHANGELOG <https://github.com/openwisp/luci-openwisp/blob/master/CHANGELOG.rst>`_.

License
-------

See `LICENSE <https://github.com/openwisp/luci-openwisp/blob/master/LICENSE>`_.

Support
-------

See `OpenWISP Support Channels <http://openwisp.org/support.html>`_.
