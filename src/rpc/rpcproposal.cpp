// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2017-2019 The WaykiChain Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "tx/proposaltx.h"
#include "rpc/core/rpcserver.h"
#include "rpc/core/rpccommons.h"


SysParamType  GetParamType(const string  paramName){
    auto itr = paramNameToSysParamTypeMap.find(paramName);
    if(itr == paramNameToSysParamTypeMap.end())
        return NULL_SYS_PARAM_TYPE;
    else
        return std::get<1>(itr->second);

}

Value getproposal(const Array& params, bool fHelp){

    if(fHelp || params.size() != 1){

        throw runtime_error(
                "getproposal \"proposalid\"\n"
                "get a proposal by proposal id\n"
                "\nArguments:\n"
                "1.\"proposalid\":      (string, required) the proposal id \n"

                "\nExamples:\n"
                + HelpExampleCli("getproposal", "02sov0efs3ewdsxcfresdfdsadfgdsasdfdsadfdsdfsdfsddfge32ewsrewsowekdsx")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("getproposal", "02sov0efs3ewdsxcfresdfdsadfgdsasdfdsadfdsdfsdfsddfge32ewsrewsowekdsx")

        );
    }
    uint256 proposalId = uint256S(params[0].get_str()) ;
    std::shared_ptr<CProposal> pp ;
    if(pCdMan->pSysGovernCache->GetProposal(proposalId, pp)){
        return pp->ToJson() ;
    }
    return Object();
}


Value submitparamgovernproposal(const Array& params, bool fHelp){

    if(fHelp || params.size() < 3 || params.size() > 4){

        throw runtime_error(
                "submitparamgovernproposal \"addr\" \"param_name\" \"param_value\" [\"fee\"]\n"
                "create proposal about param govern\n"
                "\nArguments:\n"
                "1.\"addr\":             (string, required) the tx submitor's address\n"
                "2.\"param_name\":       (string, required) the name of param, the param list can be found in document \n"
                "3.\"param_value\":      (numberic, required) the param value that will be updated to \n"
                "4.\"fee\":              (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitparamgovernproposal", "0-1 ASSET_ISSUE_FEE  10000 WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitparamgovernproposal", "0-1 ASSET_ISSUE_FEE  10000 WICC:1:WI")

                );

    }


    EnsureWalletIsUnlocked();

    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    string paramName = params[1].get_str() ;
    uint64_t paramValue = AmountToRawValue(params[2]) ;
    ComboMoney fee          = RPC_PARAM::GetFee(params, 3, PROPOSAL_REQUEST_TX);
    int32_t validHeight  = chainActive.Height();
    CAccount account = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(account, fee.symbol, SUB_FREE, fee.GetSawiAmount());

    CParamsGovernProposal proposal ;


    SysParamType  type = GetParamType(paramName) ;
    if(type == SysParamType::NULL_SYS_PARAM_TYPE)
        throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("system param type(%s) is not exist",paramName));

    proposal.param_values.push_back(std::make_pair(type, paramValue));

    CProposalCreateTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol;
    tx.valid_height = validHeight;
    tx.proposalBean = CProposalStorageBean(std::make_shared<CParamsGovernProposal>(proposal)) ;
    return SubmitTx(account.keyid, tx) ;

}


