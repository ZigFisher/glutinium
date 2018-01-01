
ubnt2nag
========

Nagios plugin for UBNT devices


Licence
=======

This script is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License as published by the Free Software Foundation;
either version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public License along with this
script; if not, please visit http://www.gnu.org/copyleft/gpl.html for more information.


Main features
=============

* Ruby small script, easy to understand and hack
* Support 'status_cgi' and 'mca_status' commands


News
====

16.10.2013 Release 0.3

* add pnp4nagios graphics template - uptime, connected users, load average

14.10.2013 Release 0.2

* add pnp4nagios graphics template - traffic, signal and noise, wifi ccq, wifi rate

12.10.2013 Release 0.1

* initial


Install complex
===============

	git clone https://github.com/slayer/ubnt2nag.git /var/lib/nagios/ubnt2nag
	cp /var/lib/nagios/ubnt2nag/check_ubnt.php /usr/share/pnp4nagios/html/templates.dist/check_ubnt.php


Usage
=====

	ubnt2nag -h [user@]host[:port] -u user [-p password] -k ssh_key_file [-v] command


Example Nagios config
=====================

	define command{
	  command_name           check_ubnt
	  command_line           (BUNDLE_GEMFILE=/var/lib/nagios/ubnt2nag/Gemfile /usr/local/rbenv/shims/bundle exec /var/lib/nagios/ubnt2nag/ubnt2nag -h $HOSTADDRESS$ -u $ARG1$ -k $ARG2$ $ARG3$)
	}

	define service {
	  use                    generic-service,srv-pnp
	  hostgroup_name         wl-kiev-bs1,wl-kiev-bs2,wl-kiev-bs3
	  service_description    Ubnt Graph
	  check_command          check_ubnt!admin!/etc/nagios3/ssh/kiev_ubiquiti.priv!mca_status
	  normal_check_interval  1
	  retry_check_interval   1
	  notifications_enabled  0
	  notification_interval  0 ; set > 0 if you want to be renotified
	}


Todo notes
==========

	1. Если отсутствует параметр у lanSpeed (и других), то вместо скорости писать "Off"
	2. В параметре platform (и других) заменять встречающиеся пробелы на подчеркивания
	3. Обрабатывать переменную uptime как 0d:0h:0m (дни, часы, минуты)
	4. Урезать вывод версии прошивки на три окончания XM.ar7240.v5.5.0.02.ubnt-ic.12536.130323.1646 до XM.ar7240.v5.5.0.02.ubnt-ic
	5. Добавить переменную ping до сервера, указываемого так-же в командной строке
	6. Закоментировать в коде использование status_cgi и переключение между режимами
	7. Разобраться с цветовыми схемами в rrd, GRADIENT и т.д.
	8. Облагородить скрипт запуска

