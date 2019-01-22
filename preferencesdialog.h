#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class Settings;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    void setSettings(Settings *settings);

    void loadSettings();

private slots:
    void onAccepted();

private:
    Ui::PreferencesDialog *ui;

    Settings *m_settings;
};

#endif // PREFERENCESDIALOG_H
