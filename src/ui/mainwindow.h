#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_playButton_clicked();

    void on_volumeSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    core::Player *player;

    void play();
    void stop();
};

#endif // MAINWINDOW_H
