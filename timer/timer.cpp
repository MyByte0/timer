#include "timer.h"
#include <iterator>

std::atomic<int64_t> Timer::s_nCountCreate(0);

TimerQueue::TimerQueue() : m_timers(), m_head_time(INTPTR_MAX)
{
}

TimerQueue::~TimerQueue()
{
  for(auto &elem:m_timers)
  {
    delete elem.second;
  }
}

TimerFd TimerQueue::addTimer(TimerCallFunction callback, int64_t time_when, int64_t interval)
{
  Timer *timer = new Timer(std::move(callback), time_when, interval);
  addTimerInLoop(timer);
  return TimerFd(timer, timer->getSequence());
}

void TimerQueue::cancel(TimerFd timerFd)
{
	cancelInLoop(timerFd);
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	bool earliestTrigger = insert(timer);
}

void TimerQueue::cancelInLoop(TimerFd timerFd)
{
	ActiveTimer timer(timerFd.m_timer, timerFd.m_nSequence);
	ActiveTimerSet::iterator it = m_activeTimers.find(timer);

	
	if (it!= m_activeTimers.end()) 
	{
		size_t n = m_timers.erase(Entry(it->first->getExpiration(), it->first));
		delete it->first;
		m_activeTimers.erase(it);
	}
	
}

void TimerQueue::handleRead(int64_t time_now)
{
	if (time_now<m_head_time)
	{
		return;
	}
	
	std::vector<Entry> expired = getExpired(time_now);

	for(auto& elem:expired)
	{
		elem.second->call();
	}

	reset(expired, time_now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(int64_t now)
{
	std::vector<Entry> expired;
	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = m_timers.lower_bound(sentry);

	std::copy(m_timers.begin(), end, std::back_inserter(expired));
	m_timers.erase(m_timers.begin(), end);
	
	for (auto &elem : expired)
	{
		ActiveTimer timer(elem.second, elem.second->getSequence());
		m_activeTimers.erase(timer);
	}

	return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired, int64_t now)
{
  int64_t next_expired = 0;

  for (auto &elem : expired)
  {
	  ActiveTimer timer(elem.second, elem.second->getSequence());

	  if (elem.second->isRepeated())
	  {
        elem.second->restart(now);
		insert(elem.second);
	  }
	  
	  else
	  {
		delete elem.second;
	  }
  }

  if (!m_timers.empty())
  {
	  next_expired = m_timers.begin()->second->getExpiration();
  }
}

bool TimerQueue::insert(Timer *timer)
{
  bool earliestTrigger = false;
  int64_t when = timer->getExpiration();

  TimerList::iterator it = m_timers.begin();

  if (it == m_timers.end() || when<it->first)
  {
    m_head_time = when;
	earliestTrigger = true;
  }

  m_timers.insert(Entry(when, timer));
  m_activeTimers.insert(ActiveTimer(timer, timer->getSequence()));

  return earliestTrigger;
}