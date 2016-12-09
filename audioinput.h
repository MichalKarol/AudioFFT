//    Copyright (C) 2016 Michał Karol <michal.p.karol@gmail.com>

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include "globals.h"
#include <QObject>
#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class AudioInput : public QObject {
    Q_OBJECT
public:
    explicit AudioInput(QObject *parent = 0);
    ~AudioInput();
    QStringList availableAudioDevices();

private:
    QAudioInput* input = nullptr;
    QAudioFormat format;
    QList <QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    uint samplesAtOnce = 1; // samples number change depending on load
    uint fftSize = DEFAULT_FFT_SIZE; // Default FFT size
    int step =  1; // Used for tuning samplesAtOnce value
    int counter = 0; // Checking number of errors in time
    int errors = 0; // Number of errors (samplesAtOnce value too small)

    QIODevice* device = nullptr;
    QByteArray buffer;

    uint cutoffValue();

public slots:
    void initialize();
    void setAudioDevice(uint audioDeviceId);
    void setAudioDeviceState(bool state);
    void setFFTSize(uint fftSize);

private slots:
    void readSignal();
    void stateChanged(QAudio::State state);

signals:
    void audioDeviceStateChange(bool state);
    void signalData(shared_ptr<vector<vector<float>>> data);
    void error(QString error);
};

#endif // AUDIOINPUT_H
