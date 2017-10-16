/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#include <QDesktopWidget>
#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Player),
    _hideTimeout(10)
{
    ui->setupUi(this);

    // Set transparent background
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    centralWidget()->setMouseTracking(true);

    // initialize variables
    _controls = new PlayControls();
    _videoplayer = new VideoPlayer();
    _toolBar = new QToolBar(this);
    _fpsL = new QLabel(this);
    _tools = new Tools;
    _cpuTimer = new QTimer(this);
    _hideControlsTimer = new QTimer(this);


    // Setup controls
    setUpPlayControls();

#if defined PLANA || defined PLAND
    setUpFpsLabel();
#endif

    // Connect signals
    connect(_cpuTimer, SIGNAL(timeout()), this, SLOT(updateCPUusage()));
    connect(_hideControlsTimer, SIGNAL(timeout()), this, SLOT(onControlsTimeout()));
    connect(_controls, SIGNAL(showOptions()), this, SLOT(onOpenFile()));
    connect(_controls, SIGNAL(setFullScreen()), this, SLOT(onSetFullScreen()));
    connect(_controls,SIGNAL(play(bool,int)), _videoplayer, SLOT(play(bool,int)));
    connect(_controls, SIGNAL(pause()), _videoplayer, SLOT(pause()));
    connect(_controls, SIGNAL(setVolume(int)), _videoplayer, SLOT(setVolume(int)));
    connect(_controls, SIGNAL(setMute(bool)), _videoplayer, SLOT(setMute(bool)));
    connect(_videoplayer, SIGNAL(durationChanged(qint64)), _controls, SLOT(durationChanged(qint64)));
    connect(_videoplayer, SIGNAL(positionChanged(qint64)), _controls, SLOT(positionChanged(qint64)));
    connect(_videoplayer, SIGNAL(fpsChanged(QString)), this, SLOT(fpsChanged(QString)));
    connect(_videoplayer, SIGNAL(playState(int)), _controls, SLOT(setPlayState(int)));

    // Set full screen
    this->setWindowState(Qt::WindowFullScreen);

    // Start timers
    _cpuTimer->start(1000);
    _hideControlsTimer->start(_hideTimeout * 1000);

    _videoplayer->setMedia(QString("/opt/VideoPlayer/media/Microchip-masters.mp4"));
    _videoplayer->play(_controls->getCurrentMute(), _controls->getCurrentVolume());
}

Player::~Player()
{
    delete ui;
}

void Player::setUpPlayControls(){

    _toolBar->addWidget(_controls);

#if !DESB
    // Set toolbar background color
    _toolBar->setStyleSheet("QToolBar { background-color: rgba(76,76,76,180); border-width: 0px;}");
#else
    _toolBar->setStyleSheet("QToolBar { background-color: rgba(255,255,255,50); border-width: 0px; border-radius: 10px; margin-bottom: 5px; margin-left: 5px; margin-right: 5px;}");
#endif

    // don't move
    _toolBar->setMovable(false);
    // Place at the bottom of the window
    addToolBar(Qt::BottomToolBarArea, _toolBar);

    // File Dialog
    QString filter = "media (*.avi *.mp4)";
    _openFileDialog = new QFileDialog(this);
    _openFileDialog->setFileMode(QFileDialog::ExistingFile);
    _openFileDialog->setNameFilter(filter);
    _openFileDialog->setDirectory("/opt/VideoPlayer/media");

    QDesktopWidget desk;
    QRect screenres = desk.screenGeometry(0);
    _openFileDialog->setGeometry(QRect(20, 20, screenres.width()-20, screenres.height()-20));

    _openFileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    _openFileDialog->setStyleSheet("background-color: rgba(255,255,255,150); font-size: 24px; font-family: 'Arial'");

    // Disable seeking
    _controls->allowSeek(false);
    // Disable volume controls as we are not playing audio
    _controls->enableVolumeControl(true);
}

void Player::setUpFpsLabel(){
#if !DESB
    _fpsL->setStyleSheet("QLabel { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B0CACACA, stop: 0.5 #B0808080, stop:1 #B0CACACA); font-size: 24px; font-family: 'Arial'; border-radius: 20px;}");
#else
    _fpsL->setStyleSheet("QLabel { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B0CACACA, stop:1 #B0CACACA); font: 75 16pt KacstOne; border-radius: 10px;}");
#endif
    _fpsL->setMargin(10);
    _fpsL->setText("fps:\nbps:");
    _fpsL->setGeometry(10, 10, 200, 80);
}

/* Update CPU usage widget  */
void Player::updateCPUusage(){
    _tools->updateCpuUsage();
    _controls->setCPUusage(_tools->cpu_usage[0]);
}

/* Update perf information */
void Player::fpsChanged(QString perf){
    this->_fpsL->setText(perf);
}

/* Hide controls when fullscreen button pressed*/
void Player::onSetFullScreen(){
    // Just hide controls as we are already on full screen mode
    if(! _toolBar->isHidden()){
        _hideControlsTimer->stop();
        _toolBar->hide();
        _fpsL->hide();
    }
}

/* Hide controls after some time with no mouse activity  */
void Player::onControlsTimeout(){
    if(! _toolBar->isHidden())
        _toolBar->hide();
    if(! _fpsL->isHidden())
        _fpsL->hide();
    _hideControlsTimer->stop();
}

/* Show controls on mouse Click*/
void Player::mousePressEvent(QMouseEvent *){
    if(_toolBar->isHidden()){ // Show controls if hidden
       _toolBar->show();
       _hideControlsTimer->start(_hideTimeout * 1000);
    }
#if defined PLANA || defined PLAND
    if(_fpsL->isHidden())
           _fpsL->show();
#endif
}

void Player::mouseMoveEvent(QMouseEvent *){
    if(_toolBar->isHidden() && _hideControlsTimer->isActive()) // Show controls if hidden
        _toolBar->show();

#if defined PLANA || defined PLAND
    if(_fpsL->isHidden())
           _fpsL->show();
#endif

    if(_hideControlsTimer->isActive()) _hideControlsTimer->start(_hideTimeout * 1000); //restart timer
}

/* Set player URI */
void Player::onOpenFile(){
    QString filename;
    if(_openFileDialog->exec()){
        filename = _openFileDialog->selectedFiles().first();
        _videoplayer->pause();
        _videoplayer->setMedia(filename);
        _videoplayer->play(_controls->getCurrentMute(), _controls->getCurrentVolume());

    }
}

/***
 ** Catch the "K1" capacitive button press and close the application
 **/
void Player::keyPressEvent( QKeyEvent *k )
{
    if(k->key() == 48){
        onSetFullScreen();
        qApp->exit();
    }
}
