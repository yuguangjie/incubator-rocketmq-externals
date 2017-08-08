/********************************************************************
author: qiwei.wqw@alibab-inc.com
*********************************************************************/
#ifndef __DEFAULTMQPULLCONSUMER_H__
#define __DEFAULTMQPULLCONSUMER_H__

#include <set>
#include <string>
#include "MQConsumer.h"
#include "MQMessageQueue.h"
#include "MQueueListener.h"
#include "RocketMQClient.h"

namespace metaq {
class Rebalance;
class SubscriptionData;
class OffsetStore;
class PullAPIWrapper;
class ConsumerRunningInfo;
//<!***************************************************************************
class ROCKETMQCLIENT_API DefaultMQPullConsumer : public MQConsumer {
 public:
  DefaultMQPullConsumer(const string& groupname);
  virtual ~DefaultMQPullConsumer();

  //<!begin mqadmin;
  virtual void start();
  virtual void shutdown();
  //<!end mqadmin;

  //<!begin MQConsumer
  virtual void sendMessageBack(MQMessageExt& msg, int delayLevel);
  virtual void fetchSubscribeMessageQueues(const string& topic,
                                           vector<MQMessageQueue>& mqs);
  virtual void doRebalance();
  virtual void persistConsumerOffset();
  virtual void persistConsumerOffsetByResetOffset();
  virtual void updateTopicSubscribeInfo(const string& topic,
                                        vector<MQMessageQueue>& info);
  virtual ConsumeType getConsumeType();
  virtual ConsumeFromWhere getConsumeFromWhere();
  virtual void getSubscriptions(vector<SubscriptionData>&);
  virtual void updateConsumeOffset(const MQMessageQueue& mq, int64 offset);
  virtual void removeConsumeOffset(const MQMessageQueue& mq);
  virtual void producePullMsgTask(PullRequest*);
  virtual Rebalance* getRebalance() const;
  //<!end MQConsumer;

  void registerMessageQueueListener(const string& topic,
                                    MQueueListener* pListener);
  /**
  * pull msg from specified queue, if no msg in queue, return directly
  *
  * @param mq
  *            specify the pulled queue
  * @param subExpression
  *            set filter expression for pulled msg, broker will filter msg actively
  *            Now only OR operation is supported, eg: "tag1 || tag2 || tag3"
  *            if subExpression is setted to "null" or "*"��all msg will be subscribed
  * @param offset
  *            specify the started pull offset
  * @param maxNums
  *            specify max msg num by per pull
  * @return  
  *            accroding to PullResult
  */
  virtual PullResult pull(const MQMessageQueue& mq, const string& subExpression,
                          int64 offset, int maxNums);
  virtual void pull(const MQMessageQueue& mq, const string& subExpression,
                    int64 offset, int maxNums, PullCallback* pPullCallback);

  /**
  * pull msg from specified queue, if no msg, broker will suspend the pull request 20s
  *
  * @param mq
  *            specify the pulled queue
  * @param subExpression
  *            set filter expression for pulled msg, broker will filter msg actively
  *            Now only OR operation is supported, eg: "tag1 || tag2 || tag3"
  *            if subExpression is setted to "null" or "*"��all msg will be subscribed
  * @param offset
  *            specify the started pull offset
  * @param maxNums
  *            specify max msg num by per pull
  * @return  
  *            accroding to PullResult
  */
  PullResult pullBlockIfNotFound(const MQMessageQueue& mq,
                                 const string& subExpression, int64 offset,
                                 int maxNums);
  void pullBlockIfNotFound(const MQMessageQueue& mq,
                           const string& subExpression, int64 offset,
                           int maxNums, PullCallback* pPullCallback);

  virtual ConsumerRunningInfo* getConsumerRunningInfo() { return NULL; }
  /**
  * ��ȡ���ѽ��ȣ�����-1��ʾ����
  *
  * @param mq
  * @param fromStore
  * @return
  */
  int64 fetchConsumeOffset(const MQMessageQueue& mq, bool fromStore);
  /**
  * ����topic��ȡMessageQueue���Ծ��ⷽʽ�����ڶ����Ա֮�����
  *
  * @param topic
  *            ��ϢTopic
  * @return ���ض��м���
  */
  void fetchMessageQueuesInBalance(const string& topic,
                                   vector<MQMessageQueue> mqs);

  // temp persist consumer offset interface, only valid with
  // RemoteBrokerOffsetStore, updateConsumeOffset should be called before.
  void persistConsumerOffset4PullConsumer(const MQMessageQueue& mq);

 private:
  void checkConfig();
  void copySubscription();

  PullResult pullSyncImpl(const MQMessageQueue& mq, const string& subExpression,
                          int64 offset, int maxNums, bool block);

  void pullAsyncImpl(const MQMessageQueue& mq, const string& subExpression,
                     int64 offset, int maxNums, bool block,
                     PullCallback* pPullCallback);

  void subscriptionAutomatically(const string& topic);

 private:
  set<string> m_registerTopics;

  MQueueListener* m_pMessageQueueListener;
  OffsetStore* m_pOffsetStore;
  Rebalance* m_pRebalance;
  PullAPIWrapper* m_pPullAPIWrapper;
};
//<!***************************************************************************
}  //<!end namespace;
#endif