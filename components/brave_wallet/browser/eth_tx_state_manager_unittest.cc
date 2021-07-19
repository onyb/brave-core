/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_tx_state_manager.h"

#include "base/strings/string_number_conversions.h"
#include "base/time/time.h"
#include "base/values.h"
#include "brave/components/brave_wallet/browser/eth_address.h"
#include "brave/components/brave_wallet/browser/pref_names.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/test/base/testing_browser_process.h"
#include "chrome/test/base/testing_profile_manager.h"
#include "components/prefs/pref_service.h"
#include "content/public/test/browser_task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace brave_wallet {

class EthTxStateManagerUnitTest : public testing::Test {
 public:
  EthTxStateManagerUnitTest()
      : testing_profile_manager_(TestingBrowserProcess::GetGlobal()) {}
  ~EthTxStateManagerUnitTest() override {}

 protected:
  void SetUp() override {
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    ASSERT_TRUE(testing_profile_manager_.SetUp(temp_dir_.GetPath()));
  }

  PrefService* GetPrefs() {
    return ProfileManager::GetActiveUserProfile()->GetPrefs();
  }

  content::BrowserTaskEnvironment task_environment_;
  TestingProfileManager testing_profile_manager_;
  base::ScopedTempDir temp_dir_;
};

TEST_F(EthTxStateManagerUnitTest, GenerateMetaID) {
  EXPECT_NE(EthTxStateManager::GenerateMetaID(),
            EthTxStateManager::GenerateMetaID());
}

TEST_F(EthTxStateManagerUnitTest, TxMetaAndValue) {
  EthTransaction tx(EthTransaction::TxData(
      0x09, 0x4a817c800, 0x5208,
      EthAddress::FromHex("0x3535353535353535353535353535353535353535"),
      0x0de0b6b3a7640000, std::vector<uint8_t>()));

  EthTxStateManager::TxMeta meta(tx);
  meta.id = EthTxStateManager::GenerateMetaID();
  meta.status = EthTxStateManager::TransactionStatus::SUBMITTED;
  meta.from = EthAddress::FromHex("0x2f015c60e0be116b1f0cd534704db9c92118fb6a");
  meta.last_gas_price = 0x1234;
  meta.created_time = base::Time::Now();
  meta.submitted_time = base::Time::Now();
  meta.confirmed_time = base::Time::Now();

  TransactionReceipt tx_receipt;
  tx_receipt.transaction_hash =
      "0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238";
  tx_receipt.transaction_index = 0x1;
  tx_receipt.block_number = 0xb;
  tx_receipt.block_hash =
      "0xc6ef2fc5426d6ad6fd9e2a26abeab0aa2411b7ab17f30a99d3cb96aed1d1055b";
  tx_receipt.cumulative_gas_used = 0x33bc;
  tx_receipt.gas_used = 0x4dc;
  tx_receipt.contract_address = "0xb60e8dd61c5d32be8058bb8eb970870f07233155";
  tx_receipt.status = true;

  meta.tx_receipt = tx_receipt;
  meta.tx_hash =
      "0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238";

  base::Value meta_value = EthTxStateManager::TxMetaToValue(meta);
  auto meta_from_value = EthTxStateManager::ValueToTxMeta(meta_value);
  ASSERT_NE(meta_from_value, absl::nullopt);
  EXPECT_EQ(meta_from_value->id, meta.id);
  EXPECT_EQ(meta_from_value->status, meta.status);
  EXPECT_EQ(meta_from_value->from, meta.from);
  EXPECT_EQ(meta_from_value->last_gas_price, meta.last_gas_price);
  EXPECT_EQ(meta_from_value->created_time, meta.created_time);
  EXPECT_EQ(meta_from_value->submitted_time, meta.submitted_time);
  EXPECT_EQ(meta_from_value->confirmed_time, meta.confirmed_time);
  EXPECT_EQ(meta_from_value->tx_receipt, meta.tx_receipt);
  EXPECT_EQ(meta_from_value->tx_hash, meta.tx_hash);
  EXPECT_EQ(meta_from_value->tx, meta.tx);

  EXPECT_EQ(*meta_from_value, meta);
}