Value submitcdpparamgovernproposal(const Array& params, bool fHelp){

    if(fHelp || params.size() < 3 || params.size() > 4){

        throw runtime_error(
                "submitcdpparamgovernproposal \"addr\" \"param_name\" \"param_value\" \"bcoin_symbole\" \"scoin_symbol\" [\"fee\"]\n"
                "create proposal about cdp  param govern\n"
                "\nArguments:\n"
                "1.\"addr\":             (string, required) the tx submitor's address\n"
                "2.\"param_name\":       (string, required) the name of param, the param list can be found in document \n"
                "3.\"param_value\":      (numberic, required) the param value that will be updated to \n"
                "4.\"bcoin_symbo\":      (string,required) the base coin symbol\n"
                "5.\"scoin_symbo\":      (string,required) the stable coin symbol\n"
                "6.\"fee\":              (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitcdpparamgovernproposal", "0-1 ASSET_ISSUE_FEE  10000 WICC WUSD WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitcdpparamgovernproposal", "0-1 ASSET_ISSUE_FEE  10000 WICC WUSD WICC:1:WI")

        );

    }


    EnsureWalletIsUnlocked();

    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    string paramName = params[1].get_str() ;
    string bcoinSymbol = params[3].get_str() ;
    string scoinSymbol = params[4].get_str() ;
    uint64_t paramValue = AmountToRawValue(params[2]) ;
    ComboMoney fee          = RPC_PARAM::GetFee(params, 3, PROPOSAL_REQUEST_TX);
    int32_t validHeight  = chainActive.Height();
    CAccount account = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(account, fee.symbol, SUB_FREE, fee.GetSawiAmount());

    CCdpParamGovernProposal proposal ;


    SysParamType  type = GetParamType(paramName) ;
    if(type == SysParamType::NULL_SYS_PARAM_TYPE)
        throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("system param type(%s) is not exist",paramName));

    proposal.param_values.push_back(std::make_pair(type, paramValue));
    proposal.coinPair = CCdpCoinPair(bcoinSymbol, scoinSymbol) ;

    CProposalCreateTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol;
    tx.valid_height = validHeight;
    tx.proposalBean = CProposalStorageBean(std::make_shared<CCdpParamGovernProposal>(proposal)) ;
    return SubmitTx(account.keyid, tx) ;

}

Value submitgovernerupdateproposal(const Array& params , bool fHelp) {

    if(fHelp || params.size() < 3 || params.size() > 4){

        throw runtime_error(
                "submitgovernerupdateproposal \"addr\" \"governer_uid\" \"operate_type\" [\"fee\"]\n"
                "create proposal about  add/remove a governer \n"
                "\nArguments:\n"
                "1.\"addr\":             (string, required) the tx submitor's address\n"
                "2.\"governer_uid\":     (string, required) the governer's uid\n"
                "3.\"operate_type\":     (numberic, required) the operate type \n"
                "                         1 stand for add\n"
                "                         2 stand for remove\n"
                "4.\"fee\":              (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitgovernerupdateproposal", "0-1 100-2 1  WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitgovernerupdateproposal", "0-1 100-2 1  WICC:1:WI")

        );

    }

    EnsureWalletIsUnlocked();

    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    CRegID governerId = CRegID(params[1].get_str()) ;
    uint64_t operateType = AmountToRawValue(params[2]) ;
    ComboMoney fee          = RPC_PARAM::GetFee(params, 3, PROPOSAL_REQUEST_TX);
    int32_t validHeight  = chainActive.Height();
    CAccount account = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(account, fee.symbol, SUB_FREE, fee.GetSawiAmount());

    CGovernerUpdateProposal proposal ;
    proposal.governer_regid = governerId ;
    proposal.operate_type = ProposalOperateType(operateType);

    CProposalCreateTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol ;
    tx.valid_height = validHeight;
    tx.proposalBean = CProposalStorageBean(std::make_shared<CGovernerUpdateProposal>(proposal)) ;
    return SubmitTx(account.keyid, tx) ;

}

