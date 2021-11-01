#!/usr/bin/env bash

cd bin
./ADDER_TEST
./BIAS_TEST
./CHANNEL_TEST
./Gain_TEST
./FEEDBACK_TEST
./SUBTRACTOR_TEST
./MODULATOR_TEST
./RECEIVER_TEST
./TRANSMITTER_TEST
./SYNCHRONIZER_TEST
./NLS_TEST

./MAIN_TOP ../input_data/modulator_test.txt ../input_data/message_data.txt