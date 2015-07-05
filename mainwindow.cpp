#include <QClipboard>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QTextCodec>
#include <QFileDialog>
#include <QFontDialog>
#include <QTextLayout>
#include <QPainter>
#include <QBitmap>
#include <QBuffer>
#include <QLabel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkdialog.h"
#include "urldialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    documentCodec("UTF-8")
{
    ui->setupUi(this);
    setupStatus();
    setupMenus();
    reset();

    setFont(QFont("Bitstream Vera Sans Mono"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStatus()
{
    connect(this, SIGNAL(encodingUpdated(QLatin1String)), SLOT(updateEncoding(QLatin1String)));
    connect(this, SIGNAL(lineEndingsUpdated(QString)), SLOT(updateLineEndings(QString)));

    encodingLabel = new QLabel();
    encodingLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    lineEndingsLabel = new QLabel();
    lineEndingsLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    ui->statusBar->addPermanentWidget(encodingLabel);
    ui->statusBar->addPermanentWidget(lineEndingsLabel);
}

void MainWindow::setupMenus()
{
    // Set action of menu items.
    connect(ui->actionNew, SIGNAL(triggered()), SLOT(reset()));
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), SLOT(saveAs()));

    // Set the context menu for the text editing widget.
    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit, &QPlainTextEdit::customContextMenuRequested, [=](const QPoint &pt) {
        ui->menuEdit->exec(ui->textEdit->mapToGlobal(pt));
    });

    // Set initial state of menu items.
    ui->actionPaste->setEnabled(qApp->clipboard()->mimeData()->hasText());

    // Setup menu items to be disabled and re-enabled as necessary.
    connect(qApp->clipboard(), &QClipboard::dataChanged, [=]() {
        ui->actionPaste->setEnabled(qApp->clipboard()->mimeData()->hasText());
    });
    connect(ui->textEdit, &QPlainTextEdit::textChanged, [=]() {
        ui->actionSelectAll->setDisabled(ui->textEdit->document()->isEmpty());
    });
}

void MainWindow::setupDocument(const QUrl &url)
{
    if (!url.isValid())
        setWindowTitle(tr("Untitled") + "[*]");
    else
        setWindowTitle(url.fileName() + "[*]");

    documentUrl = url;

    ui->textEdit->document()->setModified(false);
    setWindowModified(false);
}

bool MainWindow::readDocument(const QUrl &url)
{
    QTextCodec *codec;
    NetworkDialog netDiag;
    QLatin1String documentCodec = QLatin1String("");

    netDiag.setUrl(url);
    netDiag.setMode(NetworkDialog::Get);
    netDiag.start();

    if (netDiag.exec() != QDialog::Accepted)
        return false;

    setupDocument(url);
    documentCodec = codingMan.guess(netDiag.data());
    if (documentCodec == "UTF-8+BOM") {
        codec = QTextCodec::codecForName("UTF-8");
        netDiag.data().remove(0, 3);
    }
    else codec = QTextCodec::codecForName(documentCodec.data());
    ui->textEdit->setPlainText(codec->toUnicode(netDiag.data()));
    ui->statusBar->showMessage(tr("Loaded %1").arg(url.toDisplayString()));

    emit encodingUpdated(documentCodec);
    return true;
}

bool MainWindow::writeDocument(const QUrl &url)
{
    NetworkDialog netDiag;
    QTextCodec *codec;
    QByteArray data;
    QString text;

    text = ui->textEdit->toPlainText();
    data.reserve(text.size());

    if (documentCodec == "UTF-8+BOM") {
        codec = QTextCodec::codecForName("UTF-8");
        data.append("\xEF\xBB\xBF");
    }
    else codec = QTextCodec::codecForName(documentCodec.data());
    data.append(codec->fromUnicode(text));

    netDiag.setUrl(url);
    netDiag.setMode(NetworkDialog::Put);
    netDiag.setData(data);
    netDiag.start();

    if (netDiag.exec() != QDialog::Accepted)
        return false;

    setupDocument(url);
    ui->statusBar->showMessage(tr("Saved successfully to %1").arg(url.toDisplayString()));

    return true;
}

QUrl MainWindow::promptUrl(MainWindow::PromptType type, MainWindow::PromptMode mode)
{
    if (type == LocalFile) {
        QUrl fileUrl;

        if (mode == Open)
            fileUrl = QFileDialog::getOpenFileUrl(this);
        else if (mode == Save)
            fileUrl = QFileDialog::getSaveFileUrl(this);

        if (fileUrl.isEmpty())
            return QUrl();
        else
            return fileUrl;
    } else if (type == Network) {
        UrlDialog urlDialog(mode == Open ? UrlDialog::Open : UrlDialog::Save, documentUrl);
        urlDialog.exec();
        return urlDialog.url();
    }
    return QUrl();
}

