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
#include "../atomics/gain.hpp"
#include "../atomics/bias.hpp"
#include "../atomics/modulator.hpp"
#include "../atomics/adder.hpp"
#include "../atomics/subtractor.hpp"


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
struct input_gain : public in_port<Signal_t>{};

/***** Define output ports for coupled model *****/
struct output : public out_port<Signal_t>{};
struct outp : public out_port<Signal_t>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<Signal_t,T> {
public:
    InputReader_Message_t () = default;
    InputReader_Message_t (const char* file_path) : iestream_input<Signal_t,T>(file_path) {}
};

int main(int argc, char ** argv) {

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input = "../input_data/modulator_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model
            <InputReader_Message_t, TIME, const char* >("input_reader" , std::move(i_input));


    /****** Gain atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> gain1 = dynamic::translate::make_dynamic_atomic_model<Gain, TIME>("gain1");

    /****** Bias atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> bias1 = dynamic::translate::make_dynamic_atomic_model<Bias, TIME>("bias1");

    /****** EO modulator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> modulator = dynamic::translate::make_dynamic_atomic_model<Modulator, TIME>("modulator");



    /******* Gain-Bias circuit COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {typeid(input_gain)};
    dynamic::modeling::Ports oports_TOP = {typeid(outp)};
    dynamic::modeling::Models submodels_TOP = {gain1, bias1};
    dynamic::modeling::EICs eics_TOP = {
            cadmium::dynamic::translate::make_EIC<input_gain, Gain_defs::in>("gain1")
    };
    dynamic::modeling::EOCs eocs_TOP = {
            dynamic::translate::make_EOC<Bias_defs::out,outp>("bias1")
    };
    dynamic::modeling::ICs ics_TOP = {
            dynamic::translate::make_IC<Gain_defs::out, Bias_defs::in>("gain1","bias1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> FEEDBACK;
    FEEDBACK = make_shared<dynamic::modeling::coupled<TIME>>(
            "FEEDBACK", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /******* Non-Linear System COUPLED MODEL********/
    dynamic::modeling::Ports iport = {};
    dynamic::modeling::Ports oport = {typeid(output)};
    dynamic::modeling::Models submodel = {input_reader, FEEDBACK, modulator};
    dynamic::modeling::EICs eics = {};
    dynamic::modeling::EOCs eocs = {
            dynamic::translate::make_EOC<Modulator_defs::out,output>("modulator")
    };
    dynamic::modeling::ICs ics = {
            dynamic::translate::make_IC<iestream_input_defs<Signal_t>::out,Modulator_defs::in>("input_reader","modulator"),
            dynamic::translate::make_IC<Modulator_defs::out,input_gain>("modulator","FEEDBACK"),
            dynamic::translate::make_IC<outp, Modulator_defs::in>("FEEDBACK","modulator")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> NLS;
    NLS = make_shared<dynamic::modeling::coupled<TIME>>(
            "NLS", submodel, iport, oport, eics, eocs, ics
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/nonlinear_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/nonlinear_output_state.txt");
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
    dynamic::engine::runner<NDTime, logger_top> r(NLS, TIME("00:00:00"));
    r.run_until(NDTime("00:01:30:000"));

    return 0;
}