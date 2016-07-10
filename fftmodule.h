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
#ifndef FFTMODULE_H
#define FFTMODULE_H

#include <QObject>

class FFTModule : public QObject
{
    Q_OBJECT
public:
    explicit FFTModule(QObject *parent = 0);
    ~FFTModule();

private:
    uint fftSize = 1024; // Default FFT size
    double* array = nullptr;
    int* ip = nullptr;
    double* w = nullptr;

public slots:
    void initialize();
    void setFFTSize(uint fftSize);
    void signalData(float** data, uint samplesNumber);

signals:
    void calculated(double** data, uint samplesNumber);
    void error(QString error);
};

#endif // FFTMODULE_H
