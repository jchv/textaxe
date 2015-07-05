#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>

#include "networkdialog.h"
#include "ui_networkdialog.h"


NetworkDialog::NetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDialog)
{
    ui->setupUi(this);
}

NetworkDialog::~NetworkDialog()
{
    delete ui;
}

void NetworkDialog::setUrl(const QUrl &url)
{
    this->url = url;
}

void NetworkDialog::setMode(NetworkDialog::Mode mode)
{
    this->mode = mode;
}

QByteArray &NetworkDialog::data()
{
    return buffer;
}

void NetworkDialog::setData(const QByteArray &data)
{
    buffer = data;
}

typedef void (QNetworkReply::*netErrSig)(QNetworkReply::NetworkError);

void NetworkDialog::start()
{
    QNetworkRequest req(url);
    QNetworkReply *reply;

    if (mode == Get)
        reply = netMan.get(req);
    else if (mode == Put)
        reply = netMan.put(req, buffer);

    auto updateProgress = [=](qint64 bytesTransfered, qint64 bytesTotal) {
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(bytesTotal);
        ui->progressBar->setValue(bytesTransfered);
    };

    connect(reply, &QNetworkReply::downloadProgress, updateProgress);

    connect(reply, &QNetworkReply::uploadProgress, updateProgress);

    connect(reply, (netErrSig)&QNetworkReply::error, [=](QNetworkReply::NetworkError) {
        QMessageBox::critical(this, tr("IO Error"), reply->errorString());
    });

    connect(reply, &QNetworkReply::finished, [=]() {
        if (mode == Get)
            buffer = reply->readAll();
        done(reply->error() ? QDialog::Rejected : QDialog::Accepted);
    });

}
