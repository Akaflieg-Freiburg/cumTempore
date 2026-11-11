/***************************************************************************
 *   Copyright (C) 2026 by Stefan Kebekus                                  *
 *   stefan.kebekus@math.uni-freiburg.de                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <https://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "CountdownTimer.h"

#include <QDateTime>

#include <algorithm>

namespace {

// Interval at which the remaining time is refreshed while running. Short
// enough for a smoothly moving progress ring.
constexpr int tickIntervalMs = 100;

constexpr qint64 msPerSecond = 1000;
constexpr qint64 msPerMinute = 60 * msPerSecond;

} // namespace


CountdownTimer::CountdownTimer(QObject* parent)
    : QObject(parent)
    , m_clock([]() { return QDateTime::currentMSecsSinceEpoch(); })
{
    m_tick.setTimerType(Qt::PreciseTimer);
    m_tick.setInterval(tickIntervalMs);
    connect(&m_tick, &QTimer::timeout, this, &CountdownTimer::update);
}


//
// Properties
//

void CountdownTimer::setDurationMs(qint64 durationMs)
{
    durationMs = std::max<qint64>(durationMs, 0);
    if (durationMs == m_durationMs)
    {
        return;
    }
    m_durationMs = durationMs;
    emit durationMsChanged();

    if (m_state == Idle)
    {
        setTotalMs(m_durationMs);
        update();
    }
}


qint64 CountdownTimer::remainingMs() const
{
    switch (m_state)
    {
    case Running:
        return m_endMs - m_clock();
    case Paused:
        return m_pausedRemainingMs;
    case Idle:
        break;
    }
    return m_durationMs;
}


qint64 CountdownTimer::displaySeconds() const
{
    auto const remaining = remainingMs();
    if (remaining > 0)
    {
        // Round up
        return (remaining + msPerSecond - 1) / msPerSecond;
    }
    // Overtime: round the elapsed overtime down, keep the sign
    return -((-remaining) / msPerSecond);
}


double CountdownTimer::progress() const
{
    if (m_totalMs <= 0)
    {
        return 0.0;
    }
    return std::clamp(static_cast<double>(remainingMs()) / static_cast<double>(m_totalMs), 0.0, 1.0);
}


CountdownTimer::Phase CountdownTimer::phase() const
{
    if (m_state == Idle)
    {
        return Normal;
    }
    auto const remaining = remainingMs();
    if (remaining <= 0)
    {
        return Overtime;
    }
    if (remaining <= m_warningThresholdMs)
    {
        return Warning;
    }
    return Normal;
}


void CountdownTimer::setWarningThresholdMs(qint64 thresholdMs)
{
    if (thresholdMs == m_warningThresholdMs)
    {
        return;
    }
    m_warningThresholdMs = thresholdMs;
    emit warningThresholdMsChanged();
    update();
}


//
// Actions
//

void CountdownTimer::start()
{
    switch (m_state)
    {
    case Running:
        return;
    case Idle:
        if (m_durationMs <= 0)
        {
            return;
        }
        setTotalMs(m_durationMs);
        m_endMs = m_clock() + m_durationMs;
        break;
    case Paused:
        m_endMs = m_clock() + m_pausedRemainingMs;
        break;
    }
    setState(Running);
    m_tick.start();
    update();
}


void CountdownTimer::pause()
{
    if (m_state != Running)
    {
        return;
    }
    m_pausedRemainingMs = m_endMs - m_clock();
    m_tick.stop();
    setState(Paused);
    update();
}


void CountdownTimer::toggle()
{
    if (m_state == Running)
    {
        pause();
    }
    else
    {
        start();
    }
}


void CountdownTimer::reset()
{
    m_tick.stop();
    setState(Idle);
    setTotalMs(m_durationMs);
    update();
}


void CountdownTimer::addMinute()
{
    switch (m_state)
    {
    case Idle:
        return;
    case Running:
        // In overtime, restart the countdown from one minute rather than
        // eating into the overtime.
        m_endMs = std::max(m_endMs, m_clock()) + msPerMinute;
        break;
    case Paused:
        m_pausedRemainingMs = std::max<qint64>(m_pausedRemainingMs, 0) + msPerMinute;
        break;
    }
    // The ring shows the remaining part of the total; enlarge the total if
    // the remaining time would exceed it.
    setTotalMs(std::max(m_totalMs, remainingMs()));
    update();
}


//
// Internals
//

void CountdownTimer::setClock(Clock clock)
{
    m_clock = std::move(clock);
    update();
}


void CountdownTimer::update()
{
    emit remainingMsChanged();

    auto const seconds = displaySeconds();
    if (seconds != m_lastDisplaySeconds)
    {
        m_lastDisplaySeconds = seconds;
        emit displaySecondsChanged();
    }

    auto const currentPhase = phase();
    if (currentPhase != m_lastPhase)
    {
        m_lastPhase = currentPhase;
        emit phaseChanged();
    }
}


void CountdownTimer::setState(State state)
{
    if (state == m_state)
    {
        return;
    }
    m_state = state;
    emit stateChanged();
}


void CountdownTimer::setTotalMs(qint64 totalMs)
{
    if (totalMs == m_totalMs)
    {
        return;
    }
    m_totalMs = totalMs;
    emit totalMsChanged();
}
