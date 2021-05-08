.POSIX:
.SUFFIXES:

include config.mk

# Makefiles are stupidly finicky good god
ASS:=$(addprefix $(OUTDIR)/$(ASSDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.s)))
INT:=$(addprefix $(OUTDIR)/$(INTDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.e)))
OBJ:=$(addprefix $(OUTDIR)/$(OBJDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.o)))

all: $(PROJECT)
setup:
	@mkdir -pv $(dir $(ASS)) 2>/dev/null || :
	@mkdir -pv $(dir $(INT)) 2>/dev/null || :
	@mkdir -pv $(dir $(OBJ)) 2>/dev/null || :
options:
	@echo "$(PROJECT) Build options:"
	@echo "CFLAGS        = $(CFLAGS)"
	@echo "CPPFLAGS      = $(CPPFLAGS)"
	@echo "STCPPFLAGS    = $(STCPPFLAGS)"
	@echo "$(PROJECT)_CFLAGS   = $($(PROJECT)_CFLAGS)"
	@echo "$(PROJECT)_CPPFLAGS = $($(PROJECT)_CPPFLAGS)"
	@echo "$(PROJECT)_LDFLAGS  = $($(PROJECT)_LDFLAGS)"
	@echo "CC            = $(CC)"

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
 ifeq ($(TARGET),LINUX)
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

clean:
	make -f Make.check clean
	$(RM) $(ASS)
	$(RM) $(INT)
	$(RM) $(OBJ)
	$(RM) $(OUTDIR)/$(TARGETNAME)
ifeq ($(TARGET),WINDOWS)
	$(RM) $(OUTDIR)/$(WINDEFNAME)
	$(RM) $(OUTDIR)/$(WINIMPNAME)
endif
ifeq ($(HOST),LINUX)
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

check: all ; make -f Make.check LIBNAME=$(TARGETNAME)
check-ass: ; make -f Make.check ass
check-int: ; make -f Make.check int
check-obj: ; make -f Make.check obj
check-clean: ; make -f Make.check clean
ifeq ($(MODE),STATIC)
check-again: all ; make -f Make.check again LIBNAME=$(TARGETNAME)
else
check-again: ; make -f Make.check again LIBNAME=$(TARGETNAME)
endif
again: clean all
install: all
ifeq ($(HOST),LINUX)
 ifeq ($(TARGET),LINUX)
	mkdir -pv $(prefix)/lib
	cp -fv $(OUTDIR)/$(TARGETNAME) $(prefix)/lib/
	mkdir -pv $(prefix)/include/$(PROJECT)
	cp -fv $(HDR) $(prefix)/include/$(PROJECT)
	ldconfig
 else
  # Nowhere to install when target is windows on linux host
 endif
else
	@echo NOTICE: Windows installation is manual for now.
endif
uninstall:
ifeq ($(HOST),LINUX)
 ifeq ($(TARGET),LINUX)
	rm -fv $(prefix)/lib/$(TARGETNAME)
	rm -fv $(addprefix $(prefix)/include/$(PROJECT)/,$(patsubst $(HDRDIR)/%,%,$(HDR)))
	ldconfig
 else
 endif
else
 # Nothing installed
endif

.PHONY: setup options ass int obj all again check clean install uninstall
