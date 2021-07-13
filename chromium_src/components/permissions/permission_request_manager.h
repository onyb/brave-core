/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_COMPONENTS_PERMISSIONS_PERMISSION_REQUEST_MANAGER_H_
#define BRAVE_CHROMIUM_SRC_COMPONENTS_PERMISSIONS_PERMISSION_REQUEST_MANAGER_H_

#include "brave/components/brave_wallet/common/buildflags/buildflags.h"

#if BUILDFLAG(BRAVE_WALLET_ENABLED)
#define BRAVE_PERMISSION_REQUEST_MANAGER_H_                               \
  void AcceptEthereumPermissionRequests(const std::vector<std::string>&); \
  void IgnoreEthereumPermissionRequests();
#else
#define BRAVE_PERMISSION_REQUEST_MANAGER_H_
#endif  // BUILDFLAG(BRAVE_WALLET_ENABLED)

#include "../../../../components/permissions/permission_request_manager.h"
#undef BRAVE_PERMISSION_REQUEST_MANAGER_H_

#endif  // BRAVE_CHROMIUM_SRC_COMPONENTS_PERMISSIONS_PERMISSION_REQUEST_MANAGER_H_
