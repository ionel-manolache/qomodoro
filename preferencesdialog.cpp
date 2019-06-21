#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include "settings.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(this, &QDialog::accepted, this, &PreferencesDialog::onAccepted);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setSettings(Settings *settings)
{
    m_settings = settings;
}

void PreferencesDialog::loadSettings()
{
    ui->shortBreakSpinBox->setValue(m_settings->shortBreakTime());
    ui->longBreakSpinBox->setValue(m_settings->longBreakTime());
    ui->workSpinBox->setValue(m_settings->workTime());
    ui->startTimerAutomaticallyCheckBox->setChecked(m_settings->autoStartNextTimer());
    ui->playSoundWhenTimerStartsCheckBox->setChecked(m_settings->soundOnTimerStart());
    ui->playSoundWhenTimerEndsCheckBox->setChecked(m_settings->soundOnTimerEnd());
    ui->showNotificationsWhenTimerEndsCheckBox->setChecked(m_settings->notificationOnTimerEnd());
    ui->playTickTockDuringPomodoroCheckBox->setChecked(m_settings->tickTockDuringWork());
    ui->playTickTockDuringBreakCheckBox->setChecked(m_settings->tickTockDuringBreak());
}

void PreferencesDialog::onAccepted()
{
    m_settings->setShortBreakTime(ui->shortBreakSpinBox->value());
    m_settings->setLongBreakTime(ui->longBreakSpinBox->value());
    m_settings->setWorkTime(ui->workSpinBox->value());
    m_settings->setAutoStartNextTimer(ui->startTimerAutomaticallyCheckBox->isChecked());
    m_settings->setSoundOnTimerStart(ui->playSoundWhenTimerStartsCheckBox->isChecked());
    m_settings->setSoundOnTimerEnd(ui->playSoundWhenTimerEndsCheckBox->isChecked());
    m_settings->setNotificationOnTimerEnd(ui->showNotificationsWhenTimerEndsCheckBox->isChecked());
    m_settings->setTickTockDuringWork(ui->playTickTockDuringPomodoroCheckBox->isChecked());
    m_settings->setTickTockDuringBreak(ui->playTickTockDuringBreakCheckBox->isChecked());
}
