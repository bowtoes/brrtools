.POSIX:
.SUFFIXES:

# I may be bad at making make files, ...

include config.mk

ASS:=$(addprefix $(OUTDIR)/$(ASSDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.s)))
INT:=$(addprefix $(OUTDIR)/$(INTDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.E)))
OBJ:=$(addprefix $(OUTDIR)/$(OBJDIR)/,$(patsubst $(SRCDIR)/%,%,$(SRC:.c=.o)))

all: $(PROJECT)
setup:
# {
	@mkdir -pv $(dir $(ASS)) 2>/dev/null || :
	@mkdir -pv $(dir $(INT)) 2>/dev/null || :
	@mkdir -pv $(dir $(OBJ)) 2>/dev/null || :
# }
options:
# {
	@echo "<------------------------------------->"
	@echo ""
	@echo "$(PROJECT) options:"
	@echo "LIBFILE : $(LIBFILE)"
	@echo "LIBNAME : $(LIBNAME)"
	@echo "HOST    : $(HOST)"
	@echo "TARGET  : $(TARGET)"
	@echo "MODE    : $(MODE)"
	@echo "CC      : $(CC)"
	@echo "OUTDIR  : $(OUTDIR)"
	@echo "PREFIX  : $(prefix)"
ifdef DEBUG
	@echo "DEBUG      : ON"
 ifdef MEMCHECK
	@echo "MEMCHECK   : ON"
 else
	@echo "MEMCHECK   : OFF"
 endif
else
	@echo "DEBUG      : OFF"
endif
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
# }

$(OUTDIR)/$(ASSDIR)/%.s: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CPPFLAGS) $($(PROJECT)_CFLAGS) -S $< -o $@
$(OUTDIR)/$(INTDIR)/%.E: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CPPFLAGS) $($(PROJECT)_CFLAGS) -E $< -o $@
$(OUTDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CPPFLAGS) $($(PROJECT)_CFLAGS) -c $< -o $@
$(ASS) $(INT) $(OBJ): $(HDR) Makefile config.mk

$(PROJECT): options setup $(OBJ)
ifeq ($(MODE),SHARED) # {
 ifeq ($(TARGET),UNIX)
	$(CC) -o $(OUTDIR)/$(LIBFILE) $(OBJ) $($(PROJECT)_LDFLAGS)
 else # Windows target
	$(CC) -o $(OUTDIR)/$(LIBFILE) $(OBJ) $($(PROJECT)_LDFLAGS) -Wl,--output-def,$(OUTDIR)/$(WINDEFFILE)
	$(CC) -o $(OUTDIR)/$(LIBFILE) $(OBJ) $($(PROJECT)_LDFLAGS) -Wl,--kill-at
	$(DLLTOOL) --kill-at -d $(OUTDIR)/$(WINDEFFILE) -D $(OUTDIR)/$(TARGETFILE) -l $(OUTDIR)/$(WINIMPFILE)
 endif
else # Static
	$(AR) -crs $(OUTDIR)/$(LIBFILE) $(OBJ)
endif # }
ass: setup options $(ASS) ;
int: setup options $(INT) ;
obj: setup options $(OBJ) ;
aio: setup options $(ASS) $(INT) $(OBJ) ;
clean:
# {
	$(RM) $(ASS)
	$(RM) $(INT)
	$(RM) $(OBJ)
	$(RM) $(OUTDIR)/$(LIBFILE)
ifeq ($(TARGET),WINDOWS)
	$(RM) $(OUTDIR)/$(WINDEFFILE)
	$(RM) $(OUTDIR)/$(WINIMPFILE)
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
endif # }
again: clean all
install: all
ifeq ($(HOST),UNIX) # {
 ifeq ($(TARGET),UNIX)
	mkdir -pv $(prefix)/lib
	cp -fv $(OUTDIR)/$(LIBFILE) $(prefix)/lib/
	mkdir -pv $(prefix)/include/$(PROJECT)
	cp -fv $(HDR) $(prefix)/include/$(PROJECT)
	ldconfig
 else
	@echo Nowhere to install when target is windows on unix host
 endif
else
	@echo NOTICE: Windows installation is manual for now.
endif # }
uninstall:
ifeq ($(HOST),UNIX) # {
 ifeq ($(TARGET),UNIX)
	rm -fv $(prefix)/lib/$(LIBFILE)
	rm -fv $(addprefix $(prefix)/include/$(PROJECT)/,$(patsubst $(HDRDIR)/%,%,$(HDR)))
	ldconfig
 else
 endif
else
 # Nothing installed on windows
endif # }

