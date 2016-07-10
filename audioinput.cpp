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
#include "audioinput.h"
#include <QtEndian>

AudioInput::AudioInput(QObject *parent) : QObject(parent) {
}
AudioInput::~AudioInput() {
    delete input; input = nullptr;
    buffer.clear();
}

QStringList AudioInput::availableAudioDevices() {
    QStringList output;
    for (QAudioDeviceInfo info : devices) {
        output.append(info.deviceName());
    }
    return output;
}

uint AudioInput::cutoffValue() {
    return samplesAtOnce * fftSize * sizeof(float);
}

// Public slots
void AudioInput::initialize() {
    format = QAudioDeviceInfo::defaultInputDevice().preferredFormat();
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::Float);


    setFFTSize(fftSize);
    setAudioDevice(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).indexOf(QAudioDeviceInfo::defaultInputDevice()));

}

void AudioInput::setAudioDevice(uint audioDeviceId) {
    if (!devices.isEmpty() && audioDeviceId < static_cast<uint>(devices.size())) {
        if (devices[audioDeviceId].isFormatSupported(format)) {
            delete input; input = nullptr;
            device = nullptr;

            input = new QAudioInput(devices[audioDeviceId], format, this);
            connect(input, &QAudioInput::stateChanged, this, &AudioInput::stateChanged);
            emit audioDeviceStateChange(false);
        } else {
           emit error("Format is not supported. Cannot record using that device.");
        }
    }
}
void AudioInput::setAudioDeviceState(bool state) {
    if (input != nullptr) {
        if (state) {
            device = input->start();

            if (device != nullptr) {
                connect(device, &QIODevice::readyRead, this, &AudioInput::readSignal);
                emit audioDeviceStateChange(true);
            } else {
                emit error("Cannot read from device.");
            }
        } else {
            input->stop();
            device = nullptr;
            emit audioDeviceStateChange(false);
        }
    } else {
        emit error("Input device is not properly set");
    }
}
void AudioInput::setFFTSize(uint fftSize) {
    this->fftSize = fftSize;
    samplesAtOnce = ((4096 / fftSize) + 1);
    step = format.bytesForDuration(2000000) / (fftSize * sizeof(float)) + 1; // Tune samplesAtOnce value every 2 sec
    counter = 0;
    errors = 0;
    buffer.clear();
    buffer.reserve(20 * cutoffValue());
}

// Private slots
void AudioInput::readSignal() {
    buffer.append(device->readAll());

    if (static_cast<uint>(buffer.size()) > cutoffValue()) {
        uchar* bufferPointer = reinterpret_cast<uchar*>(buffer.data());
        float** data = new float*[samplesAtOnce];

        for (uint i = 0; i < samplesAtOnce; i++) {
            data[i] = new float[fftSize];
            for (uint j = 0; j < fftSize; j++) {
                data[i][j] = qFromLittleEndian<float>(bufferPointer + (((i * fftSize) + j) * sizeof(float)));
            }
        }

        buffer.remove(0, cutoffValue());
        emit signalData(data, samplesAtOnce);

        // Tuning samples number
        counter++;
        errors += (buffer.size() > static_cast<int>(10 * fftSize * sizeof(float)) ? +1 : -1);

        if (counter == step) {
            if (abs((errors * 10) / step) > 2) {
                samplesAtOnce = std::max(1U, samplesAtOnce + (errors > 0 ? +1 : -1));
            }
            errors = 0;
            counter = 0;
        }
    }
}
void AudioInput::stateChanged(QAudio::State state) {
    if (state == QAudio::StoppedState && input->error() != QAudio::NoError) {
        emit error("Input device sttoped due to the error nr " + QString::number(static_cast<uint>(input->error())));
        emit audioDeviceStateChange(false);
    }
}
