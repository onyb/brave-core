/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/permission_bubble/permission_prompt_impl.h"
#include "brave/browser/ui/views/permission_bubble/ethereum_permission_prompt_impl.h"
#include "chrome/browser/ui/permission_bubble/permission_prompt.h"
#include "components/permissions/request_type.h"

#define CreatePermissionPrompt CreatePermissionPrompt_ChromiumImpl
#include "../../../../../../../chrome/browser/ui/views/permission_bubble/permission_prompt_impl.cc"
#undef CreatePermissionPrompt

std::unique_ptr<permissions::PermissionPrompt> CreatePermissionPrompt(
    content::WebContents* web_contents,
    permissions::PermissionPrompt::Delegate* delegate) {
  Browser* browser = chrome::FindBrowserWithWebContents(web_contents);
  if (!browser) {
    DLOG(WARNING) << "Permission prompt suppressed because the WebContents is "
                     "not attached to any Browser window.";
    return nullptr;
  }

  if (delegate->Requests()[0]->GetRequestType() ==
      permissions::RequestType::kBraveEthereum) {
    return std::make_unique<EthereumPermissionPromptImpl>(browser, web_contents,
                                                          delegate);
  }

  return CreatePermissionPrompt_ChromiumImpl(web_contents, delegate);
}
