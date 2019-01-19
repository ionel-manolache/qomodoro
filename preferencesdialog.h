#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    int shortBreakTime();
    void setShortBreakTime(int shortBreak);
    int longBreakTime();
    void setLongBreakTime(int longBreak);
    int workTime();
    void setWorkTime(int work);

    bool startTimerAutomatically();
    void setStartTimerAutomatically(bool on);

    bool soundOnTimerStart();
    void setSoundOnTimerStart(bool on);
    bool soundOnTimerEnd();
    void setSoundOnTimerEnd(bool on);
    bool notificationOnTimerEnd();
    void setNotificationOnTimerEnd(bool on);

    bool tickTockDuringWork();
    void setTickTockDuringWork(bool on);
    bool tickTockDuringBreak();
    void setTickTockDuringBreak(bool on);

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
