/***************************************************************************
 *   Copyright (C) 2010 by Till Theato (root@ttill.de)                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/


#include "doubleparameterwidget.h"
#include "dragvalue.h"

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QToolButton>

#include <KIcon>
#include <KLocalizedString>


DoubleParameterWidget::DoubleParameterWidget(const QString &name, int value, int min, int max, int defaultValue, const QString &comment, const QString suffix, QWidget *parent) :
        QWidget(parent),
        m_default(defaultValue)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_name = new QLabel(name, this);
    layout->addWidget(m_name, 0, 0);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(min, max);
    //m_slider->setPageStep((max - min) / 10);
    layout->addWidget(m_slider, 0, 1);

    m_dragVal = new DragValue(this);
    m_dragVal->setRange(min, max);
    m_dragVal->setPrecision(0);
    layout->addWidget(m_dragVal, 0, 2);

    m_spinBox = new QSpinBox(this);
    m_spinBox->setRange(min, max);
    m_spinBox->setKeyboardTracking(false);
    if (!suffix.isEmpty())
        m_spinBox->setSuffix(suffix);
    //layout->addWidget(m_spinBox, 0, 2);
    m_spinBox->setHidden(true);

    QToolButton *reset = new QToolButton(this);
    reset->setAutoRaise(true);
    reset->setIcon(KIcon("edit-undo"));
    reset->setToolTip(i18n("Reset to default value"));
    layout->addWidget(reset, 0, 3);

    m_commentLabel = new QLabel(comment, this);
    m_commentLabel->setWordWrap(true);
    m_commentLabel->setTextFormat(Qt::RichText);
    m_commentLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_commentLabel->setFrameShape(QFrame::StyledPanel);
    m_commentLabel->setFrameShadow(QFrame::Raised);
    m_commentLabel->setHidden(true);
    layout->addWidget(m_commentLabel, 1, 0, 1, -1);

    connect(m_slider,  SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(m_dragVal, SIGNAL(valueChanged(qreal, bool)), this, SLOT(slotSetValue(qreal, bool)));
    connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(reset,     SIGNAL(clicked(bool)),     this, SLOT(slotReset()));

    //m_spinBox->setValue(value);
    m_dragVal->setValue(value);
}

void DoubleParameterWidget::setValue(int value)
{
    m_slider->blockSignals(true);
    m_spinBox->blockSignals(true);
    m_dragVal->blockSignals(true);

    m_slider->setValue(value);
    m_spinBox->setValue(value);
    m_dragVal->setValue(value);

    m_slider->blockSignals(false);
    m_spinBox->blockSignals(false);
    m_dragVal->blockSignals(false);

    emit valueChanged(value);
}

void DoubleParameterWidget::slotSetValue(qreal value, bool final)
{
    if (final)
        setValue((int)value);
}

int DoubleParameterWidget::getValue()
{
    return m_spinBox->value();
}

void DoubleParameterWidget::setName(const QString& name)
{
    m_name->setText(name);
}

void DoubleParameterWidget::slotReset()
{
    setValue(m_default);
}

void DoubleParameterWidget::slotShowComment( bool show)
{
    if (m_commentLabel->text() != QString()) {
        m_commentLabel->setVisible(show);
        if (show)
            layout()->setContentsMargins(0, 0, 0, 15);
        else
            layout()->setContentsMargins(0, 0, 0, 0);
    }
}

#include "doubleparameterwidget.moc"
