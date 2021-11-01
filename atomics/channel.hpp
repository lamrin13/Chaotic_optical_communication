//
// Created by Nirmal on 2021-10-24.
//

#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>

#include "../data_structures/signal.hpp"

#define ATTNUATION 0.9
using namespace cadmium;
using namespace std;

//Port definition
struct Channel_defs{
    struct out : public out_port<Signal_t> { };
    struct in : public in_port<Signal_t> { };
};

template<typename TIME>
class Channel{
public:
    //Parameters to be overwriten when instantiating the atomic model
    TIME   preparationTime;
    // default constructor
    Channel() noexcept{
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
    using input_ports=std::tuple<typename Channel_defs::in>;
    using output_ports=std::tuple<typename Channel_defs::out>;

    // internal transition
    void internal_transition() {
        state.active = false;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        if(get_messages<typename Channel_defs::in>(mbs).size()>1)
            assert(false && "one message per time unit");
        vector<Signal_t> message_port_in;
        message_port_in = get_messages<typename Channel_defs::in>(mbs);
        state.active = true;
        state.intensity =  ATTNUATION * message_port_in[0].intensity;
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
        get_messages<typename Channel_defs::out>(bags).push_back(out_aux);
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

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Channel<TIME>::state_type& i) {
        os << "Intensity: " << i.intensity;
        return os;
    }
};


#endif // __CHANNEL_HPP__