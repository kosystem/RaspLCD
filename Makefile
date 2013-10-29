build:
	g++ -o raspLCD main.cpp Lcd22inch.cpp Lcd22inch.h -l bcm2835
run:
	sudo ./raspLCD
clean:
	rm -f raspLCD

