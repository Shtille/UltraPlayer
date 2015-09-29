#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../core/bass_player.h"
#include <QStyle>
#include <QMediaPlayer>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, player(new core::BassPlayer())
{
    // Setup UI
    ui->setupUi(this);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    // Set URL (is it necessary?)
}

MainWindow::~MainWindow()
{
    delete player;
    delete ui;
}

void MainWindow::on_playButton_clicked()
{
    if (player->state() == core::Player::State::kPlaying)
    {
        // Stop playback
        stop();
    }
    else // ! playing
    {
        // Start playback
        play();
    }
}

void MainWindow::on_volumeSlider_sliderMoved(int position)
{
    double fract = (double)position / 100.0;
    player->SetVolume(fract);
}

void MainWindow::play()
{
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    player->Play();
}

void MainWindow::stop()
{
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    player->Stop();
}
