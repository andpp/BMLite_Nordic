# Binary sources
VPATH += $(DEPTH)../hcp/src/

C_SRCS += $(notdir $(wildcard $(DEPTH)../hcp/src/*.c))

C_INC += -I$(DEPTH)../hcp/inc