QImage MainWindow::renderDocument()
{
    QImage output;
    QPainter *painter;
    QTextDocument *document;

    document = ui->textEdit->document()->clone();

    painter = new QPainter();
    painter->setFont(ui->textEdit->font());

    ui->textEdit->document()->drawContents(painter);
    document->setDefaultFont(ui->textEdit->font());

    output = QImage(document->size().toSize(), QImage::Format_ARGB32);
    output.fill(Qt::white);

    document->documentLayout()->setPaintDevice(&output);
    painter->begin(&output);
    document->drawContents(painter);
    painter->end();

    delete painter;
    return output;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (!promptSave())
        e->ignore();
}

void MainWindow::reset()
{
    if (promptSave())
        ui->textEdit->clear();
    setWindowModified(false);
    setupDocument(QUrl());
    emit encodingUpdated(QLatin1String("UTF-8"));
    emit lineEndingsUpdated(QString("LF"));
}

bool MainWindow::open()
{
    if (!promptSave())
        return false;

    QUrl url = promptUrl(LocalFile, Open);

    if (url.isEmpty())
        return false;

    return readDocument(url);
}

bool MainWindow::save()
{
    if (!documentUrl.isValid())
        return saveAs();

    return writeDocument(documentUrl);
}

bool MainWindow::saveAs()
{
    QUrl url = promptUrl(LocalFile, Save);

    if (url.isEmpty())
        return false;

    return writeDocument(url);
}

bool MainWindow::promptSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;

    QMessageBox savePrompt;
    savePrompt.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    savePrompt.setIcon(QMessageBox::NoIcon);
    savePrompt.setText(tr("This file has unsaved changes."));

    switch(savePrompt.exec()) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

void MainWindow::setFont(const QFont &font)
{
    ui->textEdit->setFont(font);
    ui->textEdit->setTabStopWidth(4 * ui->textEdit->fontMetrics().width('w'));
}

void MainWindow::setEncoding(const QLatin1String &encoding)
{
    emit encodingUpdated(encoding);
}

void MainWindow::setLineEndings(const QString &lineEndings)
{
    emit lineEndingsUpdated(lineEndings);
}

void MainWindow::updateEncoding(const QLatin1String &encoding)
{
    documentCodec = encoding;
    encodingLabel->setText(encoding);
}

void MainWindow::updateLineEndings(const QString &lineEndings)
{
    lineEndingsLabel->setText(lineEndings);
}

void MainWindow::on_actionNetworkGet_triggered()
{
    if (!promptSave())
        return;

    QUrl url = promptUrl(Network, Open);

    if (url.isEmpty())
        return;

    readDocument(url);
}

void MainWindow::on_actionNetworkPut_triggered()
{
    QUrl url = promptUrl(Network, Save);

    if (url.isEmpty())
        return;

    writeDocument(url);
}

void MainWindow::on_actionWordwrap_triggered()
{
    if (ui->actionWordwrap->isChecked())
        ui->textEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    else
        ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
}

void MainWindow::on_actionSelectFont_triggered()
{
    QFontDialog fontDialog;
    fontDialog.setCurrentFont(ui->textEdit->font());
    if (fontDialog.exec() == QFontDialog::Accepted)
        setFont(fontDialog.currentFont());
}

void MainWindow::on_action2ch_Mode_triggered()
{
    auto typefaces = QFontDatabase().families();
    if (typefaces.contains("MS PGothic")) {
        setFont(QFont("MS PGothic", 12));
    } else {
        /* We bundle Mona so that SJIS art can still be rendered. */
        QMessageBox::information(
            this,
            tr("Substitute Font"),
            tr("2ch text is normally rendered with MS PGothic, "
               "but the font was not found. A compatible "
               "alternative was selected instead.")
        );
        setFont(QFont("Mona", 12));
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionRender_triggered()
{
    QByteArray data;
    QBuffer *buffer;
    QImage output;
    QUrl dest;

    dest = QFileDialog::getSaveFileUrl(
        this,
        tr("Export to..."),
        QUrl(),
        tr("PNG image (*.png)")
    );

    if (!dest.isValid())
        return;

    buffer = new QBuffer(&data);

    output = renderDocument();
    output.save(buffer, "PNG");

    delete buffer;

    NetworkDialog netDiag;
    netDiag.setUrl(dest);
    netDiag.setMode(NetworkDialog::Put);
    netDiag.setData(data);
    netDiag.start();

    if (netDiag.exec() != QDialog::Accepted)
        return;

    ui->statusBar->showMessage(tr("Rendered document to %1").arg(dest.toDisplayString()));
}
