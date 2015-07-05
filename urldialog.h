#pragma once

#include <QDialog>
#include <QUrl>

namespace Ui { class UrlDialog; }

class UrlDialog : public QDialog
{
    Q_OBJECT

public:
    enum Type { Open, Save };

    UrlDialog(Type type, QUrl url);
    ~UrlDialog();

    QUrl url();

private:
    Type type;
    QUrl fileUrl;
    Ui::UrlDialog *ui;
};
