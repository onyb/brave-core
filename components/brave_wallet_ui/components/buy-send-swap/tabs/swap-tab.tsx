import * as React from 'react'
import {
  UserAccountType,
  AssetOptionType,
  NetworkOptionsType,
  OrderTypes,
  BuySendSwapViewTypes,
  SlippagePresetObjectType,
  ExpirationPresetObjectType
} from '../../../constants/types'
import { AssetOptions } from '../../../options/asset-options'
import { NetworkOptions } from '../../../options/network-options'
import {
  Header,
  SelectAccount,
  SelectAsset,
  SelectNetwork,
  Swap
} from '..'

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
  onSubmitSwap: () => void
  flipSwapAssets: () => void
  onSelectNetwork: (network: NetworkOptionsType) => void
  onSelectAccount: (account: UserAccountType) => void
  onToggleOrderType: () => void
  onSelectSwapAsset: (asset: AssetOptionType, toOrFrom: string) => void
  onSelectSlippageTolerance: (slippage: SlippagePresetObjectType) => void
  onSelectExpiration: (expiration: ExpirationPresetObjectType) => void
  onSetExchangeRate: (value: string) => void
  onSetFromAmount: (value: string) => void
  onSetToAmount: (value: string) => void
  onSelectPresetAmount: (percent: number) => void
}

function SwapTab (props: Props) {
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
    onSubmitSwap,
    flipSwapAssets,
    onSelectNetwork,
    onSelectAccount,
    onToggleOrderType,
    onSelectSwapAsset,
    onSelectSlippageTolerance,
    onSelectExpiration,
    onSetExchangeRate,
    onSetFromAmount,
    onSetToAmount,
    onSelectPresetAmount
  } = props
  const [swapView, setSwapView] = React.useState<BuySendSwapViewTypes>('swap')
  const [isSelectingAsset, setIsSelectingAsset] = React.useState<string>('')
  const [filteredAssetList, setFilteredAssetList] = React.useState<AssetOptionType[]>(AssetOptions)

  const onChangeSwapView = (view: BuySendSwapViewTypes, option?: 'to' | 'from') => {
    if (option) {
      setIsSelectingAsset(option)
      setSwapView(view)
    } else {
      setSwapView(view)
    }
  }

  const onClickSelectNetwork = (network: NetworkOptionsType) => () => {
    onSelectNetwork(network)
    setSwapView('swap')
  }

  const onClickSelectAccount = (account: UserAccountType) => () => {
    onSelectAccount(account)
    setSwapView('swap')
  }

  const onSelectAsset = (asset: AssetOptionType) => () => {
    onSelectSwapAsset(asset, isSelectingAsset)
    setSwapView('swap')
  }

  const onFilterAssetList = (asset: AssetOptionType) => {
    const newList = AssetOptions.filter((assets) => assets !== asset)
    setFilteredAssetList(newList)
  }

  const onInputChange = (value: string, name: string) => {
    if (name === 'to') {
      onSetToAmount(value)
    }
    if (name === 'from') {
      onSetFromAmount(value)
    }
    if (name === 'rate') {
      onSetExchangeRate(value)
    }
  }

  const goBack = () => {
    setSwapView('swap')
  }

  return (
    <>
      {swapView === 'swap' &&
        <>
          <Header
            selectedAccount={selectedAccount}
            selectedNetwork={selectedNetwork}
            onChangeSwapView={onChangeSwapView}
          />
          <Swap
            toAsset={swapToAsset}
            fromAsset={swapFromAsset}
            toAmount={toAmount}
            fromAmount={fromAmount}
            orderType={orderType}
            exchangeRate={exchangeRate}
            slippageTolerance={slippageTolerance}
            orderExpiration={orderExpiration}
            onInputChange={onInputChange}
            onFlipAssets={flipSwapAssets}
            onSubmitSwap={onSubmitSwap}
            onSelectPresetAmount={onSelectPresetAmount}
            onSelectSlippageTolerance={onSelectSlippageTolerance}
            onSelectExpiration={onSelectExpiration}
            onChangeSwapView={onChangeSwapView}
            onToggleOrderType={onToggleOrderType}
            onFilterAssetList={onFilterAssetList}
            fromAssetBalance={fromAssetBalance}
            toAssetBalance={toAssetBalance}
          />
        </>
      }
      {swapView === 'acounts' &&
        <SelectAccount
          accounts={accounts}
          onSelectAccount={onClickSelectAccount}
          onBack={goBack}
        />
      }
      {swapView === 'assets' &&
        <SelectAsset
          assets={filteredAssetList}
          onSelectAsset={onSelectAsset}
          onBack={goBack}
        />
      }
      {swapView === 'networks' &&
        <SelectNetwork
          networks={NetworkOptions}
          onSelectNetwork={onClickSelectNetwork}
          onBack={goBack}
        />
      }
    </>
  )
}

export default SwapTab
