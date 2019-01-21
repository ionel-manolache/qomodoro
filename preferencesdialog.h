#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class QSettings;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    void setSettings(QSettings *settings);
    QSettings* settings();

    void loadSettings();

private slots:
    void onAccepted();

private:
    bool getBool(QString key, bool def);
    int getInt(QString key, int def);

private:
    Ui::PreferencesDialog *ui;

    QSettings *m_settings;
};

#endif // PREFERENCESDIALOG_H
