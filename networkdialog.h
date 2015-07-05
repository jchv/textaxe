#pragma once

#include <QByteArray>
#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>

namespace Ui { class NetworkDialog; }

class NetworkDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { Get, Put };

    explicit NetworkDialog(QWidget *parent = 0);
    ~NetworkDialog();

    void setUrl(const QUrl &url);

    void setMode(Mode mode);

    QByteArray &data();
    void setData(const QByteArray &data);

    void start();

private:
    QUrl url;
    Mode mode;
    QByteArray buffer;
    QNetworkAccessManager netMan;
    Ui::NetworkDialog *ui;
};
