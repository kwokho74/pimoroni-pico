BSEC_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(BSEC_MOD_DIR)/bsec_wrapper.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(BSEC_MOD_DIR)
LDFLAGS_USERMOD += -L$(BSEC_MOD_DIR)/lib -lalgobsec
