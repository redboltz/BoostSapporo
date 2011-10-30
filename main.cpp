// Copyright 2011 Takatoshi Kondo All rights reserved

#include "atm_all.hpp"
#include "atm_bio_auth.hpp"

int main()
{
    Atm::All<Atm::BioAuth> t;
    t.start();
    t.process_event(Atm::HumanDetect());
    t.process_event(Atm::ChooseWithdraw());
    t.process_event(Atm::CardDetect());
    t.process_event(Atm::FingerDetect());
    t.process_event(Atm::AuthSuccess(Atm::AccountInfo(100)));
    t.process_event(Atm::EnterAmount(50));
    t.process_event(Atm::Ok());
    t.process_event(Atm::Ok());
}
