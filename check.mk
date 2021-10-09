CHECK_ASS:=$(addprefix $(OUTDIR)/$(ASSDIR)/,$(patsubst $(SRCROOT)/%,%,$(CHECK_SRC:.c=.s)))
CHECK_INT:=$(addprefix $(OUTDIR)/$(INTDIR)/,$(patsubst $(SRCROOT)/%,%,$(CHECK_SRC:.c=.e)))
CHECK_OBJ:=$(addprefix $(OUTDIR)/$(OBJDIR)/,$(patsubst $(SRCROOT)/%,%,$(CHECK_SRC:.c=.o)))
ifeq ($(TARGET),UNIX)
 CHECK_FILE:=$(PROJECT)check
else
 CHECK_FILE:=$(PROJECT)check.exe
endif

check-setup:
	@mkdir -pv $(dir $(CHECK_ASS)) 2>/dev/null || :
	@mkdir -pv $(dir $(CHECK_INT)) 2>/dev/null || :
	@mkdir -pv $(dir $(CHECK_OBJ)) 2>/dev/null || :
check-options:
	@echo "<------------------------------------->"
	@echo ""
	@echo "$(CHECK_TARGET) options:"
	@echo "LIBNAME    : $(TARGETBASE)"
	@echo "CHECK_FILE : $(CHECK_FILE)"
	@echo "HOST       : $(HOST)"
	@echo "TARGET     : $(TARGET)"
	@echo "MODE       : $(TARGETMODE)"
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

$(OUTDIR)/$(CHKDIR)/$(ASSDIR)/%.s: $(SRCROOT)/%.c
	$(CC) $($(PROJECT)_CHECK_CPPFLAGS) $($(PROJECT)_CHECK_CFLAGS) -S $< -o $@
$(OUTDIR)/$(CHKDIR)/$(INTDIR)/%.E: $(SRCROOT)/%.c
	$(CC) $($(PROJECT)_CHECK_CPPFLAGS) $($(PROJECT)_CHECK_CFLAGS) -E $< -o $@
$(OUTDIR)/$(CHKDIR)/$(OBJDIR)/%.o: $(SRCROOT)/%.c
	$(CC) $($(PROJECT)_CHECK_CPPFLAGS) $($(PROJECT)_CHECK_CFLAGS) -c $< -o $@
$(CHECK_ASS) $(CHECK_INT) $(CHECK_OBJ): $(CHECK_HDR) Makefile config.mk

check: all check-setup check-options $(CHECK_OBJ)
ifeq ($(TARGETMODE),SHARED)
	$(CC) -o $(OUTDIR)/$(CHECK_FILE) $(CHECK_OBJ) $($(PROJECT)_CHECK_LDFLAGS) -L$(OUTDIR) -l$(TARGETBASE)
else
	$(CC) -o $(OUTDIR)/$(CHECK_FILE) $(CHECK_OBJ) $(OUTDIR)/lib$(TARGETNAME) $($(PROJECT)_CHECK_LDFLAGS)
endif
check-ass: setup check-options $(CHECK_ASS)
check-int: setup check-options $(CHECK_INT)
check-obj: setup check-options $(CHECK_OBJ)
check-aio: setup check-options $(CHECK_ASS) $(CHECK_INT) $(CHECK_OBJ)
check-clean:
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
endif
check-again: check-clean check ;

.PHONY: check check-options check-ass check-int check-obj check-aio check-clean
.PHONY: check-again
