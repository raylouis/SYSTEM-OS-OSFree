
# MAKE file for BCDvr.MAK     Version 1.1

# Revised:  1994-04-21

# Copyright � 1987-1994  Prominare Inc.

# MAKE file created by Prominare Builder  Version 2.1b

# Macro definitions

CC=BCC
C_SW=-Ox -v -w -c -IJ:\BCOS2\INCLUDE;D:\Include;N:\MMOS2\MMTOOLKT\H;
RC_SW=-r 
LNK_SW=-v -Toe -A:2 -B:0x00010000 -LJ:\BCOS2\LIB;N:\MMOS2\MMTOOLKT\LIB;

Driver.Exe: Driver.Obj
 Tlink $(LNK_SW) @BCDvr.Lnk;

Driver.Obj: Driver.C
 $(CC) $(C_SW)-o$*.Obj Driver.C

