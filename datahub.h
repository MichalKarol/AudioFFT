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
#ifndef DATAHUB_H
#define DATAHUB_H

#include "widget.h"
#include "audioinput.h"
#include "fftmodule.h"
#include <QObject>
#include <QThread>

class DataHub : public QObject {
    Q_OBJECT
public:
    explicit DataHub(Widget* widget, QObject *parent = 0);
    ~DataHub();

private:
    Widget* widget = nullptr;

    QThread* thread[2] = {nullptr, nullptr};
    AudioInput* audioInput = nullptr;
    FFTModule* fftModule = nullptr;

public slots:

private slots:
    void clearEventQueue();

signals:
    void error(QString error);
};

#endif // DATAHUB_H
