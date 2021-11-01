CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
signal.o: data_structures/signal.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/signal.cpp -o build/signal.o

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o

main_gain_test.o: test/main_gain_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_gain_test.cpp -o build/main_gain_test.o

main_bias_test.o: test/main_bias_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_bias_test.cpp -o build/main_bias_test.o

main_channel_test.o: test/main_channel_test.cpp
	$(CC) -g -c $(CFLAG) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_channel_test.cpp -o build/main_channel_test.o

main_synchronizer_test.o: test/main_synchronizer_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_synchronizer_test.cpp -o build/main_synchronizer_test.o

main_modulator_test.o: test/main_modulator_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_modulator_test.cpp -o build/main_modulator_test.o

main_modulator_receiver_test.o: test/main_modulator_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_modulator_receiver_test.cpp -o build/main_modulator_receiver_test.o

main_adder_test.o: test/main_adder_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_adder_test.cpp -o build/main_adder_test.o

main_subtractor_test.o: test/main_subtractor_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_subtractor_test.cpp -o build/main_subtractor_test.o

feedback.o: test/feedback.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/feedback.cpp -o build/feedback.o

non_linear_system_test.o: test/non_linear_system_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/non_linear_system_test.cpp -o build/non_linear_system_test.o

transmitter.o: test/transmitter.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/transmitter.cpp -o build/transmitter.o

receiver.o: test/receiver.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/receiver.cpp -o build/receiver.o

simulation: signal.o main_top.o
	$(CC) -g -o bin/MAIN_TOP build/main_top.o build/signal.o

tests: signal.o feedback.o main_channel_test.o main_synchronizer_test.o main_adder_test.o main_modulator_test.o main_bias_test.o main_gain_test.o main_subtractor_test.o transmitter.o receiver.o non_linear_system_test.o
	$(CC) -g -o bin/ADDER_TEST build/main_adder_test.o build/signal.o
	$(CC) -g -o bin/BIAS_TEST build/main_bias_test.o build/signal.o
	$(CC) -g -o bin/CHANNEL_TEST build/main_channel_test.o build/signal.o
	$(CC) -g -o bin/Gain_TEST build/main_gain_test.o build/signal.o
	$(CC) -g -o bin/FEEDBACK_TEST build/feedback.o build/signal.o
	$(CC) -g -o bin/SUBTRACTOR_TEST build/main_subtractor_test.o build/signal.o
	$(CC) -g -o bin/MODULATOR_TEST build/main_modulator_test.o build/signal.o
	$(CC) -g -o bin/RECEIVER_TEST build/receiver.o build/signal.o
	$(CC) -g -o bin/TRANSMITTER_TEST build/transmitter.o build/signal.o
	$(CC) -g -o bin/SYNCHRONIZER_TEST build/main_synchronizer_test.o build/signal.o
	$(CC) -g -o bin/NLS_TEST build/non_linear_system_test.o build/signal.o

all: simulation tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*