TEST_F(EthTxStateManagerUnitTest, TxOperations) {
  GetPrefs()->ClearPref(kBraveWalletTransactions);
  EthTxStateManager tx_state_manager(GetPrefs());

  EthTxStateManager::TxMeta meta;
  meta.id = "001";
  EXPECT_FALSE(GetPrefs()->HasPrefPath(kBraveWalletTransactions));
  // Add
  tx_state_manager.AddOrUpdateTx(meta);
  EXPECT_TRUE(GetPrefs()->HasPrefPath(kBraveWalletTransactions));
  {
    const auto* dict = GetPrefs()->GetDictionary(kBraveWalletTransactions);
    ASSERT_TRUE(dict);
    EXPECT_EQ(dict->DictSize(), 1u);
    const base::Value* value = dict->FindKey("001");
    ASSERT_TRUE(value);
    auto meta_from_value = EthTxStateManager::ValueToTxMeta(*value);
    ASSERT_NE(meta_from_value, absl::nullopt);
    EXPECT_EQ(*meta_from_value, meta);
  }

  meta.tx_hash = "0xabcd";
  // Update
  tx_state_manager.AddOrUpdateTx(meta);
  {
    const auto* dict = GetPrefs()->GetDictionary(kBraveWalletTransactions);
    ASSERT_TRUE(dict);
    EXPECT_EQ(dict->DictSize(), 1u);
    const base::Value* value = dict->FindKey("001");
    ASSERT_TRUE(value);
    auto meta_from_value = EthTxStateManager::ValueToTxMeta(*value);
    ASSERT_NE(meta_from_value, absl::nullopt);
    EXPECT_EQ(meta_from_value->tx_hash, meta.tx_hash);
  }

  meta.id = "002";
  meta.tx_hash = "0xabff";
  // Add another one
  tx_state_manager.AddOrUpdateTx(meta);
  {
    const auto* dict = GetPrefs()->GetDictionary(kBraveWalletTransactions);
    ASSERT_TRUE(dict);
    EXPECT_EQ(dict->DictSize(), 2u);
  }

  // Get
  {
    EthTxStateManager::TxMeta meta_fetched;
    ASSERT_FALSE(tx_state_manager.GetTx("001", nullptr));
    ASSERT_FALSE(tx_state_manager.GetTx("003", &meta_fetched));
    ASSERT_TRUE(tx_state_manager.GetTx("001", &meta_fetched));
    EXPECT_EQ(meta_fetched.id, "001");
    EXPECT_EQ(meta_fetched.tx_hash, "0xabcd");

    ASSERT_TRUE(tx_state_manager.GetTx("002", &meta_fetched));
    EXPECT_EQ(meta_fetched.id, "002");
    EXPECT_EQ(meta_fetched.tx_hash, "0xabff");
  }

  // Delete
  tx_state_manager.DeleteTx("001");
  {
    const auto* dict = GetPrefs()->GetDictionary(kBraveWalletTransactions);
    ASSERT_TRUE(dict);
    EXPECT_EQ(dict->DictSize(), 1u);
  }

  // Purge
  tx_state_manager.WipeTxs();
  EXPECT_FALSE(GetPrefs()->HasPrefPath(kBraveWalletTransactions));
}

TEST_F(EthTxStateManagerUnitTest, GetTransactionsByStatus) {
  GetPrefs()->ClearPref(kBraveWalletTransactions);
  EthTxStateManager tx_state_manager(GetPrefs());

  auto addr1 =
      EthAddress::FromHex("0x3535353535353535353535353535353535353535");
  auto addr2 =
      EthAddress::FromHex("0x2f015c60e0be116b1f0cd534704db9c92118fb6a");

  for (size_t i = 0; i < 20; ++i) {
    EthTxStateManager::TxMeta meta;
    meta.from =
        EthAddress::FromHex("0x3333333333333333333333333333333333333333");
    meta.id = base::NumberToString(i);
    if (i % 2 == 0) {
      if (i % 4 == 0)
        meta.from = addr1;
      meta.status = EthTxStateManager::TransactionStatus::CONFIRMED;
    } else {
      if (i % 5 == 0)
        meta.from = addr2;
      meta.status = EthTxStateManager::TransactionStatus::SUBMITTED;
    }
    tx_state_manager.AddOrUpdateTx(meta);
  }

  EXPECT_EQ(
      tx_state_manager
          .GetTransactionsByStatus(
              EthTxStateManager::TransactionStatus::APPROVED, absl::nullopt)
          .size(),
      0u);
  EXPECT_EQ(
      tx_state_manager
          .GetTransactionsByStatus(
              EthTxStateManager::TransactionStatus::CONFIRMED, absl::nullopt)
          .size(),
      10u);
  EXPECT_EQ(
      tx_state_manager
          .GetTransactionsByStatus(
              EthTxStateManager::TransactionStatus::SUBMITTED, absl::nullopt)
          .size(),
      10u);

  EXPECT_EQ(tx_state_manager
                .GetTransactionsByStatus(
                    EthTxStateManager::TransactionStatus::APPROVED, addr1)
                .size(),
            0u);

  auto confirmed_addr1 = tx_state_manager.GetTransactionsByStatus(
      EthTxStateManager::TransactionStatus::CONFIRMED, addr1);
  EXPECT_EQ(confirmed_addr1.size(), 5u);
  for (const auto& meta : confirmed_addr1) {
    unsigned id;
    ASSERT_TRUE(base::StringToUint(meta.id, &id));
    EXPECT_EQ(id % 4, 0u);
  }

  auto submitted_addr2 = tx_state_manager.GetTransactionsByStatus(
      EthTxStateManager::TransactionStatus::SUBMITTED, addr2);
  EXPECT_EQ(submitted_addr2.size(), 2u);
  for (const auto& meta : submitted_addr2) {
    unsigned id;
    ASSERT_TRUE(base::StringToUint(meta.id, &id));
    EXPECT_EQ(id % 5, 0u);
  }
}

}  // namespace brave_wallet
