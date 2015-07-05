#pragma once

#include <QMainWindow>
#include <QUrl>

#include "codingmanager.h"

namespace Ui { class MainWindow; }
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupStatus();
    void setupMenus();
    void setupDocument(const QUrl &url);
    bool readDocument(const QUrl &url);
    bool writeDocument(const QUrl &url);

    enum PromptType { LocalFile, Network };
    enum PromptMode { Open, Save };
    QUrl promptUrl(PromptType type, PromptMode mode);
    QImage renderDocument();

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void encodingUpdated(const QLatin1String &encoding);
    void lineEndingsUpdated(const QString &lineEndings);

public slots:
    void reset();
    bool open();
    bool save();
    bool saveAs();
    bool promptSave();
    void setFont(const QFont &font);
    void setEncoding(const QLatin1String &encoding);
    void setLineEndings(const QString &lineEndings);

private slots:
    void updateEncoding(const QLatin1String &encoding);
    void updateLineEndings(const QString &lineEndings);

    void on_actionNetworkGet_triggered();
    void on_actionNetworkPut_triggered();
    void on_actionWordwrap_triggered();
    void on_actionSelectFont_triggered();
    void on_action2ch_Mode_triggered();
    void on_actionQuit_triggered();
    void on_actionRender_triggered();

private:
    Ui::MainWindow *ui;
    QUrl documentUrl;
    QLatin1String documentCodec;
    CodingManager codingMan;
    QLabel *encodingLabel;
    QLabel *lineEndingsLabel;
};
