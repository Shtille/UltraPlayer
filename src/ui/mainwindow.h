#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../core/message.h"

class QMediaPlayer;
class QMediaContent;

namespace Ui {
class MainWindow;
}

namespace core {
    class Player;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual bool event(QEvent *event);

private slots:
    void on_playButton_clicked();

    void on_volumeSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    core::Player *player;

    void play();
    void stop();

    static void OnStatusChange(const char* status);
    static void OnTitleChange(const char* title);
    static void OnMessageChange(message::Type type, const char* caption, const char* text);
    static void OnPlaybackFailure();

    static MainWindow * instance_;
};

#endif // MAINWINDOW_H