Value submitdexswitchproposal(const Array& params, bool fHelp) {

    if(fHelp || params.size() < 3 || params.size() > 4){

        throw runtime_error(
                "submitdexswitchproposal \"addr\" \"dexid\" \"operate_type\" [\"fee\"]\n"
                "create proposal about enable/disable dexoperator\n"
                "\nArguments:\n"
                "1.\"addr\":             (string, required) the tx submitor's address\n"
                "2.\"dexid\":            (numberic, required) the dexoperator's id\n"
                "3.\"operate_type\":     (numberic, required) the operate type \n"
                "                         1 stand for enable\n"
                "                         2 stand for disable\n"
                "4.\"fee\":              (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitdexswitchproposal", "0-1 1 1  WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitdexswitchproposal", "0-1 1 1  WICC:1:WI")

        );

    }

    EnsureWalletIsUnlocked();
    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    uint64_t dexId = params[1].get_int();
    uint64_t operateType = params[2].get_int();
    ComboMoney fee          = RPC_PARAM::GetFee(params, 3, PROPOSAL_REQUEST_TX);
    int32_t validHeight  = chainActive.Height();
    CAccount account = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(account, fee.symbol, SUB_FREE, fee.GetSawiAmount());

    CDexSwitchProposal proposal ;
    proposal.dexid = dexId ;
    proposal.operate_type = ProposalOperateType(operateType);

    CProposalCreateTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol ;
    tx.valid_height = validHeight;
    tx.proposalBean = CProposalStorageBean(std::make_shared<CDexSwitchProposal>(proposal)) ;
    return SubmitTx(account.keyid, tx) ;
}

Value submitproposalapprovaltx(const Array& params, bool fHelp){

    if(fHelp || params.size() < 2 || params.size() > 3){
        throw runtime_error(
                "submitproposalapprovaltx \"addr\" \"proposalid\" [\"fee\"]\n"
                "assent a proposal\n"
                "\nArguments:\n"
                "1.\"addr\":             (string, required) the tx submitor's address\n"
                "2.\"proposalid\":       (numberic, required) the dexoperator's id\n"
                "3.\"fee\":              (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitproposalapprovaltx", "0-1 1 1  WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitproposalapprovaltx", "0-1 1 1  WICC:1:WI")

        );
    }


    EnsureWalletIsUnlocked();
    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    uint256 proposalId = uint256S(params[1].get_str()) ;
    ComboMoney fee          = RPC_PARAM::GetFee(params, 2, PROPOSAL_REQUEST_TX);
    int32_t validHegiht  = chainActive.Height();
    CAccount account = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(account, fee.symbol, SUB_FREE, fee.GetSawiAmount());

    CProposalAssentTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol;
    tx.valid_height = validHegiht;
    tx.txid = proposalId ;
    return SubmitTx(account.keyid, tx) ;

}

Value submitminerfeeproposal(const Array& params, bool fHelp) {
    if(fHelp || params.size() < 3 || params.size() > 4){

        throw runtime_error(
                "submitminerfeeproposal \"addr\" \"tx_type\" \"fee_info\"  [\"fee\"]\n"
                "create proposal about enable/disable dexoperator\n"
                "\nArguments:\n"
                "1.\"addr\":             (string, required) the tx submitor's address\n"
                "2.\"tx_type\":          (numberic, required) the tx type you can get the list by command \"getminminerfee\" \n"
                "3.\"fee_info\":         (combomoney, required) the miner fee symbol, example:WICC, WUSD \n"
                "4.\"fee\":              (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitminerfeeproposal", "0-1 1 WICC:1:WI  WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitminerfeeproposal", "0-1 1 1  WICC:1:WI")

        );

    }

    EnsureWalletIsUnlocked();
    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    uint8_t txType = params[1].get_int();
    ComboMoney feeInfo = RPC_PARAM::GetComboMoney(params[2],SYMB::WICC);
    ComboMoney fee          = RPC_PARAM::GetFee(params, 3, PROPOSAL_REQUEST_TX);
    int32_t validHeight  = chainActive.Height();
    CAccount account = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(account, fee.symbol, SUB_FREE, fee.GetSawiAmount());

    CMinerFeeProposal proposal ;
    proposal.tx_type = TxType(txType)  ;
    proposal.fee_symbol = feeInfo.symbol;
    proposal.fee_sawi_amount = feeInfo.GetSawiAmount();

    CProposalCreateTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol ;
    tx.valid_height = validHeight;
    tx.proposalBean = CProposalStorageBean(std::make_shared<CMinerFeeProposal>(proposal)) ;


    return SubmitTx(account.keyid, tx) ;

}

