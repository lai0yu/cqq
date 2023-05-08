CC 				    := gcc
TARGET_S		  	?= bin/cqqserver
TARGET_C      		?= bin/cqqclient

INCDIRS_S 			:= 	src/lib/json_util \
						src/lib/sqlite_util \
						src/app/cqqserver/service/account \
						src/app/cqqserver/main
					

INCDIRS_C			:=	src/lib/json_util \
						src/lib/sqlite_util \
						src/app/cqqclient/main

VPATH           	:= src/lib/json_util \
						src/lib/sqlite_util \
						src/app/cqqserver/service/account \
						src/app/cqqserver/main \
						src/app/cqqclient/main

		   
INCLUDE_S			:= $(patsubst %, -c %, $(INCDIRS_S))
CFILES_S			:= $(foreach dir, $(INCDIRS_S), $(wildcard $(dir)/*.c))
CFILENDIR_S			:= $(notdir  $(CFILES_S))
COBJS_S				:= $(patsubst %, objs/%, $(CFILENDIR_S:.c=.o))
OBJS_S				:= $(COBJS_S)

$(TARGET_S) : $(OBJS_S)
	$(CC) $(COBJS_S) -o $@ -lpthread -lsqlite3 -ljson
	
$(OBJS_S) : objs/%.o : %.c
	$(CC) -c $< -o $@

				   
INCLUDE_C			:= $(patsubst %, -c %, $(INCDIRS_C))
CFILES_C			:= $(foreach dir, $(INCDIRS_C), $(wildcard $(dir)/*.c))
CFILENDIR_C			:= $(notdir  $(CFILES_C))
COBJS_C				:= $(patsubst %, objc/%, $(CFILENDIR_C:.c=.o))
OBJS_C				:= $(COBJS_C)


$(TARGET_C) : $(OBJS_C)
	$(CC) $(COBJS_C) -o $@ -lpthread -lsqlite3 -ljson

$(OBJS_C) : objc/%.o : %.c
	$(CC) -c $< -o $@ 

print_s:
	@echo INCLUDE_S = $(INCLUDE_S)
	@echo CFILES_S = $(CFILES_S)
	@echo CFILENDIR_S = $(CFILENDIR_S)
	@echo COBJS_S = $(COBJS_S)
	@echo OBJS_S = $(OBJS_S)
	@echo $(CC) $(COBJS_S) -o $@ -lpthread -lsqlite3 -ljso
print_c:
	@echo INCLUDE_C = $(INCLUDE_C)
	@echo CFILES_C = $(CFILES_C)
	@echo CFILENDIR_C = $(CFILENDIR_C)
	@echo COBJS_C = $(COBJS_C)
	@echo OBJS_C = $(OBJS_C)

.PHONY: clean
clean:
	rm -rf $(TARGET_S) $(OBJS_S)
	rm -rf $(TARGET_C) $(OBJS_C)

	
