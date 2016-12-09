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
#include "fftmodule.h"
#include "fft4g.c"

FFTModule::FFTModule(QObject *parent) : QObject(parent) {
}

FFTModule::~FFTModule() {
    clear();
}

// Private methods
void FFTModule::clear() {
    delete[] array; array = nullptr;
    delete[] ip; ip = nullptr;
    delete[] w; w = nullptr;
}

// Public slots
void FFTModule::initialize() {
    setFFTSize(fftSize);
}

void FFTModule::setFFTSize(uint fftSize) {
    clear();
    this->fftSize = fftSize;
    array = new double[fftSize];
    ip = new int[static_cast<int>(2 + sqrt(fftSize / 2))];
    ip[0] = 0;
    w = new double[fftSize / 2];
}
void FFTModule::signalData(shared_ptr<vector<vector<float>>> data) {
    if (!(*data).empty() && fftSize == (*data)[0].size()) {
        shared_ptr<vector<vector<double>>> output = std::make_shared<vector<vector<double>>>((*data).size(), vector<double>(((fftSize / 2) + 1)));

        for (uint i = 0; i < (*data).size(); i++) {
            for (uint j = 0; j < fftSize; j++) array[j] = static_cast<double>((*data)[i][j]);

            // Using Hann window
            for (uint j = 0; j < fftSize; j++) {
                double x = 2 * M_PI * j / (fftSize-1);
                array[j] *= .5 - .5 * cos(x);
            }

            // FFT
            rdft(fftSize, 1, array, ip, w);

            uint j;
            (*output)[i][0] = array[0] * array[0];
            for (j = 2; j < fftSize; j += 2) {
                (*output)[i][(j >> 1)] = array[j] * array[j]
                        + array[j + 1] * array[j + 1];
            }
            (*output)[i][(j >> 1)] = array[1] * array[1];

            // Getting magnitude of data
            for (uint j = 0; j < ((fftSize / 2) + 1); j++) {
                (*output)[i][j] = 10. * (log((*output)[i][j] * (2. / fftSize) + 1e-6) / log(10)) / -60.;
            }

        }

        emit calculated(output);
    }
}
