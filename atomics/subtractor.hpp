//
// Created by Nirmal on 2021-10-24.
//

#ifndef __SUB_HPP
#define __SUB_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>

#include "../data_structures/signal.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Subtractor_defs{
    struct out : public out_port<Signal_t> { };
    struct in1 : public in_port<Signal_t> { };
    struct in2 : public in_port<Signal_t> { };
};

template<typename TIME>
class Subtractor{
public:
    //Parameters to be overwriten when instantiating the atomic model
    TIME   preparationTime;
    // default constructor
    Subtractor() noexcept{
        preparationTime  = TIME("00:00:00");
        state.active = true;
    }

    // state definition
    struct state_type{
        bool active;
        float intensity;
    };
    state_type state;
    // ports definition
    using input_ports=std::tuple<typename Subtractor_defs::in1, typename Subtractor_defs::in2>;

    using output_ports=std::tuple<typename Subtractor_defs::out>;

    // internal transition
    void internal_transition() {
        state.active = false;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        vector<Signal_t> message_port_in1, message_port_in2;
        message_port_in1 = get_messages<typename Subtractor_defs::in1>(mbs);
        message_port_in2 = get_messages<typename Subtractor_defs::in2>(mbs);
        state.active = true;
        if(message_port_in1.size()>0 && message_port_in2.size()>0)
            state.intensity = message_port_in1[0].intensity - message_port_in2[0].intensity;
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
        Signal_t out_aux;
        out_aux = Signal_t(state.intensity);
        get_messages<typename Subtractor_defs::out>(bags).push_back(out_aux);
        return bags;
    }

    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.active) {
            next_internal = preparationTime;
        }else {
            next_internal = std::numeric_limits<TIME>::infinity();
        }
        return next_internal;
    }

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Subtractor<TIME>::state_type& i) {
        os << "Intensity: " << i.intensity;
        return os;
    }
};

#endif //CHAOTIC_OPTICAL_COMMUNICATION_ADDER_HPP
