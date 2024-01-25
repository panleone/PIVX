#!/usr/bin/env python3
# Copyright (c) 2024 The PIVX Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test OP_EXCHANGEADDR.

Test that the OP_EXCHANGEADDR soft-fork activates at (regtest) block height
1001, and that the following is capable
    t > e
    e > t
and not capable
    t > s > e
    e > s
"""

from decimal import Decimal

from test_framework.test_framework import PivxTestFramework
from test_framework.util import assert_equal, assert_raises_rpc_error

FEATURE_PRE_SPLIT_KEYPOOL = 169900

class ExchangeAddrTest(PivxTestFramework):
    def set_test_params(self):
        self.num_nodes = 2
        self.extra_args = [['-whitelist=127.0.0.1','-regtest'], ['-regtest']]
        self.setup_clean_chain = True

    def run_test(self):
        # Mine and activate exchange addresses
        self.nodes[0].generate(1000)
        assert_equal(self.nodes[0].getblockcount(), 1000)
        self.nodes[0].generate(1)

        # Get addresses for testing
        ex_addr = self.nodes[1].getnewexchangeaddress()
        t_addr = self.nodes[0].getnewaddress()

        # Attempt to send funds from transparent address to exchange address
        ex_addr_validation_result = self.nodes[0].validateaddress(ex_addr)
        assert_equal(ex_addr_validation_result['isvalid'], True)
        self.nodes[0].sendtoaddress(ex_addr, 2.0)
        self.nodes[0].generate(6)
        self.sync_all()

        # Verify balance
        node_bal = self.nodes[1].getbalance()
        assert_equal(node_bal, 2)

        # Attempt to send funds from exchange address back to transparent address
        tx2 = self.nodes[0].sendtoaddress(t_addr, 1.0)
        self.nodes[0].generate(6)
        self.sync_all()
        ex_result = self.nodes[0].gettransaction(tx2)

        # Assert that the transaction ID in the result matches tx2
        assert_equal(ex_result['txid'], tx2)

        # Transparent to Shield to Exchange should fail
        # Check wallet version
        wallet_info = self.nodes[0].getwalletinfo()
        if wallet_info['walletversion'] < FEATURE_PRE_SPLIT_KEYPOOL:
            self.log.info("Pre-HD wallet version detected. Skipping Shield tests.")
            return
        sapling_addr = self.nodes[0].getnewshieldaddress()
        self.nodes[0].sendtoaddress(sapling_addr, 2.0)
        self.nodes[0].generate(6)
        sap_to_ex = [{"address": ex_addr, "amount": Decimal('1')}]

        # Expect shieldsendmany to fail with bad-txns-invalid-sapling
        expected_error_code = -4
        expected_error_message = "Failed to accept tx in the memory pool (reason: bad-txns-invalid-sapling)"
        assert_raises_rpc_error(
            expected_error_code,
            expected_error_message,
            self.nodes[0].shieldsendmany,
            sapling_addr, sap_to_ex, 1, 0, sapling_addr
        )

        # Generate a new shielded address
        new_shielded_addr = self.nodes[1].getnewshieldaddress()

        # Attempt to send funds from exchange address to shielded address
        expected_error_message = "Failed to build transaction: Failed to sign transaction"

        assert_raises_rpc_error(
            expected_error_code,
            expected_error_message,
            self.nodes[1].sendtoaddress,
            new_shielded_addr, 1.0
        )


if __name__ == "__main__":
    ExchangeAddrTest().main()

