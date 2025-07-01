//===================================================================
// Filename:  EventMgr.h
// Desc:      something like the EventBus pattern
//
// Created:   01.07.2025  by DimaSkup
//===================================================================
#ifndef EVENT_MGR_H
#define EVENT_MGR_H

#include "Event.h"
#include <deque>

class EventMgr
{
public:
    inline void AddEvent(Event event)
    {
        m_Events.push_back(event);
    }

public:
    std::deque<Event> m_Events;
};

//===================================================================
// Global instance of the event manager
//===================================================================
extern EventMgr g_EventMgr;

#endif
