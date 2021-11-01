This folder contains the ALTERNATE BIT PROTOCOL DEVS model implemented in Cadmium

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt	
Chaotic_Optical_Communication.pdf
makefile

atomics [This folder contains atomic models implemented in Cadmium]
	adder.hpp
	bias.hpp
	channel.hpp
	gain.hpp
	modulator.hpp
	subtractor.hpp
	synchronizer.hpp
bin [This folder will be created automatically the first time you compile the poject.
     It will contain all the executables]
build [This folder will be created automatically the first time you compile the poject.
       It will contain all the build files (.o) generated during compilation]
data_structures [This folder contains Signal data structure used in the model]
	signal.hpp
	signal.cpp
input_data [This folder contains all the input data to run the model and the tests]
	adder_input1.txt
	adder_input2.txt
	bias_test.txt
	channel_test.txt
	gain.txt
	message_data.txt
	modulator_data.txt
	receiver_input.txt
	subtractor_input1.txt
	subtractor_input2.txt
	synchronizer_test.txt
simulation_results [This folder will be created automatically the first time you compile the poject.
                    It will store the outputs from your simulations and tests]
test [This folder the unit test of the atomic models]
	feedbacl.cpp
	main_adder_test.cpp
	main_bias_test.cpp
	main_channel_test.cpp
	main_gain_test.cpp
	main_modulator_test.cpp
	main_subtractor_test.cpp
	main_synchronizer_test.cpp
	non_linear_system_test.cpp
	receiver.cpp
	transmitter.cpp
top_model [This folder contains the Chaotic optical communication top model]	
	main.cpp
	
/*************/
/****STEPS****/
/*************/

0 - Chaotic_Optical_Communication.pdf contains the explanation of this model

1 - Update include path in the makefile in this folder and subfolders. You need to update the following lines:
	INCLUDECADMIUM=-I ../../cadmium/include
	INCLUDEDESTIMES=-I ../../DESTimes/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
	Do the same for the DESTimes library
    NOTE: if you follow the step by step installation guide you will not need to update these paths.
2 - Compile the project and the tests
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the project folder
	2 - To compile the project and the tests, type in the terminal:
			make clean; make all
3 - Run tests on atomic and coupled models
	1 - Open the terminal in the bin folder. 
	2 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE" (For windows, "./NAME_OF_THE_COMPILED_FILE.exe"). 
	3 - To check the output of the test, go to the folder simulation_results and open  output files
			
4 - Run the top model
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the bin folder.
	3 - To run the model, type in the terminal "./NAME_OF_THE_COMPILED_FILE NAME_OF_THE_INPUT_FILE". For this test you need to type:
		./MAIN_TOP ../input_data/modulator_data.txt ../input_data/message_data.txt(for Windows: ./MAIN_TOP.exe ../input_data/modulator_data.txt ../input_data/message_data.txt)
	4 - To check the output of the model, go to the folder simulation_results and open "Chaotic_Comm_output_messages.txt" and "Chaotic_Comm_output_state.txt"
	5 - To run the model with different inputs
		5.1. Create new .txt files with the same structure as modulator_data.txt and message_data.txt in the folder input_data
		5.2. Run the model using the instructions in step 4.3
		5.3. If you want to keep the output, rename "Chaotic_Comm_output_messages.txt" and "Chaotic_Comm_output_state.txt". Otherwise it will be overwritten when you run the next simulation.

5 - Run all the tests and top model with .sh file
    1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the project folder
    2 - run "chmod u+x run.sh" to make executable shell script
    3 - run "./run.sh" to run all the tests and main model. Check for the results in simulation_results folder.