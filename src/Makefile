# Makefile for Lepton_Data_Sender 

.PHONY: run

run: Lepton_Data_Sender
	./Lepton_Data_Sender $(LEPTON_VISUALIZER_IP)

Lepton_Data_Sender: Lepton_Data_Sender.cpp
	g++ -o Lepton_Data_Sender Lepton_Data_Sender.cpp -L/usr/local/lib/arm-linux-gnueabihf/ -lmraa

clean:
	rm -rf Lepton_Data_Sender
