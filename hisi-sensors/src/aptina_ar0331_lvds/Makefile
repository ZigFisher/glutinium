
all:
	@rm -rf obj
	@make -f Makefile.mode CMOS_MODE=WDR
	@rm -rf obj
	@make -f Makefile.mode CMOS_MODE=LIN
clean:
	@make clean -f Makefile.mode CMOS_MODE=WDR
	@make clean -f Makefile.mode CMOS_MODE=LIN
	@rm -rf obj
