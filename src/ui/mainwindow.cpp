#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../core/custom_events.h"
#include "../core/bass_player.h"
#include <QStyle>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QMediaMetaData>

MainWindow * MainWindow::instance_ = nullptr;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, player(new core::BassPlayer())
{
    instance_ = this;

    // Setup UI
    ui->setupUi(this);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    // Add self as observer
    player->AddStatusObserver(MainWindow::OnStatusChange);
    player->AddTitleObserver(MainWindow::OnTitleChange);
    player->AddMessageObserver(MainWindow::OnMessageChange);
    player->AddPlaybackFailureObserver(MainWindow::OnPlaybackFailure);

    // Set URL (is it necessary?)
}

MainWindow::~MainWindow()
{
    delete player;
    delete ui;

    instance_ = nullptr;
}
bool MainWindow::event(QEvent *event)
{
    if (event->type() == StatusEvent::type()) // status has changed
    {
        StatusEvent * e = static_cast<StatusEvent*>(event);
        ui->statusBar->showMessage(e->status());
        return true;
    }
    else if (event->type() == TitleEvent::type())
    {
        // On title change notification
        return true;
    }
    else if (event->type() == MessageEvent::type())
    {
        MessageEvent * e = static_cast<MessageEvent*>(event);
        switch (e->msg_type())
        {
        case message::kInformation:
            QMessageBox::information(this, e->caption(), e->text());
            break;
        case message::kError:
            QMessageBox::critical(this, e->caption(), e->text());
            break;
        case message::kWarning:
            QMessageBox::warning(this, e->caption(), e->text());
            break;
        }
        return true;
    }
    else if (event->type() == PlaybackFailureEvent::type())
    {
        // On playback failure notification
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        return true;
    }
    else
        return QMainWindow::event(event);
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
void MainWindow::OnStatusChange(const char* status)
{
    QCoreApplication::postEvent(instance_, new StatusEvent(status));
}
void MainWindow::OnTitleChange(const char* title)
{
    QCoreApplication::postEvent(instance_, new TitleEvent(title));
}
void MainWindow::OnMessageChange(message::Type type, const char* caption, const char* text)
{
    QCoreApplication::postEvent(instance_, new MessageEvent(type, caption, text));
}
void MainWindow::OnPlaybackFailure()
{
    QCoreApplication::postEvent(instance_, new PlaybackFailureEvent());
}
