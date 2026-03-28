.PHONY: app
.SILENT: app

.PHONY: stm32

all: stm32 app

app:
	python3.14 app/app.py

stm32:
	$(MAKE) -C stm32

stm32-flash: stm32
	st-flash write stm32/build/stm32.bin 0x8000000

ocd:
	openocd -f openocd.cfg

gdb:
	arm-none-eabi-gdb stm32/build/stm32.elf
