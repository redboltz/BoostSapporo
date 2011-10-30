// Copyright 2011 Takatoshi Kondo All rights reserved

#if !defined(ATM_WITHDRAW_HPP)
#define ATM_WITHDRAW_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

#include "atm_account_info.hpp"
#include "atm_card_detect.hpp"

namespace Atm {
    namespace msm = boost::msm;
    namespace msmf = boost::msm::front;
    namespace mpl = boost::mpl;

    // ----- Events
    struct Cancel {};
    struct Ok {};
    struct EnterAmount {
        EnterAmount(int amount_):amount(amount_) {}
        int amount;
    };

    // ----- State machine
    template <class AuthMethod>
    struct Withdraw_:msm::front::state_machine_def<Withdraw_<AuthMethod> >
    {
        // States
        struct Entry      :msm::front::entry_pseudo_state<> {};
        struct EntryByCard:msm::front::entry_pseudo_state<> {};
        struct Exit       :msm::front::exit_pseudo_state<msmf::none> {};
        struct WaitingCard:msm::front::state<> 
        {
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter WaitingCard" << std::endl;
                std::cout << "Please insert your card" << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  WaitingCard" << std::endl;
            }
        };
        struct WithdrawAuth:AuthMethod
        {
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter WithdrawAuth" << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  WithdrawAuth" << std::endl;
            }
        };
        struct EnteringAmount:msm::front::state<> 
        {
            // Entry action
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter EnteringAmount" << std::endl;
                std::cout << "Input amount of money" << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  EnteringAmount" << std::endl;
            }
            EnteringAmount():amount(0), info(0) {}
            int amount;
            AccountInfo info;
        };
        struct InsufficientFunds:msm::front::state<> 
        {
            // Entry action
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter InsufficientFunds" << std::endl;
                std::cout << "Insufficient Funds" << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  InsufficientFunds" << std::endl;
            }
        };
        struct DisplayingBalance:msm::front::state<> 
        {
            // Entry action
            template <class Event,class Fsm>
            void on_entry(Event const&, Fsm&) const {
                std::cout << "[DBG] Enter DisplayingBalance" << std::endl;
                std::cout << "Your balance:" << balance << std::endl;
            }
            template <class Event,class Fsm>
            void on_exit(Event const&, Fsm&) const {
                std::cout << "[DBG] Exit  DisplayingBalance" << std::endl;
            }
            void setBalance(int balance_) {
                balance = balance_;
            }
            int balance;
        };
        // Guards
        struct CheckAmount {
            template <class Event, class Fsm, class SourceState, class TargetState>
            bool operator()(Event const&, Fsm&, SourceState& s, TargetState&) const 
            {
                return s.amount <= s.info.balance;
            }
        };
        // Actions
        struct SetAccountInfo {
            template <class Event, class Fsm, class SourceState, class TargetState>
            void operator()(Event const& e, Fsm&, SourceState&, TargetState& t) const 
            {
                t.info = e;
            }
        };
        struct SetAmount {
            template <class Event, class Fsm, class SourceState, class TargetState>
            void operator()(Event const& e, Fsm&, SourceState&, TargetState& t) const 
            {
                t.amount = e.amount;
            }
        };
        struct Pay {
            template <class Event, class Fsm, class SourceState, class TargetState>
            void operator()(Event const&, Fsm&, SourceState& s, TargetState& t) const 
            {
                t.setBalance(s.info.balance - s.amount);
            }
        };

        // Set initial state
        typedef Entry initial_state;

        typedef typename WithdrawAuth::template entry_pt<typename AuthMethod::Derived::Entry>       AuthEntry;
        typedef typename WithdrawAuth::template exit_pt <typename AuthMethod::Derived::ExitFail>    AuthExitFail;
        typedef typename WithdrawAuth::template exit_pt <typename AuthMethod::Derived::ExitSuccess> AuthExitSuccess;
        // Transition table
        struct transition_table:mpl::vector<
            //          Start              Event        Next               Action          Guard
            msmf::Row < Entry,             msmf::none,  WaitingCard,       msmf::none,     msmf::none  >,
            msmf::Row < EntryByCard,       msmf::none,  AuthEntry,         msmf::none,     msmf::none  >,
            msmf::Row < WaitingCard,       CardDetect,  AuthEntry,         msmf::none,     msmf::none  >,
            msmf::Row < AuthExitFail,      msmf::none,  Exit,              msmf::none,     msmf::none  >,
            msmf::Row < AuthExitSuccess,   AccountInfo, EnteringAmount,    SetAccountInfo, msmf::none  >,
            msmf::Row < EnteringAmount,    Ok,          InsufficientFunds, msmf::none,     msmf::none  >, /*else*/
            msmf::Row < EnteringAmount,    Ok,          DisplayingBalance, Pay,            CheckAmount >,  
            msmf::Row < EnteringAmount,    EnterAmount, msmf::none,        SetAmount,      msmf::none  >, 
            msmf::Row < InsufficientFunds, Ok,          EnteringAmount,    msmf::none,     msmf::none  >, 
            msmf::Row < DisplayingBalance, Ok,          Exit,              msmf::none,     msmf::none  > 
        > {};
    };

    // Pick a back-end
    template <class AuthMethod>
    struct Withdraw:msm::back::state_machine<Withdraw_<AuthMethod> > {};
}

#endif // ATM_WITHDRAW_HPP
