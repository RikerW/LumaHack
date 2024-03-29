#!/usr/bin/osascript
# LumaHack 3.6  LumaHackRecover.applescript $NHDT-Date: 1524684596 2018/04/25 19:29:56 $  $NHDT-Branch: LumaHack-3.6.0 $:$NHDT-Revision: 1.9 $
# Copyright (c) Kenneth Lorber, Kensington, Maryland, 2009
# LumaHack may be freely redistributed.  See license for details. 



set canceled to false
try
	display dialog "Welcome to the LumaHack recover program.  Please make sure LumaHack is not running before continuing.  Ready?" with title "LumaHackRecover"
on error number -128
	set canceled to true
end try
if not canceled then
	set hpath to the path to me
	set mpath to the POSIX path of hpath
	considering case
		--set lastpos to offset of "/lumahackdir" in mpath
		--set lastpos to lastpos + (length of "/lumahackdir")
		--set rawpath to (get text 1 through lastpos of mpath) & "/recover.pl"
		--set safepath to the quoted form of rawpath
		set safepath to the quoted form of "/Library/Nethack/lumahackdir/recover.pl"
	end considering
	do shell script safepath
	display dialog result with title "LumaHackRecover Output"
end if
