/**
 a instance timer for windows and linux
**/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <atomic>
#include <functional>
#include <cstdint>

typedef std::function<void()> TimerCallFunction;

class Timer
{
public:
  Timer(TimerCallFunction callback, int64_t time, int64_t interval)
    : m_callback(std::move(callback)), m_nExpiration(time), m_nInterval(interval), m_bRepeated(interval > 0), m_nSequence(++s_nCountCreate)
  {}

  void call() const
  {
    m_callback();
  }
  
  int64_t getExpiration() const{
    return m_nExpiration;
  }
  int64_t getSequence() const{
    return m_nSequence;
  }
  bool isRepeated() const{
    return m_bRepeated;
  }
  
  void restart(int64_t timeNow)
  {
  
    if (m_bRepeated) 
    {
      m_nExpiration = timeNow + m_nInterval;
    }
    else
    {
      m_nExpiration = 0;
    }
  
  }
  
  static int64_t getCount()
  {
    return s_nCountCreate;
  }

private:
  static std::atomic<int64_t> s_nCountCreate;

  const TimerCallFunction m_callback;
  int64_t m_nExpiration;
  const int64_t m_nInterval;
  const int64_t m_nSequence;
  const bool m_bRepeated;
}

class TimerFd{
public:
  TimerFd() : m_timer(NULL), m_nSequence(0)
  {}

  TimerFd(Timer *timer, int64_t seq) : m_timer(timer), m_nSequence(seq)
  {}

private:
  Timer*    m_timer;
  int64_t   m_nSequence;
}

#endif