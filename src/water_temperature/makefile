CC         = g++ -Wall -Wno-unused-but-set-variable
MAKE       = make -f makefile
APPNAME    = water_temperature
OBJECTS    = water_temperature.o ../general_function/timestring.o \
../general_function/general_functions.o \
../options/options.o options_waterTemp.o 
CCOPTS     = -c
OPTIMIZE   = -O3
#DEBUG      = -g



#INCDIROPTS  = -L/usr/lib/libmysqlpp.so -I/usr/include/mysql  -I/usr/include/mysql++ -I../general_function -I../options 
#LNKLIB      = -lmysqlclient -lmysqlpp
INCDIROPTS  = -L/usr/local/lib/libmysqlpp.so -L/usr/lib64/mysql -I/usr/include/mysql  -I/usr/local/include/mysql++ -I../general_function -I../options 
LNKLIB      = -lmysqlpp -lmysqlclient






$(APPNAME): $(OBJECTS)
	$(CC) $(INCDIROPTS) $(LNKOPTS) -o $(APPNAME) $(OBJECTS) $(LNKLIB) 

.cpp.o: 
	$(CC) -c $(INCDIROPTS) $(DEBUG) -o $*.o $*.cpp 

all:	clean 
	@$(MAKE) $(APPNAME)
	echo all: make complete

clean:
	@echo -n "Make clean in all Subdirectories ... "; \
	  rm -f $(OBJECTS) $(APPNAME); \
	  rm -f *~ *% core a.out "#"*"#"; \
	  for DIR in `find . -type d -print` ; do \
	    rm -f $$DIR/*~ $$DIR/core $$DIR/a.out $$DIR/"#"*"#"; \
	  done; \
	  echo Done.
