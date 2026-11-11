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

#pragma once

#include <QObject>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

#include <functional>

/*! \brief Countdown timer that keeps counting into overtime
 *
 *  The timer stores the absolute end time rather than counting ticks, so it
 *  stays correct when the event loop is blocked or the app is suspended. It
 *  uses wall-clock time (milliseconds since the epoch) on purpose: monotonic
 *  clocks stop while a mobile device sleeps.
 *
 *  Once the end time has passed, remainingMs becomes negative and the timer
 *  keeps running; the magnitude is the overtime.
 */
class CountdownTimer : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum State : quint8
    {
        Idle,    ///< Not started; remainingMs equals durationMs
        Running, ///< Counting down, or counting overtime
        Paused   ///< Stopped, remembers the remaining time
    };
    Q_ENUM(State)

    enum Phase : quint8
    {
        Normal,   ///< Idle, or more than warningThresholdMs left
        Warning,  ///< Running/paused with at most warningThresholdMs left
        Overtime  ///< Running/paused with no time left
    };
    Q_ENUM(Phase)

    using Clock = std::function<qint64()>;

    explicit CountdownTimer(QObject* parent = nullptr);

    /*! \brief Duration the timer starts with, in milliseconds
     *
     *  Changing the duration while the timer is running or paused does not
     *  affect the current run.
     */
    Q_PROPERTY(qint64 durationMs READ durationMs WRITE setDurationMs NOTIFY durationMsChanged)
    [[nodiscard]] qint64 durationMs() const { return m_durationMs; }
    void setDurationMs(qint64 durationMs);

    /*! \brief Total length of the current run, including added minutes
     *
     *  Equals durationMs when idle. Used to compute the progress.
     */
    Q_PROPERTY(qint64 totalMs READ totalMs NOTIFY totalMsChanged)
    [[nodiscard]] qint64 totalMs() const { return m_totalMs; }

    /*! \brief Remaining time in milliseconds; negative in overtime */
    Q_PROPERTY(qint64 remainingMs READ remainingMs NOTIFY remainingMsChanged)
    [[nodiscard]] qint64 remainingMs() const;

    /*! \brief Remaining time in whole seconds, as shown to the user
     *
     *  Positive values are rounded up, so that a fresh 5-minute timer shows
     *  5:00 for its first second and the display reaches 0:00 exactly when
     *  the time is up. Negative values are the elapsed overtime, rounded
     *  down. This property changes once per second, so it is cheap to bind
     *  text to.
     */
    Q_PROPERTY(qint64 displaySeconds READ displaySeconds NOTIFY displaySecondsChanged)
    [[nodiscard]] qint64 displaySeconds() const;

    /*! \brief Fraction of the run that remains, between 0 and 1 */
    Q_PROPERTY(double progress READ progress NOTIFY remainingMsChanged)
    [[nodiscard]] double progress() const;

    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    [[nodiscard]] State state() const { return m_state; }

    Q_PROPERTY(Phase phase READ phase NOTIFY phaseChanged)
    [[nodiscard]] Phase phase() const;

    /*! \brief Remaining time at or below which the phase is Warning */
    Q_PROPERTY(qint64 warningThresholdMs READ warningThresholdMs WRITE setWarningThresholdMs NOTIFY warningThresholdMsChanged)
    [[nodiscard]] qint64 warningThresholdMs() const { return m_warningThresholdMs; }
    void setWarningThresholdMs(qint64 thresholdMs);

    /*! \brief Starts an idle timer; resumes a paused one */
    Q_INVOKABLE void start();

    /*! \brief Pauses a running timer */
    Q_INVOKABLE void pause();

    /*! \brief Pauses a running timer, starts/resumes otherwise */
    Q_INVOKABLE void toggle();

    /*! \brief Returns to the idle state */
    Q_INVOKABLE void reset();

    /*! \brief Adds one minute to a running or paused timer */
    Q_INVOKABLE void addMinute();

    /*! \brief Replaces the clock, for unit tests */
    void setClock(Clock clock);

    /*! \brief Re-reads the clock and emits change signals
     *
     *  Called by the internal tick timer. Unit tests call it after advancing
     *  their fake clock.
     */
    void update();

signals:
    void durationMsChanged();
    void totalMsChanged();
    void remainingMsChanged();
    void displaySecondsChanged();
    void stateChanged();
    void phaseChanged();
    void warningThresholdMsChanged();

private:
    void setState(State state);
    void setTotalMs(qint64 totalMs);

    Clock m_clock;
    QTimer m_tick;

    qint64 m_durationMs {0};
    qint64 m_totalMs {0};
    qint64 m_warningThresholdMs {5 * 60 * 1000};
    State m_state {Idle};

    // Valid while running: end time in clock milliseconds
    qint64 m_endMs {0};
    // Valid while paused: remaining milliseconds
    qint64 m_pausedRemainingMs {0};

    // Cached values, used to emit change signals only on actual changes
    qint64 m_lastDisplaySeconds {0};
    Phase m_lastPhase {Normal};
};
