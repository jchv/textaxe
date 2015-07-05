#include "urldialog.h"
#include "ui_urldialog.h"

UrlDialog::UrlDialog(UrlDialog::Type type, QUrl url)
    : ui(new Ui::UrlDialog), type(type)
{
    ui->setupUi(this);

    if (type == Open)
        setWindowTitle("Open from URL");
    else if (type == Save)
        setWindowTitle("Save to URL");

    if (url.isValid())
        ui->lineEdit->setText(url.toString());

    connect(this, &UrlDialog::accepted, [=]() {
        fileUrl = QUrl(ui->lineEdit->text());
    });
}

UrlDialog::~UrlDialog()
{
    delete ui;
}

QUrl UrlDialog::url()
{
    return fileUrl;
}
