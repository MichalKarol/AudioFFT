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
#include "widget.h"

class PowersOf2SpinBox: public QSpinBox {
    Q_OBJECT

public:
    PowersOf2SpinBox(QWidget* parent = nullptr): QSpinBox(parent) {
    }

protected:
    void stepBy(int steps) {
        int current = value();

        if (steps > 0) {
            for (int i = 0; i < steps; i++) current *=2;
        } else {
            for (int i = 0; i < abs(steps); i++) current /=2;
        }

        setValue(current);
    }
};

Widget::Widget(QWidget *parent): QWidget(parent) {
    startStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "", this);
    startStopButton->setProperty("user_playIcon", true);
    connect(startStopButton, &QPushButton::clicked, [&]() -> void {
                emit audioDeviceStateChange(startStopButton->property("user_playIcon").toBool());
            });
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startStopButton);


    QGroupBox* box = new QGroupBox("Settings");
    QFormLayout* settings = new QFormLayout(box);

    audioDevices = new QComboBox(this);
    setAudioDevice = new QPushButton("Set", this);
    connect(setAudioDevice, &QPushButton::clicked, [&]() -> void {
                emit audioDeviceChange(audioDevices->currentIndex());
            });
    QHBoxLayout* audioLayout = new QHBoxLayout();
    audioLayout->addWidget(audioDevices);
    audioLayout->addWidget(setAudioDevice);
    settings->addRow("Audio device:", audioLayout);


    fftSize = new PowersOf2SpinBox(this);
    fftSize->setRange(128, 16384);
    fftSize->setValue(DEFAULT_FFT_SIZE);
    setFFTSize = new QPushButton("Set", this);
    connect(setFFTSize, &QPushButton::clicked, [&]() -> void {
                emit fftSizeChange(fftSize->value());
            });
    QHBoxLayout* fftLayout = new QHBoxLayout();
    fftLayout->addWidget(fftSize);
    fftLayout->addWidget(setFFTSize);
    settings->addRow("FFT size:", fftLayout);

    mainLayout->addWidget(box);

    image = QImage(imageWidth, imageHeight, QImage::Format_Grayscale8);
    image.fill(Qt::white);
    waterfall = new QLabel(this);
    waterfall->setPixmap(QPixmap::fromImage(image));
    mainLayout->addWidget(waterfall);
}

Widget::~Widget() {
}

// Public slots
void Widget::setAudioDevices(QStringList devices) {
    audioDevices->clear();
    audioDevices->addItems(devices);
}

void Widget::audioDeviceStateChanged(bool state) {
    startStopButton->setIcon(style()->standardIcon((state? QStyle::SP_MediaStop : QStyle::SP_MediaPlay)));
    startStopButton->setProperty("user_playIcon", !state);
}

void Widget::drawData(std::shared_ptr<vector<vector<double>>> data) {
    uint size = std::min(static_cast<uint>((*data).size()), imageHeight);
    int offset = std::max(static_cast<int>((*data).size()) - static_cast<int>(imageHeight) - 1, 0);

    // Moving images samplesNumber up
    for (uint  i = 0; i < imageHeight - size; i++) {
        uchar* pointerFrom = image.scanLine(i + (*data).size());
        uchar* pointerTo = image.scanLine(i);
        for (uint  j = 0; j < imageWidth; j++) {
            *(pointerTo + j) = *(pointerFrom + j);
        }
    }

    // Drawing data as a pixels
    for (uint i = 0; i < (*data).size(); i++) {
        uchar* pointer = image.scanLine(imageHeight - size + i);
        uint ratio = ((*data)[offset + i].size() - 1)/(imageWidth - 1);
        uint revratio = (imageWidth - 1)/((*data)[offset + i].size() - 1);

        if (ratio < 1) {
            for (uint j = 0; j < (*data)[offset + i].size(); j++) {
                int color = (*data)[offset + i][j] * 255;
                color = (color > 255 || color < 0 ? 0 : color);
                for(uint k = 0; k < revratio && j*revratio + k < imageWidth; k++) {
                    *(pointer + j*revratio + k) = QColor(color, color, color).rgb();
                }
            }
        } else {
            for (uint j = 0; j < imageWidth; j++) {
                uint sum = 0;
                for(uint k = 0; k < ratio; k++) {
                    int color = (*data)[offset + i][j + k] * 255;
                    color = (color > 255 || color < 0 ? 0 : color);
                    sum += color;
                }
                *(pointer + j) = QColor(sum/ratio, sum/ratio, sum/ratio).rgb();
            }
        }

    }

    waterfall->setPixmap(QPixmap::fromImage(image));
}

void Widget::error(QString error) {
    QMessageBox::critical(this, "Error", error);
}

#include "widget.moc"
