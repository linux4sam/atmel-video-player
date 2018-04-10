/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#include "playcontrols.h"
#include "ui_playcontrols.h"

PlayControls::PlayControls(QWidget *parent, bool small_screen) :
    QWidget(parent),
    ui(new Ui::PlayControls),
    _onPlay(false),
    _onMute(false),
    _totalDuration(0),
    _duration("unknown"),
    _currentPosition(0),
    _allowSeek(true),
	_smallScreen(small_screen)
{
    ui->setupUi(this);

    autoFillBackground();

    // Frameless and always on top
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);

    setLayout(ui->layout);

    // connect signals
    connect(ui->volumeControl, SIGNAL(sliderMoved(int)), this, SIGNAL(setVolume(int)));
    connect(ui->mediaPosition, SIGNAL(sliderMoved(int)), this, SIGNAL(positionChanged(int)));
    connect(ui->optionsButtonn, SIGNAL(clicked()), this, SIGNAL(showOptions()));

	if(!_smallScreen)
		ui->positioLabel->hide();

#if !defined PLANA && !defined PLAND 
    ui->mediaPosition->hide();    
    ui->positioLabel->hide();
#endif
#if !defined PLANA && !defined PLANC
    ui->muteButton->hide();
    ui->volumeControl->hide();
#endif
#ifdef PLANC
    ui->cpuUsage->hide();
#endif
}

PlayControls::~PlayControls()
{
    delete ui;
}


/* Set Play/pause state */
void PlayControls::on_PlayPause_clicked()
{
    if(_onPlay) emit pause();                                       // Pipeline is playing
    else emit play(_onMute, ui->volumeControl->value());           // Pipeline is paused
}

/* Set Mute/unmute state */
void PlayControls::on_muteButton_clicked()
{
	emit setMute(!_onMute);
    if(_onMute){ // Unmute
        ui->muteButton->setIcon(QIcon(MUTE_IMG));
    }
    else{ // mute
        ui->muteButton->setIcon(QIcon(UNMUTE_IMG));
    }
    _onMute = !_onMute;
}


/* Update media duration */
void PlayControls::durationChanged(qint64 duration){
	_totalDuration = duration;
	ui->mediaPosition->setMaximum(_totalDuration);
	
	// Calculate time
	_duration = secondsToString(_totalDuration);
	updatePositionLabel(_currentPosition);
}


/* Update media position */
void PlayControls::positionChanged(qint64 position){
	_currentPosition = position;
	ui->mediaPosition->setValue(_currentPosition);
	// Update position label
	updatePositionLabel(_currentPosition);
}


/* Update media position label */
void PlayControls::updatePositionLabel(int position){
	if(_smallScreen)
    	ui->positioLabel->setText(secondsToString(position) + "/" + _duration);
}


/* Forma seconds as hh:mm:ss */
QString PlayControls::secondsToString(int seconds){
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds - h*3600 - m * 60;

    return (QString::number(h) + ":" + QString::number(m) + ":" + QString::number(s));
}


/* Full screen request */
void PlayControls::on_fullScreenButtonn_clicked()
{
    emit setFullScreen();
}

/* Update CPU usage widget */
void PlayControls::setCPUusage(int usage){
    ui->cpuUsage->setValue(usage);
}

/* Change play buton icon according to play state */
void PlayControls::setPlayIcon(bool state){
    if(state){    // Video to play state
        ui->PlayPause->setIcon(QIcon(PAUSE_IMG)); // Pause image
    }
    else{ // Video to pause
        ui->PlayPause->setIcon(QIcon(PLAY_IMG)); // Pause image
    }
}

/* Update the _onPlay flag and play button icon */
void PlayControls::setPlayState(int state){
    if(state < 2){
        _onPlay =state;
        this->setPlayIcon(state);
        ui->mediaPosition->setEnabled(_onPlay && _allowSeek); // Enable/Disable seeking
    }else{
        _onPlay = 0;
        this->setPlayIcon(0);
        ui->muteButton->setIcon(QIcon(MUTE_IMG));
        _onMute = false;
        ui->mediaPosition->setEnabled(_onPlay && _allowSeek); // Enable/Disable seeking
    }
}

/* Seek  */
void PlayControls::allowSeek(bool allow){
    _allowSeek = allow;
}

/* Volume controls enable/disable*/
void PlayControls::enableVolumeControl(bool enable){
    ui->muteButton->setEnabled(enable);
    ui->volumeControl->setEnabled(enable);
}

int PlayControls::getCurrentVolume() {
    return ui->volumeControl->value();
}

bool PlayControls::getCurrentMute() {
    return _onMute;
}
