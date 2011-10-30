// Copyright 2011 Takatoshi Kondo All rights reserved

#if !defined(ATM_ALL_HPP)
#define ATM_ALL_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

#include "atm_trade.hpp"

namespace Atm {
    namespace msm = boost::msm;
    namespace msmf = boost::msm::front;
    namespace mpl = boost::mpl;

    // ----- Events
    struct HumanDetect {};
    struct HumanAway {};

    // ----- State machine
    template <class AuthMethod>
    struct All_:msm::front::state_machine_def<All_<AuthMethod> >
    {
        // States
        struct Waiting:msm::front::state<> 
        {
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter Waiting" << std::endl;
                std::cout << "Clear Screen" << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  Waiting" << std::endl;
            }
        };
        struct InService:Trade<AuthMethod> {
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter InService" << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  InService" << std::endl;
            }
        };

        // Set initial state
        typedef Waiting initial_state;

        // Transition table
        struct transition_table:mpl::vector<
            //          Start      Event        Next       Action      Guard
            msmf::Row < Waiting,   HumanDetect, InService, msmf::none, msmf::none >,
            msmf::Row < InService, HumanAway,   Waiting,   msmf::none, msmf::none >
        > {};
    };

    // Pick a back-end
    template <class AuthMethod>
    struct All:msm::back::state_machine<All_<AuthMethod> > {};
}

#endif // ATM_ALL_HPP
