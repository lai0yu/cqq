CC 				    := gcc
TARGET		  		?= cqqserver

INCDIRS 			:= 	src/lib/json_util \
						src/lib/sqlite_util \
						src/service/account \
						src/service/friend \
						src/service/friend_chat\
						src/main
					
VPATH           	:=$(INCDIRS)
		   
INCLUDE				:= $(patsubst %, -c %, $(INCDIRS))
CFILES				:= $(foreach dir, $(INCDIRS), $(wildcard $(dir)/*.c))
CFILENDIR			:= $(notdir  $(CFILES))
COBJS				:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))
OBJS				:= $(COBJS)

$(TARGET) : $(OBJS)
	$(CC) $(COBJS) -o $@ -lpthread -ljson -lsqlite3 -luuid -g
	
$(OBJS) : obj/%.o : %.c
	$(CC) -c $< -o $@ -g


.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)

	
