.POSIX:
.SUFFIXES:

include config.mk

ASS:=$(addprefix $(OUTDIR)/$(ASSDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.s)))
INT:=$(addprefix $(OUTDIR)/$(INTDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.e)))
OBJ:=$(addprefix $(OUTDIR)/$(OBJDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.o)))

all: $(PROJECT)
setup:
	@mkdir -pv $(dir $(ASS)) 2>/dev/null || :
	@mkdir -pv $(dir $(INT)) 2>/dev/null || :
	@mkdir -pv $(dir $(OBJ)) 2>/dev/null || :
options:
	@echo "<------------------------------------->"
	@echo ""
	@echo "$(PROJECT) BUILD options:"
	@echo "TARGETNAME : $(TARGETNAME)"
	@echo "HOST       : $(HOST)"
	@echo "TARGET     : $(TARGET)"
	@echo "MODE       : $(MODE)"
	@echo ""
	@echo "$(PROJECT)_CFLAGS   =$($(PROJECT)_CFLAGS)"
	@echo "$(PROJECT)_CPPFLAGS =$($(PROJECT)_CPPFLAGS)"
	@echo "$(PROJECT)_LDFLAGS  =$($(PROJECT)_LDFLAGS)"
	@echo "CFLAGS            =$(CFLAGS)"
	@echo "CPPFLAGS          =$(CPPFLAGS)"
	@echo "LDFLAGS           =$(LDFLAGS)"
	@echo "CC                =$(CC)"
	@echo ""
	@echo "<------------------------------------->"

$(OUTDIR)/$(ASSDIR)/%.s: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CPPFLAGS) $($(PROJECT)_CFLAGS) -S $< -o $@
$(OUTDIR)/$(INTDIR)/%.e: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CPPFLAGS) $($(PROJECT)_CFLAGS) -E $< -o $@
$(OUTDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CPPFLAGS) $($(PROJECT)_CFLAGS) -c $< -o $@

$(ASS): $(HDR) Makefile config.mk
$(INT): $(HDR) Makefile config.mk
$(OBJ): $(HDR) Makefile config.mk

$(PROJECT): options setup $(OBJ)
ifeq ($(MODE),SHARED)
 ifeq ($(TARGET),UNIX)
	$(CC) -o $(OUTDIR)/$(TARGETNAME) $(OBJ) $($(PROJECT)_LDFLAGS)
 else # Windows target
	$(CC) -o $(OUTDIR)/$(TARGETNAME) $(OBJ) $($(PROJECT)_LDFLAGS) -Wl,--output-def,$(OUTDIR)/$(WINDEFNAME)
	$(CC) -o $(OUTDIR)/$(TARGETNAME) $(OBJ) $($(PROJECT)_LDFLAGS) -Wl,--kill-at
	$(DLLTOOL) --kill-at -d $(OUTDIR)/$(WINDEFNAME) -D $(OUTDIR)/$(TARGETNAME) -l $(OUTDIR)/$(WINIMPNAME)
 endif ## TARGET
else # Static
	$(AR) -crs $(OUTDIR)/$(TARGETNAME) $(OBJ)
endif ## MODE

ass: setup options $(ASS) ;
int: setup options $(INT) ;
obj: setup options $(OBJ) ;
aio: setup options $(ASS) $(INT) $(OBJ) ;

clean:
	$(RM) $(ASS)
	$(RM) $(INT)
	$(RM) $(OBJ)
	$(RM) $(OUTDIR)/$(TARGETNAME)
ifeq ($(TARGET),WINDOWS)
	$(RM) $(OUTDIR)/$(WINDEFNAME)
	$(RM) $(OUTDIR)/$(WINIMPNAME)
endif
ifeq ($(HOST),UNIX)
	find $(OUTDIR)/$(ASSDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
	find $(OUTDIR)/$(INTDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
	find $(OUTDIR)/$(OBJDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
	find $(OUTDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
else
	$(RMDIR) $(OUTDIR)/$(ASSDIR) 2>$(NULL) || :
	$(RMDIR) $(OUTDIR)/$(OBJDIR) 2>$(NULL) || :
	$(RMDIR) $(OUTDIR)/$(INTDIR) 2>$(NULL) || :
	$(RMDIR) $(OUTDIR) 2>$(NULL) || :
endif

again: clean all

check: all ; make -f Make.check LIBNAME=$(TARGETNAME)
check-options: ; make -f Make.check LIBNAME=$(TARGETNAME) options
check-ass: ; make -f Make.check LIBNAME=$(TARGETNAME) ass
check-int: ; make -f Make.check LIBNAME=$(TARGETNAME) int
check-obj: ; make -f Make.check LIBNAME=$(TARGETNAME) obj
check-aio: ; make -f Make.check LIBNAME=$(TARGETNAME) aio
check-clean: ; make -f Make.check clean
ifeq ($(MODE),STATIC)
check-again: all ; make -f Make.check again LIBNAME=$(TARGETNAME)
else
check-again: ; make -f Make.check again LIBNAME=$(TARGETNAME)
endif

install: all
ifeq ($(HOST),UNIX)
 ifeq ($(TARGET),UNIX)
	mkdir -pv $(prefix)/lib
	cp -fv $(OUTDIR)/$(TARGETNAME) $(prefix)/lib/
	mkdir -pv $(prefix)/include/$(PROJECT)
	cp -fv $(HDR) $(prefix)/include/$(PROJECT)
	ldconfig
 else
	@echo Nowhere to install when target is windows on unix host
 endif
else
	@echo NOTICE: Windows installation is manual for now.
endif

uninstall:
ifeq ($(HOST),UNIX)
 ifeq ($(TARGET),UNIX)
	rm -fv $(prefix)/lib/$(TARGETNAME)
	rm -fv $(addprefix $(prefix)/include/$(PROJECT)/,$(patsubst $(HDRDIR)/%,%,$(HDR)))
	ldconfig
 else
 endif
else
 # Nothing installed
endif

.PHONY: setup options ass int obj aio all clean again check check-ass check-int check-obj check-aio check-clean check-again install uninstall
