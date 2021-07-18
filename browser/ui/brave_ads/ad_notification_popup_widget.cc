/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_ads/ad_notification_popup_widget.h"

#include <utility>

#include "ui/gfx/geometry/rect.h"
#include "ui/native_theme/native_theme.h"
#include "ui/views/widget/widget_delegate.h"

#if defined(OS_WIN)
#include "ui/views/widget/desktop_aura/desktop_native_widget_aura.h"
#endif

namespace brave_ads {

AdNotificationPopupWidget::AdNotificationPopupWidget() = default;

void AdNotificationPopupWidget::InitWidget(views::WidgetDelegate* delegate,
                                           const gfx::Rect& bounds) {
  DCHECK(delegate);

  views::Widget::InitParams params;
  params.delegate = delegate;
  params.type = views::Widget::InitParams::TYPE_WINDOW_FRAMELESS;
  params.z_order = ui::ZOrderLevel::kFloatingWindow;
  params.opacity = views::Widget::InitParams::WindowOpacity::kTranslucent;
  params.shadow_type = views::Widget::InitParams::ShadowType::kNone;
  params.bounds = bounds;

#if defined(OS_WIN)
  // We want to ensure that this toast always goes to the native desktop,
  // not the Ash desktop (since there is already another toast contents view
  // there
  if (!params.parent) {
    DCHECK(!params.native_widget);
    params.native_widget = new views::DesktopNativeWidgetAura(this);
  }
#endif  // defined(OS_WIN)

  Init(std::move(params));
}

const ui::NativeTheme* AdNotificationPopupWidget::GetNativeTheme() const {
  // Ad notification popup widget is created without parent and context
  // specified. In this case default implementation for Linux uses
  // system theme which is not suitable for us. Therefore we return browser
  // native theme instance directly.
  return ui::NativeTheme::GetInstanceForNativeUi();
}

}  // namespace brave_ads
