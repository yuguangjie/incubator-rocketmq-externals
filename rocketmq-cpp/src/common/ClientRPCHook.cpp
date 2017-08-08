/********************************************************************
author: qiwei.wqw@alibaba-inc.com
*********************************************************************/
#include "ClientRPCHook.h"
#include "CommandHeader.h"
#include "Logging.h"
#include "spas_client.h"
#include "string"

namespace metaq {

const string SessionCredentials::AccessKey = "AccessKey";
const string SessionCredentials::SecretKey = "SecretKey";
const string SessionCredentials::Signature = "Signature";
const string SessionCredentials::SignatureMethod = "SignatureMethod";
const string SessionCredentials::ONSChannelKey = "OnsChannel";

void ClientRPCHook::doBeforeRequest(const string& remoteAddr,
                                    RemotingCommand& request) {
  CommandHeader* header = request.getCommandHeader();

  map<string, string> requestMap;
  string totalMsg;

  requestMap.insert(pair<string, string>(SessionCredentials::AccessKey,
                                         sessionCredentials.getAccessKey()));
  requestMap.insert(pair<string, string>(SessionCredentials::ONSChannelKey,
                                         sessionCredentials.getAuthChannel()));

  LOG_DEBUG("before insert declared filed,MAP SIZE is:%zu", requestMap.size());
  if (header != NULL) {
    header->SetDeclaredFieldOfCommandHeader(requestMap);
  }
  LOG_DEBUG("after insert declared filed, MAP SIZE is:%zu", requestMap.size());

  map<string, string>::iterator it = requestMap.begin();
  for (; it != requestMap.end(); ++it) {
    totalMsg.append(it->second);
  }
  if (request.getMsgBody().length() > 0) {
    LOG_DEBUG("msgBody is:%s, msgBody length is:%zu",
              request.getMsgBody().c_str(), request.getMsgBody().length());

    totalMsg.append(request.getMsgBody());
  }
  LOG_DEBUG("total msg info are:%s, size is:%zu", totalMsg.c_str(),
            totalMsg.size());
  char* pSignature =
      metaqSignature::spas_sign(totalMsg.c_str(), totalMsg.size(),
                                sessionCredentials.getSecretKey().c_str());
  // char *pSignature = spas_sign(totalMsg.c_str(),
  // sessionCredentials.getSecretKey().c_str());

  if (pSignature != NULL) {
    string signature(static_cast<const char*>(pSignature));
    request.addExtField(SessionCredentials::Signature, signature);
    request.addExtField(SessionCredentials::AccessKey,
                        sessionCredentials.getAccessKey());
    request.addExtField(SessionCredentials::ONSChannelKey,
                        sessionCredentials.getAuthChannel());
    metaqSignature::spas_mem_free(pSignature);
  } else {
    LOG_ERROR("signature for request failed");
  }
}
}