ifdef CHECK_SRC
CHECK_ASS:=$(addprefix $(OUTDIR)/$(ASSDIR)/,$(patsubst $(SRCDIR)/%,%,$(CHECK_SRC:.c=.s)))
CHECK_INT:=$(addprefix $(OUTDIR)/$(INTDIR)/,$(patsubst $(SRCDIR)/%,%,$(CHECK_SRC:.c=.E)))
CHECK_OBJ:=$(addprefix $(OUTDIR)/$(OBJDIR)/,$(patsubst $(SRCDIR)/%,%,$(CHECK_SRC:.c=.o)))
ifeq ($(TARGET),UNIX) # {
 CHECK_FILE:=$(PROJECT)check
else
 CHECK_FILE:=$(PROJECT)check.exe
endif # }

check-setup:
# {
	@mkdir -pv $(dir $(CHECK_ASS)) 2>/dev/null || :
	@mkdir -pv $(dir $(CHECK_INT)) 2>/dev/null || :
	@mkdir -pv $(dir $(CHECK_OBJ)) 2>/dev/null || :
# }
check-options:
# {
	@echo "<------------------------------------->"
	@echo ""
	@echo "$(CHECK_TARGET) options:"
	@echo "LIBNAME    : $(LIBNAME)"
	@echo "CHECK_FILE : $(CHECK_FILE)"
	@echo "HOST       : $(HOST)"
	@echo "TARGET     : $(TARGET)"
	@echo "MODE       : $(MODE)"
	@echo ""
	@echo "$(PROJECT)_CHECK_CFLAGS   =$($(PROJECT)_CHECK_CFLAGS)"
	@echo "$(PROJECT)_CHECK_CPPFLAGS =$($(PROJECT)_CHECK_CPPFLAGS)"
	@echo "$(PROJECT)_CHECK_LDFLAGS  =$($(PROJECT)_CHECK_LDFLAGS)"
	@echo "CFLAGS             =$(CFLAGS)"
	@echo "CPPFLAGS           =$(CPPFLAGS)"
	@echo "LDFLAGS            =$(LDFLAGS)"
	@echo "CC                 =$(CC)"
	@echo ""
	@echo "<------------------------------------->"
# }

$(OUTDIR)/$(CHKDIR)/$(ASSDIR)/%.s: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CHECK_CPPFLAGS) $($(PROJECT)_CHECK_CFLAGS) -S $< -o $@
$(OUTDIR)/$(CHKDIR)/$(INTDIR)/%.E: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CHECK_CPPFLAGS) $($(PROJECT)_CHECK_CFLAGS) -E $< -o $@
$(OUTDIR)/$(CHKDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $($(PROJECT)_CHECK_CPPFLAGS) $($(PROJECT)_CHECK_CFLAGS) -c $< -o $@
$(CHECK_ASS) $(CHECK_INT) $(CHECK_OBJ): $(CHECK_HDR) Makefile config.mk

check: all check-setup check-options $(CHECK_OBJ)
ifeq ($(MODE),SHARED) # {
	$(CC) -o $(OUTDIR)/$(CHECK_FILE) $(CHECK_OBJ) $($(PROJECT)_CHECK_LDFLAGS) -L$(OUTDIR) -l$(LIBNAME)
else
	$(CC) -o $(OUTDIR)/$(CHECK_FILE) $(CHECK_OBJ) $(OUTDIR)/$(LIBFILE) $($(PROJECT)_CHECK_LDFLAGS)
endif # }
check-ass: setup check-options $(CHECK_ASS)
check-int: setup check-options $(CHECK_INT)
check-obj: setup check-options $(CHECK_OBJ)
check-aio: setup check-options $(CHECK_ASS) $(CHECK_INT) $(CHECK_OBJ)
check-clean:
# {
	$(RM) $(CHECK_ASS)
	$(RM) $(CHECK_INT)
	$(RM) $(CHECK_OBJ)
	$(RM) $(OUTDIR)/$(CHECK_FILE)
ifeq ($(HOST),UNIX)
	find $(OUTDIR)/$(CHKDIR)/$(ASSDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
	find $(OUTDIR)/$(CHKDIR)/$(INTDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
	find $(OUTDIR)/$(CHKDIR)/$(OBJDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
	find $(OUTDIR)/$(CHKDIR) -type d -exec rmdir -v --ignore-fail-on-non-empty {} + 2>/dev/null || :
else
	$(RMDIR) $(OUTDIR)/$(CHKDIR)/$(ASSDIR) 2>$(NULL) || :
	$(RMDIR) $(OUTDIR)/$(CHKDIR)/$(OBJDIR) 2>$(NULL) || :
	$(RMDIR) $(OUTDIR)/$(CHKDIR)/$(INTDIR) 2>$(NULL) || :
	$(RMDIR) $(OUTDIR)/$(CHKDIR) 2>$(NULL) || :
endif # }
check-again: check-clean check ;

.PHONY: check check-options check-ass check-int check-obj check-aio check-clean
.PHONY: check-again
endif

.PHONY: setup options ass int obj aio all clean again install uninstall