Value submitcointransferproposal( const Array& params, bool fHelp) {

    if(fHelp || params.size() < 4 || params.size() > 5){

        throw runtime_error(
                "submitcointransferproposal \"from_uid\" \"to_uid\" \"symbol:amount:unit\"  [\"fee\"]\n"
                "create proposal about enable/disable dexoperator\n"
                "\nArguments:\n"
                "1.\"tx_uid\":                (string, required) the tx submitor's address\n"
                "2.\"from_uid\":              (string, required) the address that transfer from\n"
                "3.\"to_uid\":                (string, required) the address that tranfer to \n"
                "4.\"amount\":                (combomoney, required) the tansfer amount \n"
                "5.\"fee\":                   (combomoney, optional) the tx fee \n"
                "\nExamples:\n"
                + HelpExampleCli("submitminerfeeproposal", "0-1 1 WICC:1:WI  WICC:1:WI")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("submitminerfeeproposal", "0-1 1 1  WICC:1:WI")

        );

    }

    EnsureWalletIsUnlocked();
    const CUserID& txUid = RPC_PARAM::GetUserId(params[0], true);
    const CUserID& fromUid = RPC_PARAM:: GetUserId(params[1]) ;
    const CUserID& toUid = RPC_PARAM:: GetUserId(params[2]) ;

    ComboMoney transferInfo = RPC_PARAM::GetComboMoney(params[3],SYMB::WICC);
    ComboMoney fee          = RPC_PARAM::GetFee(params, 3, PROPOSAL_REQUEST_TX);
    int32_t validHeight  = chainActive.Height();

    auto pSymbolErr = pCdMan->pAssetCache->CheckTransferCoinSymbol(transferInfo.symbol);
    if (pSymbolErr)
        throw JSONRPCError(REJECT_INVALID, strprintf("Invalid coin symbol=%s! %s", transferInfo.symbol, *pSymbolErr));

    if (transferInfo.GetSawiAmount() == 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Coins is zero!");

    CAccount fromAccount = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, fromUid);
    RPC_PARAM::CheckAccountBalance(fromAccount, transferInfo.symbol, SUB_FREE, transferInfo.GetSawiAmount());

    CAccount txAccount = RPC_PARAM::GetUserAccount(*pCdMan->pAccountCache, txUid);
    RPC_PARAM::CheckAccountBalance(txAccount, fee.symbol, SUB_FREE, fee.GetSawiAmount());


    CCoinTransferProposal proposal ;
    proposal.from_uid = fromUid ;
    proposal.to_uid = toUid ;
    proposal.token = transferInfo.symbol ;
    proposal.amount = transferInfo.GetSawiAmount() ;


    CProposalCreateTx tx ;
    tx.txUid        = txUid;
    tx.llFees       = fee.GetSawiAmount();
    tx.fee_symbol    = fee.symbol ;
    tx.valid_height = validHeight;
    tx.proposalBean = CProposalStorageBean(std::make_shared<CCoinTransferProposal>(proposal)) ;


    return SubmitTx(txAccount.keyid, tx) ;

}

