//
// Created by Nirmal on 2021-10-24.
//

#ifndef __MOD_HPP__
#define __MOD_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <math.h>

#include "../data_structures/signal.hpp"

#define IN_BEAM 10
using namespace cadmium;
using namespace std;

//Port definition
struct Modulator_defs{
    struct out : public out_port<Signal_t> { };
    struct in : public in_port<Signal_t> { };
    struct trigger : public in_port<Signal_t>{ };
};

template<typename TIME>
class Modulator{
    public:
    //Parameters to be overwriten when instantiating the atomic model
        TIME   preparationTime;
        // default constructor
        Modulator() noexcept{
            preparationTime  = TIME("00:00:10");
            state.active = true;
        }

        // state definition
        struct state_type{
            bool active;
            float intensity;
        };
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename Modulator_defs::in, typename Modulator_defs::trigger>;
        using output_ports=std::tuple<typename Modulator_defs::out>;

        // internal transition
        void internal_transition() {
            state.active = false;
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            vector<Signal_t> message_port_in;
            message_port_in = get_messages<typename Modulator_defs::in>(mbs);
            state.active = true;
            float cosin = cos(message_port_in[0].intensity);
            state.intensity = IN_BEAM * pow(cosin, 2);
            message_port_in.erase(message_port_in.begin());
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
            get_messages<typename Modulator_defs::out>(bags).push_back(out_aux);
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

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Modulator<TIME>::state_type& i) {
            os << "Intensity: " << i.intensity;
            return os;
        }
};


#endif // __RECEIVER_HPP__