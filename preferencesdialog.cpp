#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

int PreferencesDialog::shortBreakTime()
{
    return ui->shortBreakSpinBox->value();
}

void PreferencesDialog::setShortBreakTime(int shortBreak)
{
    ui->shortBreakSpinBox->setValue(shortBreak);
}

int PreferencesDialog::longBreakTime()
{
    return ui->longBreakSpinBox->value();
}

void PreferencesDialog::setLongBreakTime(int longBreak)
{
    ui->longBreakSpinBox->setValue(longBreak);
}

int PreferencesDialog::workTime()
{
    return ui->workSpinBox->value();
}

void PreferencesDialog::setWorkTime(int work)
{
    ui->workSpinBox->setValue(work);
}

bool PreferencesDialog::startTimerAutomatically()
{
    return ui->startTimerAutomaticallyCheckBox->isChecked();
}

void PreferencesDialog::setStartTimerAutomatically(bool on)
{
    ui->startTimerAutomaticallyCheckBox->setChecked(on);
}

bool PreferencesDialog::soundOnTimerStart()
{
    return ui->playSoundWhenTimerStartsCheckBox->isChecked();
}

void PreferencesDialog::setSoundOnTimerStart(bool on)
{
    ui->playSoundWhenTimerStartsCheckBox->setChecked(on);
}

bool PreferencesDialog::soundOnTimerEnd()
{
    return ui->playSoundWhenTimerEndsCheckBox->isChecked();
}

void PreferencesDialog::setSoundOnTimerEnd(bool on)
{
    ui->playSoundWhenTimerEndsCheckBox->setChecked(on);
}

bool PreferencesDialog::notificationOnTimerEnd()
{
    return ui->showNotificationsWhenTimerEndsCheckBox->isChecked();
}

void PreferencesDialog::setNotificationOnTimerEnd(bool on)
{
    ui->showNotificationsWhenTimerEndsCheckBox->setChecked(on);
}

bool PreferencesDialog::tickTockDuringWork()
{
    return ui->playTickTockDuringPomodoroCheckBox->isChecked();
}

void PreferencesDialog::setTickTockDuringWork(bool on)
{
    ui->playTickTockDuringPomodoroCheckBox->setChecked(on);
}

bool PreferencesDialog::tickTockDuringBreak()
{
    return ui->playTickTockDuringBreakCheckBox->isChecked();
}

void PreferencesDialog::setTickTockDuringBreak(bool on)
{
    ui->playTickTockDuringBreakCheckBox->setChecked(on);
}