Value getsysparam(const Array& params, bool fHelp){

    if(fHelp || params.size() > 1){

        throw runtime_error(
                "getsysparam \"param_name\"\n"
                "create proposal about param govern\n"
                "\nArguments:\n"
                "1.\"param_name\":      (string, optional) param name, list all parameters when it's absent \n"

                "\nExamples:\n"
                + HelpExampleCli("getsysparam", "")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("getsysparam", "")
        );

    }

    if(params.size() == 1){

        string paramName = params[0].get_str() ;
        SysParamType st ;
        auto itr = paramNameToSysParamTypeMap.find(paramName) ;
        if( itr == paramNameToSysParamTypeMap.end()){
            throw JSONRPCError(RPC_INVALID_PARAMETER, "param name is illegal");
        }
        st = std::get<1>(itr->second) ;
        uint64_t pv ;
        if(!pCdMan->pSysParamCache->GetParam(st, pv)){
            throw JSONRPCError(RPC_INVALID_PARAMETER, "get param error");
        }

        Object obj ;
        obj.push_back(Pair(paramName, pv));
        return obj;

    } else {
        Object obj;
        for(auto kv:paramNameToSysParamTypeMap){
            auto paramName = kv.first ;
            uint64_t pv = 0;
            pCdMan->pSysParamCache->GetParam(std::get<1>(kv.second), pv);

            obj.push_back(Pair(paramName, pv)) ;

        }
        return obj ;
    }
}


Value getcdpparam(const Array& params, bool fHelp){

    if(fHelp || params.size() > 1){

        throw runtime_error(
                "getcdpparam \"bcoin_symbol\" \"scoin_symbol\" \"param_name\" \n"
                "create proposal about param govern\n"
                "\nArguments:\n"
                "1.\"bcoin_symbol\":     (string,required) the base coin symbol\n"
                "2.\"scoin_symbol\":     (string,required) the stable coin symbol\n"
                "3.\"param_name\":      (string, optional) param name, list all parameters when it's absent \n"

                "\nExamples:\n"
                + HelpExampleCli("getcdpparam", "WICC WUSD")
                + "\nAs json rpc call\n"
                + HelpExampleRpc("getcdpparam", "WICC WUSD")
        );

    }

    string bcoinSymbol = params[1].get_str();
    string scoinSymbol = params[2].get_str();
    CCdpCoinPair coinPair = CCdpCoinPair(bcoinSymbol,scoinSymbol) ;

    if(params.size() == 1){

        string paramName = params[0].get_str() ;
        CdpParamType cpt ;
        auto itr = paramNameToCdpParamTypeMap.find(paramName) ;
        if( itr == paramNameToCdpParamTypeMap.end()){
            throw JSONRPCError(RPC_INVALID_PARAMETER, "param name is illegal");
        }
        cpt = std::get<1>(itr->second) ;

        uint64_t pv ;
        if(!pCdMan->pSysParamCache->GetCdpParam(coinPair,cpt, pv)){
            throw JSONRPCError(RPC_INVALID_PARAMETER, "get param error or coin pair error");
        }

        Object obj ;
        obj.push_back(Pair(paramName, pv));
        return obj;
    }else{
        Object obj;
        for(auto kv:paramNameToCdpParamTypeMap){
            auto paramName = kv.first ;
            uint64_t pv ;
            pCdMan->pSysParamCache->GetCdpParam(coinPair,std::get<1>(kv.second), pv);
            obj.push_back(Pair(paramName, pv)) ;
        }
        return obj ;
    }


}

Value getminminerfee(const Array& params, bool fHelp) {
    if(fHelp || params.size() != 0){

        throw runtime_error(
                "getminminerfee\n"
                "\nget all tx minimum fee.\n"
                "\nExamples:\n" +
                HelpExampleCli("getinfo", "") + "\nAs json rpc\n" + HelpExampleRpc("getinfo", ""));
    }

    Array arr;
    for(auto kv: kTxFeeTable){
        Object o ;
        o.push_back(Pair("tx_name", std::get<0>(kv.second)));
        o.push_back(Pair("tx_code", kv.first));
        uint64_t feeOut;
        if(GetTxMinFee(kv.first,chainActive.Height(), SYMB::WICC,feeOut))
            o.push_back(Pair("min_fee_wicc", feeOut));
        if(GetTxMinFee(kv.first,chainActive.Height(), SYMB::WUSD,feeOut))
            o.push_back(Pair("min_fee_wusd", feeOut));
        o.push_back(Pair("can_update", std::get<5>(kv.second)));
        arr.push_back(o);

    }


    return arr ;


}