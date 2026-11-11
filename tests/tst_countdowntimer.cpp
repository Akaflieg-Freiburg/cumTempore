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

#include <QSignalSpy>
#include <QTest>

class TestCountdownTimer : public QObject
{
    Q_OBJECT

private:
    // Fake clock; tests advance it by hand
    qint64 now {1'000'000};

    void advance(CountdownTimer& timer, qint64 ms)
    {
        now += ms;
        timer.update();
    }

    void setUp(CountdownTimer& timer, qint64 durationMs)
    {
        timer.setClock([this]() { return now; });
        timer.setDurationMs(durationMs);
    }

private slots:
    void idleShowsDuration()
    {
        CountdownTimer timer;
        setUp(timer, 90'000);
        QCOMPARE(timer.state(), CountdownTimer::Idle);
        QCOMPARE(timer.remainingMs(), 90'000);
        QCOMPARE(timer.displaySeconds(), 90);
        QCOMPARE(timer.phase(), CountdownTimer::Normal);
        QCOMPARE(timer.totalMs(), 90'000);
    }

    void negativeDurationIsClamped()
    {
        CountdownTimer timer;
        setUp(timer, -5);
        QCOMPARE(timer.durationMs(), 0);
    }

    void zeroDurationDoesNotStart()
    {
        CountdownTimer timer;
        setUp(timer, 0);
        timer.start();
        QCOMPARE(timer.state(), CountdownTimer::Idle);
    }

    void countsDownAndRoundsUp()
    {
        CountdownTimer timer;
        setUp(timer, 10'000);
        timer.start();
        QCOMPARE(timer.state(), CountdownTimer::Running);
        QCOMPARE(timer.displaySeconds(), 10);
        advance(timer, 1);
        QCOMPARE(timer.displaySeconds(), 10);
        advance(timer, 999);
        QCOMPARE(timer.displaySeconds(), 9);
        advance(timer, 8'999);
        QCOMPARE(timer.displaySeconds(), 1);
        advance(timer, 1);
        QCOMPARE(timer.displaySeconds(), 0);
        QCOMPARE(timer.remainingMs(), 0);
    }

    void overtimeCountsUpwards()
    {
        CountdownTimer timer;
        setUp(timer, 1'000);
        timer.start();
        advance(timer, 1'000);
        QCOMPARE(timer.phase(), CountdownTimer::Overtime);
        QCOMPARE(timer.displaySeconds(), 0);
        advance(timer, 999);
        QCOMPARE(timer.displaySeconds(), 0);
        advance(timer, 1);
        QCOMPARE(timer.displaySeconds(), -1);
        advance(timer, 61'000);
        QCOMPARE(timer.displaySeconds(), -62);
        QCOMPARE(timer.state(), CountdownTimer::Running);
        QCOMPARE(timer.progress(), 0.0);
    }

    void phases()
    {
        CountdownTimer timer;
        setUp(timer, 6 * 60'000);
        QSignalSpy phaseSpy(&timer, &CountdownTimer::phaseChanged);
        timer.start();
        QCOMPARE(timer.phase(), CountdownTimer::Normal);
        advance(timer, 60'000 - 1);
        QCOMPARE(timer.phase(), CountdownTimer::Normal);
        advance(timer, 1); // exactly 5:00 left
        QCOMPARE(timer.phase(), CountdownTimer::Warning);
        advance(timer, 5 * 60'000);
        QCOMPARE(timer.phase(), CountdownTimer::Overtime);
        QCOMPARE(phaseSpy.count(), 2);
        timer.reset();
        QCOMPARE(timer.phase(), CountdownTimer::Normal);
        QCOMPARE(phaseSpy.count(), 3);
    }

    void warningThresholdIsConfigurable()
    {
        CountdownTimer timer;
        setUp(timer, 20'000);
        timer.setWarningThresholdMs(10'000);
        timer.start();
        advance(timer, 9'999);
        QCOMPARE(timer.phase(), CountdownTimer::Normal);
        advance(timer, 1);
        QCOMPARE(timer.phase(), CountdownTimer::Warning);
    }

    void pauseAndResume()
    {
        CountdownTimer timer;
        setUp(timer, 60'000);
        timer.start();
        advance(timer, 20'000);
        timer.pause();
        QCOMPARE(timer.state(), CountdownTimer::Paused);
        QCOMPARE(timer.remainingMs(), 40'000);

        // Time passing while paused does not count
        advance(timer, 3'600'000);
        QCOMPARE(timer.remainingMs(), 40'000);

        timer.toggle();
        QCOMPARE(timer.state(), CountdownTimer::Running);
        advance(timer, 10'000);
        QCOMPARE(timer.remainingMs(), 30'000);

        timer.toggle();
        QCOMPARE(timer.state(), CountdownTimer::Paused);
    }

    void largeJumpsAreHandled()
    {
        // The app was suspended for a while: the timer still shows the
        // correct value once it gets to run again.
        CountdownTimer timer;
        setUp(timer, 45 * 60'000);
        timer.start();
        advance(timer, 50 * 60'000);
        QCOMPARE(timer.phase(), CountdownTimer::Overtime);
        QCOMPARE(timer.displaySeconds(), -5 * 60);
    }

    void resetRestoresDuration()
    {
        CountdownTimer timer;
        setUp(timer, 60'000);
        timer.start();
        advance(timer, 20'000);
        timer.addMinute();
        timer.reset();
        QCOMPARE(timer.state(), CountdownTimer::Idle);
        QCOMPARE(timer.remainingMs(), 60'000);
        QCOMPARE(timer.totalMs(), 60'000);
    }

    void durationChangeWhileRunningKeepsRun()
    {
        CountdownTimer timer;
        setUp(timer, 60'000);
        timer.start();
        timer.setDurationMs(10'000);
        QCOMPARE(timer.remainingMs(), 60'000);
        QCOMPARE(timer.totalMs(), 60'000);
        timer.reset();
        QCOMPARE(timer.remainingMs(), 10'000);
    }

    void addMinute()
    {
        CountdownTimer timer;
        setUp(timer, 120'000);

        // No effect when idle
        timer.addMinute();
        QCOMPARE(timer.remainingMs(), 120'000);

        timer.start();
        advance(timer, 30'000);
        timer.addMinute();
        QCOMPARE(timer.remainingMs(), 150'000);
        QCOMPARE(timer.totalMs(), 150'000);
        QCOMPARE(timer.progress(), 1.0);

        // In overtime, adding a minute leaves one minute
        advance(timer, 200'000);
        QCOMPARE(timer.phase(), CountdownTimer::Overtime);
        timer.addMinute();
        QCOMPARE(timer.remainingMs(), 60'000);
        QCOMPARE(timer.phase(), CountdownTimer::Warning);

        // Also while paused
        timer.pause();
        timer.addMinute();
        QCOMPARE(timer.remainingMs(), 120'000);
    }

    void progress()
    {
        CountdownTimer timer;
        setUp(timer, 100'000);
        QCOMPARE(timer.progress(), 1.0);
        timer.start();
        advance(timer, 25'000);
        QCOMPARE(timer.progress(), 0.75);
    }

    void displaySecondsSignalOncePerSecond()
    {
        CountdownTimer timer;
        setUp(timer, 10'000);
        timer.start();
        QSignalSpy spy(&timer, &CountdownTimer::displaySecondsChanged);
        for (int i = 0; i < 20; ++i)
        {
            advance(timer, 100);
        }
        QCOMPARE(spy.count(), 2);
    }

    void realClockTicks()
    {
        // Smoke test with the real clock and the internal tick timer
        CountdownTimer timer;
        timer.setDurationMs(60'000);
        QSignalSpy spy(&timer, &CountdownTimer::remainingMsChanged);
        timer.start();
        QTRY_VERIFY_WITH_TIMEOUT(spy.count() >= 3, 2000);
        QVERIFY(timer.remainingMs() < 60'000);
    }
};

QTEST_GUILESS_MAIN(TestCountdownTimer)
#include "tst_countdowntimer.moc"
