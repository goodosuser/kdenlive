/*
Copyright (C) 2012  Till Theato <root@ttill.de>
This file is part of kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#include "doubleparametereffectstackitem.h"
#include "dragvalue.h"
#include <QWidget>
#include <QGridLayout>


DoubleParameterEffectStackItem::DoubleParameterEffectStackItem(const QString& name, double value, double min, double max, const QString& comment, int id, const QString suffix, int decimals, AbstractEffectStackItem* parent) :
    AbstractEffectStackItem(parent)
{
    m_ui = new QWidget(m_parent->getWidget());
    m_ui->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    QGridLayout *layout = new QGridLayout(m_ui);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // WARNING: Using "value" as "default"
    m_dragValue = new DragValue(name, value, decimals, min, max, id, suffix, m_ui);
    layout->addWidget(m_dragValue, 0, 1);

    m_dragValue->setValue(value);
    connect(m_dragValue, SIGNAL(valueChanged(double, bool)), this, SLOT(slotSetValue(double, bool)));
}

void DoubleParameterEffectStackItem::setValue(double value)
{
    m_dragValue->blockSignals(true);
    m_dragValue->setValue(value);
    m_dragValue->blockSignals(false);
}

void DoubleParameterEffectStackItem::valueChanged(double value, bool final)
{
    if (final) {
        emit valueChanged(value);
    }
}
