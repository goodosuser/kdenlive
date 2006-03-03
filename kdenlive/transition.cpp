/***************************************************************************
                          transition.cpp  -  description
                             -------------------
    begin                : Tue Jan 24 2006
    copyright            : (C) 2006 by Jean-Baptiste Mardelle
    email                : jb@ader.ch 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "transition.h"
#include "docclipref.h"

#include <kdebug.h>
#include <qdom.h>

/* Transitions can be either be
    1) placed on a track. They are not dependant of any clip // not implemented yet
    2) Attached to one clip. They move with the clip
    3) Attached to two clips. They automatically adjust to the length of the overlapping area
*/

/* create an "automatic" transition (type 3) */
Transition::Transition(const DocClipRef * clipa, const DocClipRef * clipb)
{
    m_invertTransition = false;
    m_singleClip = true;
    m_transitionType = "luma";

    if (clipb) {
        // Transition is an automatic transition between 2 clips
        
        m_singleClip = false;
        if (clipa->trackNum()<clipb->trackNum()) {
            m_referenceClip = clipb;
            m_secondClip = clipa;
        }
        else {
            m_referenceClip = clipa;
            m_secondClip = clipb;
        }
    
        if (clipa->trackStart() < clipb->trackStart()) m_invertTransition = true;
    }
    else {
        // Transition is attached to a single clip
        m_referenceClip = clipa;
        m_transitionStart = GenTime(0.0);
        
        // Default duration = 1,5 seconds
        if (GenTime(1.5) > m_referenceClip->cropDuration()) 
            m_transitionDuration = m_referenceClip->cropDuration();
        else m_transitionDuration = GenTime(1.5);
        m_secondClip = 0;
    }
}

/* create an "simple" transition (type 2) */
Transition::Transition(const DocClipRef * clipa)
{
    m_invertTransition = false;
    m_singleClip = true;
    m_transitionType = "luma";

        m_referenceClip = clipa;
        m_transitionStart = GenTime(0.0);
        
        // Default duration = 2.5 seconds
        if (GenTime(1.5) > m_referenceClip->cropDuration()) 
            m_transitionDuration = m_referenceClip->cropDuration();
        else m_transitionDuration = GenTime(2.5);
        m_secondClip = 0;
}

Transition::~Transition()
{
}

void Transition::setTransitionType(QString newType)
{
    m_transitionType = newType;
}

QString Transition::transitionType()
{
    return m_transitionType;
}

void Transition::setTransitionParameters(const QMap < QString, QString > parameters)
{
    m_transitionParameters = parameters;
}

const QMap < QString, QString > Transition::transitionParameters()
{
    return m_transitionParameters;
}

bool Transition::invertTransition()
{
    if (!m_singleClip) {
        if (m_referenceClip->trackStart() < m_secondClip->trackStart()) return true;
        else return false;
    }
    return m_invertTransition;
}

void Transition::setTransitionDirection(bool inv)
{
    m_invertTransition = inv;
}


int Transition::transitionStartTrack()
{
    return m_referenceClip->trackNum();
}

int Transition::transitionEndTrack()
{
    if (!m_singleClip) return m_secondClip->trackNum();
    // #Warning: Should point to the previous video track. Currently we substract by 2 
    // because we don't want the audio track, but should find a better way to get the track number.
    else if (transitionStartTrack()>1) return m_referenceClip->trackNum()-2; 
    else return transitionStartTrack()-1;
}

GenTime Transition::transitionStartTime()
{
    if (!m_singleClip) {
        GenTime startb = m_secondClip->trackStart();
        GenTime starta = m_referenceClip->trackStart();
        if (startb > m_referenceClip->trackEnd()) return m_referenceClip->trackEnd() - GenTime(0.12);
        if (startb > starta)
	   return startb;
        return starta;
    }
    else return m_referenceClip->trackStart() + m_transitionStart;
}


GenTime Transition::transitionEndTime()
{
    if (!m_singleClip) {
        GenTime endb = m_secondClip->trackEnd();
        GenTime enda = m_referenceClip->trackEnd();
        if (m_secondClip->trackStart() > enda) return enda;
        if (endb < m_referenceClip->trackStart()) return m_referenceClip->trackStart() + GenTime(0.12);
        else if (endb > enda) return enda;
        else return endb;
    }
    else { 
        if (m_transitionStart + m_transitionDuration > m_referenceClip->cropDuration())
            return m_referenceClip->trackEnd();
        return m_referenceClip->trackStart() + m_transitionStart + m_transitionDuration;
    }
}

void Transition::resizeTransitionStart(GenTime time)
{
    if (!m_singleClip) return; //cannot resize automatic transitions
    if (time < m_referenceClip->trackStart()) time = m_referenceClip->trackStart();
    // Transitions shouldn't be shorter than 3 frames, about 0.12 seconds
    if ( transitionEndTime().ms() - time.ms() < 120.0) time = transitionEndTime() - GenTime(0.12);
    m_transitionDuration =m_transitionDuration - (time - m_referenceClip->trackStart() - m_transitionStart);
    m_transitionStart = time - m_referenceClip->trackStart();
    
}

void Transition::resizeTransitionEnd(GenTime time)
{
    if (!m_singleClip) return; //cannot resize automatic transitions
    if (time > m_referenceClip->trackEnd()) time = m_referenceClip->trackEnd();
    // Transitions shouldn't be shorter than 3 frames, about 0.12 seconds
    if ( time.ms() - transitionStartTime().ms() < 120.0) time = transitionStartTime() + GenTime(0.12);
    m_transitionDuration = time - ( m_referenceClip->trackStart() + m_transitionStart);
}

void Transition::moveTransition(GenTime time)
{
    if (!m_singleClip) return; //cannot move automatic transitions
    if (m_transitionStart + time < GenTime(0.0)) m_transitionStart = GenTime(0.0);
    else if ( m_transitionStart + time > m_referenceClip->cropDuration())
        m_transitionStart = m_referenceClip->cropDuration() - GenTime(0.12);
    else m_transitionStart = m_transitionStart + time;
}



bool Transition::hasClip(const DocClipRef * clip)
{
    if (clip == m_secondClip) return true;
    return false;
}

Transition *Transition::clone()
{
    if (m_singleClip || m_secondClip == 0)
        return new Transition::Transition(m_referenceClip);
    else
        return new Transition::Transition(m_referenceClip, m_secondClip);
}
