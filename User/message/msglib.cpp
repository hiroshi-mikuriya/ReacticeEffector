/// @file      message/msglib.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "msglib.h"
#include <cstring> // memcpy

namespace
{
/// @brief メール情報
struct MailInfo
{
  osThreadId threadId; ///< タスクID
  osMailQId mailId;    ///< メールID
};
/// 最大メール数
constexpr uint32_t MAX_MAIL_INFO_COUNT = 8;
/// メール情報実態
MailInfo s_mails[MAX_MAIL_INFO_COUNT] = {};
/// @brief 指定したタスクIDと一致するメール情報取得
/// @param[in] threadId タスクID
/// @retval 0以外　メール情報のポインタ
/// @retval 0 見つからない
MailInfo *find(osThreadId threadId)
{
  for (uint32_t i = 0; i < MAX_MAIL_INFO_COUNT; ++i)
  {
    MailInfo *info = &s_mails[i];
    if (info->threadId == threadId)
    {
      return info;
    }
  }
  return 0;
}
} // namespace

osStatus satoh::addMsgTarget(uint32_t msgCount) noexcept
{
  osThreadId threadId = osThreadGetId();
  if (threadId == 0)
  {
    return osErrorOS;
  }
  if (find(threadId) != 0)
  {
    return osErrorValue;
  }
  MailInfo *info = find(0);
  if (info == 0)
  {
    return osErrorNoMemory;
  }
  info->threadId = threadId;
  osMailQDef_t mail{};
  mail.item_sz = sizeof(Message);
  mail.queue_sz = msgCount;
  info->mailId = osMailCreate(&mail, threadId);
  return info->mailId ? osOK : osErrorResource;
}

osStatus satoh::sendMsg(osThreadId threadId, msg::ID type) noexcept
{
  return sendMsg(threadId, type, 0, 0);
}

osStatus satoh::sendMsg(osThreadId threadId, msg::ID type, void const *bytes, uint16_t size) noexcept
{
  MailInfo *info = find(threadId);
  if (info == 0)
  {
    return osErrorParameter;
  }
  Message *m = static_cast<Message *>(osMailAlloc(info->mailId, 0));
  if (m == 0)
  {
    return osEventTimeout;
  }
  m->type = type;
  m->size = size;
  if (0 < size && bytes)
  {
    memcpy(m->bytes, bytes, size);
  }
  return osMailPut(info->mailId, m);
}

satoh::Result satoh::recvMsg(uint32_t millisec) noexcept
{
  osThreadId threadId = osThreadGetId();
  if (threadId == 0)
  {
    return Result(osErrorOS);
  }
  MailInfo *info = find(threadId);
  if (info == 0)
  {
    return Result(osErrorParameter);
  }
  osEvent res = osMailGet(info->mailId, millisec);
  if (res.status != osEventMail)
  {
    return Result(res.status);
  }
  Message *m = static_cast<Message *>(res.value.p);
  osStatus st = m ? osOK : osErrorValue;
  return Result(st, m, info->mailId);
}

satoh::Result::Result(osStatus status, Message *msg, osMailQId mail) noexcept //
    : status_(status),                                                        //
      msg_(msg),                                                              //
      mail_(mail)                                                             //
{
}
satoh::Result::Result(osStatus status) noexcept //
    : status_(status),                          //
      msg_(0),                                  //
      mail_(0)                                  //
{
}
satoh::Result::~Result()
{
  reset();
}
satoh::Result::Result(Result &&that) noexcept //
    : status_(that.status_),                  //
      msg_(that.msg_),                        //
      mail_(that.mail_)                       //
{
  that.status_ = osOK;
  that.msg_ = 0;
  that.mail_ = 0;
}
satoh::Result &satoh::Result::operator=(Result &&that) noexcept
{
  if (this != &that)
  {
    reset();
    status_ = that.status_;
    msg_ = that.msg_;
    mail_ = that.mail_;
    that.status_ = osOK;
    that.msg_ = 0;
    that.mail_ = 0;
  }
  return *this;
}
void satoh::Result::reset() noexcept
{
  if (msg_)
  {
    osMailFree(mail_, msg_);
    msg_ = 0;
  }
}
osStatus satoh::Result::status() const noexcept
{
  return status_;
}
satoh::Message const *satoh::Result::msg() const noexcept
{
  return msg_;
}