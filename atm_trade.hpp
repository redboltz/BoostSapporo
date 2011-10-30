// Copyright 2011 Takatoshi Kondo All rights reserved

#if !defined(ATM_TRADE_HPP)
#define ATM_TRADE_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

#include "atm_withdraw.hpp"
#include "atm_card_detect.hpp"

namespace Atm {
    struct ChooseWithdraw {};
    template <class AuthMethod>
    struct Trade_:msm::front::state_machine_def<Trade_<AuthMethod> >
    {
        struct Choosing:msm::front::state<> 
        {
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "1.Withdraw, 2.N/A, 3.N/A ..." << std::endl;
            }
        };
        struct Withdrawing:Withdraw<AuthMethod> {};
        typedef Choosing initial_state;
        typedef typename Withdrawing::template entry_pt<typename Withdraw_<AuthMethod>::Entry>       WithDrawEntry;
        typedef typename Withdrawing::template entry_pt<typename Withdraw_<AuthMethod>::EntryByCard> WithDrawEntryByCard;
        typedef typename Withdrawing::template exit_pt <typename Withdraw_<AuthMethod>::Exit>        WithDrawExit;
        struct transition_table:mpl::vector<
            //          Start         Event           Next                 Action      Guard
            msmf::Row < Choosing,     ChooseWithdraw, WithDrawEntry,       msmf::none, msmf::none >,
            msmf::Row < Choosing,     CardDetect,     WithDrawEntryByCard, msmf::none, msmf::none >,
            msmf::Row < WithDrawExit, msmf::none,     Choosing,            msmf::none, msmf::none >
        > {};
    };
    template <class AuthMethod>
    struct Trade:msm::back::state_machine<Trade_<AuthMethod> > {};
}

#endif // ATM_TRADE_HPP
