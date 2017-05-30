# Makefile (starter) for Mission Incomputable

MAKE = make

# These targets do not produce real "targets"
.PHONY: all clean start-proxy stop-proxy

############## default: make all libs and programs ##########

all: 
	@echo "Making everything..."
	$(MAKE) -C field-agent
	$(MAKE) -C guide-agent
	$(MAKE) -C game-server

########################### proxy ###########################

# Start the proxy running - add flags as desired. Options: -all, -fa2gs, -gs2fa
start-proxy: proxy/proxy
	@cd proxy; node proxy -all

# Kill any running instances of the proxy.
stop-proxy:
	@pkill -f 'node proxy'

# Use npm to install all proxy dependencies (only need to do this initially).
install-proxy-deps:
	@cd proxy; npm install

########################### proxy ###########################

clean:
	@echo "Cleaning everything..."
	$(MAKE) -C field-agent clean
	$(MAKE) -C guide-agent clean
	$(MAKE) -C game-server clean
	rm -f *~ *.o
