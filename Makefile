CC 				    := gcc
TARGET		  	?= bin/cqqserver
TARGET_C      		?= bin/cqqclient

INCDIRS 			:= 	src/lib/json_util \
						src/lib/sqlite_util \
						src/app/cqqserver/service/account \
						src/app/cqqserver/main
					
VPATH           	:=$(INCDIRS)
		   
INCLUDE			:= $(patsubst %, -c %, $(INCDIRS))
CFILES			:= $(foreach dir, $(INCDIRS), $(wildcard $(dir)/*.c))
CFILENDIR			:= $(notdir  $(CFILES))
COBJS				:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))
OBJS				:= $(COBJS)

$(TARGET) : $(OBJS)
	$(CC) $(COBJS) -o $@ -lpthread -lsqlite3 -ljson
	
$(OBJS) : obj/%.o : %.c
	$(CC) -c $< -o $@


.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)

	
