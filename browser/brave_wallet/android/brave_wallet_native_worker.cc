/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_wallet/android/brave_wallet_native_worker.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "brave/browser/brave_wallet/brave_wallet_service_factory.h"
#include "brave/build/android/jni_headers/BraveWalletNativeWorker_jni.h"
#include "brave/components/brave_wallet/browser/asset_ratio_controller.h"
#include "brave/components/brave_wallet/browser/brave_wallet_service.h"
#include "brave/components/brave_wallet/browser/hd_keyring.h"
#include "brave/components/brave_wallet/browser/keyring_controller.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"

namespace {

brave_wallet::BraveWalletService* GetBraveWalletService() {
  return brave_wallet::BraveWalletServiceFactory::GetForContext(
      ProfileManager::GetActiveUserProfile()->GetOriginalProfile());
}

}  // namespace

namespace chrome {
namespace android {

BraveWalletNativeWorker::BraveWalletNativeWorker(
    JNIEnv* env,
    const base::android::JavaRef<jobject>& obj)
    : weak_java_brave_wallet_native_worker_(env, obj), weak_ptr_factory_(this) {
  Java_BraveWalletNativeWorker_setNativePtr(env, obj,
                                            reinterpret_cast<intptr_t>(this));
}

BraveWalletNativeWorker::~BraveWalletNativeWorker() {}

void BraveWalletNativeWorker::Destroy(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& jcaller) {
  delete this;
}

base::android::ScopedJavaLocalRef<jstring>
BraveWalletNativeWorker::GetRecoveryWords(JNIEnv* env) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();
  keyring_controller->GetMnemonicForDefaultKeyring();
  return base::android::ConvertUTF8ToJavaString(
      env, keyring_controller->GetMnemonicForDefaultKeyring());
}

bool BraveWalletNativeWorker::IsWalletLocked(JNIEnv* env) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();
  return keyring_controller->IsLocked();
}

base::android::ScopedJavaLocalRef<jstring>
BraveWalletNativeWorker::CreateWallet(
    JNIEnv* env,
    const base::android::JavaParamRef<jstring>& password) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();
  auto* keyring = keyring_controller->CreateDefaultKeyring(
      base::android::ConvertJavaStringToUTF8(env, password));
  if (keyring) {
    keyring->AddAccounts();
  }

  return base::android::ConvertUTF8ToJavaString(
      env, keyring_controller->GetMnemonicForDefaultKeyring());
}

void BraveWalletNativeWorker::LockWallet(JNIEnv* env) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();
  keyring_controller->Lock();
}

bool BraveWalletNativeWorker::UnlockWallet(
    JNIEnv* env,
    const base::android::JavaParamRef<jstring>& password) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();

  return keyring_controller->Unlock(
      base::android::ConvertJavaStringToUTF8(env, password));
}

base::android::ScopedJavaLocalRef<jstring>
BraveWalletNativeWorker::RestoreWallet(
    JNIEnv* env,
    const base::android::JavaParamRef<jstring>& mnemonic,
    const base::android::JavaParamRef<jstring>& password) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();
  auto* keyring = keyring_controller->RestoreDefaultKeyring(
      base::android::ConvertJavaStringToUTF8(env, mnemonic),
      base::android::ConvertJavaStringToUTF8(env, password));
  if (keyring) {
    keyring->AddAccounts();
  }
  return base::android::ConvertUTF8ToJavaString(
      env, keyring_controller->GetMnemonicForDefaultKeyring());
}

void BraveWalletNativeWorker::ResetWallet(JNIEnv* env) {
  auto* keyring_controller = GetBraveWalletService()->keyring_controller();
  keyring_controller->Reset();
}

void BraveWalletNativeWorker::GetAssetPrice(
    JNIEnv* env,
    const base::android::JavaParamRef<jstring>& asset) {
  brave_wallet::BraveWalletService* brave_wallet_service =
      GetBraveWalletService();
  if (brave_wallet_service) {
    auto* asset_ratio_controller =
        brave_wallet_service->asset_ratio_controller();
    asset_ratio_controller->GetPrice(
        base::android::ConvertJavaStringToUTF8(env, asset),
        base::BindOnce(&BraveWalletNativeWorker::OnGetPrice,
                       weak_ptr_factory_.GetWeakPtr()));
  }
}

void BraveWalletNativeWorker::OnGetPrice(bool success,
                                         const std::string& price) {
  JNIEnv* env = base::android::AttachCurrentThread();
  Java_BraveWalletNativeWorker_OnGetPrice(
      env, weak_java_brave_wallet_native_worker_.get(env),
      base::android::ConvertUTF8ToJavaString(env, price), success);
}

void BraveWalletNativeWorker::GetAssetPriceHistory(
    JNIEnv* env,
    const base::android::JavaParamRef<jstring>& asset,
    const jint timeFrameType) {
  brave_wallet::BraveWalletService* brave_wallet_service =
      GetBraveWalletService();
  if (brave_wallet_service) {
    auto* asset_ratio_controller =
        brave_wallet_service->asset_ratio_controller();

    brave_wallet::mojom::AssetPriceTimeframe time_frame;
    if (timeFrameType == 0) {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::Live;
    } else if (timeFrameType == 1) {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::OneDay;
    } else if (timeFrameType == 2) {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::OneWeek;
    } else if (timeFrameType == 3) {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::OneMonth;
    } else if (timeFrameType == 4) {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::ThreeMonths;
    } else if (timeFrameType == 5) {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::OneYear;
    } else {
      time_frame = brave_wallet::mojom::AssetPriceTimeframe::All;
    }

    asset_ratio_controller->GetPriceHistory(
        base::android::ConvertJavaStringToUTF8(env, asset), time_frame,
        base::BindOnce(&BraveWalletNativeWorker::OnGetPriceHistory,
                       weak_ptr_factory_.GetWeakPtr()));
  }
}

void BraveWalletNativeWorker::OnGetPriceHistory(
    bool success,
    std::vector<brave_wallet::mojom::AssetTimePricePtr> values) {
  std::string price_history;
  base::Value list(base::Value::Type::LIST);
  for (const auto& asset_time_price : values) {
    base::Value dict(base::Value::Type::DICTIONARY);
    dict.SetDoubleKey("time", (asset_time_price->date).ToDoubleT());
    dict.SetStringKey("price", asset_time_price->price);
    list.Append(std::move(dict));
  }
  base::JSONWriter::Write(list, &price_history);

  JNIEnv* env = base::android::AttachCurrentThread();
  Java_BraveWalletNativeWorker_OnGetPriceHistory(
      env, weak_java_brave_wallet_native_worker_.get(env),
      base::android::ConvertUTF8ToJavaString(env, price_history), success);
}

static void JNI_BraveWalletNativeWorker_Init(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& jcaller) {
  new BraveWalletNativeWorker(env, jcaller);
}

}  // namespace android
}  // namespace chrome
