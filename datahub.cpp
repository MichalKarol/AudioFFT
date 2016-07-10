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
#include "datahub.h"

DataHub::DataHub(Widget* widget, QObject *parent): QObject(parent), widget(widget) {
    thread[0] = new QThread(this);
    thread[0]->setObjectName("AudioInput thread");
    audioInput = new AudioInput();
    audioInput->moveToThread(thread[0]);
    connect(thread[0], &QThread::started, audioInput, &AudioInput::initialize);
    connect(audioInput, &AudioInput::destroyed, thread[0], &QThread::quit);
    thread[0]->start();

    widget->setAudioDevices(audioInput->availableAudioDevices());

    thread[1] = new QThread(this);
    thread[1]->setObjectName("FFT thread");
    fftModule = new FFTModule();
    fftModule->moveToThread(thread[1]);
    connect(thread[1], &QThread::started, fftModule, &FFTModule::initialize);
    connect(fftModule, &AudioInput::destroyed, thread[1], &QThread::quit);
    thread[1]->start();

    // Connections in data hub
    // dataHub connections
    connect(widget, &Widget::fftSizeChange, this, &DataHub::clearEventQueue);
    connect(this, &DataHub::error, widget, &Widget::error);

    // audioInput connections
    connect(widget, &Widget::audioDeviceChange, audioInput, &AudioInput::setAudioDevice);
    connect(widget, &Widget::audioDeviceStateChange, audioInput, &AudioInput::setAudioDeviceState);
    connect(widget, &Widget::fftSizeChange, audioInput, &AudioInput::setFFTSize);
    connect(audioInput, &AudioInput::audioDeviceStateChange, widget, &Widget::audioDeviceStateChanged);
    connect(audioInput, &AudioInput::signalData, fftModule, &FFTModule::signalData);
    connect(audioInput, &AudioInput::error, widget, &Widget::error);

    // fftModule connections
    connect(widget, &Widget::fftSizeChange, fftModule, &FFTModule::setFFTSize);
    connect(fftModule, &FFTModule::calculated, widget, &Widget::drawData);
    connect(fftModule, &FFTModule::error, widget, &Widget::error);
}

DataHub::~DataHub() {
    audioInput->deleteLater(); audioInput = nullptr;
    fftModule->deleteLater(); fftModule = nullptr;
    for (int i = 0; i < 2; i++) {
        thread[i]->quit();
        thread[i]->wait();
        delete thread[i]; thread[i] = nullptr;
    }
}

void DataHub::clearEventQueue() {
    disconnect(audioInput, &AudioInput::signalData, fftModule, &FFTModule::signalData);
    disconnect(fftModule, &FFTModule::calculated, widget, &Widget::drawData);

    connect(audioInput, &AudioInput::signalData, fftModule, &FFTModule::signalData);
    connect(fftModule, &FFTModule::calculated, widget, &Widget::drawData);
}
