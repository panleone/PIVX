#!/usr/bin/env python3
# Copyright (c) 2017 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test BIP39 seedphrase feature."""

import os

from test_framework.test_framework import PivxTestFramework
from test_framework.util import (
    assert_equal,
)


class WalletBip39Test(PivxTestFramework):
    def generate_and_fund_addresses(self, fShield, fFund, seedid, count):
        gen_addresses = []
        add = None
        for i in range(count):
            add = (
                self.nodes[0].getnewshieldaddress()
                if fShield
                else self.nodes[0].getnewaddress()
            )
            gen_addresses.append(add)
            add_info = self.nodes[0].getaddressinfo(add)
            assert_equal(add_info["hdseedid"], seedid)
            if fFund:
                self.nodes[1].sendtoaddress(add, 1)
                self.nodes[1].generate(1)
        return gen_addresses

    def generate_and_fund_transparent_addr(self, seedid, count):
        return self.generate_and_fund_addresses(False, True, seedid, count)

    def generate_and_fund_shield_addr(self, seedid, count):
        return self.generate_and_fund_addresses(True, True, seedid, count)

    def generate_transparent_addr(self, seedid, count):
        return self.generate_and_fund_addresses(False, False, seedid, count)

    def generate_shield_addr(self, seedid, count):
        return self.generate_and_fund_addresses(True, False, seedid, count)

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 2
        self.extra_args = [
            [
                "-seedphrase=abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about",
                "-nuparams=v5_shield:1",
            ],
            ["-nuparams=v5_shield:1"],
        ]

    def run_test(self):
        # Make sure we use hd
        if "-legacywallet" in self.nodes[0].extra_args:
            self.log.info("Exiting HD test for non-HD wallets")
            return

        masterkeyid = self.nodes[0].getwalletinfo()["hdseedid"]

        # Verify that the masterkeyid is generated correctly
        assert_equal(masterkeyid, "4842c1950b135ae8d53514361917b9157a4b43b9")

        gen_transparent_addresses = []
        gen_shield_addresses = []

        NUM_HD_ADDS = 5
        NUM_SHIELD_ADDS = 5

        # Generate some addresses and fund them
        self.log.info("Testing transparent transactions ...")
        self.nodes[1].generate(101)
        gen_transparent_addresses = self.generate_and_fund_transparent_addr(
            masterkeyid, NUM_HD_ADDS
        )
        assert_equal(len(gen_transparent_addresses), NUM_HD_ADDS)

        # Generate some shield addresses and fund them
        self.log.info("Testing shield transactions ...")
        gen_shield_addresses = self.generate_and_fund_shield_addr(
            masterkeyid, NUM_SHIELD_ADDS
        )
        assert_equal(len(gen_shield_addresses), NUM_SHIELD_ADDS)

        # Let's generate more blocks to confirm the total balance
        self.nodes[1].generate(101)
        assert_equal(
            self.nodes[0].getwalletinfo()["balance"], NUM_SHIELD_ADDS + NUM_HD_ADDS
        )

        # Let's see if we can backup simply from the seed phrase
        self.log.info("Removing backup files ...")
        self.stop_node(0)
        os.remove(
            os.path.join(self.nodes[0].datadir, "regtest", "wallets", "wallet.dat")
        )
        self.start_node(0, extra_args=self.extra_args[0])

        # Restore addresses
        self.log.info("Restore from seed phrase ...")
        new_gen_transparent_addresses = self.generate_transparent_addr(
            masterkeyid, NUM_HD_ADDS
        )
        new_gen_shield_addresses = self.generate_shield_addr(
            masterkeyid, NUM_SHIELD_ADDS
        )
        assert_equal(gen_shield_addresses, new_gen_shield_addresses)
        assert_equal(gen_transparent_addresses, new_gen_transparent_addresses)

        # Restart, zap, and check balance: 1 PIV * (NUM_HD_ADDS + NUM_SHIELD_ADDS) recovered from seed phrase
        self.stop_node(0)
        self.start_node(0, extra_args=self.extra_args[0] + ["-zapwallettxes=1"])
        assert_equal(self.nodes[0].getbalance(), NUM_HD_ADDS + NUM_SHIELD_ADDS)


if __name__ == "__main__":
    WalletBip39Test().main()
