CC         = g++ -Wall
MAKE       = make -f makefile
APPNAME    = create_scenarios
OBJECTS    = create_scenarios.o ../general_function/timestring.o ../general_function/general_functions.o ../general_function/country.o
CCOPTS     = -c
OPTIMIZE   = -O3
#DEBUG      = -g




INCDIROPTS  = -L/usr/local/lib/libmysqlpp.so -I/usr/include/mysql  -I/usr/local/include/mysql++ 
LNKLIB      = -lmysqlclient -lmysqlpp






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
