
.PHONY:clean all rel
all:
	@echo -e "\e[0;32;1m--Compiling 'sensor'... Configs as follow:\e[0;36;1m"
	@echo ---- CROSS=$(CROSS)
	@echo ---- HIARCH=$(HIARCH), HICHIP=$(HICHIP), CVER=$(CVER), HIDBG=$(HIDBG)
	@echo ---- SDK_PATH=$(SDK_PATH) , PARAM_FILE=$(PARAM_FILE)
	@echo ---- LINUX_ROOT=$(LINUX_ROOT)
	@echo -e "\e[0m"
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" `; \
	   do cd `dirname $$x`; if [ $$? ]; then make || exit 1; cd ../; fi; done

clean:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" `; \
	   do cd `dirname $$x`; if [ $$? ]; then make clean; cd ../; fi; done

