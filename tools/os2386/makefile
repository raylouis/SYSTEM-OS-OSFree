#
#
#

PROJ = os2386
DIRS = libc
lbcs =  bdcalls.lbc doscalls.lbc helpmgr.lbc sub32.lbc  &
#       kbdcalls.lbc moncalls.lbc moucalls.lbc viocalls.lbc &
        msg.lbc nampipes.lbc nls.lbc pmctls.lbc pmdrag.lbc pmgpi.lbc &
        pmspl.lbc pmshapi.lbc pmviop.lbc pmwin.lbc pmwp.lbc quecalls.lbc sesmgr.lbc
UNI2H = 1

TARGETS  = $(PATH)$(PROJ).lib subdirs

!include $(%ROOT)/mk/libsos2.mk

.SUFFIXES:
.SUFFIXES: .lib .lbc .def .obj .asm

LOPT = -q -n
#DEST = $(BLD)lib

.lib: $(PATH)

.lbc: $(PATH)

.obj: $(PATH)

.def: $(MYDIR)

.asm: $(MYDIR)

.asm.obj: .AUTODEPEND
 $(SAY) Assembling $[. $(LOG)
 $(ASM) $(ASMOPT) -fr=$^*.err -fo=$^@ $[@ $(LOG)

$(PATH)os2386.lbc: $(lbcs)
        %create $@
        @for %f in ($<) do cat %f >>$@

.def.lbc:
        sed -f $(MYDIR)def2lbc.sed $(MYDIR)$[@ >$(PATH)$^@

$(PATH)$(PROJ).lib: $(PATH)os2386.lbc $(PATH)msgseg32.obj
        $(LIB) $(LOPT) $@ @$[@ +$]@
