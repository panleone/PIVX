// Copyright (c) 2017-2022 The PIVX Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#include "stakeinput.h"

#include "amount.h"
#include "chain.h"
#include "streams.h"
#include "txdb.h"
#include "validation.h"

static bool HasStakeMinAgeOrDepth(int nHeight, uint32_t nTime, const CBlockIndex* pindex)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    if (consensus.NetworkUpgradeActive(nHeight + 1, Consensus::UPGRADE_ZC_PUBLIC) &&
            !consensus.HasStakeMinAgeOrDepth(nHeight, nTime, pindex->nHeight, pindex->nTime)) {
        return error("%s : min age violation - height=%d - time=%d, nHeightBlockFrom=%d, nTimeBlockFrom=%d",
                     __func__, nHeight, nTime, pindex->nHeight, pindex->nTime);
    }
    return true;
}

CPivStake* CPivStake::NewPivStake(const CTxIn& txin, int nHeight, uint32_t nTime)
{
    if (txin.IsZerocoinSpend()) {
        error("%s: unable to initialize CPivStake from zerocoin spend", __func__);
        return nullptr;
    }

    // Look for the stake input in the coins cache first
    const Coin& coin = pcoinsTip->AccessCoin(txin.prevout);
    if (!coin.IsSpent()) {
        const CBlockIndex* pindexFrom = mapBlockIndex.at(chainActive[coin.nHeight]->GetBlockHash());
        // Check that the stake has the required depth/age
        if (!HasStakeMinAgeOrDepth(nHeight, nTime, pindexFrom)) {
            return nullptr;
        }
        // All good
        return new CPivStake(coin.out, txin.prevout, pindexFrom);
    }

    // Otherwise find the previous transaction in database
    uint256 hashBlock;
    CTransactionRef txPrev;
    if (!GetTransaction(txin.prevout.hash, txPrev, hashBlock, true)) {
        error("%s : INFO: read txPrev failed, tx id prev: %s", __func__, txin.prevout.hash.GetHex());
        return nullptr;
    }
    const CBlockIndex* pindexFrom = nullptr;
    if (mapBlockIndex.count(hashBlock)) {
        CBlockIndex* pindex = mapBlockIndex.at(hashBlock);
        if (chainActive.Contains(pindex)) pindexFrom = pindex;
    }
    // Check that the input is in the active chain
    if (!pindexFrom) {
        error("%s : Failed to find the block index for stake origin", __func__);
        return nullptr;
    }
    // Check that the stake has the required depth/age
    if (!HasStakeMinAgeOrDepth(nHeight, nTime, pindexFrom)) {
        return nullptr;
    }
    // All good
    return new CPivStake(txPrev->vout[txin.prevout.n], txin.prevout, pindexFrom);
}

bool CPivStake::GetTxOutFrom(CTxOut& out) const
{
    out = outputFrom;
    return true;
}

CTxIn CPivStake::GetTxIn() const
{
    return CTxIn(outpointFrom.hash, outpointFrom.n);
}

CAmount CPivStake::GetValue() const
{
    return outputFrom.nValue;
}

CDataStream CPivStake::GetUniqueness() const
{
    //The unique identifier for a PIV stake is the outpoint
    CDataStream ss(SER_NETWORK, 0);
    ss << outpointFrom.n << outpointFrom.hash;
    return ss;
}

CShieldStake* CShieldStake::NewShieldStake(const SpendDescription& spendDescription, CAmount noteAmount, int nHeight, uint32_t nTime)
{
    // TODO: add previous block and all of that stuff
    return new CShieldStake(spendDescription.nullifier, noteAmount);
}

//The block that the UTXO was added to the chain
const CBlockIndex* CPivStake::GetIndexFrom() const
{
    // Sanity check, pindexFrom is set on the constructor.
    if (!pindexFrom) throw std::runtime_error("CPivStake: uninitialized pindexFrom");
    return pindexFrom;
}

// A Unique identifier of a shield note
CDataStream CShieldStake::GetUniqueness() const
{
    CDataStream ss(SER_NETWORK, 0);
    ss << this->nullifier;
    return ss;
}
