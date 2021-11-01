//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/signal.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/modulator.hpp"
#include "../atomics/bias.hpp"
#include "../atomics/gain.hpp"
#include "../atomics/adder.hpp"
#include "../atomics/subtractor.hpp"
#include "../atomics/synchronizer.hpp"
#include "../atomics/channel.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct input_tx : public in_port<Signal_t>{};
struct input_rx : public in_port<Signal_t>{};
struct input_gain : public in_port<Signal_t>{};
struct input_gain_r : public in_port<Signal_t>{};
struct input_nls : public in_port<Signal_t>{};
struct input_nls_r : public in_port<Signal_t>{};
struct input_m : public in_port<Signal_t>{};

/***** Define output ports for coupled model *****/
struct output_gain : public out_port<Signal_t>{};
struct output_gain_r : public out_port<Signal_t>{};
struct output_nls : public out_port<Signal_t>{};
struct output_nls_r : public out_port<Signal_t>{};
struct output_tx : public out_port<Signal_t>{};
struct output_rx : public out_port<Signal_t>{};
struct output_top : public out_port<Signal_t>{};
struct output_receiver : public out_port<Signal_t>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<Signal_t,T> {
public:
    InputReader_Message_t () = default;
    InputReader_Message_t (const char* file_path) : iestream_input<Signal_t,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 3) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << " path to message signal file " << endl;
        return 1;
    }

    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model
            <InputReader_Message_t, TIME, const char* >("input_reader" , std::move(i_input));

    string message = argv[2];
    const char * message_input = message.c_str();
    shared_ptr<dynamic::modeling::model> message_reader = dynamic::translate::make_dynamic_atomic_model
            <InputReader_Message_t, TIME, const char* >("message_reader", std::move(message_input));


    /****** Gain atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> gain1 = dynamic::translate::make_dynamic_atomic_model<Gain, TIME>("gain1");

    /****** Bias atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> bias1 = dynamic::translate::make_dynamic_atomic_model<Bias, TIME>("bias1");

    /****** EO modulator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> modulator1 = dynamic::translate::make_dynamic_atomic_model<Modulator, TIME>("modulator1");

    /****** Adder automic model instantiation ****************/
    shared_ptr<dynamic::modeling::model> adder = dynamic::translate::make_dynamic_atomic_model<Adder, TIME>("adder");

    /****** Channel atomic model instantiation **************/
    shared_ptr<dynamic::modeling::model> channel = dynamic::translate::make_dynamic_atomic_model<Channel, TIME>("channel");

    /****** Gain atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> gain2 = dynamic::translate::make_dynamic_atomic_model<Gain, TIME>("gain2");

    /****** Bias atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> bias2 = dynamic::translate::make_dynamic_atomic_model<Bias, TIME>("bias2");

    /****** EO modulator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> modulator2 = dynamic::translate::make_dynamic_atomic_model<Modulator, TIME>("modulator2");

    /****** SYNC atomic model instantiation *****************/
    shared_ptr<dynamic::modeling::model> sync = dynamic::translate::make_dynamic_atomic_model<Synchronizer, TIME>("sync");

    /****** Subtractor automic model instantiation ****************/
    shared_ptr<dynamic::modeling::model> subtractor = dynamic::translate::make_dynamic_atomic_model<Subtractor, TIME>("subtractor");

    /************************************************
     * Transmitter coupled models
     ***********************************************/
    /******* Gain-Bias circuit COUPLED MODEL********/
    dynamic::modeling::Ports iports_FB = {typeid(input_gain)};
    dynamic::modeling::Ports oports_FB = {typeid(output_gain)};
    dynamic::modeling::Models submodels_FB = {gain1, bias1};
    dynamic::modeling::EICs eics_FB = {
            cadmium::dynamic::translate::make_EIC<input_gain, Gain_defs::in>("gain1")
    };
    dynamic::modeling::EOCs eocs_FB = {
            dynamic::translate::make_EOC<Bias_defs::out,output_gain>("bias1")
    };
    dynamic::modeling::ICs ics_FB = {
            dynamic::translate::make_IC<Gain_defs::out, Bias_defs::in>("gain1","bias1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> FEEDBACK;
    FEEDBACK = make_shared<dynamic::modeling::coupled<TIME>>(
            "FEEDBACK", submodels_FB, iports_FB, oports_FB, eics_FB, eocs_FB, ics_FB
    );

    /******* Non-Linear System COUPLED MODEL********/
    dynamic::modeling::Ports iport_NLS = {typeid(input_nls)};
    dynamic::modeling::Ports oport_NLS = {typeid(output_nls)};
    dynamic::modeling::Models submodel_NLS = {FEEDBACK, modulator1};
    dynamic::modeling::EICs eics_NLS = {
            dynamic::translate::make_EIC<input_nls,Modulator_defs::in>("modulator1")
    };
    dynamic::modeling::EOCs eocs_NLS = {
            dynamic::translate::make_EOC<Modulator_defs::out,output_nls>("modulator1")
    };
    dynamic::modeling::ICs ics_NLS = {
            dynamic::translate::make_IC<Modulator_defs::out,input_gain>("modulator1","FEEDBACK"),
            dynamic::translate::make_IC<output_gain, Modulator_defs::in>("FEEDBACK","modulator1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> NLS;
    NLS = make_shared<dynamic::modeling::coupled<TIME>>(
            "NLS", submodel_NLS, iport_NLS, oport_NLS, eics_NLS, eocs_NLS, ics_NLS
    );

    /******* Transmitter COUPLED MODEL********/
    dynamic::modeling::Ports iport_tx = {typeid(input_tx),typeid(input_m)};
    dynamic::modeling::Ports oport_tx = {typeid(output_tx)};
    dynamic::modeling::Models submodel_tx = { NLS,adder};
    dynamic::modeling::EICs eics_tx = {
            dynamic::translate::make_EIC<input_tx,input_nls>("NLS"),
            dynamic::translate::make_EIC<input_m,Adder_defs::in1>("adder")
    };
    dynamic::modeling::EOCs eocs_tx = {
            dynamic::translate::make_EOC<Adder_defs::out,output_tx>("adder")
    };
    dynamic::modeling::ICs ics_tx = {
            dynamic::translate::make_IC<output_nls, Adder_defs::in2>("NLS","adder")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TRANSMITTER;
    TRANSMITTER = make_shared<dynamic::modeling::coupled<TIME>>(
            "TRANSMITTER", submodel_tx, iport_tx, oport_tx, eics_tx, eocs_tx, ics_tx
    );

    /***********************************************
     * Receiver coupled models
     *********************************************/
    /******* Gain-Bias circuit COUPLED MODEL********/
    dynamic::modeling::Ports iports_FBR = {typeid(input_gain_r)};
    dynamic::modeling::Ports oports_FBR = {typeid(output_gain_r)};
    dynamic::modeling::Models submodels_FBR = {gain2, bias2};
    dynamic::modeling::EICs eics_FBR = {
            cadmium::dynamic::translate::make_EIC<input_gain_r, Gain_defs::in>("gain2")
    };
    dynamic::modeling::EOCs eocs_FBR = {
            dynamic::translate::make_EOC<Bias_defs::out,output_gain_r>("bias2")
    };
    dynamic::modeling::ICs ics_FBR = {
            dynamic::translate::make_IC<Gain_defs::out, Bias_defs::in>("gain2","bias2")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> RECEIVER_FEEDBACK;
    RECEIVER_FEEDBACK = make_shared<dynamic::modeling::coupled<TIME>>(
            "RECEIVER_FEEDBACK", submodels_FBR, iports_FBR, oports_FBR, eics_FBR, eocs_FBR, ics_FBR
    );

    /******* Non-Linear System COUPLED MODEL********/
    dynamic::modeling::Ports iport_NLSR = {typeid(input_nls_r)};
    dynamic::modeling::Ports oport_NLSR = {typeid(output_nls_r)};
    dynamic::modeling::Models submodel_NLSR = {RECEIVER_FEEDBACK, modulator2};
    dynamic::modeling::EICs eics_NLSR = {
            dynamic::translate::make_EIC<input_nls_r,Modulator_defs::in>("modulator2")
    };
    dynamic::modeling::EOCs eocs_NLSR = {
            dynamic::translate::make_EOC<Modulator_defs::out,output_nls_r>("modulator2")
    };
    dynamic::modeling::ICs ics_NLSR = {
            dynamic::translate::make_IC<Modulator_defs::out,input_gain_r>("modulator2","RECEIVER_FEEDBACK"),
            dynamic::translate::make_IC<output_gain_r, Modulator_defs::in>("RECEIVER_FEEDBACK","modulator2")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> RECEIVER_NLS;
    RECEIVER_NLS = make_shared<dynamic::modeling::coupled<TIME>>(
            "RECEIVER_NLS", submodel_NLSR, iport_NLSR, oport_NLSR, eics_NLSR, eocs_NLSR, ics_NLSR
    );

    /******* Receiver COUPLED MODEL********/
    dynamic::modeling::Ports iport_rx = {typeid(input_rx)};
    dynamic::modeling::Ports oport_rx = {typeid(output_rx)};
    dynamic::modeling::Models submodel_rx = {RECEIVER_NLS, subtractor, sync};
    dynamic::modeling::EICs eics_rx = {
            dynamic::translate::make_EIC<input_rx,input_nls_r>("RECEIVER_NLS"),
            dynamic::translate::make_EIC<input_rx,Subtractor_defs::in1>("subtractor")
    };
    dynamic::modeling::EOCs eocs_rx = {
            dynamic::translate::make_EOC<Subtractor_defs::out,output_rx>("subtractor")
    };
    dynamic::modeling::ICs ics_rx = {
            dynamic::translate::make_IC<output_nls_r, Synchronizer_defs::in>("RECEIVER_NLS","sync"),
            dynamic::translate::make_IC<Synchronizer_defs::out, Subtractor_defs::in2>("sync","subtractor")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> RECEIVER;
    RECEIVER = make_shared<dynamic::modeling::coupled<TIME>>(
            "RECEIVER", submodel_rx, iport_rx, oport_rx, eics_rx, eocs_rx, ics_rx
    );

    /******* TOP Circuit COUPLED MODEL********/
    dynamic::modeling::Ports iport = {};
    dynamic::modeling::Ports oport = {typeid(output_top)};
    dynamic::modeling::Models submodel = {input_reader, message_reader, TRANSMITTER, channel, RECEIVER};
    dynamic::modeling::EICs eics = {};
    dynamic::modeling::EOCs eocs = {
            dynamic::translate::make_EOC<output_rx,output_top>("RECEIVER")
    };
    dynamic::modeling::ICs ics = {
            dynamic::translate::make_IC<iestream_input_defs<Signal_t>::out,input_tx>("input_reader","TRANSMITTER"),
            dynamic::translate::make_IC<iestream_input_defs<Signal_t>::out,input_m>("message_reader","TRANSMITTER"),
            dynamic::translate::make_IC<output_tx,Channel_defs::in>("TRANSMITTER","channel"),
            dynamic::translate::make_IC<Channel_defs::out,input_rx>("channel","RECEIVER"),
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
            "TOP", submodel, iport, oport, eics, eocs, ics
    );


    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/Chaotic_Comm_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/Chaotic_Comm_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("00:05:00:000"));
    return 0;
}