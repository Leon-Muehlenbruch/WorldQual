CC         = g++ -Wall -Wno-unused-but-set-variable
MAKE       = make -f makefile
APPNAME    = rtf_input_corr_factor
OBJECTS    = rtf_input_corr_factor.o ../general_function/timestring.o \
../general_function/country.o \
../general_function/general_functions.o \
../options/options.o options_rtf_input.o 


# ../options/options_scen.o 
CCOPTS     = -c
OPTIMIZE   = -O3
#DEBUG      = -g




#INCDIROPTS  = -L/usr/lib/libmysqlpp.so -I/usr/include/mysql  -I/usr/include/mysql++ -I../general_function -I../options 
INCDIROPTS  = -L/usr/local/lib/libmysqlpp.so -L/usr/lib64/mysql/libmysqlclient.so -I/usr/include/mysql  -I/usr/local/include/mysql++ -I../general_function -I../options 
#LNKLIB      = -lmysqlclient -lmysqlpp
LNKLIB      = -lmysqlpp



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
