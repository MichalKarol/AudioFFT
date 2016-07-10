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
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void setAudioDevices(QStringList devices);

private:
    // Layout elements
    QPushButton* startStopButton = nullptr;

    QComboBox* audioDevices = nullptr;
    QPushButton* setAudioDevice = nullptr;

    QSpinBox* fftSize = nullptr;
    QPushButton* setFFTSize = nullptr;

    QLabel* waterfall = nullptr;

    // Data
    QImage image;
    uint imageHeight = 600;
    uint imageWidth = 513;

public slots:
    void audioDeviceStateChanged(bool state);
    void drawData(double** data, uint samplesNumber);
    void error(QString error);

private slots:

signals:
    void audioDeviceChange(uint audioDeviceId);
    void audioDeviceStateChange(bool state);
    void fftSizeChange(uint fftSize);

};

#endif // WIDGET_H
