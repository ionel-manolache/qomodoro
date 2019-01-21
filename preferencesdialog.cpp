#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QSettings>

static const QString shortBreakString = QStringLiteral("shortBreakTime");
static const QString longBreakString = QStringLiteral("longBreakTime");
static const QString workString = QStringLiteral("workTime");
static const QString autoStartString = QStringLiteral("autoStartTimer");
static const QString soundOnTimerStartString = QStringLiteral("soundOnTimerStart");
static const QString soundOnTimerEndString = QStringLiteral("soundOnTimerEnd");
static const QString notificationOnTimerEndString = QStringLiteral("notificationOnTimerEnd");
static const QString tickTockDuringWorkString = QStringLiteral("tickTockDuringWork");
static const QString tickTockDuringBreakString = QStringLiteral("tickTockDuringBreak");

static const int DEFAULT_WORK = 25;
static const int DEFAULT_BREAK = 5;
static const int DEFAULT_BIG_BREAK = 30;

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(this, &QDialog::accepted, this, &PreferencesDialog::onAccepted);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setSettings(QSettings *settings)
{
    m_settings = settings;
}

QSettings* PreferencesDialog::settings()
{
    return m_settings;
}

void PreferencesDialog::loadSettings()
{
    ui->shortBreakSpinBox->setValue(getInt(shortBreakString, DEFAULT_BREAK));
    ui->longBreakSpinBox->setValue(getInt(longBreakString, DEFAULT_BIG_BREAK));
    ui->workSpinBox->setValue(getInt(workString, DEFAULT_WORK));
    ui->startTimerAutomaticallyCheckBox->setChecked(getBool(autoStartString, false));
    ui->playSoundWhenTimerStartsCheckBox->setChecked(getBool(soundOnTimerStartString, true));
    ui->playSoundWhenTimerEndsCheckBox->setChecked(getBool(soundOnTimerEndString, true));
    ui->showNotificationsWhenTimerEndsCheckBox->setChecked(getBool(notificationOnTimerEndString, false));
    ui->playTickTockDuringPomodoroCheckBox->setChecked(getBool(tickTockDuringWorkString, false));
    ui->playTickTockDuringBreakCheckBox->setChecked(getBool(tickTockDuringBreakString, false));
}

void PreferencesDialog::onAccepted()
{
    m_settings->setValue(QStringLiteral("shortBreakTime"), ui->shortBreakSpinBox->value());
    m_settings->setValue(QStringLiteral("longBreakTime"), ui->longBreakSpinBox->value());
    m_settings->setValue(QStringLiteral("workTime"), ui->workSpinBox->value());
    m_settings->setValue(QStringLiteral("autoStartTimer"), ui->startTimerAutomaticallyCheckBox->isChecked());
    m_settings->setValue(QStringLiteral("soundOnTimerStart"), ui->playSoundWhenTimerStartsCheckBox->isChecked());
    m_settings->setValue(QStringLiteral("soundOnTimerEnd"), ui->playSoundWhenTimerEndsCheckBox->isChecked());
    m_settings->setValue(QStringLiteral("notificationOnTimerEnd"), ui->showNotificationsWhenTimerEndsCheckBox->isChecked());
    m_settings->setValue(QStringLiteral("tickTockDuringWork"), ui->playTickTockDuringPomodoroCheckBox->isChecked());
    m_settings->setValue(QStringLiteral("tickTockDuringBreak"), ui->playTickTockDuringBreakCheckBox->isChecked());
}

bool PreferencesDialog::getBool(QString key, bool def)
{
    return m_settings->value(key, def).toBool();
}

int PreferencesDialog::getInt(QString key, int def)
{
    return m_settings->value(key, def).toInt();
}
