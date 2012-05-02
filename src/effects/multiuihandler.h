/*
Copyright (C) 2012  Till Theato <root@ttill.de>
This file is part of kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#ifndef MULTIUIHANDLER_H
#define MULTIUIHANDLER_H

#include <QObject>
#include <QMap>

enum EffectUiTypes { EffectStackEffectUi, TimelineEffectUi, MonitorEffectUi };


class MultiUiHandler : public QObject, private QMap <int, QObject*>
{
    Q_OBJECT

public:
    MultiUiHandler(MultiUiHandler* parent = 0);
    ~MultiUiHandler() {};

    void addUi(EffectUiTypes type, QObject *ui);

signals:
    void createUi(EffectUiTypes type, QObject *parent);

private:
    MultiUiHandler *m_parent;
};

#endif
