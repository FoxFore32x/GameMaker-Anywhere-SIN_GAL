.PHONY: clean win windows linux

#Windows
win windows:
	$(MAKE) -f platform/windows/makefile

#Linux
linux:
	$(MAKE) -f platform/linux/makefile

clean:
	$(MAKE) -f platform/windows/makefile clean
	$(MAKE) -f platform/linux/makefile clean
