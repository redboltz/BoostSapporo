// Copyright 2011 Takatoshi Kondo All rights reserved

#if !defined(ATM_ACCOUNT_INFO_HPP)
#define ATM_ACCOUNT_INFO_HPP

namespace Atm {
    struct AccountInfo {
        AccountInfo(int amount_):balance(amount_) {}
        int balance;
    };
}

#endif // ATM_ACCOUNT_INFO_HPP
