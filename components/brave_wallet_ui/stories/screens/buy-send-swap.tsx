import * as React from 'react'
import {
  BuySendSwapTypes,
  UserAccountType,
  NetworkOptionsType,
  AssetOptionType,
  OrderTypes,
  SlippagePresetObjectType,
  ExpirationPresetObjectType
} from '../../constants/types'
import Swap from '../../components/buy-send-swap/tabs/swap-tab'
import Send from '../../components/buy-send-swap/tabs/send-tab'
import {
  Layout
} from '../../components/buy-send-swap'

export interface Props {
  accounts: UserAccountType[]
  orderType: OrderTypes
  swapToAsset: AssetOptionType
  swapFromAsset: AssetOptionType
  selectedNetwork: NetworkOptionsType
  selectedAccount: UserAccountType
  exchangeRate: string
  slippageTolerance: SlippagePresetObjectType
  orderExpiration: ExpirationPresetObjectType
  fromAmount: string
  toAmount: string
  fromAssetBalance: string
  toAssetBalance: string
  toAddress: string
  onSubmitSend: () => void
  onSubmitSwap: () => void
  flipSwapAssets: () => void
  onSelectNetwork: (network: NetworkOptionsType) => void
  onSelectAccount: (account: UserAccountType) => void
  onToggleOrderType: () => void
  onSelectAsset: (asset: AssetOptionType, toOrFrom: string) => void
  onSelectSlippageTolerance: (slippage: SlippagePresetObjectType) => void
  onSelectExpiration: (expiration: ExpirationPresetObjectType) => void
  onSetExchangeRate: (value: string) => void
  onSetFromAmount: (value: string) => void
  onSetToAddress: (value: string) => void
  onSetToAmount: (value: string) => void
  onSelectPresetAmount: (percent: number) => void
}

function BuySendSwap (props: Props) {
  const {
    accounts,
    orderType,
    swapToAsset,
    swapFromAsset,
    selectedNetwork,
    selectedAccount,
    exchangeRate,
    slippageTolerance,
    orderExpiration,
    fromAmount,
    toAmount,
    fromAssetBalance,
    toAssetBalance,
    toAddress,
    onSubmitSend,
    onSubmitSwap,
    flipSwapAssets,
    onSelectNetwork,
    onSelectAccount,
    onToggleOrderType,
    onSelectAsset,
    onSelectSlippageTolerance,
    onSelectExpiration,
    onSetExchangeRate,
    onSetFromAmount,
    onSetToAddress,
    onSetToAmount,
    onSelectPresetAmount
  } = props
  const [selectedTab, setSelectedTab] = React.useState<BuySendSwapTypes>('send')

  const changeTab = (tab: BuySendSwapTypes) => () => {
    setSelectedTab(tab)
  }

  return (
    <Layout selectedTab={selectedTab} onChangeTab={changeTab}>
      {selectedTab === 'swap' &&
        <Swap
          accounts={accounts}
          orderType={orderType}
          swapToAsset={swapToAsset}
          swapFromAsset={swapFromAsset}
          selectedNetwork={selectedNetwork}
          selectedAccount={selectedAccount}
          exchangeRate={exchangeRate}
          orderExpiration={orderExpiration}
          slippageTolerance={slippageTolerance}
          fromAmount={fromAmount}
          toAmount={toAmount}
          fromAssetBalance={fromAssetBalance}
          toAssetBalance={toAssetBalance}
          onSubmitSwap={onSubmitSwap}
          flipSwapAssets={flipSwapAssets}
          onSelectNetwork={onSelectNetwork}
          onSelectAccount={onSelectAccount}
          onSelectSwapAsset={onSelectAsset}
          onToggleOrderType={onToggleOrderType}
          onSelectSlippageTolerance={onSelectSlippageTolerance}
          onSelectExpiration={onSelectExpiration}
          onSetExchangeRate={onSetExchangeRate}
          onSetFromAmount={onSetFromAmount}
          onSetToAmount={onSetToAmount}
          onSelectPresetAmount={onSelectPresetAmount}
        />
      }
      {selectedTab === 'send' &&
        <Send
          accounts={accounts}
          selectedAssetAmount={fromAmount}
          selectedAssetBalance={fromAssetBalance}
          toAddress={toAddress}
          onSelectAccount={onSelectAccount}
          onSelectNetwork={onSelectNetwork}
          onSelectPresetAmount={onSelectPresetAmount}
          onSelectAsset={onSelectAsset}
          onSetFromAmount={onSetFromAmount}
          onSetToAddress={onSetToAddress}
          onSubmit={onSubmitSend}
          selectedAccount={selectedAccount}
          selectedNetwork={selectedNetwork}
          selectedAsset={swapFromAsset}
        />
      }
    </Layout>
  )
}

export default BuySendSwap
